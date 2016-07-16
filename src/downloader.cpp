/**
 * karazeh -- the library for patching software
 *
 * Copyright (C) 2011-2016 by Ahmad Amireh <ahmad@amireh.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "karazeh/downloader.hpp"

namespace kzh {
  downloader::downloader(config_t const& config, file_manager const& fmgr)
  : logger("downloader"),
    config_(config),
    retry_count_(2),
    file_manager_(fmgr)
  {
  }

  downloader::~downloader() {
  }

  void downloader::set_retry_count(int n) {
    retry_count_ = n;
  }

  int downloader::retry_count() const {
    return retry_count_;
  }

  static size_t on_curl_data(char *buffer, size_t size, size_t nmemb, void *userdata)
  {
    download_t *dl = static_cast<download_t*>(userdata);

    size_t realsize = size * nmemb;

    dl->size += realsize;

    if (dl->to_file) {
      dl->stream.write(buffer, realsize);
    } else {
      (*dl->buf) += string_t(buffer, realsize);
    }

    return realsize;
  }

  bool downloader::fetch(string_t const& in_uri, download_t* dl, bool assume_ownership) const
  {
    string_t uri(in_uri);
    if (in_uri.find("http://") == std::string::npos) {
      uri = string_t(config_.host + in_uri);
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

  bool downloader::fetch(string_t const& in_uri, string_t& out_buf) const
  {
    download_t *dl = new download_t(std::cout);
    dl->buf = &out_buf;
    dl->uri = in_uri;

    return fetch(in_uri, dl);
  }

  bool downloader::fetch(string_t const& in_uri, std::ostream& out_stream) const
  {
    download_t *dl = new download_t(out_stream);
    dl->to_file = true;
    dl->uri = in_uri;

    return fetch(in_uri, dl);
  }

  bool downloader::fetch(
    string_t const& in_uri,
    path_t const& path,
    string_t const& checksum,
    uint64_t expected_size,
    int* const nr_retries) const
  {
    for (int i = 0; i < retry_count_ + 1; ++i) {
      std::ofstream fp(path.string().c_str(), std::ios_base::trunc | std::ios_base::binary);

      if (!fp.is_open() || !fp.good()) {
        error() << "Download destination is un-writable: " << path;
        return false;
      }

      // we'll build the download_t manually because we need to reference its size
      // otherwise we could use the std::ostream& version of fetch()
      download_t dl(fp);
      dl.to_file = true;
      dl.uri = in_uri;
      dl.retry_no = i;

      if (nr_retries != nullptr) {
        (*nr_retries) = i;
      }

      if (fetch(in_uri, &dl, false)) {
        fp.close();

        if (!file_manager_.is_readable(path)) {
          return false; // this really shouldn't happen, but oh well
        }

        // validate integrity
        hasher::digest_rc rc = config_.hasher->hex_digest(path);

        if (expected_size > 0 && expected_size == dl.size && rc == checksum) {
          return true;
        }
        else if (expected_size == 0 && rc == checksum) {
          return true;
        }
        else {
          warn()
            << "Downloaded file integrity mismatch: "
            <<  rc.digest << " vs " << checksum
            << " (got " << dl.size << " out of " << expected_size << " expected bytes)";

          if (config_.verbose) {
            string_t buf;
            file_manager_.load_file(path, buf);
          }
        }
      }
      else {
        fp.close();
      }

      fp.open(path.string().c_str(), std::ios_base::trunc);

      notice() << "Retry #" << i+1;
    }

    return false;
  }
}