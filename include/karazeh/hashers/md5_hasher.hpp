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

#ifndef H_KARAZEH_HASHER_MD5_H
#define H_KARAZEH_HASHER_MD5_H

#include "karazeh/hasher.hpp"
#include "md5/md5.hpp"
// extern "C" {
// #include "md5_2/md5.h"
// }
#include "karazeh_export.h"

namespace kzh {

  class KARAZEH_EXPORT md5_hasher : public hasher
  {
    public:

    inline md5_hasher() : hasher("MD5") { }
    inline virtual ~md5_hasher() { }

    virtual digest_rc hex_digest(string_t const& data) const;
    virtual digest_rc hex_digest(std::ifstream& src) const;
    virtual digest_rc hex_digest(path_t const& path) const;
  };

} // end of namespace kzh

#endif
