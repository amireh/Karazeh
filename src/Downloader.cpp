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
 
#include "Downloader.h"
#include "Launcher.h"
#include <stdio.h>
#include <stdlib.h>

namespace Pixy {
	Downloader* Downloader::__instance;
	
	Downloader::Downloader() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Downloader");
		mLog->infoStream() << "firing up";
		
		//mHosts.push_back("http://127.0.0.1/");
		mHosts.push_back("http://www.vertigo-game.com/patches/");
    mPatchScriptName = "patch.txt";
		
		mActiveHost = 0;
		
		curl_global_init(CURL_GLOBAL_ALL);
  }
	
	Downloader::~Downloader() {
		
		curl_global_cleanup();
		mActiveHost = 0;
		mHosts.clear();
		
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
	
	void Downloader::_fetchPatchList(std::string out) {
	  using boost::filesystem::exists;
	  using boost::filesystem::is_directory;
	  using boost::filesystem::create_directory;
	  using boost::filesystem::path;	
	  
	  mLog->debugStream() << "fetching patch list";
	  
	  // first of all, we need to prepare the filesystem; directories etc
	  if (exists(path(PROJECT_TEMP_DIR))) {
	    // if it exists but not a directory... something is wrong, remove it
      if (!is_directory(path(PROJECT_TEMP_DIR)))
        // TODO: do something here, throw an exception maybe
        mLog->warnStream() << "TMP path exists but is not a directory";
      
    } else {
      // it doesn't exist, let's create our directory
      mLog->infoStream() << "TMP path doesn't exist, creating it";
      create_directory(path(PROJECT_TEMP_DIR));
    }
    
    mLog->infoStream() << "resolving patch server...";
    // find an active host
    std::list<std::string>::const_iterator _host = mHosts.begin();
    while (!mActiveHost) {
      mActiveHost = (std::string*)&(*_host);
      bool res;
      //try {
        std::string url = (*mActiveHost) + mPatchScriptName;
        mLog->debugStream() << "server: " << url;
        res = mFetcher(url, out, 3);
        //fetchFile(url, out);
      //} catch (BadRequest& e) {
      if (!res) {
        mActiveHost = 0;
        if ((++_host) == mHosts.end())
          throw BadPatchURL("could not find an active host");
      }
    }
    
	};
	

  bool 
  Downloader::_fetchRepository(Repository *inRepo, 
                             int nrRetries)
  {
	  using boost::filesystem::exists;
	  using boost::filesystem::is_directory;
	  using boost::filesystem::create_directory;
	  
	  if (nrRetries < 0)
	    nrRetries = 0;
	  
    mLog->infoStream() << "downloading patch files";
    Launcher::getSingleton().evtFetchStarted();
    
    // download all CREATE and MODIFY entries' remote files
    int i;
    int nrEntries = inRepo->getEntries().size();
    int idx = 0;
    for (i = 0; i < 2; ++i) {
      PATCHOP op = (i == 0) ? CREATE : MODIFY;
      
      std::vector<PatchEntry*> lEntries = inRepo->getEntries(op);
      PatchEntry* lEntry = 0;
      std::string url;
      while (!lEntries.empty()) {
        lEntry = lEntries.back();
        
        // build up target URL
        url = *mActiveHost + lEntry->Remote;

        // TODO: check if file has been downloaded before
        bool downloaded = false;
        if (exists(lEntry->Temp)) { 
          // compare the checksum to verify the file
          MD5 md5;
          if (md5.digestFile((char*)lEntry->Temp.c_str()) == lEntry->Checksum) {
            // no need to download
            downloaded = true;
            mLog->debugStream() << "File already downloaded, skipping";
          } else
            mLog->infoStream() << "MD5 mismatch, re-downloading " << lEntry->Temp;
        }
        
        // TODO: re-try in case of download failure
        if (!downloaded) {
          //fetchFile(url, lEntry->Temp);
          mFetcher(url, lEntry->Temp, nrRetries);
        }
        
        ++idx;
        
        Event* lEvt = EventManager::getSingleton().createEvt("PatchProgress");
        lEvt->setProperty("Progress", Utility::stringify(idx * 1.0f / nrEntries * 100.0f));
        EventManager::getSingleton().hook(lEvt);
        lEvt = 0;
        
        lEntry = 0;
        lEntries.pop_back();
      };
      
    }
    
      
    return true;
  };
	
  void Downloader::fetchFile(std::string url, std::string out)
  {
    boost::thread mWorker(mFetcher, url, out);
    mWorker.join();
  } 
  
  size_t Downloader::Fetcher::write_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
  {
    return fwrite(ptr, size, nmemb, stream);
  }
   
  size_t Downloader::Fetcher::read_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
  {
    return fread(ptr, size, nmemb, stream);
  }
   
  int Downloader::Fetcher::progress_func(void* something,
                       double t, /* dltotal */ 
                       double d, /* dlnow */ 
                       double ultotal,
                       double ulnow)
  {
    //printf("%f / %f (%g %%)\n", d, t, d*100.0/t); 
    
    return 0;
  }
    
  bool Downloader::Fetcher::operator()(std::string url, std::string out, int retries) {
    std::cout << "fetching URL: " << url << " => " << out << "...\n";

    CURL *curl;
    CURLcode res;
    FILE *outfile;
   
    curl = curl_easy_init();
    if(curl)
    {
      // TODO: make sure the directory exists and create it if it doesn't
      outfile = fopen(out.c_str(), "w");
      if (!outfile) {
        curl_easy_cleanup(curl);
        std::cout << "ERROR! Couldn't open file for writing: " << out;
        return false;
      }
      
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Fetcher::write_func);
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, Fetcher::read_func);
      curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
      curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Fetcher::progress_func);
      curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
      //curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, Bar);
   
      // curl_easy_perform() returns 0 on OK
      do {
        std::cout << "CURL: trying...\n";
        res = curl_easy_perform(curl);
        std::cout << "CURL: query result = " << res; 
      } while ((res != 0) && (--retries) > 0);

      fclose(outfile);
      if (res != 0) {
        remove(out.c_str());
      }
      
      /* always cleanup */ 
      curl_easy_cleanup(curl);

      std::cout << "done fetching file\n";
      
      return (res == 0);      
    }
  }
};
