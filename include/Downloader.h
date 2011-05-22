#ifndef H_Downloader_H
#define H_Downloader_H

#include "Pixy.h"

namespace Pixy {

class Downloader {

  public:
    ~Downloader( void );
    
	  static Downloader* getSingletonPtr();
	  static Downloader& getSingleton();
	
	  bool validateVersion();
	  void downloadUpdate(void (*callback)(int) = NULL);
	  
  private:
	  Downloader();
	  Downloader(const Downloader&) {}
	  Downloader& operator=(const Downloader&);
	  
    static Downloader *__instance;
    log4cpp::Category* mLog;
};
  
};

#endif
