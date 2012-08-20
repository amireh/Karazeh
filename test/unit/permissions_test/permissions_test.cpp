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


#include "permissions_test/permissions_test.hpp"

namespace kzh {

  permissions_test::permissions_test() : test("permissions") {
  }

  permissions_test::~permissions_test() {
  }

  int permissions_test::run(int, char**) {
    resource_manager rmgr;
    namespace fs = boost::filesystem;

    stage("Checking for read permissions on a file");
    {
      soft_assert(
        "Checking if file permissions/readable_file.txt is readable",
        rmgr.is_readable(fixture_path_ / "permissions/readable_file.txt"));
    }

    stage("Checking for write permissions on a file");
    {
      soft_assert(
        "Making sure file permissions/unwritable_file.txt isn't writable",
        !rmgr.is_writable(fixture_path_ / "permissions/unwritable_file.txt"));
    }

    stage("Checking for read permissions on a directory");
    {
      soft_assert(
        "Checking if directory permissions/readable_dir is readable",
        rmgr.is_readable(fixture_path_ / "permissions/readable_dir"));
    }

    stage("Checking for write permissions on a directory");
    {
      soft_assert(
        "Making sure directory permissions/unwritable_dir isn't writable",
        rmgr.is_writable(fixture_path_ / "permissions/unwritable_dir"));
    }

    stage("Removing an un-writable file");
    {
      fs::path p(fixture_path_ / "permissions/unwritable_dir/unwritable_file.txt");
      bool deleted = false;
      try {
        fs::remove(p);
        deleted = true;
      } catch (fs::filesystem_error& e) {
        debug() << e.what();
      }

      soft_assert(
        "Making sure I can't delete an unwritable file: " + p.string(), !deleted);
    }

    stage("Removing an un-writable directory");
    {
      fs::path p(fixture_path_ / "permissions/unwritable_dir");
      bool deleted = false;
      try {
        fs::remove_all(p);
        deleted = true;
      } catch (fs::filesystem_error& e) {
        debug() << e.what();
      }

      soft_assert(
        "Making sure I can't delete an unwritable directory permissions/unwritable_dir", !deleted);
    }


    return passed;
  }

}
