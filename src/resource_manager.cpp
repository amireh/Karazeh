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

  resource_manager::resource_manager()
  : logger("resource_mgr") {

  }

  resource_manager::~resource_manager() {

  }

  bool resource_manager::load_file(std::ifstream &fs, string_t& out_buf)
  {
    if (!fs.is_open() || !fs.good()) return false;

    while (fs.good())
      out_buf.push_back(fs.get());

    out_buf.erase(out_buf.size()-1,1);

    return true;
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

  static size_t on_curl_data(char *buffer, size_t size, size_t nmemb, void *userdata)
  {
    download_t *dl = (download_t*)userdata;

    size_t realsize = size * nmemb;
    (*dl->buf) += string_t(buffer);

    return realsize;
  }

  bool resource_manager::get_remote(string_t const& uri, string_t& out_buf)
  {
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