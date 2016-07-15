#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"
#include "karazeh/delta_encoder.hpp"
#include "karazeh/path_resolver.hpp"
#include "karazeh/file_manager.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "test_utils.hpp"
#include "catch.hpp"

namespace fs = boost::filesystem;
using namespace kzh;

TEST_CASE("DeltaEncoder") {
  path_t              archive_011;
  path_t              archive_012;
  path_t              sig_path;
  path_t              delta_path;
  path_t              target_path;
  string_t            sig_checksum;
  string_t            delta_checksum;
  string_t            target_checksum;
  kzh::path_resolver  path_resolver;
  kzh::file_manager   file_manager;
  kzh::config_t       config;
  kzh::delta_encoder  encoder;
  kzh::md5_hasher     md5_hasher;

  auto setup = [&]() {
    path_resolver.resolve(test_config.fixture_path);

    config.root_path = path_resolver.get_root_path();
    config.cache_path = test_config.temp_path / "delta_encoder_test";

    fs::create_directories(config.cache_path);

    archive_011 = path_t(config.root_path / "sample_application/0.1.1/data/common.tar");
    archive_012 = path_t(config.root_path / "sample_application/0.1.2/data/common.tar");

    sig_path    = config.cache_path / "archive_v0.1.1.signature";
    sig_checksum = string_t("e7f2c9bea103241c398b885f768d0b8b");

    delta_path  = config.cache_path / "archive_v0.1.1-0.1.2.delta";
    delta_checksum = string_t("b02c5026a9e24d0cdefa19641077ca91");

    target_path = config.cache_path / "archive_v0.1.2.tar";
    target_checksum = string_t("72eda360361e155ad8eabd07f07fa017");
  };

  auto teardown = [&]() {
    fs::remove_all(sig_path);
    fs::remove_all(delta_path);
    fs::remove_all(target_path);
    fs::remove_all(config.cache_path);
  };

  setup();

  SECTION("it should generate a signature") {
    rs_result rc;

    REQUIRE(file_manager.is_readable(archive_011));

    // generate sig
    REQUIRE(RS_DONE == encoder.signature(archive_011.c_str(), sig_path.c_str()));

    // verify the checksum
    REQUIRE(sig_checksum == md5_hasher.hex_digest(sig_path).digest);
  }

  SECTION("it should generate a delta") {
    REQUIRE(RS_DONE == encoder.signature(archive_011.c_str(), sig_path.c_str()));

    REQUIRE(file_manager.is_readable(sig_path));
    REQUIRE(file_manager.is_readable(archive_012));
    REQUIRE(file_manager.is_writable(delta_path));

    REQUIRE(RS_DONE == encoder.delta(sig_path.c_str(), archive_012.c_str(), delta_path.c_str()));

    REQUIRE(delta_checksum == md5_hasher.hex_digest(delta_path).digest);
  }


  SECTION("it should patch a file using a signature and a delta") {
    // create signature
    REQUIRE(RS_DONE == encoder.signature(archive_011.c_str(), sig_path.c_str()));

    // create delta patch
    REQUIRE(RS_DONE == encoder.delta(sig_path.c_str(), archive_012.c_str(), delta_path.c_str()));

    REQUIRE(file_manager.is_readable(delta_path));
    REQUIRE(file_manager.is_readable(archive_011));
    REQUIRE(file_manager.is_writable(target_path));

    // apply patch
    REQUIRE(RS_DONE == encoder.patch(archive_011.c_str(), delta_path.c_str(), target_path.c_str()));

    REQUIRE(target_checksum == md5_hasher.hex_digest(target_path).digest);
  }

  teardown();
}