#include "InputManager.h"

namespace Pixy
{

	InputManager *InputManager::mInputManager=NULL;

	InputManager::InputManager( void ) :
		mMouse( 0 ),
		mKeyboard( 0 ),
		mInputSystem( 0 ) {
	}

	InputManager::~InputManager( void ) {
		if( mInputSystem ) {
		    if( mMouse ) {
		        mInputSystem->destroyInputObject( mMouse );
		        mMouse = 0;
		    }

		    if( mKeyboard ) {
		        mInputSystem->destroyInputObject( mKeyboard );
		        mKeyboard = 0;
		    }

		    OIS::InputManager::destroyInputSystem(mInputSystem);

		    mInputSystem = 0;

		    // Clear Listeners
		    mKeyListeners.clear();
		    mMouseListeners.clear();
		}
	}

	void InputManager::initialise( Ogre::RenderWindow *renderWindow ) {
		if( !mInputSystem ) {
		    // Setup basic variables
		    OIS::ParamList paramList;
		    size_t windowHnd = 0;
		    std::ostringstream windowHndStr;

		    // Get window handle
	#if defined OIS_WIN32_PLATFORM
		    renderWindow->getCustomAttribute( "WINDOW", &windowHnd );
	#elif defined OIS_LINUX_PLATFORM
		    renderWindow->getCustomAttribute( "WINDOW", &windowHnd );
	#endif

		    // Fill parameter list
		    windowHndStr << windowHnd;
		    paramList.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );

		    // Create inputsystem
		    mInputSystem = OIS::InputManager::createInputSystem( paramList );

			mKeyboard = static_cast<OIS::Keyboard*>( mInputSystem->createInputObject( OIS::OISKeyboard, true ) );
			mKeyboard->setEventCallback( this );

			mMouse = static_cast<OIS::Mouse*>( mInputSystem->createInputObject( OIS::OISMouse, true ) );
			mMouse->setEventCallback( this );

			// Get window size
			unsigned int width, height, depth;
			int left, top;
			renderWindow->getMetrics( width, height, depth, left, top );

			// Set mouse region
			this->setWindowExtents( width, height );

		}
	}

	void InputManager::capture( void ) {
		// Need to capture / update each device every frame
		if( mMouse ) {
		    mMouse->capture();
		}

		if( mKeyboard ) {
		    mKeyboard->capture();
		}

	}

	void InputManager::addKeyListener( OIS::KeyListener *keyListener, const std::string& instanceName ) {
		if( mKeyboard ) {
		    // Check for duplicate items
		    itKeyListener = mKeyListeners.find( instanceName );
		    if( itKeyListener == mKeyListeners.end() ) {
		        mKeyListeners[ instanceName ] = keyListener;
		    }
		    else {
		        // Duplicate Item
		    }
		}
	}

	void InputManager::addMouseListener( OIS::MouseListener *mouseListener, const std::string& instanceName ) {
		if( mMouse ) {
		    // Check for duplicate items
		    itMouseListener = mMouseListeners.find( instanceName );
		    if( itMouseListener == mMouseListeners.end() ) {
		        mMouseListeners[ instanceName ] = mouseListener;
		    }
		    else {
		        // Duplicate Item
		    }
		}
	}

	void InputManager::removeKeyListener( const std::string& instanceName ) {
		// Check if item exists
		itKeyListener = mKeyListeners.find( instanceName );
		if( itKeyListener != mKeyListeners.end() ) {
		    mKeyListeners.erase( itKeyListener );
		}
		else {
		    // Doesn't Exist
		}
	}

	void InputManager::removeMouseListener( const std::string& instanceName ) {
		// Check if item exists
		itMouseListener = mMouseListeners.find( instanceName );
		if( itMouseListener != mMouseListeners.end() ) {
		    mMouseListeners.erase( itMouseListener );
		}
		else {
		    // Doesn't Exist
		}
	}

	void InputManager::removeKeyListener( OIS::KeyListener *keyListener ) {
		itKeyListener    = mKeyListeners.begin();
		itKeyListenerEnd = mKeyListeners.end();
		for(; itKeyListener != itKeyListenerEnd; ++itKeyListener ) {
		    if( itKeyListener->second == keyListener ) {
		        mKeyListeners.erase( itKeyListener );
		        break;
		    }
		}
	}

	void InputManager::removeMouseListener( OIS::MouseListener *mouseListener ) {
		itMouseListener    = mMouseListeners.begin();
		itMouseListenerEnd = mMouseListeners.end();
		for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
		    if( itMouseListener->second == mouseListener ) {
		        mMouseListeners.erase( itMouseListener );
		        break;
		    }
		}
	}

	void InputManager::removeAllListeners( void ) {
		mKeyListeners.clear();
		mMouseListeners.clear();
	}

	void InputManager::removeAllKeyListeners( void ) {
		mKeyListeners.clear();
	}

	void InputManager::removeAllMouseListeners( void ) {
		mMouseListeners.clear();
	}

	void InputManager::setWindowExtents( int width, int height ) {
		// Set mouse region (if window resizes, we should alter this to reflect as well)
		const OIS::MouseState &mouseState = mMouse->getMouseState();
		mouseState.width  = width;
		mouseState.height = height;
	}

	OIS::Mouse* InputManager::getMouse( void ) {
		return mMouse;
	}

	OIS::Keyboard* InputManager::getKeyboard( void ) {
		return mKeyboard;
	}

	bool InputManager::keyPressed( const OIS::KeyEvent &e ) {
		itKeyListener    = mKeyListeners.begin();
		itKeyListenerEnd = mKeyListeners.end();
		for(; itKeyListener != itKeyListenerEnd; ++itKeyListener ) {
		    itKeyListener->second->keyPressed( e );
		}

		return true;
	}

	bool InputManager::keyReleased( const OIS::KeyEvent &e ) {
		itKeyListener    = mKeyListeners.begin();
		itKeyListenerEnd = mKeyListeners.end();
		for(; itKeyListener != itKeyListenerEnd; ++itKeyListener ) {
		    itKeyListener->second->keyReleased( e );
		}

		return true;
	}

	bool InputManager::mouseMoved( const OIS::MouseEvent &e ) {
		itMouseListener    = mMouseListeners.begin();
		itMouseListenerEnd = mMouseListeners.end();
		for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
		    itMouseListener->second->mouseMoved( e );
		}

		return true;
	}

	bool InputManager::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		itMouseListener    = mMouseListeners.begin();
		itMouseListenerEnd = mMouseListeners.end();
		for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
		    itMouseListener->second->mousePressed( e, id );
		}

		return true;
	}

	bool InputManager::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) {
		itMouseListener    = mMouseListeners.begin();
		itMouseListenerEnd = mMouseListeners.end();
		for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
		    itMouseListener->second->mouseReleased( e, id );
		}

		return true;
	}

	InputManager* InputManager::getSingletonPtr( void ) {
		if( !mInputManager ) {
		    mInputManager = new InputManager();
		}

		return mInputManager;
	}

} // end of namespace Pixy
