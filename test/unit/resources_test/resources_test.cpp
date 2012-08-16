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


#include "resources_test/resources_test.hpp"
#include "karazeh/resource_manager.hpp"
#include "karazeh/hashers/md5_hasher.hpp"

namespace kzh {

  resources_test::resources_test() : test("resources") {
  }

  resources_test::~resources_test() {
  }

  int resources_test::run(int, char**) {
    resource_manager rmgr("http://localhost:9333");
    md5_hasher h;
    hasher::assign_hasher(&h);

    string_t buf;

    stage("Loading a remote resource");
    {
      soft_assert("loading remote resource from http://localhost:9333/version.xml",
                  rmgr.get_remote("/version.xml", buf));
    }

    stage("Loading a non existent file");
    {
      soft_assert("loading a remote resource that doesn't exist",
                  !rmgr.get_remote("/some_non_existent_file.xml", buf));
    }

    stage("Querying an unreachable server");
    {
      soft_assert("loading a remote resource from an unreachable server",
                  !rmgr.get_remote("http://localhost:12345/some_non_existent_file.xml", buf));
    }

    stage("Retry-able downloads");
    {
      soft_assert("Verifying integrity against an incorrect checksum",
                  !rmgr.get_remote("/current/data/hash_me.txt", "./downloads_test.tmp", "invalid_dummy_checksum"));
      
      soft_assert("Verifying integrity against an incorrect filesize",
                  !rmgr.get_remote(
                    "/current/data/hash_me.txt", 
                    "./downloads_test.tmp", 
                    "f1eb970aeb2e380593480ed76070acbe", 
                    32 /* it is 24 */));

    }

    return passed;
  }

}
