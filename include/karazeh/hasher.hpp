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

#ifndef H_KARAZEH_HASHER_H
#define H_KARAZEH_HASHER_H

#include "karazeh_export.h"
#include "karazeh/karazeh.hpp"

namespace kzh {
  class KARAZEH_EXPORT hasher
  {
    public:

    inline hasher(string_t const& name) : name_(name) { };
    inline virtual ~hasher() {};

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

      bool operator!=(string_t const& checksum) {
        return !valid || checksum != digest;
      }
    };

    /** digests can be calculated directly off raw data */
    virtual digest_rc hex_digest(string_t const& data) const = 0;

    /** digests can be calculated off data in a _valid_ file stream */
    virtual digest_rc hex_digest(std::ifstream& src) const = 0;
    virtual digest_rc hex_digest(path_t const& path) const = 0;

    inline string_t const& name() const {
      return name_;
    };

  protected:
    string_t name_;
  };

} // end of namespace kzh

#endif
