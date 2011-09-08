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
#include "PixyUtility.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
//#include <curl/types.h>
#include <curl/easy.h>
#include <boost/filesystem.hpp>
#include "md5.hpp"

namespace Pixy {

/*! \class Downloader "Downloader.h"
 * \brief
 * Uses CURL to download patch data.
 */
class Downloader {

  public:
    ~Downloader( void );

	  static Downloader* getSingletonPtr();
	  static Downloader& getSingleton();

    /*! \brief
     *  Attempts to retrieve the patch list from any of the registered
     *  patch servers.
     *
     *  The patch list is stored in PROJECT_TEMP_DIR, which is created if it
     *  does not exist. Further, this method could throw a BadPatchURL exception
     *  in the case where something goes wrong while retrieving the list.
     *
     *  \note
     *  This is called internally in the Patcher::validate() routine.
     */
	  void _fetchPatchScript(std::string out);

    /*! \brief
     *  Attempts to retrieve the patch log (latest changes) from the 
     *  active host.
     *
     *  \note
     *  This is called internally in the Patcher::validate() routine.
     *
     *  \warn
     *  MUST be called aftrer _fetchPatchScript && mActiveHost != 0
     */
	  void _fetchPatchLog(std::string out);
	  
	  /*!
	   * \brief
	   *  Downloads all the files and diffs needed by every repository only if
	   *  they have not already been downloaded. This method transmits the
	   *  PatchProgress which injects Renderers with the download progress.
	   *
	   * \remark
	   *  When a file is downloaded, its MD5 checksum is calculated and compared
	   *  against the original remote one, in case of a mismatch, the file is
	   *  re-downloaded up to nrRetries times, if still invalid, an exception will
	   *  be thrown of type BadPatchFile which contains the respective PatchEntry.
	   *
	   * TODO:
	   *  Use some kind of persistent volume to track which files have been
	   *  already downloaded in case of interruption so the list won't have to
	   *  be re-downloaded entirely.
	   *
	   * \return
	   *  true if all files were successfully downloaded, false otherwise
	   *
	   * \warn
	   *  MUST be called before the Patcher is ordered to process
	   */
	  bool _fetchRepository(Repository* inRepo, int nrRetries = 3);

    void _updateDownloadProgress(double inStep);

	protected:
    Repository *mCurrentRepo;
    pbigint_t mDownloadedBytes;

	  //! grabs a file from url using libcurl and dumps it to out
	  void fetchFile(std::string url, std::string out);

    pbigint_t calcRepositorySize(Repository* inRepo);

	  std::list<std::string> mHosts;
	  std::string* mActiveHost;
	  std::string mPatchScriptName, mPatchLogName;
	  
	  /* Karazeh expects the patch server repository to host three
	   * directories for each OS; 
	   *  "mac" for Apple patches
	   *  "win32" for Windows patches
	   *  "linux" for Linux patches
	   * mOSPrefix will be set to one of the above and will be appended
	   * to the ActiveHost or mirror
	   */
    std::string mOSPrefix;

  private:
	  Downloader();
	  Downloader(const Downloader&) {}
	  Downloader& operator=(const Downloader&);

    static Downloader *__instance;
    log4cpp::Category* mLog;

    /*! \class Fetcher
     *  \brief
     *  The Fetcher performs all the libcurl requests and does the downloading.
     *  Fetcher instances are spawned in boost::threads.
     */
    class Fetcher {
      public:
        Fetcher() { };
        Fetcher(const Fetcher& src) {
          clone(src);
        };
        Fetcher& operator=(const Fetcher& rhs) {
          if (this == &rhs)
            return (*this);

          clone(rhs);

          return (*this);
        };
        ~Fetcher() { };

        bool operator()(std::string url, std::string out, int retries=0, bool withProgress = false, CURL* curl=0);

        static size_t write_func(void *ptr, size_t size, size_t nmemb, FILE *stream);
        static size_t read_func(void *ptr, size_t size, size_t nmemb, FILE *stream);
        static size_t handle_headers(void *ptr, size_t size, size_t nmemb, void *stream);
        static int progress_func(void* something, double t, double d, double ultotal, double ulnow);
        static double delta;
        pbigint_t fileSize(std::string url, CURL* curl=0);

        static std::string last_content_length;
      private:
        void clone(const Fetcher& src) { };
    };

    Fetcher mFetcher;
};

};

#endif
