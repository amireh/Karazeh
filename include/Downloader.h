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
#include <boost/filesystem.hpp>

namespace Pixy {

class Downloader {

  public:
    ~Downloader( void );
    
	  static Downloader* getSingletonPtr();
	  static Downloader& getSingleton();
	
	  /*! 
	   * \brief
	   *  Validates current application version against the latest version
	   *  tracked in the remote server patch list. 
	   *
	   *  If there's a version mismatch,
	   *  it tracks all the remote changes, and registers them with the Patcher
	   *  in case the Launcher decides to update the app.
	   *
	   * Return:
	   *  true if version is up to date, false if a patch is due
	   *
	   * \warn
	   *  MUST be called before fetchPatchData()
	   */
	  bool validateVersion();
	  
	  /*!
	   * \brief
	   *  downloads all the files and diffs needed by every version's patch
	   *
	   * \remark
	   *  When a file is downloaded, its MD5 chuksum is calculated and compared
	   *  against the original remote one, in case of a mismatch, the file is
	   *  re-downloaded up to nrRetries times, if still invalid, an exception will
	   *  be thrown of type BadPatchFile which contains the respective PatchEntry.
	   *
	   * TODO:
	   *  Use some kind of persistent volume to track which files have been 
	   *  already downloaded in case of interruption so the list won't have to
	   *  be re-downloaded entirely.
	   *
	   * Return:
	   *  true if all files were successfully downloaded, false otherwise
	   *
	   * \warn
	   *  MUST be called before the Patcher is ordered to process
	   */
	  bool fetchPatchData(int nrRetries = 3, void (*callback)(int) = NULL);
	
	protected:
	  // grabs a file from url using HTTP and dumps it to out
	  void fetchFile(std::string url, std::string out);
	  
  private:
	  Downloader();
	  Downloader(const Downloader&) {}
	  Downloader& operator=(const Downloader&);
	  
    static Downloader *__instance;
    log4cpp::Category* mLog;
    
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
