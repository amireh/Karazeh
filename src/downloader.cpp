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
  static size_t
  on_curl_data(char *buffer, size_t size, size_t nmemb, void *userdata)
  {
    download_t *download = static_cast<download_t*>(userdata);
    size_t realsize = size * nmemb;

    if (download->stream) {
      download->stream->write(buffer, realsize);
    }

    if (download->buf) {
      (*download->buf) += string_t(buffer, realsize);
    }

    return realsize;
  }

  downloader::downloader(config_t const& config, file_manager const& fmgr)
  : logger("downloader"),
    config_(config),
    retry_count_(2),
    file_manager_(fmgr)
  {
  }

  downloader::~downloader() {
  }

  void
  downloader::set_retry_count(int n) {
    retry_count_ = n;
  }

  int
  downloader::retry_count() const {
    return retry_count_;
  }

  bool
  downloader::fetch_file(url_t const& url, download_t* download, bool assume_ownership) const
  {
    CURL* curl_ = curl_easy_init();
    CURLcode curlrc_;
    bool http_connection_successful, http_request_successful;

    if (!curl_) {
      error() << "unable to resolve URL " << url << ", aborting remote download request";
      return false;
    }

    char curlerr[CURL_ERROR_SIZE];

    info() << "Downloading " << download->url;

    curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, curlerr);
    curl_easy_setopt(curl_, CURLOPT_URL, download->url.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &on_curl_data);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, download);

    curlrc_ = curl_easy_perform(curl_);
    http_connection_successful = curlrc_ == 0;

    if (http_connection_successful) {
      long http_rc = 0;
      curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_rc);

      http_request_successful = http_rc == 200;

      if (!http_request_successful) {
        error() << "Remote server error; status code: " << http_rc;
      }
    }
    else {
      error() << "CURL connection error: " << curlrc_ << " => " << curlerr;
    }

    curl_easy_cleanup(curl_);

    if (assume_ownership) {
      delete download;
    }

    return http_connection_successful && http_request_successful;
  }

  bool
  downloader::fetch(url_t const& _url, string_t& out_buf) const
  {
    string_t url(get_full_url(_url));

    download_t *download = new download_t(url);
    download->buf = &out_buf;

    return fetch_file(url, download, true);
  }

  bool
  downloader::fetch(url_t const& _url, std::ostream& out_stream) const
  {
    string_t url(get_full_url(_url));

    download_t *download = new download_t(url);
    download->stream = &out_stream;

    return fetch_file(url, download, true);
  }

  bool
  downloader::fetch(string_t const& url, path_t const& path, string_t const& checksum, int* const retry_tally) const
  {
    // TODO: rethink about this, this really sounds like an external concern
    for (int i = 0; i < retry_count_ + 1; ++i) {
      bool fetch_successful;

      if (!file_manager_.is_writable(path)) {
        error() << "Download destination is un-writable: " << path;
        return false;
      }

      std::ofstream fp(path.string().c_str(), std::ios_base::trunc | std::ios_base::binary);

      if (retry_tally != nullptr) {
        (*retry_tally) = i;
      }

      fetch_successful = fetch(url, fp);

      fp.close();

      if (fetch_successful) {
        if (!file_manager_.is_readable(path)) {
          return false; // this really shouldn't happen, but oh well
        }

        // validate integrity
        hasher::digest_rc rc = config_.hasher->hex_digest(path);

        if (rc == checksum) {
          return true;
        }
        else {
          warn()
            << "Downloaded file integrity mismatch: "
            <<  rc.digest << " vs " << checksum;
        }
      }

      notice() << "Retry #" << i+1;
    }

    return false;
  }

  url_t
  downloader::get_full_url(string_t const& url) const {
    if (url.find("http://") == std::string::npos) {
      return url_t(config_.host + url);
    }
    else {
      return url;
    }
  }
}