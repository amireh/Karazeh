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


#include "md5_hasher_test/md5_hasher_test.hpp"
#include "karazeh/hashers/md5_hasher.hpp"

namespace kzh {

  md5_hasher_test::md5_hasher_test() : test("md5_hasher") {
  }

  md5_hasher_test::~md5_hasher_test() {
  }

  int md5_hasher_test::run(int argc, char** argv) {
    string_t path("../../fixture/hash_me.txt");
    string_t expected_digest("f1eb970aeb2e380593480ed76070acbe");

    if (argc > 1) {
      if (argc < 3) {
        error() << "Usage: ./" << name_ << " [PATH_TO_FILE] [EXPECTED_MD5SUM]";
        error() << "You must either invoke the test with no arguments or two of them.";

        return failed;
      }

      path = string_t(argv[1]);      
      expected_digest = string_t(argv[2]);      
    }

    info() << "calculating hex digest of file: " << path;
    info() << "expected checksum: " << expected_digest;

    std::ifstream fh(path.c_str());
    hasher::digest_rc digest_rc = md5_hasher().hex_digest(fh);
    fh.close();

    hard_assert("calculating digest", digest_rc.valid);

    soft_assert("comparing digests", digest_rc.digest == expected_digest);

    // info() << "Hash:" << digest;

    return passed;
  }

}
