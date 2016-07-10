#include "catch.hpp"
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/hashers/md5_hasher.hpp"

using namespace kzh;

TEST_CASE("MD5Hasher") {
  md5_hasher subject;

  SECTION("it should calculate a digest") {
    hasher::digest_rc drc = subject.hex_digest(
      (test_config.fixture_path / "hash_me.txt").make_preferred()
    );

    REQUIRE(drc.valid);
    REQUIRE(drc.digest == "f1eb970aeb2e380593480ed76070acbe");
  }
}