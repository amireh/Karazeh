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

namespace kzh {

  class resource_manager : protected logger {
  public:
    
    resource_manager();
    virtual ~resource_manager();

    /** loads the content of a file into memory */
    bool load_file(std::ifstream &fs, string_t& out_buf);

    /**
     * downloads the file found at URL and stores it in out_buf
     *
     * @return true if the file was correctly DLed, false otherwise
     */
    bool get_remote(string_t const& URL, string_t& out_buf);

    /** same as above but outputs to file instead of buffer */
    bool get_remote(string_t const& URL, std::ofstream& out_file);

  private:
  };

  struct download_t {
    string_t  *buf;
    string_t  uri;
    bool      status;
  };

} // end of namespace kzh

#endif
