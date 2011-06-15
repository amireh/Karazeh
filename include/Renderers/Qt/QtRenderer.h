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

#ifndef H_QtRenderer_H
#define H_QtRenderer_H

#include "Renderer.h"
#include "ui_karazeh.h"
#include "ui_dialog.h"
#include <QObject>
#include <QtGui/QApplication>
#include <QtGui/QtGui>

namespace Pixy {

	/*	\class QtRenderer
	 *	\brief
	 *
	 */
	class QtRenderer : public QObject, public Renderer {
    Q_OBJECT
	public:
	  QtRenderer();
		virtual ~QtRenderer();

		/* \brief
		 *
		 */
		virtual bool setup(int argc, char** argv);

		virtual void go(int argc, char** argv);

		/* \brief
		 *
		 */
 		virtual bool cleanup();

  public slots:
    void onGuiStart();

    /* handle signals */
		void handleUnableToConnect( void );
		void handleValidateStarted( void );
		void handleValidateComplete( bool inNeedUpdate, Version const& inTargetVersion );
    void handlePatchAccepted();
    void handlePatchStarted( Version const& inTargetVersion );
    void handlePatchSize( pbigint_t inBytes );
		void handlePatchProgress( float inPercent );
		void handlePatchFailed( QString inMsg, Version const& inTargetVersion );
		void handlePatchComplete( Version const& inCurrentVersion );
		void handleApplicationPatched( Version const& inCurrentVersion );
    void handleLaunchApplication();

  signals:
    void guiStarted();
		void emitUnableToConnect( void );
		void emitValidateStarted( void );
		void emitValidateComplete( bool inNeedUpdate, Version const& inTargetVersion );
		void emitPatchStarted( Version const& inTargetVersion );
    void emitPatchSize( pbigint_t inBytes );
		void emitPatchProgress( float inPercent );
		void emitPatchFailed( QString inMsg, Version const& inTargetVersion );
		void emitPatchComplete( Version const& inCurrentVersion );
		void emitApplicationPatched( Version const& inCurrentVersion );

  public:
    /* emit signals */
		virtual void injectUnableToConnect( void );
		virtual void injectValidateStarted( void );
		virtual void injectValidateComplete( bool inNeedUpdate, Version const& inTargetVersion );
		virtual void injectPatchStarted( Version const& inTargetVersion );
    virtual void injectPatchSize( pbigint_t inBytes );
		virtual void injectPatchProgress( float inPercent );
		virtual void injectPatchFailed( std::string inMsg, Version const& inTargetVersion );
		virtual void injectPatchComplete( Version const& inCurrentVersion );
		virtual void injectApplicationPatched( Version const& inCurrentVersion );

		QWidget* getWindow();
		void setupWidgets();

	protected:

    Ui::MainWindow mUI;
    Ui::Dialog mDlgUI;

		static void startQt(int argc, char** argv);

		/*virtual bool evtUnableToConnect(Event* inEvt);
		virtual bool evtValidateStarted(Event* inEvt);
		virtual bool evtValidateComplete(Event* inEvt);
		virtual bool evtPatchStarted(Event* inEvt);
		virtual bool evtPatchProgress(Event* inEvt);
		virtual bool evtPatchFailed(Event* inEvt);
		virtual bool evtPatchComplete(Event* inEvt);
		virtual bool evtApplicationPatched(Event* inEvt);*/

		int margc;
		char **margv;

		QApplication* mApp;
		QDialog *mYesNoDlg;
		QVBoxLayout *mLayout;
		QPushButton *mQuitButton;
		QMainWindow *mWindow;
	private:
		QtRenderer(const QtRenderer& src);
		QtRenderer& operator=(const QtRenderer& rhs);
	};
}
#endif
