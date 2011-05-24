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

namespace Pixy {
	Downloader* Downloader::__instance;
	
	Downloader::Downloader() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Downloader");
		mLog->infoStream() << "firing up";
		
		curl_global_init(CURL_GLOBAL_ALL);
		mHost = "http://127.0.0.1/";
		mPort = "80";
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
	
	void Downloader::fetchPatchList(std::string out) {
	  using boost::filesystem::exists;
	  using boost::filesystem::is_directory;
	  using boost::filesystem::create_directory;
	  using boost::filesystem::path;	
	  // first of all, we need to prepare the filesystem; directories etc
	  if (exists(path(PROJECT_TEMP_DIR))) {
	    // if it exists but not a directory... something is wrong, remove it
      if (!is_directory(path(PROJECT_TEMP_DIR)))
        mLog->warnStream() << "TMP path exists but is not a directory";
      
    } else {
      // it doesn't exist, let's create our directory
      create_directory(path(PROJECT_TEMP_DIR));
      mLog->infoStream() << "TMP path doesn't exist, creating it";
    }
    
	  std::string url = mHost + "/patchlist.txt";
	  // get the patch list
	  try {
      fetchFile(url, out);
    } catch (std::exception &e) {
      mLog->errorStream() << "could not download patch list! error: " << e.what();
      throw new BadPatchURL("could not download patch list from '" + url + "'");
    }
    
	};
	


  bool 
  Downloader::fetchRepository(Repository *inRepo, 
                             int nrRetries, 
                             void (*callback)(int))
  {
	  using boost::filesystem::exists;
	  using boost::filesystem::is_directory;
	  using boost::filesystem::create_directory;
	    
    mLog->infoStream() << "downloading patch files";
    Launcher::getSingleton().evtFetchStarted();
    
    // download all CREATE and MODIFY entries' remote files
    int i;
    for (i = 0; i < 2; ++i) {
      PATCHOP op = (i == 0) ? CREATE : MODIFY;
      
      std::vector<PatchEntry*> lEntries = inRepo->getEntries(op);
      PatchEntry* lEntry = 0;
      std::string url;
      while (!lEntries.empty()) {
        lEntry = lEntries.back();
        
        // build up target URL
        url = mHost + lEntry->Remote;

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
        if (!downloaded)
          fetchFile(url, lEntry->Temp);
        
        lEntry = 0;
        lEntries.pop_back();
      };
      
    }
        
    if (callback)
      (*callback)(0);
      
    return true;
  };
	
  void Downloader::fetchFile(std::string url, std::string out)
  {
    boost::thread mWorker(mFetcher, url, out);
    mWorker.join();
  }
  
  Downloader::Fetcher::Fetcher() {
    //std::cout << "fetcher created\n";
  }	
  
  Downloader::Fetcher::~Fetcher() {
    //std::cout << "fetcher destroyed\n";
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
    printf("%f / %f (%g %%)\n", d, t, d*100.0/t); 
    
    return 0;
  }
    
  void Downloader::Fetcher::operator()(std::string url, std::string out) {
    std::cout << "fetching URL: " << url << " => " << out << "...\n";
    //printf("fetching URL: %s => %s\n", url, out);
    //sleep(10000);
    //return;
    CURL *curl;
    CURLcode res;
    FILE *outfile;
   
    curl = curl_easy_init();
    if(curl)
    {
      // TODO: make sure the directory exists and create it if it doesn't
      outfile = fopen(out.c_str(), "w");
   
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Fetcher::write_func);
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, Fetcher::read_func);
      curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
      curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Fetcher::progress_func);
      //curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, Bar);
   
      res = curl_easy_perform(curl);
   
      fclose(outfile);
      /* always cleanup */ 
      curl_easy_cleanup(curl);
      
      std::cout << "done fetching file\n";
    }
  }
};
