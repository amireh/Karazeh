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
    logger l("md5_hasher"); l.debug() << "Calculating digest from file[" << fp << "]";
    std::ifstream fh(fp.c_str());
    digest_rc rc(hex_digest(fh));
    fh.close();

    return rc;
  }

}