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

#ifndef H_KARAZEH_SETTINGS_H
#define H_KARAZEH_SETTINGS_H

#include "karazeh/karazeh.hpp"
#include <map>

namespace kzh {

  /** this class is a temporary hack to store some library-wide settings */
  class settings {
  public:
    inline settings() {}
    inline ~settings() {}

    static void set_defaults();
    
    static void enable(string_t const& id);
    static void disable(string_t const& id);
    static void set(string_t const& id, string_t const& value);
    static bool is_enabled(string_t const& id);
    static string_t const& get(string_t const& id);

  protected:
    typedef std::map<string_t, bool>      flag_settings_t;
    typedef std::map<string_t, string_t>  literal_settings_t;
    
    static flag_settings_t flag_settings_;
    static literal_settings_t literal_settings_;
  }; 
}


#endif