#include "Launcher.h"

using namespace Pixy;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT ) {
#else
	int main( int argc, char **argv ) {
#endif
		
		Launcher *Launcher = Launcher::getSingletonPtr();
		
		try {
			Launcher->go();
		}
		catch ( Ogre::Exception& ex ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, ex.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL );
#else
			std::cerr << "An exception has occured: " << ex.getFullDescription();
#endif
		} catch (std::exception& e) {
			Ogre::String errMsg = e.what();
		}
		
		delete Launcher;
		
		return 0;
	}
	
