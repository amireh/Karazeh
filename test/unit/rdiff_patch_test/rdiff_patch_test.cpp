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


#include "rdiff_patch_test/rdiff_patch_test.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/delta_encoder.hpp"

namespace kzh {

  rdiff_patch_test::rdiff_patch_test() : test("rdiff_patch") {
  }

  rdiff_patch_test::~rdiff_patch_test() {
  }

  int rdiff_patch_test::run(int, char**) {
    static path_t const archive_010(fixture_path_ / "archives/q3_archive.0.1.0.tar");
    static path_t const archive_012(fixture_path_ / "archives/q3_archive.0.1.2.tar");
    static path_t const sig_path(fixture_path_ / "tmp" / "q3_archive.0.1.0.signature");
    static path_t const delta_path(fixture_path_ / "tmp" / "q3_archive.0.1.0-0.1.2.patch");
    static path_t const target_path(fixture_path_ / "tmp" / "q3_archive.0.1.2.patched.tar");
    static string_t const sig_checksum("5e4e9bfefd881d6a46572d30f1f93345");
    static string_t const delta_checksum("12e73f0e4c38d1051bdf5452dea2ac10");
    static string_t const target_checksum("e48ead8578ab2459989cd4de4fcba992");

    static delta_encoder encoder;

    fs::create_directories(fixture_path_ / "tmp");
    md5_hasher h;
    hasher::assign_hasher(&h);

    info() << "Old archive: " << archive_010;
    info() << "New archive: " << archive_012;

    stage("Generating signature");
    {
      info() << "Writing signature to: " << sig_path;
      rs_result rc = encoder.signature(archive_010.c_str(), sig_path.c_str());
      debug() << "rdiff_sig rc: " << rc;
      
      hard_assert("Signature generated successfully", rc == RS_DONE);

      std::ifstream sig(sig_path.c_str());
      hasher::digest_rc drc = hasher::instance()->hex_digest(sig);
      sig.close();
      hard_assert("Signature checksum is correct", drc == sig_checksum);
    }

    stage("Generating delta");
    {
      info() << "Writing delta to: " << delta_path;
      rs_result rc = encoder.delta(sig_path.c_str(), archive_012.c_str(), delta_path.c_str());
      debug() << "rdiff_sig rc: " << rc;

      hard_assert("Delta generated successfully", rc == RS_DONE);

      std::ifstream delta(delta_path.c_str());
      hasher::digest_rc drc = hasher::instance()->hex_digest(delta);
      delta.close();
      hard_assert("Delta checksum is correct", drc == delta_checksum); 
    }

    stage("Patching target");
    {
      info() << "Writing patched target at: " << delta_path;
      rs_result rc = encoder.patch(archive_010.c_str(), delta_path.c_str(), target_path.c_str());
      debug() << "rdiff_sig rc: " << rc;

      hard_assert("Target patched successfully", rc == RS_DONE);

      std::ifstream target(target_path.c_str());
      hasher::digest_rc drc = hasher::instance()->hex_digest(target);
      target.close();

      hard_assert("Target checksum is correct", drc == target_checksum);
    }

    return passed;
  }

}
