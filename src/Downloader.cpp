#include "Downloader.h"

namespace Pixy {
	Downloader* Downloader::__instance;
	
	Downloader::Downloader() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Downloader");
		mLog->infoStream() << "firing up";
	}
	
	Downloader::~Downloader() {
		
		mLog->infoStream() << "shutting down";
		
		if (mLog)
		  delete mLog;
	}
	
	Downloader* Downloader::getSingletonPtr() {
		if( !__instance ) {
		    __instance = new Downloader();
		}
		
		return __instance;
	}
	
	Downloader& Downloader::getSingleton() {
		return *getSingletonPtr();
	}
	
	bool Downloader::validateVersion() {
	
	  return true;
	};
	
  void Downloader::downloadUpdate(void (*callback)(int)) {
  
    (*callback)(0);
  };
	
};
