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

#ifndef H_GTK3Renderer_H
#define H_GTK3Renderer_H

#include "Renderer.h"
#include "PatchVersion.h"
//#include "Renderers/GTK3/GTK3Window.h"
#include <gtkmm.h>
#include <gtkmm/main.h>
#include <glibmm/dispatcher.h>
#include <sigc++/sigc++.h>

namespace Pixy {

	/*	\class GTK3Renderer
	 *	\brief
	 *
	 */
	class GTK3Renderer : public Renderer, public sigc::trackable {

	public:
	  GTK3Renderer();
		virtual ~GTK3Renderer();

		virtual bool setup(int argc, char** argv);
    virtual void go(int argc, char** argv);
 		virtual bool cleanup();

    virtual void injectUnableToConnect( void );
		virtual void injectValidateStarted( void );
		virtual void injectValidateComplete( bool inNeedUpdate, Version const& inTargetVersion );
		virtual void injectPatchStarted( Version const& inTargetVersion );
    virtual void injectPatchSize(pbigint_t inBytes);
		virtual void injectPatchProgress( float inPercent );
		virtual void injectPatchFailed( std::string inMsg, Version const& inTargetVersion );
		virtual void injectPatchComplete( Version const& inCurrentVersion );
		virtual void injectApplicationPatched( Version const& inCurrentVersion );

	protected:

    bool fShuttingDown;

    // Signal handlers:
		void handleUnableToConnect(  );
		void handleValidateStarted(  );
		void handleValidateComplete(  );
		void handlePatchStarted(  );
    void handlePatchSize( );
		void handlePatchProgress(  );
		void handlePatchFailed(  );
		void handlePatchComplete(  );
		void handleApplicationPatched(  );

    void onGuiStart();
    void doReqPatch();
    void doLaunch();

    void infoDialog(std::string inCaption, std::string inMsg);
    void errorDialog(std::string inCaption, std::string inMsg);
    bool promptDialog(std::string inCaption, std::string inMsg);



    // since we can't pass additional args using the dispatcher's slot, and our
    // args are fairly few, there's no need to create an asynchronous queue
    // we'll just hard-bind them here
    bool fNeedUpdate;
    Version mCurrentVersion, mTargetVersion;
    pbigint_t mPatchSize;
    float mProgress;
    std::string mFailMsg;

    Gtk::TextView *txtLatestChanges;
    Gtk::Label *txtStatus;
    Gtk::Button *btnLaunch;
    Gtk::ProgressBar *progressBar;
    Gtk::Window *window;
    Glib::RefPtr<Gtk::Builder> builder;

    Glib::Dispatcher d_guiStarted;
    Glib::Dispatcher d_unableToConnect;
    Glib::Dispatcher d_validateStarted;
    Glib::Dispatcher d_validateComplete;
    Glib::Dispatcher d_patchStarted;
    Glib::Dispatcher d_patchSize;
    Glib::Dispatcher d_patchProgress;
    Glib::Dispatcher d_patchFailed;
    Glib::Dispatcher d_patchComplete;
    Glib::Dispatcher d_applicationPatched;

	private:
		GTK3Renderer(const GTK3Renderer& src);
		GTK3Renderer& operator=(const GTK3Renderer& rhs);
	};
}
#endif
