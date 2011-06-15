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
//#include "Renderers/GTK3/GTK3Window.h"
#include <gtkmm.h>
#include <gtkmm/main.h>

namespace Pixy {

	/*	\class GTK3Renderer
	 *	\brief
	 *
	 */
	class GTK3Renderer : public Renderer {

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
		virtual void injectPatchProgress( int inPercent );
		virtual void injectPatchFailed( std::string inMsg, Version const& inTargetVersion );
		virtual void injectPatchComplete( Version const& inCurrentVersion );
		virtual void injectApplicationPatched( Version const& inCurrentVersion );

	protected:

    bool fShuttingDown;

    void infoDialog(std::string inCaption, std::string inMsg);
    void errorDialog(std::string inCaption, std::string inMsg);
    bool promptDialog(std::string inCaption, std::string inMsg);

    //Signal handlers:
    //void evtButtonInfoClicked();
    //void evtButtonQuestionClicked();

    //Child widgets:
    //Gtk::VButtonBox m_ButtonBox;
    //Gtk::Button m_Button_Info, m_Button_Question;

    Gtk::TextView *txtLatestChanges;
    Gtk::Label *txtStatus;
    Gtk::Button *btnLaunch;
    Gtk::ProgressBar *progressBar;
    Gtk::Window *window;
    Glib::RefPtr<Gtk::Builder> builder;

	private:
		GTK3Renderer(const GTK3Renderer& src);
		GTK3Renderer& operator=(const GTK3Renderer& rhs);
	};
}
#endif
