/**
 * @file aicurlprivate.h
 * @brief Thread safe wrapper for libcurl.
 *
 * Copyright (c) 2012, Aleric Inglewood.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution.
 *
 * CHANGELOG
 *   and additional copyright holders.
 *
 *   28/04/2012
 *   Initial version, written by Aleric Inglewood @ SL
 */

#ifndef AICURLPRIVATE_H
#define AICURLPRIVATE_H

#include <sstream>
#include "llatomic.h"
#include "llrefcount.h"
#include "aicurlperhost.h"

class AIHTTPHeaders;
class AIHTTPTimeoutPolicy;
class AICurlEasyRequest;
class AICurlEasyRequestStateMachine;

namespace AICurlPrivate {

class CurlEasyRequest;
class ThreadSafeBufferedCurlEasyRequest;

namespace curlthread {
  
class MultiHandle;

// A class that keeps track of timeout administration per connection.
class HTTPTimeout : public LLRefCount {
  private:
	AIHTTPTimeoutPolicy const* mPolicy;			// A pointer to the used timeout policy.
	std::vector<U32> mBuckets;					// An array with the number of bytes transfered in each second.
	U16 mBucket;								// The bucket corresponding to mLastSecond.
	bool mNothingReceivedYet;					// Set when created, reset when the HTML reply header from the server is received.
	bool mLowSpeedOn;							// Set while uploading or downloading data.
	bool mUploadFinished;						// Used to keep track of whether upload_finished was called yet.
	S32 mLastSecond;							// The time at which lowspeed() was last called, in seconds since mLowSpeedClock.
	U32 mTotalBytes;							// The sum of all bytes in mBuckets.
	U64 mLowSpeedClock;							// Clock count at which low speed detection (re)started.
	U64 mStalled;								// The clock count at which this transaction is considered to be stalling if nothing is transfered anymore.
  public:
	static F64 const sClockWidth;				// Time between two clock ticks in seconds.
	static U64 sClockCount;						// Clock count used as 'now' during one loop of the main loop.
#if defined(CWDEBUG) || defined(DEBUG_CURLIO)
	ThreadSafeBufferedCurlEasyRequest* mLockObj;
#endif

  public:
	HTTPTimeout(AIHTTPTimeoutPolicy const* policy, ThreadSafeBufferedCurlEasyRequest* lock_obj) :
		mPolicy(policy), mNothingReceivedYet(true), mLowSpeedOn(false), mUploadFinished(false), mStalled((U64)-1)
#if defined(CWDEBUG) || defined(DEBUG_CURLIO)
		, mLockObj(lock_obj)
#endif
		{ }

	// Called after sending all headers, when body data is written the first time.
	void connected(void);

	// Called when everything we had to send to the server has been sent.
	void upload_finished(void);

	// Called when data is sent. Returns true if transfer timed out.
	bool data_sent(size_t n);

	// Called when data is received. Returns true if transfer timed out.
	bool data_received(size_t n);

	// Called immediately before done() after curl finished, with code.
	void done(AICurlEasyRequest_wat const& curlEasyRequest_w, CURLcode code);

	// Accessor.
	bool has_stalled(void) const { return mStalled < sClockCount;  }

	// Called from BufferedCurlEasyRequest::processOutput if a timeout occurred.
	void print_diagnostics(CurlEasyRequest const* curl_easy_request, char const* eff_url);

#if defined(CWDEBUG) || defined(DEBUG_CURLIO)
	void* get_lockobj(void) const { return mLockObj; }
#endif

  private:
	// (Re)start low speed transer rate detection.
	void reset_lowspeed(void);

	// Common low speed detection, Called from data_sent or data_received.
	bool lowspeed(size_t bytes);
};

} // namespace curlthread

void handle_multi_error(CURLMcode code);
inline CURLMcode check_multi_code(CURLMcode code) { AICurlInterface::Stats::multi_calls++; if (code != CURLM_OK) handle_multi_error(code); return code; }

bool curlThreadIsRunning(void);
void wakeUpCurlThread(void);
void stopCurlThread(void);
void clearCommandQueue(void);

#define DECLARE_SETOPT(param_type) \
	  CURLcode setopt(CURLoption option, param_type parameter)

// This class wraps CURL*'s.
// It guarantees that a pointer is cleaned up when no longer needed, as required by libcurl.
class CurlEasyHandle : public boost::noncopyable, protected AICurlEasyHandleEvents {
  public:
	CurlEasyHandle(void);
	~CurlEasyHandle();

  private:
	// Disallow assignment.
	CurlEasyHandle& operator=(CurlEasyHandle const*);

  public:
	// Reset all options of a libcurl session handle.
	void reset(void) { llassert(!mActiveMultiHandle); curl_easy_reset(mEasyHandle); }

	// Set options for a curl easy handle.
	DECLARE_SETOPT(long);
	DECLARE_SETOPT(long long);
	DECLARE_SETOPT(void const*);
	DECLARE_SETOPT(curl_debug_callback);
	DECLARE_SETOPT(curl_write_callback);
	//DECLARE_SETOPT(curl_read_callback);	Same type as curl_write_callback
	DECLARE_SETOPT(curl_ssl_ctx_callback);
	DECLARE_SETOPT(curl_conv_callback);
#if 0	// Not used by the viewer.
	DECLARE_SETOPT(curl_progress_callback);
	DECLARE_SETOPT(curl_seek_callback);
	DECLARE_SETOPT(curl_ioctl_callback);
	DECLARE_SETOPT(curl_sockopt_callback);
	DECLARE_SETOPT(curl_opensocket_callback);
	DECLARE_SETOPT(curl_closesocket_callback);
	DECLARE_SETOPT(curl_sshkeycallback);
	DECLARE_SETOPT(curl_chunk_bgn_callback);
	DECLARE_SETOPT(curl_chunk_end_callback);
	DECLARE_SETOPT(curl_fnmatch_callback);
#endif
	// Automatically cast int types to a long. Note that U32/S32 are int and
	// that you can overload int and long even if they have the same size.
	CURLcode setopt(CURLoption option, U32 parameter) { return setopt(option, (long)parameter); }
	CURLcode setopt(CURLoption option, S32 parameter) { return setopt(option, (long)parameter); }

	// Clone a libcurl session handle using all the options previously set.
	//CurlEasyHandle(CurlEasyHandle const& orig);

	// URL encode/decode the given string.
	char* escape(char* url, int length);
	char* unescape(char* url, int inlength , int* outlength);

	// Extract information from a curl handle.
  private:
	CURLcode getinfo_priv(CURLINFO info, void* data) const;
  public:
	// The rest are inlines to provide some type-safety.
	CURLcode getinfo(CURLINFO info, char** data) const { return getinfo_priv(info, data); }
	CURLcode getinfo(CURLINFO info, curl_slist** data) const { return getinfo_priv(info, data); }
	CURLcode getinfo(CURLINFO info, double* data) const { return getinfo_priv(info, data); }
	CURLcode getinfo(CURLINFO info, long* data) const { return getinfo_priv(info, data); }
#ifdef __LP64__	// sizeof(long) > sizeof(int) ?
	// Overload for integer types that are too small (libcurl demands a long).
	CURLcode getinfo(CURLINFO info, S32* data) const { long ldata; CURLcode res = getinfo_priv(info, &ldata); *data = static_cast<S32>(ldata); return res; }
	CURLcode getinfo(CURLINFO info, U32* data) const { long ldata; CURLcode res = getinfo_priv(info, &ldata); *data = static_cast<U32>(ldata); return res; }
#else			// sizeof(long) == sizeof(int)
	CURLcode getinfo(CURLINFO info, S32* data) const { return getinfo_priv(info, reinterpret_cast<long*>(data)); }
	CURLcode getinfo(CURLINFO info, U32* data) const { return getinfo_priv(info, reinterpret_cast<long*>(data)); }
#endif

	// Perform a file transfer (blocking).
	CURLcode perform(void);
	// Pause and unpause a connection.
	CURLcode pause(int bitmask);

	// Called when a request is queued for removal. In that case a race between the actual removal
	// and revoking of the callbacks is harmless (and happens for the raw non-statemachine version).
	void remove_queued(void) { mQueuedForRemoval = true; }
	// In case it's added after being removed.
	void add_queued(void) { mQueuedForRemoval = false; }

  private:
	CURL* mEasyHandle;
	CURLM* mActiveMultiHandle;
	mutable char* mErrorBuffer;
	AIPostFieldPtr mPostField;		// This keeps the POSTFIELD data alive for as long as the easy handle exists.
	bool mQueuedForRemoval;			// Set if the easy handle is (probably) added to the multi handle, but is queued for removal.
#ifdef SHOW_ASSERT
  public:
	bool mRemovedPerCommand;		// Set if mActiveMultiHandle was reset as per command from the main thread.
#endif

  private:
	// This should only be called from MultiHandle; add/remove an easy handle to/from a multi handle.
	friend class curlthread::MultiHandle;
	CURLMcode add_handle_to_multi(AICurlEasyRequest_wat& curl_easy_request_w, CURLM* multi_handle);
	CURLMcode remove_handle_from_multi(AICurlEasyRequest_wat& curl_easy_request_w, CURLM* multi_handle);

  public:
	// Returns true if this easy handle was added to a curl multi handle.
	bool active(void) const { return mActiveMultiHandle; }

	// Returns true when it is expected that the parent will revoke callbacks before the curl
	// easy handle is removed from the multi handle; that usually happens when an external
	// error demands termination of the request (ie, an expiration).
	bool no_warning(void) const { return mQueuedForRemoval || LLApp::isExiting(); }

	// Used for debugging purposes.
	bool operator==(CURL* easy_handle) const { return mEasyHandle == easy_handle; }

  private:
	// Call this prior to every curl_easy function whose return value is passed to check_easy_code.
	void setErrorBuffer(void) const;

	static void handle_easy_error(CURLcode code);

	// Always first call setErrorBuffer()!
	static inline CURLcode check_easy_code(CURLcode code)
	{
	  AICurlInterface::Stats::easy_calls++;
	  if (code != CURLE_OK)
		handle_easy_error(code);
	  return code;
	}

  protected:
	// Return the underlying curl easy handle.
	CURL* getEasyHandle(void) const { return mEasyHandle; }

	// Keep POSTFIELD data alive.
	void setPostField(AIPostFieldPtr const& post_field_ptr) { mPostField = post_field_ptr; }

  private:
	// Return, and possibly create, the curl (easy) error buffer used by the current thread.
	static char* getTLErrorBuffer(void);
};

// CurlEasyRequest adds a slightly more powerful interface that can be used
// to set the options on a curl easy handle.
//
// Calling sendRequest() will then connect to the given URL and perform
// the data exchange. Use getResult() to determine if an error occurred.
//
// Note that the life cycle of a CurlEasyRequest is controlled by AICurlEasyRequest:
// a CurlEasyRequest is only ever created as base class of a ThreadSafeBufferedCurlEasyRequest,
// which is only created by creating a AICurlEasyRequest. When the last copy of such
// AICurlEasyRequest is deleted, then also the ThreadSafeBufferedCurlEasyRequest is deleted
// and the CurlEasyRequest destructed.
class CurlEasyRequest : public CurlEasyHandle {
  private:
	void setPost_raw(U32 size, char const* data);
  public:
	void setPost(U32 size) { setPost_raw(size, NULL); }
	void setPost(AIPostFieldPtr const& postdata, U32 size);
	void setPost(char const* data, U32 size) { setPost(new AIPostField(data), size); }
	void setoptString(CURLoption option, std::string const& value);
	void addHeader(char const* str);
	void addHeaders(AIHTTPHeaders const& headers);

  private:
	// Callback stubs.
	static size_t headerCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
	static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
	static size_t readCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
	static CURLcode SSLCtxCallback(CURL* curl, void* sslctx, void* userdata);

	curl_write_callback mHeaderCallback;
	void* mHeaderCallbackUserData;
	curl_write_callback mWriteCallback;
	void* mWriteCallbackUserData;
	curl_read_callback mReadCallback;
	void* mReadCallbackUserData;
	curl_ssl_ctx_callback mSSLCtxCallback;
	void* mSSLCtxCallbackUserData;

  public:
	void setHeaderCallback(curl_write_callback callback, void* userdata);
	void setWriteCallback(curl_write_callback callback, void* userdata);
	void setReadCallback(curl_read_callback callback, void* userdata);
	void setSSLCtxCallback(curl_ssl_ctx_callback callback, void* userdata);

	// Call this if the set callbacks are about to be invalidated.
	void revokeCallbacks(void);

  protected:
	// Reset everything to the state it was in when this object was just created.
	// Called by BufferedCurlEasyRequest::resetState.
	void resetState(void);

  private:
	// Called from applyDefaultOptions.
	void applyProxySettings(void);

	// Used in applyDefaultOptions.
	static CURLcode curlCtxCallback(CURL* curl, void* sslctx, void* parm);

	// Called from get_timeout_object and httptimeout.
	void create_timeout_object(void);

  public:
	// Set default options that we want applied to all curl easy handles.
	void applyDefaultOptions(void);

	// Prepare the request for adding it to a multi session, or calling perform.
	// This actually adds the headers that were collected with addHeader.
	void finalizeRequest(std::string const& url, AIHTTPTimeoutPolicy const& policy, AICurlEasyRequestStateMachine* state_machine);

	// Last second initialization. Called from MultiHandle::add_easy_request.
	void set_timeout_opts(void);

  public:
	// Called by MultiHandle::finish_easy_request() to store result code that is returned by getResult.
	void storeResult(CURLcode result) { mResult = result; }

	// Called by MultiHandle::finish_easy_request() when the curl easy handle is done.
	void done(AICurlEasyRequest_wat& curl_easy_request_w, CURLcode result)
	{
	  if (mTimeout)
	  {
		// Update timeout administration.
		mTimeout->done(curl_easy_request_w, result);
	  }
	  finished(curl_easy_request_w);
	}

	// Called by MultiHandle::check_msg_queue() to fill info with the transfer info.
	void getTransferInfo(AITransferInfo* info);

	// If result != CURLE_FAILED_INIT then also info was filled.
	void getResult(CURLcode* result, AITransferInfo* info = NULL);

	// For debugging purposes.
	void print_curl_timings(void) const;

  protected:
	curl_slist* mHeaders;
	AICurlEasyHandleEvents* mHandleEventsTarget;
	CURLcode mResult;		//AIFIXME: this does not belong in the request object, but belongs in the response object.

	AIHTTPTimeoutPolicy const* mTimeoutPolicy;
	std::string mLowercaseHostname;				// Lowercase hostname (canonicalized) extracted from the url.
	PerHostRequestQueuePtr mPerHostPtr;			// Pointer to the corresponding PerHostRequestQueue.
	LLPointer<curlthread::HTTPTimeout> mTimeout;// Timeout administration object associated with last created CurlSocketInfo.
	bool mTimeoutIsOrphan;						// Set to true when mTimeout is not (yet) associated with a CurlSocketInfo.
#if defined(CWDEBUG) || defined(DEBUG_CURLIO)
  public:
	bool mDebugIsHeadOrGetMethod;
#endif

  public:
	// These two are only valid after finalizeRequest.
	AIHTTPTimeoutPolicy const* getTimeoutPolicy(void) const { return mTimeoutPolicy; }
	std::string const& getLowercaseHostname(void) const { return mLowercaseHostname; }
	// Called by CurlSocketInfo to allow access to the last (after a redirect) HTTPTimeout object related to this request.
	// This creates mTimeout (unless mTimeoutIsOrphan is set in which case it adopts the orphan).
	LLPointer<curlthread::HTTPTimeout>& get_timeout_object(void);
	// Accessor for mTimeout with optional creation of orphaned object (if lockobj != NULL).
	LLPointer<curlthread::HTTPTimeout>& httptimeout(void) { if (!mTimeout) { create_timeout_object(); mTimeoutIsOrphan = true; } return mTimeout; }
	// Return true if no data has been received on the latest socket (if any) for too long.
	bool has_stalled(void) const { return mTimeout && mTimeout->has_stalled(); }

  protected:
	// This class may only be created as base class of BufferedCurlEasyRequest.
	// Throws AICurlNoEasyHandle.
	CurlEasyRequest(void) : mHeaders(NULL), mHandleEventsTarget(NULL), mResult(CURLE_FAILED_INIT), mTimeoutPolicy(NULL), mTimeoutIsOrphan(false)
#if defined(CWDEBUG) || defined(DEBUG_CURLIO)
		, mDebugIsHeadOrGetMethod(false)
#endif
		{ applyDefaultOptions(); }
  public:
	~CurlEasyRequest();

  public:
	// Post-initialization, set the parent to pass the events to.
	void send_handle_events_to(AICurlEasyHandleEvents* target) { mHandleEventsTarget = target; }

	// For debugging purposes
	bool is_finalized(void) const { return mTimeoutPolicy; }

	// Return pointer to the ThreadSafe (wrapped) version of this object.
	inline ThreadSafeBufferedCurlEasyRequest* get_lockobj(void);
	inline ThreadSafeBufferedCurlEasyRequest const* get_lockobj(void) const;

	// PerHost API.
	PerHostRequestQueuePtr getPerHostPtr(void);						// (Optionally create and) return a pointer to the unique
																	// PerHostRequestQueue corresponding to mLowercaseHostname.
	bool removeFromPerHostQueue(AICurlEasyRequest const&) const;	// Remove this request from the per-host queue, if queued at all.
																	// Returns true if it was queued.
  protected:
	// Pass events to parent.
	/*virtual*/ void added_to_multi_handle(AICurlEasyRequest_wat& curl_easy_request_w);
	/*virtual*/ void finished(AICurlEasyRequest_wat& curl_easy_request_w);
	/*virtual*/ void removed_from_multi_handle(AICurlEasyRequest_wat& curl_easy_request_w);
};

// This class adds input/output buffers to the request and hooks up the libcurl callbacks to use those buffers.
// Received data is partially decoded and made available through various member functions.
class BufferedCurlEasyRequest : public CurlEasyRequest {
  public:
	// The type of the used buffers.
	typedef boost::shared_ptr<LLBufferArray> buffer_ptr_t;

	void resetState(void);
	void prepRequest(AICurlEasyRequest_wat& buffered_curl_easy_request_w, AIHTTPHeaders const& headers, LLHTTPClient::ResponderPtr responder);

	buffer_ptr_t& getInput(void) { return mInput; }
	buffer_ptr_t& getOutput(void) { return mOutput; }

	// Called if libcurl doesn't deliver within AIHTTPTimeoutPolicy::mMaximumTotalDelay seconds.
	void timed_out(void);

	// Called after removed_from_multi_handle was called.
	void processOutput(void);

	// Do not write more than this amount.
	//void setBodyLimit(U32 size) { mBodyLimit = size; }

    // Post-initialization, set the parent to pass the events to.
    void send_buffer_events_to(AIBufferedCurlEasyRequestEvents* target) { mBufferEventsTarget = target; }

  protected:
	// Events from this class.
	/*virtual*/ void received_HTTP_header(void);
	/*virtual*/ void received_header(std::string const& key, std::string const& value);
	/*virtual*/ void completed_headers(U32 status, std::string const& reason, AITransferInfo* info);

  private:
	buffer_ptr_t mInput;
	U8* mLastRead;										// Pointer into mInput where we last stopped reading (or NULL to start at the beginning).
	buffer_ptr_t mOutput;
	LLHTTPClient::ResponderPtr mResponder;
	//U32 mBodyLimit;									// From the old LLURLRequestDetail::mBodyLimit, but never used.
	U32 mStatus;										// HTTP status, decoded from the first header line.
	std::string mReason;								// The "reason" from the same header line.
	S32 mRequestTransferedBytes;
	S32 mResponseTransferedBytes;
	AIBufferedCurlEasyRequestEvents* mBufferEventsTarget;

  public:
	static LLChannelDescriptors const sChannels;		// Channel object for mInput (channel out()) and mOutput (channel in()).

  private:
	// This class may only be created by constructing a ThreadSafeBufferedCurlEasyRequest.
	friend class ThreadSafeBufferedCurlEasyRequest;
	BufferedCurlEasyRequest(void);
  public:
	~BufferedCurlEasyRequest();

  private:
    static size_t curlWriteCallback(char* data, size_t size, size_t nmemb, void* user_data);
    static size_t curlReadCallback(char* data, size_t size, size_t nmemb, void* user_data);
    static size_t curlHeaderCallback(char* data, size_t size, size_t nmemb, void* user_data);

	// Called from curlHeaderCallback.
	void setStatusAndReason(U32 status, std::string const& reason);

	// Called from processOutput by in case of an error.
	void print_diagnostics(CURLcode code);

  public:
	// Return pointer to the ThreadSafe (wrapped) version of this object.
	ThreadSafeBufferedCurlEasyRequest* get_lockobj(void);
	ThreadSafeBufferedCurlEasyRequest const* get_lockobj(void) const;

	// Return true when prepRequest was already called and the object has not been
	// invalidated as a result of calling timed_out().
	bool isValid(void) const { return mResponder; }
};

inline ThreadSafeBufferedCurlEasyRequest* CurlEasyRequest::get_lockobj(void)
{
  return static_cast<BufferedCurlEasyRequest*>(this)->get_lockobj();
}

inline ThreadSafeBufferedCurlEasyRequest const* CurlEasyRequest::get_lockobj(void) const
{
  return static_cast<BufferedCurlEasyRequest const*>(this)->get_lockobj();
}

// This class wraps BufferedCurlEasyRequest for thread-safety and adds a reference counter so we can
// copy it around cheaply and it gets destructed automatically when the last instance is deleted.
// It guarantees that the CURL* handle is never used concurrently, which is not allowed by libcurl.
// As AIThreadSafeSimpleDC contains a mutex, it cannot be copied. Therefore we need a reference counter for this object.
class ThreadSafeBufferedCurlEasyRequest : public AIThreadSafeSimple<BufferedCurlEasyRequest> {
  public:
	// Throws AICurlNoEasyHandle.
	ThreadSafeBufferedCurlEasyRequest(void) : mReferenceCount(0)
        { new (ptr()) BufferedCurlEasyRequest;
		  Dout(dc::curl, "Creating ThreadSafeBufferedCurlEasyRequest with this = " << (void*)this);
		  AICurlInterface::Stats::ThreadSafeBufferedCurlEasyRequest_count++; }
	~ThreadSafeBufferedCurlEasyRequest()
	    { Dout(dc::curl, "Destructing ThreadSafeBufferedCurlEasyRequest with this = " << (void*)this);
		  --AICurlInterface::Stats::ThreadSafeBufferedCurlEasyRequest_count; }

  private:
	LLAtomicU32 mReferenceCount;

	friend void intrusive_ptr_add_ref(ThreadSafeBufferedCurlEasyRequest* p);	// Called by boost::intrusive_ptr when a new copy of a boost::intrusive_ptr<ThreadSafeBufferedCurlEasyRequest> is made.
	friend void intrusive_ptr_release(ThreadSafeBufferedCurlEasyRequest* p);	// Called by boost::intrusive_ptr when a boost::intrusive_ptr<ThreadSafeBufferedCurlEasyRequest> is destroyed.
};

// The curl easy request type wrapped in a reference counting pointer.
typedef boost::intrusive_ptr<ThreadSafeBufferedCurlEasyRequest> BufferedCurlEasyRequestPtr;

// This class wraps CURLM*'s.
// It guarantees that a pointer is cleaned up when no longer needed, as required by libcurl.
class CurlMultiHandle : public boost::noncopyable {
  public:
	CurlMultiHandle(void);
	~CurlMultiHandle();

  private:
	// Disallow assignment.
	CurlMultiHandle& operator=(CurlMultiHandle const*);

  private:
	static LLAtomicU32 sTotalMultiHandles;

  protected:
	CURLM* mMultiHandle;

  public:
	// Set options for a curl multi handle.
	CURLMcode setopt(CURLMoption option, long parameter);
	CURLMcode setopt(CURLMoption option, curl_socket_callback parameter);
	CURLMcode setopt(CURLMoption option, curl_multi_timer_callback parameter);
	CURLMcode setopt(CURLMoption option, void* parameter);

	// Returns total number of existing CURLM* handles (excluding ones created outside this class).
	static U32 getTotalMultiHandles(void) { return sTotalMultiHandles; }
};

// Overload the setopt methods in order to enforce the correct types (ie, convert an int to a long).

// curl_multi_setopt may only be passed a long,
inline CURLMcode CurlMultiHandle::setopt(CURLMoption option, long parameter)
{
  llassert(option == CURLMOPT_MAXCONNECTS || option == CURLMOPT_PIPELINING);
  return check_multi_code(curl_multi_setopt(mMultiHandle, option, parameter));
}

// ... or a function pointer,
inline CURLMcode CurlMultiHandle::setopt(CURLMoption option, curl_socket_callback parameter)
{
  llassert(option == CURLMOPT_SOCKETFUNCTION);
  return check_multi_code(curl_multi_setopt(mMultiHandle, option, parameter));
}

inline CURLMcode CurlMultiHandle::setopt(CURLMoption option, curl_multi_timer_callback parameter)
{
  llassert(option == CURLMOPT_TIMERFUNCTION);
  return check_multi_code(curl_multi_setopt(mMultiHandle, option, parameter));
}

// ... or an object pointer.
inline CURLMcode CurlMultiHandle::setopt(CURLMoption option, void* parameter)
{
  llassert(option == CURLMOPT_SOCKETDATA || option == CURLMOPT_TIMERDATA);
  return check_multi_code(curl_multi_setopt(mMultiHandle, option, parameter));
}

} // namespace AICurlPrivate

#endif
