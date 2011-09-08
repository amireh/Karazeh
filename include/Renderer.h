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
#include <string>
#include "Repository.h"
#include "PatchVersion.h"

namespace log4cpp { class Category; }
namespace Pixy {

	/*! \class Renderer
	 * \brief
	 *	The Renderer is the interface the user will interact with. Renderers are
	 *  injected on each significant stage of the patching process to either report
   *  to the user or ask for input.
   *
   * \remarks
   * The Renderer is the component responsible for the main loop of the application,
   * this is done so for compatibility with GUI toolkits that own the main loop like
   * Qt and wxWidgets. See Renderer::go() for more details.
	 */
	class Renderer {

	public:
	  inline Renderer() { mLog = 0; fSetup = false; mName = "Renderer"; };
		inline virtual ~Renderer() { mLog = 0; fSetup = false; mName = ""; };

		/*! \brief
		 *  Renderer names are only used for logging.
		 */
		inline virtual std::string& getName() { return mName; };

		/*! \brief
		 *  Renderers are the first components to be initialised. Allocate all
		 *  the resources you need here, along with any initialisation code for
		 *  the engine used.
		 *
		 */
		virtual bool setup(int argc, char** argv)=0;

		/*! \brief
		 *  Your main loop should reside in this method, you can fire up your chosen
     * GUI toolkit's main loop or write your own.
     *
     * Note that since the patching is done in a separate thread, Renderers will
     * be injected from that thread which is *not* the main one, ie the GUI one.
     * Most toolkits do not support GUI operations from outside the main thread,
     * that's why you have to make sure that you pass off the event in an appropriate
     * way to your toolkit to handle. See QtRenderer for an example.
		 */
		virtual void go(int argc, char** argv)=0;

		/*! \brief
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
		
    virtual void injectShowPatchLog( std::string inLogPath )=0;

		/*! \brief
     *  Contains whether the application is out of date or up to date. In the
     * case that an update is due, inTargetVersion holds the version to which
     * an update is available.
     */
		virtual void injectValidateComplete( bool inNeedUpdate, Version const& inTargetVersion )=0;

		/*! \brief
     *  Triggered when the Patcher has started updating to a certain version.
     *
     * \param inTargetVersion represents the version being updating to.
     */
		virtual void injectPatchStarted( Version const& inTargetVersion )=0;

    virtual void injectPatchSize( pbigint_t inBytes )=0;

		/*! \brief
     *  Triggered while the Downloader is fetching files from the remote
     *  patch server indicating how much it has completed.
     */
		virtual void injectPatchProgress( float inPercent )=0;

		/*! \brief
     *  Triggered if any error occurs within the Patcher or the Downloader
     *  while patching a certain repository.
     */
		virtual void injectPatchFailed( std::string inMsg, Version const& inTargetVersion )=0;

		/*! \brief
     *  Indicates that the Patcher has finished processing a single repository.
     *
     *  \param inCurrentVersion: the version to which the application was updated
     */
		virtual void injectPatchComplete( Version const& inCurrentVersion )=0;

		/*! \brief
     *  Triggered when the Patcher is done processing all the repositories
     *  and has successfully updated the application to the latest version.
     */
		virtual void injectApplicationPatched( Version const& inCurrentVersion )=0;

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
