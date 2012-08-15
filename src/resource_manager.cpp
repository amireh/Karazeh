/*
 *  Copyright (c) 2011-2012 Ahmad Amireh <kandie@mxvt.net>
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

#include "karazeh/resource_manager.hpp"

namespace kzh {

  path_t  resource_manager::root_path_, 
          resource_manager::tmp_path_, 
          resource_manager::bin_path_;

  resource_manager::resource_manager(string_t const& host)
  : logger("resource_mgr"),
    host_(host) {
  }

  resource_manager::~resource_manager() {
  }

  string_t const& resource_manager::host_address() const {
    return host_;
  }

  void resource_manager::resolve_paths(path_t root) {
    if (!root_path_.empty())
      return;

    bool overridden = false;

    // locate the binary and build its path
    #if KZH_PLATFORM == KZH_PLATFORM_LINUX
      // Linux:
      debug() << "Platform: Linux";

      /** use binreloc and boost::filesystem to build up our paths */
      int brres = br_init(0);
      if (brres == 0) {
        error() << "binreloc could not be initialised";
        throw internal_error("Unable to resolve paths! binreloc could not be initialized");
      }

      char *p = br_find_exe_dir(".");
      bin_path_ = path_t(p).make_preferred();
      free(p);

    #elif KZH_PLATFORM == KZH_PLATFORM_APPLE
      // OS X:
      debug() << "Platform: OS X";
      // use NSBundlePath() to build up our paths
      bin_path_ = path_t(Utility::macBundlePath() + "/Contents/MacOS").make_preferred();

      // create the folders if they doesn't exist
      boost::filesystem::create_directory(path_t(bin_path_ + "/../Resources").make_preferred());
      boost::filesystem::create_directory(path_t(bin_path_ + "/../Resources/.kzh").make_preferred());
      boost::filesystem::create_directory(path_t(bin_path_ + "/../Resources/.kzh/tmp").make_preferred());

      tmp_path_ = (bin_path_.remove_leaf() / path_t("/Resources/.kzh/tmp").make_preferred());
      
      overridden = true;
    #else
      // Windows:
      debug() << "Platform: Windows";
      // use GetModuleFileName() and boost::filesystem to build up our paths on Windows
      TCHAR szPath[MAX_PATH];

      if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
      {
        error() << "Unable to resolve path: " << GetLastError();;
        throw internal_error("Unable to resolve paths! GetModuleFileName() failed. See the log for the error.");
      }

      bin_path_ = path_t(string_t(szPath)).remove_filename().make_preferred();
    #endif

    // root is KZH_DISTANCE_FROM_ROOT directories up from the binary's
    root_path_ = bin_path_;
    for (int i=0; i < KZH_DISTANCE_FROM_ROOT; ++i) {
      root_path_ = root_path_.remove_leaf();
    }

    root_path_ = root_path_.make_preferred();
    
    if (!root.empty()) {
      root_path_ = root;
    }

    if (!overridden) {
      boost::filesystem::create_directory(root_path_ / ".kzh");
      boost::filesystem::create_directory(root_path_ / ".kzh" / "tmp");
      tmp_path_ = (root_path_ / ".kzh" / "tmp").make_preferred();
    }

    debug() << "Root path: " <<  root_path_;
    debug() << "Binary path: " <<  bin_path_;
    debug() << "Temp path: " <<  tmp_path_;
  }

  path_t const& resource_manager::root_path() {
    return root_path_;
  }
  path_t const& resource_manager::tmp_path() {
    return tmp_path_;
  }
  path_t const& resource_manager::bin_path() {
    return bin_path_;
  }

  bool resource_manager::load_file(std::ifstream &fs, string_t& out_buf)
  {
    if (!fs.is_open() || !fs.good()) return false;

    while (fs.good())
      out_buf.push_back(fs.get());

    out_buf.erase(out_buf.size()-1,1);

    return true;
  }

  bool resource_manager::load_file(string_t const& path, string_t& out_buf) {
    std::ifstream fs(path.c_str());
    return load_file(fs, out_buf);
  }

  // bool resource_manager::get_resource(string_t const& resource_path, string_t& out_buf)
  // {
  //   bool is_remote = (resource_path.substr(0,4) == "http");
  //   if (is_remote)
  //     return get_remote(resource_path, out_buf);

  //   using boost::filesystem::exists;

  //   path_t p(resource_path);

  //   if (!exists(p))
  //   {
  //     p = root_path_ / p;
  //     if (!exists(p)) {
  //       error() << "local resource does not exist: " << resource_path;
  //       return false;
  //     }
  //   }

  //   std::ifstream fs(p.string());
  //   // open the file and load it into the string
  //   if (!fs.is_open() || !fs.good())
  //   {
  //     error() << "local resource is not readable; " << resource_path;
  //     return false;
  //   }

  //   bool result = load_file(fs, out_buf);
  //   fs.close();

  //   return result;
  // }

  bool resource_manager::is_readable(path_t const& resource) const {
    return is_readable(path_t(resource).make_preferred().string());
  }
  bool resource_manager::is_readable(string_t const& resource) const {
    using boost::filesystem::path;
    using boost::filesystem::exists;
    using boost::filesystem::is_regular_file;

    path fp(resource);
    if (exists(fp)) {
      std::ifstream fs(resource.c_str());
      bool readable = fs.is_open() && fs.good();
      fs.close();
      return is_regular_file(fp) && readable;
    }

    return false;
   }

  static size_t on_curl_data(char *buffer, size_t size, size_t nmemb, void *userdata)
  {
    download_t *dl = (download_t*)userdata;

    size_t realsize = size * nmemb;
    (*dl->buf) += string_t(buffer);

    return realsize;
  }

  bool resource_manager::get_remote(string_t const& in_uri, string_t& out_buf)
  {
    string_t uri(in_uri);
    if (uri.find("http://") == std::string::npos) {
      uri = string_t(host_ + in_uri);
    }
    
    CURL* curl_ = curl_easy_init();
    CURLcode curlrc_;

    if (!curl_) {
      error() << "unable to resolve URL " << uri << ", aborting remote download request";
      return false;
    }

    download_t *dl = new download_t();
    dl->buf = &out_buf;
    dl->status = false;
    dl->uri = uri;

    char curlerr[CURL_ERROR_SIZE];

    curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, curlerr);
    curl_easy_setopt(curl_, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &on_curl_data);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, dl);

    curlrc_ = curl_easy_perform(curl_);
    if (curlrc_ != 0) {
      error() << "a CURL error was encountered; " << curlrc_ << " => " << curlerr;
      delete dl;

      curl_easy_cleanup(curl_);

      return false;
    }

    long http_rc = 0;
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_rc);

    if (http_rc != 200) {
      error() << "remote server error, HTTP code: " << http_rc << ", download failed";
      delete dl;

      curl_easy_cleanup(curl_);

      return false;
    }

    /* always cleanup */
    curl_easy_cleanup(curl_);
    delete dl;
    return true;
  }

}