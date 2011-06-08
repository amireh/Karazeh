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

		/* \brief
		 *
		 */
		virtual bool deferredSetup();

		virtual void go();

		/* \brief
		 *
		 */
 		virtual bool cleanup();

  public slots:
    /* handle signals */
		void handleUnableToConnect( void );
		void handleValidateStarted( void );
		void handleValidateComplete( bool inNeedUpdate, const Version& inTargetVersion );
    void handlePatchAccepted();
    void handlePatchStarted( const Version& inTargetVersion );
		void handlePatchProgress( int inPercent );
		void handlePatchFailed( std::string inMsg, const Version& inTargetVersion );
		void handlePatchComplete( const Version& inCurrentVersion );
		void handleApplicationPatched( const Version& inCurrentVersion );
    void handleLaunchApplication();

  signals:
		void emitUnableToConnect( void );
		void emitValidateStarted( void );
		void emitValidateComplete( bool inNeedUpdate, const Version& inTargetVersion );
		void emitPatchStarted( const Version& inTargetVersion );
		void emitPatchProgress( int inPercent );
		void emitPatchFailed( std::string inMsg, const Version& inTargetVersion );
		void emitPatchComplete( const Version& inCurrentVersion );
		void emitApplicationPatched( const Version& inCurrentVersion );

  public:
    /* emit signals */
		virtual void injectUnableToConnect( void );
		virtual void injectValidateStarted( void );
		virtual void injectValidateComplete( bool inNeedUpdate, const Version& inTargetVersion );
		virtual void injectPatchStarted( const Version& inTargetVersion );
		virtual void injectPatchProgress( int inPercent );
		virtual void injectPatchFailed( std::string inMsg, const Version& inTargetVersion );
		virtual void injectPatchComplete( const Version& inCurrentVersion );
		virtual void injectApplicationPatched( const Version& inCurrentVersion );

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
