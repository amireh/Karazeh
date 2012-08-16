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

#ifndef H_KARAZEH_HASHER_H
#define H_KARAZEH_HASHER_H

#include "karazeh.hpp"

namespace kzh {

  class hasher
  {
    public:
    
    inline hasher(string_t const& name) : name_(name) { }
    inline virtual ~hasher() { }

    /** 
     * all hashers must return this struct as a result of their
     * digest calculation method
     */
    struct digest_rc {
      /** must indicate whether the digest was calculated successfully */
      bool     valid;
      
      /** the calculated digest ( converted to a string if necessary ) */
      string_t digest;

      digest_rc() : valid(false) {}
      ~digest_rc() { }

      bool operator==(string_t const& checksum) {
        return valid && checksum == digest;
      }
    };

    /** digests can be calculated directly off raw data */
    virtual digest_rc hex_digest(string_t const& data) const = 0;

    /** digests can be calculated off data in a _valid_ file stream */
    virtual digest_rc hex_digest(std::ifstream& src) const = 0;

    /** A hasher must be assigned so the other components can transparently
      * use it to calculate digests.
      */
    static void assign_hasher(hasher*);

    /** Returns a usable hasher instance. */
    static hasher const* const instance();

    string_t const& name() const;

  protected:
    static hasher* hasher_instance_;
    string_t name_;
  };

} // end of namespace kzh

#endif
