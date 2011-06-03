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

#ifndef H_InputManager_H
#define H_InputManager_H

// OIS
#include <OIS/OIS.h>
#include <OIS/OISMouse.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISInputManager.h>

namespace Pixy
{
    /*! \class InputManager
     *  \brief
     *  OIS Key & Mouse Listener, input handler and dispatcher for GameManager.
     */
	class InputManager : public OIS::KeyListener, OIS::MouseListener {
		public:
			virtual ~InputManager( void );

			//! Initializes OIS input system and sets the current Renderer's
			//  window as the event capturing context.
			void initialise( size_t windowHnd, int width, int height );
		
			//! Grabs the input & dispatches events
			void capture( void );

			void addKeyListener( OIS::KeyListener *keyListener, const std::string& instanceName );
			void addMouseListener( OIS::MouseListener *mouseListener, const std::string& instanceName );

			void removeKeyListener( const std::string& instanceName );
			void removeMouseListener( const std::string& instanceName );

			void removeKeyListener( OIS::KeyListener *keyListener );
			void removeMouseListener( OIS::MouseListener *mouseListener );

			void removeAllListeners( void );
			void removeAllKeyListeners( void );
			void removeAllMouseListeners( void );

			void setWindowExtents( int width, int height );

			OIS::Mouse*    getMouse( void );
			OIS::Keyboard* getKeyboard( void );

			static InputManager* getSingletonPtr( void );
		private:
			InputManager( void );
			InputManager( const InputManager& ) { }
			InputManager & operator = ( const InputManager& );

			bool keyPressed( const OIS::KeyEvent &e );
			bool keyReleased( const OIS::KeyEvent &e );

			bool mouseMoved( const OIS::MouseEvent &e );
			bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
			bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );

			OIS::InputManager *mInputSystem;
			OIS::Mouse        *mMouse;
			OIS::Keyboard     *mKeyboard;

			std::map<std::string, OIS::KeyListener*> mKeyListeners;
			std::map<std::string, OIS::MouseListener*> mMouseListeners;


			std::map<std::string, OIS::KeyListener*>::iterator itKeyListener;
			std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;


			std::map<std::string, OIS::KeyListener*>::iterator itKeyListenerEnd;
			std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;


			static InputManager *mInputManager;
	};
} // end of namespace
#endif

