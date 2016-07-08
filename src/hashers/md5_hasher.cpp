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

#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/logger.hpp"

namespace kzh {

  hasher::digest_rc md5_hasher::hex_digest(string_t const& data) const {
    digest_rc rc;

    rc.digest = MD5(data).hex_digest();
    rc.valid = true;

    return rc;
  }

  hasher::digest_rc md5_hasher::hex_digest(std::ifstream& fh) const {
    digest_rc rc;

    if (!fh.is_open() || !fh.good()) {
      logger l("md5_hasher");
      l.error() << "filestream isn't valid! can not calculate hex digest";
      return rc;
    }

    rc.digest = MD5(fh).hex_digest();
    rc.valid = true;

    return rc;
  }

  hasher::digest_rc md5_hasher::hex_digest(path_t const& fp) const {
    std::ifstream fh(fp.c_str(), std::ifstream::in | std::ifstream::binary);
    digest_rc rc(hex_digest(fh));
    fh.close();

    return rc;
  }

}