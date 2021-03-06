/**
 * @file llmarketplacefunctions.cpp
 * @brief Implementation of assorted functions related to the marketplace
 *
 * $LicenseInfo:firstyear=2001&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "llmarketplacefunctions.h"

#include "llagent.h"
#include "llhttpclient.h"
#include "lltimer.h"
#include "lltrans.h"
#include "llviewercontrol.h"
#include "llviewermedia.h"
#include "llviewernetwork.h"
#include "hippogridmanager.h"

//
// Helpers
//

static std::string getLoginUriDomain()
{
	LLURI uri(gHippoGridManager->getConnectedGrid()->getLoginUri());
	std::string hostname = uri.hostName();	// Ie, "login.<gridid>.lindenlab.com"
	if (hostname.substr(0, 6) == "login.")
	{
		hostname = hostname.substr(6);		// "<gridid>.lindenlab.com"
	}
	return hostname;
}

// Apart from well-known cases, in general this function returns the domain of the loginUri (with the "login." stripped off).
// This should be correct for all SL BETA grids, assuming they have the form of "login.<gridId>.lindenlab.com", in which
// case it returns "<gridId>.lindenlab.com".
//
// Well-known cases that deviate from this:
// agni      --> "secondlife.com"
// damballah --> "secondlife-staging.com"
//
static std::string getMarketplaceDomain()
{
	std::string domain;
	if (gHippoGridManager->getCurrentGrid()->isSecondLife())
	{
		if (gHippoGridManager->getConnectedGrid()->isInProductionGrid())
		{
			domain = "secondlife.com";		// agni
		}
		else
		{
			// SecondLife(tm) BETA grid.
			// Using the login URI is a bit of a kludge, but it's the best we've got at the moment.
			domain = utf8str_tolower(getLoginUriDomain());				// <gridid>.lindenlab.com; ie, "aditi.lindenlab.com".
			std::string::size_type len = domain.length();
			llassert(len > 14 && domain.substr(len - 14) == ".lindenlab.com");
			if (domain == "damballah.lindenlab.com")
			{
				domain = "secondlife-staging.com";
			}
		}
	}
	else
	{
		// TODO: Find out if OpenSim, and Avination adopted any outbox stuffs, if so code HippoGridManager for this
		// Aurora grid has not.
		// For now, set domain on other grids to the loginUri domain, so we don't harass LL web services.
		domain = getLoginUriDomain(); //gHippoGridManager->getCurrentGrid()->getMarketPlaceDomain();
	}
	return domain;
}

static std::string getMarketplaceURL(const std::string& urlStringName)
{
	LLStringUtil::format_map_t domain_arg;
	domain_arg["[MARKETPLACE_DOMAIN_NAME]"] = getMarketplaceDomain();

	std::string marketplace_url = LLTrans::getString(urlStringName, domain_arg);

	return marketplace_url;
}

LLSD getMarketplaceStringSubstitutions()
{
	std::string marketplace_url = getMarketplaceURL("MarketplaceURL");
	std::string marketplace_url_create = getMarketplaceURL("MarketplaceURL_CreateStore");
	std::string marketplace_url_dashboard = getMarketplaceURL("MarketplaceURL_Dashboard");
	std::string marketplace_url_imports = getMarketplaceURL("MarketplaceURL_Imports");
	std::string marketplace_url_info = getMarketplaceURL("MarketplaceURL_LearnMore");

	LLSD marketplace_sub_map;

	marketplace_sub_map["[MARKETPLACE_URL]"] = marketplace_url;
	marketplace_sub_map["[MARKETPLACE_CREATE_STORE_URL]"] = marketplace_url_create;
	marketplace_sub_map["[MARKETPLACE_LEARN_MORE_URL]"] = marketplace_url_info;
	marketplace_sub_map["[MARKETPLACE_DASHBOARD_URL]"] = marketplace_url_dashboard;
	marketplace_sub_map["[MARKETPLACE_IMPORTS_URL]"] = marketplace_url_imports;

	return marketplace_sub_map;
}

class AIHTTPTimeoutPolicy;
extern AIHTTPTimeoutPolicy MPImportGetResponder_timeout;
extern AIHTTPTimeoutPolicy MPImportPostResponder_timeout;

namespace LLMarketplaceImport
{
	// Basic interface for this namespace


	bool hasSessionCookie();
	bool inProgress();
	bool resultPending();
	U32 getResultStatus();
	const LLSD& getResults();

	bool establishMarketplaceSessionCookie();
	bool pollStatus();
	bool triggerImport();

	// Internal state variables

	static std::string sMarketplaceCookie = "";
	static LLSD sImportId = LLSD::emptyMap();
	static bool sImportInProgress = false;
	static bool sImportPostPending = false;
	static bool sImportGetPending = false;
	static U32 sImportResultStatus = 0;
	static LLSD sImportResults = LLSD::emptyMap();

	static LLTimer slmGetTimer;
	static LLTimer slmPostTimer;

	// Responders

	class LLImportPostResponder : public LLHTTPClient::ResponderWithCompleted
	{
	public:
		AIHTTPTimeoutPolicy const& getHTTPTimeoutPolicy(void) const { return MPImportPostResponder_timeout; }

		void completed(U32 status, const std::string& reason, const LLSD& content)
		{
			slmPostTimer.stop();

			if (gSavedSettings.getBOOL("InventoryOutboxLogging"))
			{
				llinfos << " SLM POST status: " << status << llendl;
				llinfos << " SLM POST reason: " << reason << llendl;
				llinfos << " SLM POST content: " << content.asString() << llendl;

				llinfos << " SLM POST timer: " << slmPostTimer.getElapsedTimeF32() << llendl;
			}

			if ((status == MarketplaceErrorCodes::IMPORT_REDIRECT) ||
				(status == MarketplaceErrorCodes::IMPORT_AUTHENTICATION_ERROR) ||
				(status == MarketplaceErrorCodes::IMPORT_JOB_TIMEOUT))
			{
				if (gSavedSettings.getBOOL("InventoryOutboxLogging"))
				{
					llinfos << " SLM POST clearing marketplace cookie due to authentication failure or timeout" << llendl;
				}

				sMarketplaceCookie.clear();
			}

			sImportInProgress = (status == MarketplaceErrorCodes::IMPORT_DONE);
			sImportPostPending = false;
			sImportResultStatus = status;
			sImportId = content;
		}
	};

	class LLImportGetResponder : public LLHTTPClient::ResponderWithCompleted
	{
	public:
		AIHTTPTimeoutPolicy const& getHTTPTimeoutPolicy(void) const { return MPImportGetResponder_timeout; }

		/*virtual*/ bool followRedir(void) const { return true; }
		/*virtual*/ bool needsHeaders(void) const { return true; }

		void completedHeaders(U32 status, std::string const& reason, AIHTTPReceivedHeaders const& headers)
		{
			std::string set_cookie_string;
			if (headers.getFirstValue("set-cookie", set_cookie_string) && !set_cookie_string.empty())
			{
				sMarketplaceCookie = set_cookie_string;
			}
		}

		void completed(U32 status, const std::string& reason, const LLSD& content)
		{
			slmGetTimer.stop();

			if (gSavedSettings.getBOOL("InventoryOutboxLogging"))
			{
				llinfos << " SLM GET status: " << status << llendl;
				llinfos << " SLM GET reason: " << reason << llendl;
				llinfos << " SLM GET content: " << content.asString() << llendl;

				llinfos << " SLM GET timer: " << slmGetTimer.getElapsedTimeF32() << llendl;
			}

			if ((status == MarketplaceErrorCodes::IMPORT_AUTHENTICATION_ERROR) ||
				(status == MarketplaceErrorCodes::IMPORT_JOB_TIMEOUT))
			{
				if (gSavedSettings.getBOOL("InventoryOutboxLogging"))
				{
					llinfos << " SLM GET clearing marketplace cookie due to authentication failure or timeout" << llendl;
				}

				sMarketplaceCookie.clear();
			}

			sImportInProgress = (status == MarketplaceErrorCodes::IMPORT_PROCESSING);
			sImportGetPending = false;
			sImportResultStatus = status;
			sImportResults = content;
		}
	};

	// Basic API

	bool hasSessionCookie()
	{
		return !sMarketplaceCookie.empty();
	}

	bool inProgress()
	{
		return sImportInProgress;
	}

	bool resultPending()
	{
		return (sImportPostPending || sImportGetPending);
	}

	U32 getResultStatus()
	{
		return sImportResultStatus;
	}

	const LLSD& getResults()
	{
		return sImportResults;
	}

	static std::string getInventoryImportURL()
	{
		std::string url = getMarketplaceURL("MarketplaceURL");

		url += "api/1/";
		url += gAgent.getID().getString();
		url += "/inventory/import/";

		return url;
	}

	bool establishMarketplaceSessionCookie()
	{
		if (hasSessionCookie())
		{
			return false;
		}

		sImportInProgress = true;
		sImportGetPending = true;

		std::string url = getInventoryImportURL();

		if (gSavedSettings.getBOOL("InventoryOutboxLogging"))
		{
			llinfos << " SLM GET: " << url << llendl;
		}

		slmGetTimer.start();
		AIHTTPHeaders headers = LLViewerMedia::getHeaders();
		LLHTTPClient::get(url, new LLImportGetResponder(), headers);

		return true;
	}

	bool pollStatus()
	{
		if (!hasSessionCookie())
		{
			return false;
		}

		sImportGetPending = true;

		std::string url = getInventoryImportURL();

		url += sImportId.asString();

		// Make the headers for the post
		AIHTTPHeaders headers;
		headers.addHeader("Accept", "*/*");
		headers.addHeader("Cookie", sMarketplaceCookie);
		headers.addHeader("Content-Type", "application/llsd+xml");
		headers.addHeader("User-Agent", LLViewerMedia::getCurrentUserAgent());

		if (gSavedSettings.getBOOL("InventoryOutboxLogging"))
		{
			llinfos << " SLM GET: " << url << llendl;
		}

		slmGetTimer.start();
		LLHTTPClient::get(url, new LLImportGetResponder(), headers);

		return true;
	}

	bool triggerImport()
	{
		if (!hasSessionCookie())
		{
			return false;
		}

		sImportId = LLSD::emptyMap();
		sImportInProgress = true;
		sImportPostPending = true;
		sImportResultStatus = MarketplaceErrorCodes::IMPORT_PROCESSING;
		sImportResults = LLSD::emptyMap();

		std::string url = getInventoryImportURL();

		// Make the headers for the post
		AIHTTPHeaders headers;
		headers.addHeader("Accept", "*/*");
		headers.addHeader("Connection", "Keep-Alive");
		headers.addHeader("Cookie", sMarketplaceCookie);
		headers.addHeader("Content-Type", "application/xml");
		headers.addHeader("User-Agent", LLViewerMedia::getCurrentUserAgent());

		if (gSavedSettings.getBOOL("InventoryOutboxLogging"))
		{
			llinfos << " SLM POST: " << url << llendl;
		}

		slmPostTimer.start();
		LLHTTPClient::post(url, LLSD(), new LLImportPostResponder(), headers);

		return true;
	}
}


//
// Interface class
//

//static const F32 MARKET_IMPORTER_UPDATE_FREQUENCY = 300.0f; //1.0f;

//static
void LLMarketplaceInventoryImporter::update()
{
	if (instanceExists())
	{
		static LLTimer update_timer;
		if (update_timer.hasExpired())
		{
			LLMarketplaceInventoryImporter::instance().updateImport();
			static LLCachedControl<F32> MARKET_IMPORTER_UPDATE_FREQUENCY("MarketImporterUpdateFreq", 10.0f);
			update_timer.setTimerExpirySec(MARKET_IMPORTER_UPDATE_FREQUENCY);
		}
	}
}

LLMarketplaceInventoryImporter::LLMarketplaceInventoryImporter()
	: mAutoTriggerImport(false)
	, mImportInProgress(false)
	, mInitialized(false)
	, mErrorInitSignal(NULL)
	, mStatusChangedSignal(NULL)
	, mStatusReportSignal(NULL)
{
}

boost::signals2::connection LLMarketplaceInventoryImporter::setInitializationErrorCallback(const status_report_signal_t::slot_type& cb)
{
	if (mErrorInitSignal == NULL)
	{
		mErrorInitSignal = new status_report_signal_t();
	}

	return mErrorInitSignal->connect(cb);
}

boost::signals2::connection LLMarketplaceInventoryImporter::setStatusChangedCallback(const status_changed_signal_t::slot_type& cb)
{
	if (mStatusChangedSignal == NULL)
	{
		mStatusChangedSignal = new status_changed_signal_t();
	}

	return mStatusChangedSignal->connect(cb);
}

boost::signals2::connection LLMarketplaceInventoryImporter::setStatusReportCallback(const status_report_signal_t::slot_type& cb)
{
	if (mStatusReportSignal == NULL)
	{
		mStatusReportSignal = new status_report_signal_t();
	}

	return mStatusReportSignal->connect(cb);
}

void LLMarketplaceInventoryImporter::initialize()
{
	llassert(!mInitialized);

	if (!LLMarketplaceImport::hasSessionCookie())
	{
		LLMarketplaceImport::establishMarketplaceSessionCookie();
	}
}

void LLMarketplaceInventoryImporter::reinitializeAndTriggerImport()
{
	mInitialized = false;

	initialize();

	mAutoTriggerImport = true;
}

bool LLMarketplaceInventoryImporter::triggerImport()
{
	const bool import_triggered = LLMarketplaceImport::triggerImport();

	if (!import_triggered)
	{
		reinitializeAndTriggerImport();
	}

	return import_triggered;
}

void LLMarketplaceInventoryImporter::updateImport()
{
	const bool in_progress = LLMarketplaceImport::inProgress();

	if (in_progress && !LLMarketplaceImport::resultPending())
	{
		const bool polling_status = LLMarketplaceImport::pollStatus();

		if (!polling_status)
		{
			reinitializeAndTriggerImport();
		}
	}

	if (mImportInProgress != in_progress)
	{
		mImportInProgress = in_progress;

		// If we are no longer in progress
		if (!mImportInProgress)
		{
			if (mInitialized)
			{
				// Report results
				if (mStatusReportSignal)
				{
					(*mStatusReportSignal)(LLMarketplaceImport::getResultStatus(), LLMarketplaceImport::getResults());
				}
			}
			else
			{
				// Look for results success
				mInitialized = LLMarketplaceImport::hasSessionCookie();

				if (mInitialized)
				{
					// Follow up with auto trigger of import
					if (mAutoTriggerImport)
					{
						mAutoTriggerImport = false;

						mImportInProgress = triggerImport();
					}
				}
				else if (mErrorInitSignal)
				{
					(*mErrorInitSignal)(LLMarketplaceImport::getResultStatus(), LLMarketplaceImport::getResults());
				}
			}
		}

		// Make sure we trigger the status change with the final state (in case of auto trigger after initialize)
		if (mStatusChangedSignal)
		{
			(*mStatusChangedSignal)(mImportInProgress);
		}
	}
}

