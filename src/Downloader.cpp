#include "Downloader.h"
#include "Launcher.h"

namespace Pixy {
	Downloader* Downloader::__instance;
	
	Downloader::Downloader() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Downloader");
		mLog->infoStream() << "firing up";
		
		curl_global_init(CURL_GLOBAL_ALL);
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
	  using std::string;
	  using std::ifstream;
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
	  
	  string tmp = string(PROJECT_TEMP_DIR) + "patchlist.txt";
	  string url = "http://www.amireh.net/external/patch.txt";
	  string ourVersion = Launcher::getSingleton().getVersion();
	  string targetVersion = "";
	  
	  // get the patch list
	  try {
      fetchFile(url, tmp);
    } catch (std::exception &e) {
      mLog->errorStream() << "could not download patch list! error: " << e.what();
      throw new BadPatchURL("could not download patch list from '" + url + "'");
    }

    mLog->debugStream() << "patch list received, parsing it now";    
    /*
     * Check the version of the latest patch.
     * If it's newer than our version, we need to download the patch files.
     */
    string line;
    ifstream patchList(tmp.c_str());

    if (!patchList.is_open()) {
      mLog->errorStream() << "could not read patch list!";
      throw new BadFileStream("unable to read patch list!");
    }
    
    bool needPatch = false;
    // parse version sig
    if (patchList.good()) {
      getline(patchList,targetVersion);
      if (strcmp(targetVersion.c_str(), ourVersion.c_str()) != 0) {
        mLog->infoStream() << "Version mismatch, patch is due from " << ourVersion << " to " << targetVersion;
        needPatch = true;
      } else {
        mLog->infoStream() << "Application is up to date.";
      }
    }
    
    if (!needPatch) // nothing to do here
      return true;
    
    /* we need to locate our current version in the patch list, and then
     * parse the list upwards from there
     */
    bool located = false;
    while ( !located && patchList.good() )
    {
      getline(patchList,line);
      
      if (line == "") {
        continue;
      } else if (line == "-") {
        mLog->debugStream() << "skipping separator: " << line;
        continue;
      } else if (line.substr(0, 7) == "VERSION") {
        if (strcmp(line.c_str(), ourVersion.c_str()) == 0) {
          // we're done parsing
          located = true;
          mLog->debugStream() << "found our version, no more files to parse";
          break;
        } else {
          mLog->debugStream() << "skipping version signature: " << line;
          continue;
        }
      }
      
      std::cout << "Line: " << line << "\n";
      fflush(stdout);
      
      std::vector<std::string> elements = Utility::split(line.c_str(), ' ');
      if (elements.size() < 2) {
        mLog->errorStream() << "malformed line: '" << line << "', skipping";
        continue;
      }
      
      PATCHOP op;
      if (elements[0] == "C")
        op = CREATE;
      else if (elements[0] == "D")
        op = DELETE;
      else if (elements[0] == "M")
        op = MODIFY;
      else {
        mLog->errorStream() << "undefined operation symbol: " << elements[0];
        continue;
      }
      
      // add the entries to our Patcher's list for processing
      switch(op) {
        case CREATE:
        case MODIFY:
          Patcher::getSingleton().registerEntry(elements[1], elements[2], op);
          break;
        case DELETE:
          Patcher::getSingleton().registerEntry(elements[1], "", op);
          break;
      }
      
    }
    
    patchList.close();
    
    if (!located) {
      mLog->warnStream() << "possible file or local version corruption: could not locate our version in patch list";
      
      return false;
    }
    
	  return true;
	};

  bool Downloader::fetchPatchData(int nrRetries, void (*callback)(int)) {
  
    (*callback)(0);
    return true;
  };
	
  void Downloader::fetchFile(std::string url, std::string out)
  {
    boost::thread mWorker(mFetcher, url, out);
    mWorker.join();
  }
  
  Downloader::Fetcher::Fetcher() {
    std::cout << "fetcher created\n";
  }	
  
  Downloader::Fetcher::~Fetcher() {
    std::cout << "fetcher destroyed\n";
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
    }
  }
};
