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

#ifndef H_Dispatcher_H
#define H_Dispatcher_H

#include "Pixy.h"
#include "Renderer.h"
#include "Launcher.h"

namespace Pixy {

	/*! \class Dispatcher
	 *	\brief
	 *	The Dispatcher is the interface the user will interact with. Dispatchers are
	 *  injected with events that represent the stage of the patching process,
	 *  and handle all input.
	 */
	class Dispatcher {

	public:

		virtual ~Dispatcher();

		static Dispatcher* getSingletonPtr();
		static Dispatcher& getSingleton();

    /*! \brief
     *  This event is triggered when the Downloader is unable to retrieve the
     *  patch list from all the registered hosts.
     */
		void injectUnableToConnect( void );

		/*! \brief
     *  Indicates that the Patcher has started validating the application's
     *  version by parsing the patch list.
     */
		void injectValidateStarted( void );

		/*! \brief
     *  Contains whether the application is out of date or up to date.
     *
     *  Event properties:
     *    NeedUpdate: "Yes" or "No"
     */
		void injectValidateComplete( bool inNeedUpdate );

		/*! \brief
     *  Triggered when the Patcher has started updating to a certain version.
     *
     *  Event properties:
     *    Version: the version being updated to
     */
		void injectPatchStarted( Version& inTargetVersion );

		/*! \brief
     *  Triggered while the Downloader is fetching files from the remote
     *  patch server indicating how much it has completed.
     *
     *  Event properties:
     *    Progress: string containing the progress % (should be cast to int)
     */
		void injectPatchProgress( int inPercent );

		/*! \brief
     *  Triggered if any error occurs within the Patcher or the Downloader
     *  while patching a certain repository.
     *
     *  TODO: add error code or reason
     */
		void injectPatchFailed( std::string inMsg );

		/*! \brief
     *  Indicates that the Patcher has finished processing a single repository.
     *
     *  Event properties:
     *    Version: the version to which the application was updated
     */
		void injectPatchComplete( Version& inCurrentVersion );

		/*! \brief
     *  Triggered when the Patcher is done processing all the repositories
     *  and has successfully updated the application to the latest version.
     */
		void injectApplicationPatched( void );

		/* \brief
		 *  Dispatchers are the first components to be initialised. Allocate all
		 *  the resources you need here, along with any initialisation code for
		 *  the engine used.
		 *
		 *  \warn
		 *  Other components such as the InputManager are NOT initialised at this
		 *  stage yet, so make sure you do not reference them here. If you need to,
		 *  see deferredSetup() below.
		 */
		virtual bool setup();

		/* \brief
		 *  Called after all components are initialized, such as the InputManager,
		 *  Downloader, and Patcher.
		 */
		virtual bool deferredSetup();

		/* \brief
		 *  Clean up here any resources you've allocated. This will be called
		 *  on the application's shutdown.
		 */
 		virtual bool cleanup();

    /* ------
     * Event handlers: override these handlers in child to handle them. Each
     * of these events represents a stage of the process.
     * ------ */



	protected:
    log4cpp::Category* mLog;
    Renderer* mRenderer;

	private:
    static Dispatcher *__instance;

    Dispatcher();
		Dispatcher(const Dispatcher& src);
		Dispatcher& operator=(const Dispatcher& rhs);
	};
}
#endif
