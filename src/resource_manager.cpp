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
          resource_manager::bin_path_,
          resource_manager::cache_path_;

  resource_manager::resource_manager(string_t const& host)
  : logger("resource_mgr"),
    host_(host),
    nr_retries_(2)
  {
  }

  resource_manager::~resource_manager() {
  }

  string_t const& resource_manager::host_address() const {
    return host_;
  }
  
  void resource_manager::rebase(const string_t& host) {
    host_ = host;
  }

  void resource_manager::set_retry_count(int n) {
    nr_retries_ = n;
  }

  int resource_manager::retry_count() const {
    return nr_retries_;
  }

  void resource_manager::resolve_paths(path_t root) {
    if (!root_path_.empty())
      return;

    bool overridden = false;

    // locate the binary and build its path
    #if KZH_PLATFORM == KZH_PLATFORM_LINUX
      // Linux:
      debug() << "Platform: Linux";

      /** use binreloc and fs to build up our paths */
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
      fs::create_directory(path_t(bin_path_ + "/../Resources").make_preferred());
      fs::create_directory(path_t(bin_path_ + "/../Resources/.kzh").make_preferred());
      fs::create_directory(path_t(bin_path_ + "/../Resources/.kzh/cache").make_preferred());

      cache_path_ = (bin_path_.remove_leaf() / path_t("/Resources/.kzh/cache").make_preferred());
      
      overridden = true;
    #else
      // Windows:
      debug() << "Platform: Windows";
      // use GetModuleFileName() and fs to build up our paths on Windows
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
      fs::create_directory(root_path_ / ".kzh");
      cache_path_ = (root_path_ / ".kzh" / "cache").make_preferred();
    }

    debug() << "Root path: " <<  root_path_;
    debug() << "Binary path: " <<  bin_path_;
    debug() << "Cache path: " <<  cache_path_;
  }

  path_t const& resource_manager::root_path() {
    return root_path_;
  }
  path_t const& resource_manager::bin_path() {
    return bin_path_;
  }
  path_t const& resource_manager::cache_path() {
    return cache_path_;
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

  bool resource_manager::load_file(path_t const& path, string_t& out_buf) {
    std::ifstream fs(path.string().c_str());
    return load_file(fs, out_buf);
  }

  bool resource_manager::is_readable(string_t const& resource) const {
    namespace fs = fs;
    using fs::path;
    using fs::exists;
    using fs::is_regular_file;
    using fs::is_directory;

    path fp(resource);
    if (exists(fp)) {
      if (is_directory(fp)) {
        try {
          for(fs::directory_iterator it(fp); it != fs::directory_iterator(); ++it) {
            break;
          }

          return true;
        } catch (fs::filesystem_error& e) {
          error() << e.what();
          return false;
        }
      } else {
        std::ifstream fs(resource.c_str());
        bool readable = fs.is_open() && fs.good();
        fs.close();
        return is_regular_file(fp) && readable;
      }
    }

    return false;
  }
  bool resource_manager::is_readable(path_t const& resource) const {
    return is_readable(path_t(resource).make_preferred().string());
  }
  
  bool resource_manager::is_writable(string_t const& resource) const {
    using fs::path;
    using fs::exists;
    using fs::is_regular_file;

    try {
      path fp(resource);
      if (exists(fp)) {

        if (is_directory(fp)) {
          return is_writable(fp / "__karazeh_internal_directory_check__");
        }
        
        // it already exists, make sure we don't overwrite it
        std::ofstream fs(resource.c_str(), std::ios_base::app);
        bool writable = fs.is_open() && fs.good() && !fs.fail();
        fs.close();

        return is_regular_file(fp) && writable;
      } else {
        
        // try creating a file and write to it
        std::ofstream fs(resource.c_str(), std::ios_base::app);
        bool writable = fs.is_open() && fs.good() && !fs.fail();
        fs << "This was generated automatically by Karazeh and should have been deleted.";
        fs.close();

        if (exists(fp)) {
          // delete the file
          fs::remove(fp);
        }

        return writable;
      }
    } catch (...) {
      // something bad happened, it is most likely unwritable
      return false;
    }

    return false;
  }
  bool resource_manager::is_writable(path_t const& resource) const {
    return is_writable(path_t(resource).make_preferred().string());
  }

  bool resource_manager::create_directory(path_t const& in_path) {
    try {
      fs::create_directories(in_path);
    } catch (fs::filesystem_error &e) {
      error() 
        << "Unable to create directory chain @ " << in_path
        << ". Cause: " << e.what();
      
      return false;
    } catch (std::exception &e) {
      error() << "Unknown error while creating directory chain @ " << in_path;
      error() << "Possible cause: " << e.what();

      return false;
    }

    return true;
  }

  static size_t on_curl_data(char *buffer, size_t size, size_t nmemb, void *userdata)
  {
    download_t *dl = (download_t*)userdata;

    size_t realsize = size * nmemb;

    dl->size += realsize;

    if (kzh::settings::is_enabled("-v")) {
      logger l("cURL"); l.debug() << "received " << realsize << " bytes";
    }
    if (dl->to_file) {
      dl->stream.write(buffer, realsize);
    } else {
      (*dl->buf) += string_t(buffer, realsize);
    }

    return realsize;
  }

  bool resource_manager::get_remote(string_t const& in_uri, download_t* dl, bool assume_ownership)
  {
    string_t uri(in_uri);
    if (in_uri.find("http://") == std::string::npos) {
      uri = string_t(host_ + in_uri);
      dl->uri = uri;
    }

    CURL* curl_ = curl_easy_init();
    CURLcode curlrc_;

    if (!curl_) {
      error() << "unable to resolve URL " << uri << ", aborting remote download request";
      return false;
    }

    char curlerr[CURL_ERROR_SIZE];

    info() << "Downloading " << dl->uri;

    curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, curlerr);
    curl_easy_setopt(curl_, CURLOPT_URL, dl->uri.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &on_curl_data);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, dl);

    curlrc_ = curl_easy_perform(curl_);
    if (curlrc_ != 0) {
      error() << "a CURL error was encountered; " << curlrc_ << " => " << curlerr;
      
      if (assume_ownership)
        delete dl;

      curl_easy_cleanup(curl_);

      return false;
    }

    long http_rc = 0;
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_rc);

    if (http_rc != 200) {
      error() << "remote server error, HTTP code: " << http_rc << ", download failed";
      
      if (assume_ownership)
        delete dl;

      curl_easy_cleanup(curl_);

      return false;
    }

    /* always cleanup */
    curl_easy_cleanup(curl_);
    
    if (assume_ownership)
      delete dl;

    return true;
  }
  
  bool resource_manager::get_remote(string_t const& in_uri, string_t& out_buf)
  {
    download_t *dl = new download_t(std::cout);
    dl->buf = &out_buf;
    dl->uri = in_uri;

    return get_remote(in_uri, dl);
  }

  bool resource_manager::get_remote(string_t const& in_uri, std::ostream& out_stream)
  {
    download_t *dl = new download_t(out_stream);
    dl->to_file = true;
    dl->uri = in_uri;

    return get_remote(in_uri, dl);
  }

  bool resource_manager::get_remote(
    string_t const& in_uri, 
    path_t const& path, 
    string_t const& checksum,
    uint64_t expected_size,
    int* const nr_retries)
  {
    // TODO: verify a hasher instance is registered
    
    for (int i = 0; i < nr_retries_ + 1; ++i) {
      std::ofstream fp(path.string().c_str(), std::ios_base::trunc | std::ios_base::binary);

      if (!fp.is_open() || !fp.good()) {
        error() << "Download destination is un-writable: " << path;
        return false;
      }

      // we'll build the download_t manually because we need to reference its size
      // otherwise we could use the std::ostream& version of get_remote()
      download_t dl(fp);
      dl.to_file = true;
      dl.uri = in_uri;
      dl.retry_no = i;
      (*nr_retries) = i;

      if (get_remote(in_uri, &dl, false)) {
        
        fp.close();

        std::ifstream fh(path.string().c_str());
        if (!fh.is_open() || !fh.good()) { // this really shouldn't happen, but oh well
          return false;
        }

        // validate integrity
        hasher::digest_rc rc = hasher::instance()->hex_digest(fh);
        
        fh.close();

        if (expected_size > 0 && expected_size == dl.size && rc == checksum) {
          return true;
        }
        else if (expected_size == 0 && rc == checksum) {
          return true;
        } else {
          notice()
            << "Downloaded file integrity mismatch: "
            <<  rc.digest << " vs " << checksum
            << " (got " << dl.size << " out of " << expected_size << " expected bytes)";

          if (settings::is_enabled("-v")) {
            std::ifstream fh(path.string().c_str());
            string_t buf;
            load_file(fh, buf);
            debug() << "Contents (" << buf.size() << "): " << buf;
            fh.close();
          }
        }
      } else {
        fp.close();
      }

      fp.open(path.string().c_str(), std::ios_base::trunc);

      notice() << "Retry #" << i+1;
    }

    return false;
  }

  bool resource_manager::make_executable(path_t const& p) {
    using namespace fs;

    try {
      permissions(p, owner_all | group_exe | others_exe);
    } catch (fs::filesystem_error &e) {
      error() << "Unable to modify permissions of file: " << p;
      return false;
    }

    return true;
  }

  uint64_t resource_manager::stat_filesize(std::ifstream& in) {
    in.seekg(0,std::ifstream::end);
    uint64_t size = in.tellg();
    in.seekg(0);

    return size;
  }
  uint64_t resource_manager::stat_filesize(path_t const& p) {
    std::ifstream fp(p.string().c_str(), std::ios_base::binary);
    if (!fp.is_open() || !fp.good())
      return 0;

    uint64_t size = stat_filesize(fp);

    fp.close();

    return size;
  }

  bool resource_manager::get_resource(string_t const& path, string_t& out) {
    if (path.find("http") == 0) {
      return get_remote(path, out);
    } else {
      return load_file(path, out);
    }
  }
}