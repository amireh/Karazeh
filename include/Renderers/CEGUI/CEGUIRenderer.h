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

#ifndef H_CEGUIRenderer_H
#define H_CEGUIRenderer_H

#include "Renderer.h"
#include <CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

namespace Pixy {
	
	/*	\class CEGUIRenderer
	 *	\brief
	 *	
	 */
	class CEGUIRenderer :  public Renderer
	{
		
	public:
	  CEGUIRenderer();
		virtual ~CEGUIRenderer();
		
		/* \brief
		 *
		 */
		virtual bool setup();
		
		/* \brief
		 *
		 */
		virtual bool deferredSetup();

		/* \brief
		 *
		 */		
		virtual void update(unsigned long lTimeElapsed);
		
		/* \brief
		 *
		 */
 		virtual bool cleanup();

		virtual void getWindowHandle(size_t *windowHnd);
		virtual void getWindowExtents(int *width, int *height);
				
    virtual void injectError(PATCHERROR errorCode, std::string errorMsg);
    virtual void injectNotice(PATCHNOTICE noticeCode, std::string noticeMsg);
    virtual bool injectPrompt(std::string promptMsg);
    virtual void injectStatus(std::string statusMsg);
    virtual void injectProgress(int progress);

		virtual bool keyPressed( const OIS::KeyEvent &e );
		virtual bool keyReleased( const OIS::KeyEvent &e );		
		virtual bool mouseMoved( const OIS::MouseEvent &e );
		virtual bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		virtual bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
		
	protected:
		void setupResources(std::string inPath);
    
    CEGUI::System *mSystem;
    CEGUI::WindowManager *mWindowMgr;
    CEGUI::SchemeManager *mSchemeMgr;
    CEGUI::FontManager *mFontMgr;
        
	private:
		CEGUIRenderer(const CEGUIRenderer& src);
		CEGUIRenderer& operator=(const CEGUIRenderer& rhs);
	};
}
#endif
