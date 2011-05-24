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
 
#ifndef H_Downloader_H
#define H_Downloader_H

#include "Pixy.h"
#include "Patcher.h"
#include "Repository.h"
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
#include "md5.hpp"

namespace Pixy {

class Downloader {

  public:
    ~Downloader( void );
    
	  static Downloader* getSingletonPtr();
	  static Downloader& getSingleton();

	  void fetchPatchList(std::string out);
	  
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
	  bool 
	  fetchRepository(Repository* inRepo, 
	                  int nrRetries = 3, 
	                  void (*callback)(int) = NULL);
	
	protected:
	  // grabs a file from url using HTTP and dumps it to out
	  void fetchFile(std::string url, std::string out);
	  
	  std::string mHost, mPort;
	  
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
