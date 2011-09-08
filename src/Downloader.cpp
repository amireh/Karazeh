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
  std::string Downloader::Fetcher::last_content_length = "";
  double Downloader::Fetcher::delta = 0;

	Downloader::Downloader() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Downloader");
		mLog->infoStream() << "firing up";

    mDownloadedBytes = 0;
    mCurrentRepo = 0;

    // find out which OS we're running
#if PIXY_PLATFORM == PIXY_PLATFORM_WIN32
    mOSPrefix = "win32";
#elif PIXY_PLATFORM == PIXY_PLATFORM_APPLE
    mOSPrefix = "mac";
#else
    mOSPrefix = "linux";
#endif

    // Build hosts list
    using boost::filesystem::exists;
    using boost::filesystem::path;

    path lPath = path(Launcher::getSingleton().getBinPath()) / path(PIXY_MIRRORS_RESOURCE);
    if (exists(lPath)) {
      std::ifstream lMirrorsFile(lPath.c_str());
      if (lMirrorsFile.is_open() && lMirrorsFile.good()) {
        std::string lMirror;
        while (lMirrorsFile.good()) {
          getline(lMirrorsFile, lMirror);
          // TODO: some sanity checks on the mirror url
          mHosts.push_back(lMirror + mOSPrefix + "/");
        }
        mLog->infoStream() << "registered " << mHosts.size() << " patch mirrors";
        lMirrorsFile.close();
      }
    }

    // add hardcoded/fallback hosts
		//mHosts.push_back("http://127.0.0.1/");
		mHosts.push_back("http://www.vertigo-game.com/patches/" + mOSPrefix + "/");

    mLog->infoStream() << "registered " << mHosts.size() << " patch hosts";
    mPatchScriptName = "patch.txt";
    mPatchLogName = "latest_changes.txt";

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

	void Downloader::_fetchPatchScript(std::string out) {

	  using boost::filesystem::exists;
	  using boost::filesystem::is_directory;
	  using boost::filesystem::create_directory;
	  using boost::filesystem::path;

	  mLog->debugStream() << "fetching patch script";

	  // first of all, we need to prepare the filesystem; directories etc
	  if (exists(path(Launcher::getSingleton().getTempPath()))) {
	    // if it exists but not a directory... something is wrong, remove it
      if (!is_directory(path(Launcher::getSingleton().getTempPath())))
        // TODO: do something here, throw an exception maybe
        mLog->warnStream() << "TMP path exists but is not a directory";

    } else {
      // it doesn't exist, let's create our directory
      mLog->infoStream() << "TMP path doesn't exist, creating it";
      create_directory(path(Launcher::getSingleton().getTempPath()));
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
	
	void Downloader::_fetchPatchLog(std::string out)
	{
    assert(mActiveHost);
    std::string url = (*mActiveHost) + mPatchLogName;
    mLog->infoStream() << "fetching patch log from " << url;
    
    bool res = mFetcher(url, out, 3);
    if (!res) {
      throw BadPatchURL("could not find an active host for patch log!");
    }
	}

  pbigint_t
  Downloader::calcRepositorySize(Repository* inRepo)
  {

    std::cout << "Calculating repositority size " << inRepo->getVersion().toNumber() << "\n";
    int i;
    pbigint_t lSize = 0;
    //int nrEntries = inRepo->getEntries().size();
    std::string lUrl;
    std::vector<PatchEntry*> lEntries;

    CURL *curl = curl_easy_init();

    if (inRepo->isArchived()) {
      PatchEntry* lArchive = inRepo->getArchive();
      lUrl = *mActiveHost + lArchive->Remote;

      lArchive->Filesize = mFetcher.fileSize(lUrl, curl);
      lSize = lArchive->Filesize;

      curl_easy_cleanup(curl);

      inRepo->setSize(lSize);
      return lSize;
    };

    for (i = 0; i < 2; ++i) {
      PATCHOP op = (i == 0) ? P_CREATE : P_MODIFY;

      lEntries = inRepo->getEntries(op);
      PatchEntry* lEntry = 0;
      while (!lEntries.empty()) {
        lEntry = lEntries.back();

        // build up target URL
        lUrl = *mActiveHost + lEntry->Remote;

        lEntry->Filesize = mFetcher.fileSize(lUrl, curl);
        lSize += lEntry->Filesize;

        //std::cout << "Found file with size: " << lEntry->Filesize << "\n";

        lEntry = 0;
        lEntries.pop_back();
      };

    }
    curl_easy_cleanup(curl);

    inRepo->setSize(lSize);

    std::cout << "Repositority " << inRepo->getVersion().toNumber() << " is " << lSize << " bytes large\n";
    return lSize;
  }

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
    this->calcRepositorySize(inRepo);
    Launcher::getSingleton().getRenderer()->injectPatchSize(inRepo->getSize());

    bool realProgress = false;
#ifdef KARAZEH_REAL_PROGRESS
    mCurrentRepo = inRepo;
    mDownloadedBytes = 0;
    realProgress = true;
#else
    int idx = 0;
    int nrEntries = inRepo->getEntries().size();
#endif

    // download all CREATE and MODIFY entries' remote files
    int i;

    std::string url;
    std::vector<PatchEntry*> lEntries;
    CURL *curl = curl_easy_init();

    // if it's an archived repository, fetch the archive only
    if (inRepo->isArchived()) {

      PatchEntry *lArchive = inRepo->getArchive();
      bool downloaded = false;
      if (exists(lArchive->Temp)) {
        // compare the checksum to verify the file
        MD5 md5;
        if (md5.digestFile((char*)lArchive->Temp.c_str()) == lArchive->Checksum) {
          // no need to download
          downloaded = true;
          mLog->debugStream() << "Archive already downloaded, skipping";
        } else
          mLog->infoStream() << "MD5 mismatch, re-downloading " << lArchive->Temp;
      }

      // TODO: re-try in case of download failure
      std::string lUrl = *mActiveHost + lArchive->Remote;
      mLog->infoStream() << "downloading archive " << lUrl;
      if (!downloaded) {
        //fetchFile(url, lEntry->Temp);
        mFetcher(lUrl, lArchive->Temp, nrRetries, realProgress, curl);
      }

      curl_easy_cleanup(curl);
      return true;
    };

    // it's not archived, get all the files
    for (i = 0; i < 2; ++i) {
      PATCHOP op = (i == 0) ? P_CREATE : P_MODIFY;

      lEntries = inRepo->getEntries(op);
      PatchEntry* lEntry = 0;
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
          mFetcher(url, lEntry->Temp, nrRetries, realProgress, curl);
        }

#ifndef KARAZEH_REAL_PROGRESS
        ++idx;
        Launcher::getSingleton().getRenderer()->injectPatchProgress(idx * 1.0f / (nrEntries-1) * 100.0f);
#endif

        lEntry = 0;
        lEntries.pop_back();
      };

    }
    curl_easy_cleanup(curl);

    return true;
  };
/*
  void Downloader::fetchFile(std::string url, std::string out)
  {
    mFetcher(url,out);
    //boost::thread mWorker(mFetcher, url, out);
    //mWorker.join();
  }
*/
  size_t Downloader::Fetcher::write_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
  {
    return fwrite(ptr, size, nmemb, stream);
  }

  size_t Downloader::Fetcher::read_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
  {
    return fread(ptr, size, nmemb, stream);
  }

  void Downloader::_updateDownloadProgress(double inStep) {
    mDownloadedBytes += inStep;

    Launcher::getSingleton().getRenderer()->
      injectPatchProgress(mDownloadedBytes * 1.0f / mCurrentRepo->getSize() * 100.0f);
  }

  int Downloader::Fetcher::progress_func(void* something,
                       double t, /* dltotal */
                       double d, /* dlnow */
                       double ultotal,
                       double ulnow)
  {
    Downloader::getSingleton()._updateDownloadProgress(d-delta);

    //printf("%f / %f (%g %%), delta = %f\n", d, t, d*100.0/t, d - delta);
    delta = d;

    return 0;
  }

  bool Downloader::Fetcher::operator()(std::string url, std::string out, int retries, bool withProgress, CURL* curl) {

    //std::cout << "fetching URL: " << url << " => " << out << "...\n";

    //CURL *curl;
    CURLcode res;
    FILE *outfile;

    bool external = true; // external handle? do we have to clean up?

    if (!curl) {
      curl = curl_easy_init();
      external = false;
    }

    if(curl)
    {
      // TODO: make sure the directory exists and create it if it doesn't
      outfile = fopen(out.c_str(), "w");
      if (!outfile) {
        if (!external)
          curl_easy_cleanup(curl);
        //std::cout << "ERROR! Couldn't open file for writing: " << out;
        return false;
      }

      delta = 0;

      curl_easy_reset(curl);

      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Fetcher::write_func);
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, Fetcher::read_func);
      if (withProgress) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Fetcher::progress_func);
      }
      curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
      //curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, Bar);

      // curl_easy_perform() returns 0 on OK
      do {
        //std::cout << "CURL: trying...\n";
        res = curl_easy_perform(curl);
        //std::cout << "CURL: query result = " << res;
      } while ((res != 0) && (--retries) > 0);

      fclose(outfile);
      if (res != 0) {
        remove(out.c_str());
      }

      /* always cleanup */
      if (!external)
        curl_easy_cleanup(curl);

      //std::cout << "done fetching file\n";

      return (res == 0);
    }

    return true;
  }

  size_t Downloader::Fetcher::handle_headers(void *ptr, size_t size, size_t nmemb, void *stream)
  {
    using std::string;

    //std::cout << "Header str: " << (char*)ptr << "\n";
    //const boost::regex content_length_regex("Content-Length: [0-9]{1,}"); // Regex do video do youtube...
    //const boost::regex content_length_remove_regex("Content-Length: ");
    int numbytes = size*nmemb;
    // The data is not null-terminated, so get the last character, and replace
    // it with '\0'.
    char lastchar = *((char *) ptr + numbytes - 1);
    *((char *) ptr + numbytes - 1) = '\0';
    //string last_char = ((char *)ptr);

    string header((char*)ptr);
    if (header.find("Content-Length") != string::npos)
      last_content_length = header.substr(16); // "Content-Length: "

    /*if (regex_search(last_char,content_length_regex) == 1) // Se for 1, foi retornado sim para o match
    {
      last_content_length = regex_replace(last_char, content_length_remove_regex, nothing);
    }*/

    return size*nmemb;
  }

  pbigint_t Downloader::Fetcher::fileSize(std::string url, CURL* curl) {
    bool external = (curl != 0);
    if (!curl) {
      curl = curl_easy_init();
    }
    curl_easy_reset(curl);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, handle_headers);

    CURLcode res = curl_easy_perform(curl);

    if (!external)
      curl_easy_cleanup(curl);

    return atoll(last_content_length.c_str());
  };
};
