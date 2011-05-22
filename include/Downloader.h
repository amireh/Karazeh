#ifndef H_Downloader_H
#define H_Downloader_H

#include "Pixy.h"
#include "Patcher.h"
#include "Utility.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include <curl/types.h>  
#include <curl/easy.h>
#include <boost/thread.hpp>

namespace Pixy {

class Downloader {

  public:
    ~Downloader( void );
    
	  static Downloader* getSingletonPtr();
	  static Downloader& getSingleton();
	
	  bool validateVersion();
	  void downloadUpdate(void (*callback)(int) = NULL);
	
	protected:
	  // grabs a file from url using HTTP and dumps it to out
	  void fetchFile(std::string url, std::string out);
	  
  private:
	  Downloader();
	  Downloader(const Downloader&) {}
	  Downloader& operator=(const Downloader&);
	  
    static Downloader *__instance;
    log4cpp::Category* mLog;
    //boost::thread *mWorker;
    
    class Fetcher {
      public:
        Fetcher();
        Fetcher(const Fetcher& src) {
          clone(src);
        };
        Fetcher& operator=(const Fetcher& rhs) {
          if (this == &rhs)
            return (*this);
          
          clone(rhs);
          
          return (*this);
        };
        virtual ~Fetcher();
        
        void operator()(std::string url, std::string out);
        static size_t write_func(void *ptr, size_t size, size_t nmemb, FILE *stream);
        static size_t read_func(void *ptr, size_t size, size_t nmemb, FILE *stream);
        static int progress_func(void* something, double t, double d, double ultotal, double ulnow);
        
      private:
        void clone(const Fetcher& src) {
        
        };
    };
    
    Fetcher mFetcher;
};
  
};

#endif
