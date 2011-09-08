/*
 *  Copyright (c) 2011 Ahmad Amireh <ahmad@amireh.net>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#ifndef H_VanillaRenderer_H
#define H_VanillaRenderer_H

#include "Renderer.h"

namespace Pixy {

	/*	\class VanillaRenderer
	 *	\brief
	 *
	 */
	class VanillaRenderer : public Renderer {

	public:
	  VanillaRenderer();
		virtual ~VanillaRenderer();

		/* \brief
		 *
		 */
		virtual bool setup(int argc, char** argv);


		/* \brief
		 *
		 */
 		virtual bool cleanup();

    /* emit signals */
		virtual void injectUnableToConnect( void );
		virtual void injectValidateStarted( void );
		virtual void injectShowPatchLog( std::string inLogPath );
		virtual void injectValidateComplete( bool inNeedUpdate, Version const& inTargetVersion );
		virtual void injectPatchStarted( Version const& inTargetVersion );
    virtual void injectPatchSize( long long inSize );
		virtual void injectPatchProgress( float inPercent );
		virtual void injectPatchFailed( std::string inMsg, Version const& inTargetVersion );
		virtual void injectPatchComplete( Version const& inCurrentVersion );
		virtual void injectApplicationPatched( Version const& inCurrentVersion );

		virtual void go(int argc, char** argv);

	protected:
    long long lPatchSize;
    bool fShuttingDown;
    bool fWantsToLaunch;

	private:
		VanillaRenderer(const VanillaRenderer& src);
		VanillaRenderer& operator=(const VanillaRenderer& rhs);
	};
}
#endif
