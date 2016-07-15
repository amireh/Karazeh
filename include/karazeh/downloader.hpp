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

#ifndef H_KARAZEH_DOWNLOADER_H
#define H_KARAZEH_DOWNLOADER_H

#include <curl/curl.h>
#include <boost/filesystem.hpp>
#include "binreloc/binreloc.h"
#include "karazeh_export.h"
#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/hasher.hpp"
#include "karazeh/file_manager.hpp"
#include "karazeh/config.hpp"

namespace kzh {
  struct download_t;

  class KARAZEH_EXPORT downloader : protected logger {
  public:
    downloader(config_t const&, file_manager const&);
    virtual ~downloader();

    /** The number of times to retry a download */
    int retry_count() const;
    void set_retry_count(int);

    /**
     * Downloads the file found at URI and stores it in out_buf. If
     * @URI does not start with http:// then it will be prefixed by
     * the assigned server URI.
     *
     * @return true if the file was correctly DLed, false otherwise
     */
    bool fetch(string_t const& URI, string_t& out_buf) const;

    /** same as above but outputs to file instead of buffer */
    bool fetch(string_t const& URI, std::ostream& out_file) const;

    /**
     * Downloads the file found at the given URI and verifies
     * its integrity against the given checksum. The download
     * will be retried up to retry_count() times.
     *
     * Returns true if the file was downloaded and its integrity verified.
     */
    bool fetch(
      string_t const& URI,
      path_t const& path_to_file,
      string_t const& checksum,
      uint64_t expected_size_bytes = 0,
      int* const nr_retries = NULL
    ) const;

  private:
    const config_t &config_;
    const file_manager& file_manager_;

    bool fetch(string_t const& URI, download_t*, bool assume_ownership = true) const;

    int retry_count_;
  };

  /** Used internally by the downloader to manage downloads */
  struct KARAZEH_EXPORT download_t {
    inline explicit
    download_t(std::ostream& s)
    : buf(nullptr),
      to_file(false),
      status(false),
      size(0),
      retry_no(0),
      stream(s) {
    }

    string_t      *buf;
    string_t      uri;
    bool          status;
    bool          to_file;
    std::ostream  &stream;
    uint64_t      size;
    int           retry_no;
  };
} // end of namespace kzh

#endif
