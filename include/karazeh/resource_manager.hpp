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

#ifndef H_KARAZEH_RESOURCE_MANAGER_H
#define H_KARAZEH_RESOURCE_MANAGER_H

#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include <curl/curl.h>
#include <boost/filesystem.hpp>
#include "binreloc/binreloc.h"

namespace kzh {

  typedef boost::filesystem::path path_t;

  class resource_manager : protected logger {
  public:
    
    resource_manager();
    virtual ~resource_manager();

    void resolve_paths(path_t root = "");

    /** Loads the content of a file into memory */
    bool load_file(std::ifstream &fs, string_t& out_buf);
    bool load_file(string_t const& path, string_t& out_buf);

    /** Checks if the resource at the given path exists and is readable. */
    bool is_readable(path_t const &path) const;
    bool is_readable(string_t const &path) const;

    /**
     * downloads the file found at URL and stores it in out_buf
     *
     * @return true if the file was correctly DLed, false otherwise
     */
    bool get_remote(string_t const& URL, string_t& out_buf);

    /** same as above but outputs to file instead of buffer */
    bool get_remote(string_t const& URL, std::ofstream& out_file);

    static path_t const& root_path();
    static path_t const& tmp_path();
    static path_t const& bin_path();

  private:
    static path_t root_path_, tmp_path_, bin_path_;
  };

  struct download_t {
    string_t  *buf;
    string_t  uri;
    bool      status;
  };

} // end of namespace kzh

#endif
