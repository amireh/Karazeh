#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/downloader.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "catch.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace kzh;

TEST_CASE("Downloader") {
  const path_t temp_file_path = test_config.temp_path / "downloader_test.out";

  config_t config_(sample_config);
  file_manager  file_manager_;
  downloader subject(config_, file_manager_);

  SECTION("it should load a remote resource") {
    string_t buf;
    std::ostringstream s;

    REQUIRE(subject.fetch("/sample_application/manifests/version.xml", buf));
    REQUIRE(subject.fetch("/sample_application/manifests/version.xml", s));
    REQUIRE(buf == s.str());

    REQUIRE_FALSE(subject.fetch("/version_woohoohaha.xml", buf));
  }

  // takes too long, meh
  //
  // GIVEN("An unreachable host") {
  //   config_.host = "http://localhost.3456:9123";
  //   string_t buf;
  //
  //   THEN("it should not bork when attempting to query it") {
  //     REQUIRE_FALSE(subject.fetch("/sample_application/manifests/version.xml", buf));
  //   }
  // }

  GIVEN("A host with an out-of-bound port") {
    config_.host = "http://localhost.3456:91234123"; // OOB port
    string_t buf;

    THEN("it should not bork when attempting to query it") {
      REQUIRE_FALSE(subject.fetch("/sample_application/manifests/version.xml", buf));
    }
  }

  SECTION("it should retry if there's a checksum mismatch") {
    int nr_retries = -1;

    subject.set_retry_count(1);

    REQUIRE_FALSE(subject.fetch("/hash_me.txt",
      temp_file_path,
      "dummy_checksum",
      0,
      &nr_retries
    ));

    REQUIRE(nr_retries == 1);
  }

  SECTION("it should not retry if the checksum matches") {
    int nr_retries = -1;

    subject.set_retry_count(3);

    REQUIRE(subject.fetch(
      "/hash_me.txt",
      temp_file_path,
      "f1eb970aeb2e380593480ed76070acbe",
      0,
      &nr_retries
    ));

    REQUIRE(nr_retries == 0);
  }

  SECTION("it should retry if there is a size mismatch") {
    int nr_retries = -1;
    subject.set_retry_count(3);

    REQUIRE_FALSE(
      subject.fetch(
        "/hash_me.txt",
        temp_file_path,
        "f1eb970aeb2e380593480ed76070acbe",
        32, /* it is 24 */
        &nr_retries
      )
    );

    REQUIRE(nr_retries == 3);
  }

  SECTION("it should not retry if the size matches") {
    int nr_retries = -1;

    REQUIRE(
      subject.fetch(
        "/hash_me.txt",
        temp_file_path,
        "f1eb970aeb2e380593480ed76070acbe",
        24,
        &nr_retries
      )
    );

    REQUIRE(nr_retries == 0);
  }

  if (fs::exists(temp_file_path)) {
    fs::remove(temp_file_path);
  }
}