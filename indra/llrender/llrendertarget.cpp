/** 
 * @file llrendertarget.cpp
 * @brief LLRenderTarget implementation
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "linden_common.h"

#include "llrendertarget.h"
#include "llrender.h"
#include "llgl.h"

LLRenderTarget* LLRenderTarget::sBoundTarget = NULL;
U32 LLRenderTarget::sBytesAllocated = 0;

void check_framebuffer_status()
{
	if (gDebugGL)
	{
		GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		switch (status)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			break;
		default:
			llwarns << "check_framebuffer_status failed -- " << std::hex << status << std::dec << llendl;
			ll_fail("check_framebuffer_status failed");	
			break;
		}
	}
}

bool LLRenderTarget::sUseFBO = false;

LLRenderTarget::LLRenderTarget() :
	mResX(0),
	mResY(0),
	mFBO(0),
	mDepth(0),
	mStencil(0),
	mUseDepth(false),
	mRenderDepth(false),
	mUsage(LLTexUnit::TT_TEXTURE),
	mSampleBuffer(NULL)
{
}

LLRenderTarget::~LLRenderTarget()
{
	release();
}

void LLRenderTarget::resize(U32 resx, U32 resy, U32 color_fmt)
{
	//for accounting, get the number of pixels added/subtracted
	S32 pix_diff = (resx*resy)-(mResX*mResY);

	mResX = resx;
	mResY = resy;

	for (U32 i = 0; i < mTex.size(); ++i)
	{ //resize color attachments
		gGL.getTexUnit(0)->bindManual(mUsage, mTex[i]);
		LLImageGL::setManualImage(LLTexUnit::getInternalType(mUsage), 0, color_fmt, mResX, mResY, GL_RGBA, GL_UNSIGNED_BYTE, NULL, false);
		sBytesAllocated += pix_diff*4;
	}

	if (mDepth)
	{ //resize depth attachment
		if (mStencil && mFBO)
		{
			//use render buffers where stencil buffers are in play
			glBindRenderbuffer(GL_RENDERBUFFER, mDepth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mResX, mResY);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		else
		{
			gGL.getTexUnit(0)->bindManual(mUsage, mDepth);
			U32 internal_type = LLTexUnit::getInternalType(mUsage);
			if(!mStencil)
				LLImageGL::setManualImage(internal_type, 0, GL_DEPTH_COMPONENT24, mResX, mResY, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL, false);
			else
				LLImageGL::setManualImage(internal_type, 0, GL_DEPTH24_STENCIL8, mResX, mResY, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL, false);
		}

		sBytesAllocated += pix_diff*4;
	}
	if(mSampleBuffer)
		mSampleBuffer->resize(resx,resy);
}

void LLRenderTarget::setSampleBuffer(LLMultisampleBuffer* buffer)
{
	mSampleBuffer = buffer;
}

bool LLRenderTarget::allocate(U32 resx, U32 resy, U32 color_fmt, bool depth, bool stencil, LLTexUnit::eTextureType usage, bool use_fbo)
{
	stop_glerror();
	release();
	stop_glerror();

	mResX = resx;
	mResY = resy;

	mStencil = stencil;
	mUsage = usage;
	mUseDepth = depth;

	if ((sUseFBO || use_fbo) && gGLManager.mHasFramebufferObject)
	{
		glGenFramebuffers(1, (GLuint *) &mFBO);

		if (depth)
		{
			if (!allocateDepth())
			{
				llwarns << "Failed to allocate depth buffer for render target." << llendl;
				return false;
			}
		}

		if (mDepth)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
			if (mStencil)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepth);
				stop_glerror();
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepth);
				stop_glerror();
			}
			else
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, LLTexUnit::getInternalType(mUsage), mDepth, 0);
				stop_glerror();
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		stop_glerror();
	}

	return addColorAttachment(color_fmt);
}

bool LLRenderTarget::addColorAttachment(U32 color_fmt)
{
	if (color_fmt == 0)
	{
		return true;
	}

	U32 offset = mTex.size();
	if (offset >= 4 ||
		(offset > 0 && (mFBO == 0 || !gGLManager.mHasDrawBuffers)))
	{
		llerrs << "Too many color attachments!" << llendl;
	}

	U32 tex;
	LLImageGL::generateTextures(mUsage, color_fmt, 1, &tex);
	gGL.getTexUnit(0)->bindManual(mUsage, tex);

	stop_glerror();


	{
		clear_glerror();
		LLImageGL::setManualImage(LLTexUnit::getInternalType(mUsage), 0, color_fmt, mResX, mResY, GL_RGBA, GL_UNSIGNED_BYTE, NULL, false);
		if (glGetError() != GL_NO_ERROR)
		{
			llwarns << "Could not allocate color buffer for render target." << llendl;
			return false;
		}
	}
	
	sBytesAllocated += mResX*mResY*4;

	stop_glerror();

	
	if (offset == 0)
	{ //use bilinear filtering on single texture render targets that aren't multisampled
		gGL.getTexUnit(0)->setTextureFilteringOption(LLTexUnit::TFO_BILINEAR);
		stop_glerror();
	}
	else
	{ //don't filter data attachments
		gGL.getTexUnit(0)->setTextureFilteringOption(LLTexUnit::TFO_POINT);
		stop_glerror();
	}

	if (mUsage != LLTexUnit::TT_RECT_TEXTURE)
	{
		gGL.getTexUnit(0)->setTextureAddressMode(LLTexUnit::TAM_MIRROR);
		stop_glerror();
	}
	else
	{
		// ATI doesn't support mirrored repeat for rectangular textures.
		gGL.getTexUnit(0)->setTextureAddressMode(LLTexUnit::TAM_CLAMP);
		stop_glerror();
	}

	if (mFBO)
	{
		stop_glerror();
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+offset,
			LLTexUnit::getInternalType(mUsage), tex, 0);
			stop_glerror();

		check_framebuffer_status();
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	mTex.push_back(tex);
	mInternalFormat.push_back(color_fmt);

	if (gDebugGL)
	{ //bind and unbind to validate target
		bindTarget();
		flush();
	}

	return true;
}

bool LLRenderTarget::allocateDepth()
{
	if (mStencil && mFBO)
	{
		//use render buffers where stencil buffers are in play
		glGenRenderbuffers(1, (GLuint *) &mDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, mDepth);
		stop_glerror();
		clear_glerror();
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mResX, mResY);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	else
	{
		if(!mStencil)
			LLImageGL::generateTextures(mUsage, GL_DEPTH_COMPONENT24, 1, &mDepth);
		else
			LLImageGL::generateTextures(mUsage, GL_DEPTH24_STENCIL8, 1, &mDepth);
		gGL.getTexUnit(0)->bindManual(mUsage, mDepth);
		
		U32 internal_type = LLTexUnit::getInternalType(mUsage);
		stop_glerror();
		clear_glerror();
		if(!mStencil)
			LLImageGL::setManualImage(internal_type, 0, GL_DEPTH_COMPONENT24, mResX, mResY, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL, false);
		else
			LLImageGL::setManualImage(internal_type, 0, GL_DEPTH24_STENCIL8, mResX, mResY, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL, false);
		gGL.getTexUnit(0)->setTextureFilteringOption(LLTexUnit::TFO_POINT);
	}

	if (glGetError() != GL_NO_ERROR)
	{
		llwarns << "Unable to allocate depth buffer for render target." << llendl;
		return false;
	}

	sBytesAllocated += mResX*mResY*4;

	return true;
}

void LLRenderTarget::shareDepthBuffer(LLRenderTarget& target)
{
	if (!mFBO || !target.mFBO)
	{
		llerrs << "Cannot share depth buffer between non FBO render targets." << llendl;
	}

	if (target.mDepth)
	{
		llerrs << "Attempting to override existing depth buffer.  Detach existing buffer first." << llendl;
	}

	if (target.mUseDepth)
	{
		llerrs << "Attempting to override existing shared depth buffer. Detach existing buffer first." << llendl;
	}

	if (mDepth)
	{
		stop_glerror();
		glBindFramebuffer(GL_FRAMEBUFFER, target.mFBO);
		stop_glerror();

		if (mStencil)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepth);
			stop_glerror();
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepth);			
			stop_glerror();
			target.mStencil = true;
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, LLTexUnit::getInternalType(mUsage), mDepth, 0);
			stop_glerror();
		}

		check_framebuffer_status();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		target.mUseDepth = true;
	}
}

void LLRenderTarget::release()
{
	if (mDepth)
	{
		if (mStencil && mFBO)
		{
			glDeleteRenderbuffers(1, (GLuint*) &mDepth);
			stop_glerror();
		}
		else
		{
			//Release before delete.
			if(mFBO)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, LLTexUnit::getInternalType(mUsage), 0, 0);
			}
			LLImageGL::deleteTextures(mUsage, 0, 0, 1, &mDepth, true);
			stop_glerror();
		}
		mDepth = 0;

		sBytesAllocated -= mResX*mResY*4;
	}
	else if (mUseDepth && mFBO)
	{ //detach shared depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		if (mStencil)
		{ //attached as a renderbuffer
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			mStencil = false;
		}
		else
		{ //attached as a texture
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, LLTexUnit::getInternalType(mUsage), 0, 0);
		}
		mUseDepth = false;
	}

	if (mFBO)
	{
		glDeleteFramebuffers(1, (GLuint *) &mFBO);
		mFBO = 0;
	}

	if (mTex.size() > 0)
	{
		sBytesAllocated -= mResX*mResY*4*mTex.size();
		LLImageGL::deleteTextures(mUsage, mInternalFormat[0], 0, mTex.size(), &mTex[0], true);
		mTex.clear();
		mInternalFormat.clear();
	}

	mResX = mResY = 0;

	mSampleBuffer = NULL;
	sBoundTarget = NULL;
}

void LLRenderTarget::bindTarget()
{
	if (mFBO)
	{
		stop_glerror();
		if (mSampleBuffer)
		{
			mSampleBuffer->bindTarget(this);
			stop_glerror();
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
			stop_glerror();
			if (gGLManager.mHasDrawBuffers)
			{ //setup multiple render targets
				GLenum drawbuffers[] = {GL_COLOR_ATTACHMENT0,
										GL_COLOR_ATTACHMENT1,
										GL_COLOR_ATTACHMENT2,
										GL_COLOR_ATTACHMENT3};
				glDrawBuffersARB(mTex.size(), drawbuffers);
			}
			
			if (mTex.empty())
			{ //no color buffer to draw to
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}

			check_framebuffer_status();

			stop_glerror();
		}
	}

	glViewport(0, 0, mResX, mResY);
	sBoundTarget = this;
}

// static
void LLRenderTarget::unbindTarget()
{
	if (gGLManager.mHasFramebufferObject)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	sBoundTarget = NULL;
}

void LLRenderTarget::clear(U32 mask_in)
{
	U32 mask = GL_COLOR_BUFFER_BIT;
	if (mUseDepth)
	{
		mask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
	}
	if (mFBO)
	{
		check_framebuffer_status();
		stop_glerror();
		glClear(mask & mask_in);
		stop_glerror();
	}
	else
	{
		LLGLEnable scissor(GL_SCISSOR_TEST);
		glScissor(0, 0, mResX, mResY);
		stop_glerror();
		glClear(mask & mask_in);
	}
}

U32 LLRenderTarget::getTexture(U32 attachment) const
{
	if (attachment > mTex.size()-1)
	{
		llerrs << "Invalid attachment index." << llendl;
	}
	if (mTex.empty())
	{
		return 0;
	}
	return mTex[attachment];
}

void LLRenderTarget::bindTexture(U32 index, S32 channel)
{
	gGL.getTexUnit(channel)->bindManual(mUsage, getTexture(index));
}

void LLRenderTarget::flush(bool fetch_depth)
{
	gGL.flush();
	if (!mFBO)
	{
		gGL.getTexUnit(0)->bind(this);
		glCopyTexSubImage2D(LLTexUnit::getInternalType(mUsage), 0, 0, 0, 0, 0, mResX, mResY);
		stop_glerror();

		if (fetch_depth)
		{
			if (!mDepth)
			{
				allocateDepth();
			}

			gGL.getTexUnit(0)->bind(this,true);
			glCopyTexSubImage2D(LLTexUnit::getInternalType(mUsage), 0, 0, 0, 0, 0, mResX, mResY);
			stop_glerror();
			//glCopyTexImage2D(LLTexUnit::getInternalType(mUsage), 0, GL_DEPTH24_STENCIL8, 0, 0, mResX, mResY, 0);
		}

		gGL.getTexUnit(0)->disable();
	}
	else
	{
		stop_glerror();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		stop_glerror();

		if (mSampleBuffer)
		{
			LLGLEnable multisample(GL_MULTISAMPLE);
			stop_glerror();
			glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
			stop_glerror();
			check_framebuffer_status();
			glBindFramebuffer(GL_READ_FRAMEBUFFER, mSampleBuffer->mFBO);
			check_framebuffer_status();

			stop_glerror();
			if(gGLManager.mIsATI)
			{
				glBlitFramebuffer(0, 0, mResX, mResY, 0, 0, mResX, mResY, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				glBlitFramebuffer(0, 0, mResX, mResY, 0, 0, mResX, mResY, GL_STENCIL_BUFFER_BIT, GL_NEAREST);
			}
			else
			{
				glBlitFramebuffer(0, 0, mResX, mResY, 0, 0, mResX, mResY, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
			}
			stop_glerror();		

			//Following case never currently evalutes true, but it's still good to have.
			if (mTex.size() > 1)
			{
				for (U32 i = 1; i < mTex.size(); ++i)
				{
					glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
					glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
					stop_glerror();
					glBlitFramebuffer(0, 0, mResX, mResY, 0, 0, mResX, mResY, GL_COLOR_BUFFER_BIT, GL_NEAREST);
					stop_glerror();
				}

				/*for (U32 i = 1; i < mTex.size(); ++i)
				{

					glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
										LLTexUnit::getInternalType(mUsage), mTex[i], 0);
					stop_glerror();
					glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mSampleBuffer->mTex[i]);
					stop_glerror();
					glBlitFramebuffer(0, 0, mResX, mResY, 0, 0, mResX, mResY, GL_COLOR_BUFFER_BIT, GL_NEAREST);		
					stop_glerror();
				}

				for (U32 i = 0; i < mTex.size(); ++i)
				{
					glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i,
										LLTexUnit::getInternalType(mUsage), mTex[i], 0);
					stop_glerror();
					glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_RENDERBUFFER, mSampleBuffer->mTex[i]);
					stop_glerror();
				}*/
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}
void LLRenderTarget::copyContents(LLRenderTarget& source, S32 srcX0, S32 srcY0, S32 srcX1, S32 srcY1,
						S32 dstX0, S32 dstY0, S32 dstX1, S32 dstY1, U32 mask, U32 filter)
{
	GLboolean write_depth = mask & GL_DEPTH_BUFFER_BIT ? TRUE : FALSE;

	LLGLDepthTest depth(write_depth, write_depth);

	gGL.flush();
	if (!source.mFBO || !mFBO)
	{
		llwarns << "Cannot copy framebuffer contents for non FBO render targets." << llendl;
		return;
	}

	if (mSampleBuffer)
	{
		mSampleBuffer->copyContents(source, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
	}
	else
	{
		if (mask == GL_DEPTH_BUFFER_BIT && !mStencil && source.mStencil != mStencil)
		{
			stop_glerror();
		
			glBindFramebuffer(GL_FRAMEBUFFER, source.mFBO);
			check_framebuffer_status();
			gGL.getTexUnit(0)->bind(this, true);
			stop_glerror();
			glCopyTexSubImage2D(LLTexUnit::getInternalType(mUsage), 0, srcX0, srcY0, dstX0, dstY0, dstX1, dstY1);
			stop_glerror();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			stop_glerror();
		}
		else
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, source.mFBO);
			stop_glerror();
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO);
			stop_glerror();
			check_framebuffer_status();
			stop_glerror();
			if(gGLManager.mIsATI && mask & GL_STENCIL_BUFFER_BIT)
			{
				mask &= ~GL_STENCIL_BUFFER_BIT;
				glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_STENCIL_BUFFER_BIT, filter);
			}
			if(mask)
				glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
			stop_glerror();
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			stop_glerror();
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			stop_glerror();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			stop_glerror();
		}
	}
}

//static
void LLRenderTarget::copyContentsToFramebuffer(LLRenderTarget& source, S32 srcX0, S32 srcY0, S32 srcX1, S32 srcY1,
						S32 dstX0, S32 dstY0, S32 dstX1, S32 dstY1, U32 mask, U32 filter)
{
	if (!source.mFBO)
	{
		llwarns << "Cannot copy framebuffer contents for non FBO render targets." << llendl;
		return;
	}
	{
		GLboolean write_depth = mask & GL_DEPTH_BUFFER_BIT ? TRUE : FALSE;

		LLGLDepthTest depth(write_depth, write_depth);
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, source.mSampleBuffer ? source.mSampleBuffer->mFBO : source.mFBO);
		stop_glerror();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		stop_glerror();
		check_framebuffer_status();
		stop_glerror();
		if(gGLManager.mIsATI && mask & GL_STENCIL_BUFFER_BIT)
		{
			mask &= ~GL_STENCIL_BUFFER_BIT;
			glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_STENCIL_BUFFER_BIT, filter);
		}
		if(mask)
			glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
		stop_glerror();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		stop_glerror();
	}
}

bool LLRenderTarget::isComplete() const
{
	return (!mTex.empty() || mDepth) ? true : false;
}

void LLRenderTarget::getViewport(S32* viewport)
{
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = mResX;
	viewport[3] = mResY;
}

//==================================================
// LLMultisampleBuffer implementation
//==================================================
LLMultisampleBuffer::LLMultisampleBuffer() :
	mSamples(0),
	mColorFormat(0)
{

}

LLMultisampleBuffer::~LLMultisampleBuffer()
{
	release();
}

void LLMultisampleBuffer::release()
{
	if (mFBO)
	{
		glDeleteFramebuffers(1, (GLuint *) &mFBO);
		mFBO = 0;
	}

	if (mTex.size() > 0)
	{
		sBytesAllocated -= mResX*mResY*4*mTex.size();
		glDeleteRenderbuffers(mTex.size(), (GLuint *) &mTex[0]);
		mTex.clear();
		
	}

	if (mDepth)
	{
		sBytesAllocated -= mResX*mResY*4;
		glDeleteRenderbuffers(1, (GLuint *) &mDepth);
		mDepth = 0;
	}
}

void LLMultisampleBuffer::bindTarget()
{
	bindTarget(this);
}

void LLMultisampleBuffer::bindTarget(LLRenderTarget* ref)
{
	if (!ref)
	{
		ref = this;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	if (gGLManager.mHasDrawBuffers)
	{ //setup multiple render targets
		GLenum drawbuffers[] = {GL_COLOR_ATTACHMENT0,
								GL_COLOR_ATTACHMENT1,
								GL_COLOR_ATTACHMENT2,
								GL_COLOR_ATTACHMENT3};
		glDrawBuffersARB(ref->mTex.size(), drawbuffers);
	}

	check_framebuffer_status();

	glViewport(0, 0, mResX, mResY);

	sBoundTarget = this;
}

bool LLMultisampleBuffer::allocate(U32 resx, U32 resy, U32 color_fmt, bool depth, bool stencil,  LLTexUnit::eTextureType usage, bool use_fbo )
{
	return allocate(resx,resy,color_fmt,depth,stencil,usage,use_fbo,2);
}

bool LLMultisampleBuffer::allocate(U32 resx, U32 resy, U32 color_fmt, bool depth, bool stencil,  LLTexUnit::eTextureType usage, bool use_fbo, U32 samples )
{
	release();
	stop_glerror();

	if (!gGLManager.mHasFramebufferMultisample || !gGLManager.mHasFramebufferObject || !(sUseFBO || use_fbo))
		return false;

	if(color_fmt != GL_RGBA)
	{
		llwarns << "Unsupported color format: " << color_fmt << llendl;
		return false;
	}

	//Restrict to valid sample count
	{
		mSamples = samples;
		//mSamples = llmin(mSamples, (U32)4);	//Cap to prevent memory bloat.
		mSamples = llmin(mSamples, (U32) gGLManager.mMaxSamples);
	}

	if (mSamples <= 1)
		return false;
	
	mResX = resx;
	mResY = resy;

	mUsage = usage;
	mUseDepth = depth;
	mStencil = stencil;
	mColorFormat = color_fmt;

	{

		if (depth)
		{
			stop_glerror();
			if(!allocateDepth())
			{
				release();
				return false;
			}
			stop_glerror();
		}
		glGenFramebuffers(1, (GLuint *) &mFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		stop_glerror();
		clear_glerror();
		if (mDepth)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepth);
			if (mStencil)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepth);			
			}
		}
		
		stop_glerror();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		stop_glerror();
		
		
	}

	return addColorAttachment(color_fmt);
}

void LLMultisampleBuffer::resize(U32 resx, U32 resy)
{
	//for accounting, get the number of pixels added/subtracted
	S32 pix_diff = (resx*resy)-(mResX*mResY);
		
	mResX = resx;
	mResY = resy;

	for (U32 i = 0; i < mTex.size(); ++i)
	{ //resize color attachments
		glBindRenderbuffer(GL_RENDERBUFFER, mTex[i]);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, mColorFormat, mResX, mResY);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		sBytesAllocated += pix_diff*4;
	}

	if (mDepth)
	{ //resize depth attachment
		if (mStencil)
		{
			//use render buffers where stencil buffers are in play
			glBindRenderbuffer(GL_RENDERBUFFER, mDepth);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH24_STENCIL8, mResX, mResY);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		else
		{
			glBindRenderbuffer(GL_RENDERBUFFER, mDepth);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH_COMPONENT24, mResX, mResY);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			
		}
		sBytesAllocated += pix_diff*4;
	}
}

bool LLMultisampleBuffer::addColorAttachment(U32 color_fmt)
{
	if (color_fmt == 0)
	{
		return true;
	}

	U32 offset = mTex.size();
	if (offset >= 4 ||
		(offset > 0 && (mFBO == 0 || !gGLManager.mHasDrawBuffers)))
	{
		llerrs << "Too many color attachments!" << llendl;
	}

	U32 tex;
	glGenRenderbuffers(1, &tex);
	glBindRenderbuffer(GL_RENDERBUFFER, tex);
	stop_glerror();
	clear_glerror();
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, color_fmt, mResX, mResY);
	if (glGetError() != GL_NO_ERROR)
	{
		llwarns << "Unable to allocate color buffer for multisample render target." << llendl;
		release();
		return false;
	}
	
	sBytesAllocated += mResX*mResY*4;
	
	if (mFBO)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+offset, GL_RENDERBUFFER, tex);
		check_framebuffer_status();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	mTex.push_back(tex);
	return true;
}

bool LLMultisampleBuffer::allocateDepth()
{
	glGenRenderbuffers(1, (GLuint* ) &mDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepth);
	stop_glerror();
	clear_glerror();
	if (mStencil)
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH24_STENCIL8, mResX, mResY);
	}
	else
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH_COMPONENT24, mResX, mResY);
	}

	if (glGetError() != GL_NO_ERROR)
	{
		llwarns << "Unable to allocate depth buffer for multisample render target." << llendl;
		return false;
	}
	
	sBytesAllocated += mResX*mResY*4;
	
	return true;
}
