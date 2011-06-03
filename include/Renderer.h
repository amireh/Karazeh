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

#ifndef H_Renderer_H
#define H_Renderer_H

#include "Pixy.h"
#include "Repository.h"

namespace Pixy {

	/*! \class Renderer
	 *	\brief
	 *	The Renderer is the interface the user will interact with. Renderers are
	 *  injected with events that represent the stage of the patching process,
	 *  and handle all input.
	 */
	class Renderer {

	public:
	  inline Renderer() { mLog = 0; fSetup = false; mName = "Renderer"; };
		inline virtual ~Renderer() { mLog = 0; fSetup = false; mName = ""; };

		/*! \brief
		 *  Renderer names are only used for logging.
		 */
		inline virtual std::string& getName() { return mName; };

		/* \brief
		 *  Renderers are the first components to be initialised. Allocate all
		 *  the resources you need here, along with any initialisation code for
		 *  the engine used.
		 *
		 *  \warn
		 *  Other components such as the InputManager are NOT initialised at this
		 *  stage yet, so make sure you do not reference them here. If you need to,
		 *  see deferredSetup() below.
		 */
		virtual bool setup()=0;

		/* \brief
		 *  Called after all components are initialized, such as the InputManager,
		 *  Downloader, and Patcher.
		 */
		virtual bool deferredSetup()=0;

		/* \brief
		 *  Called within the application's main loop. Handle any continuous logic
		 *  here; ie draw, process events.
		 */
		virtual void update(unsigned long lTimeElapsed)=0;

		/* \brief
		 *  Clean up here any resources you've allocated. This will be called
		 *  on the application's shutdown.
		 */
 		virtual bool cleanup()=0;

    /* ------
     * Event handlers: override these handlers in child to handle them. Each
     * of these events represents a stage of the process.
     * ------ */

    /*! \brief
     *  This event is triggered when the Downloader is unable to retrieve the
     *  patch list from all the registered hosts.
     */
		virtual void injectUnableToConnect( void )=0;

		/*! \brief
     *  Indicates that the Patcher has started validating the application's
     *  version by parsing the patch list.
     */
		virtual void injectValidateStarted( void )=0;

		/*! \brief
     *  Contains whether the application is out of date or up to date.
     *
     *  Event properties:
     *    NeedUpdate: "Yes" or "No"
     */
		virtual void injectValidateComplete( bool inNeedUpdate, const Version& inTargetVersion )=0;

		/*! \brief
     *  Triggered when the Patcher has started updating to a certain version.
     *
     *  Event properties:
     *    Version: the version being updated to
     */
		virtual void injectPatchStarted( const Version& inTargetVersion )=0;

		/*! \brief
     *  Triggered while the Downloader is fetching files from the remote
     *  patch server indicating how much it has completed.
     *
     *  Event properties:
     *    Progress: string containing the progress % (should be cast to int)
     */
		virtual void injectPatchProgress( int inPercent )=0;

		/*! \brief
     *  Triggered if any error occurs within the Patcher or the Downloader
     *  while patching a certain repository.
     *
     *  TODO: add error code or reason
     */
		virtual void injectPatchFailed( std::string inMsg, const Version& inTargetVersion )=0;

		/*! \brief
     *  Indicates that the Patcher has finished processing a single repository.
     *
     *  Event properties:
     *    Version: the version to which the application was updated
     */
		virtual void injectPatchComplete( const Version& inCurrentVersion )=0;

		/*! \brief
     *  Triggered when the Patcher is done processing all the repositories
     *  and has successfully updated the application to the latest version.
     */
		virtual void injectApplicationPatched( const Version& inCurrentVersion )=0;

	protected:
    log4cpp::Category* mLog;
    bool fSetup;

    std::string mName; //! used merely for logging purposes

	private:
		Renderer(const Renderer& src);
		Renderer& operator=(const Renderer& rhs);
	};
}
#endif
