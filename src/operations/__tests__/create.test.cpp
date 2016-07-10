#include "catch.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"
#include "karazeh/operations/create.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/release_manifest.hpp"
#include "test_utils.hpp"
#include <boost/filesystem.hpp>

using namespace kzh;
namespace fs = boost::filesystem;

TEST_CASE("create_operation - Staging") {
  config_t          config_(kzh::sample_config);
  file_manager      file_manager_;
  md5_hasher        md5_hasher_;
  downloader        downloader_(kzh::sample_config, file_manager_);
  release_manifest  manifest_;

  create_operation subject(
    config_,
    file_manager_,
    downloader_,
    manifest_
  );

  subject.dst_path = "CreateOperationTest/bar.txt";
  subject.src_uri  = "/hash_me.txt";
  subject.src_checksum = "f1eb970aeb2e380593480ed76070acbe";
  subject.src_size = 24;

  SECTION("It creates the destination directory if it doesn't already exist") {

    REQUIRE(subject.stage() == STAGE_OK);

    REQUIRE(file_manager_.exists(
      (sample_config.cache_path / subject.dst_path).parent_path()
    ));
  }

  WHEN("The cache directory does not exist...") {
    config_.cache_path = (test_config.temp_path / "some_custom_cache").make_preferred();
    manifest_.checksum = "somethingsomething";

    THEN("it creates it") {
      REQUIRE(subject.stage() == STAGE_OK);
      REQUIRE(file_manager_.is_directory(
        config_.cache_path / "somethingsomething/CreateOperationTest"
      ));
    }
  }

  fs::remove_all((config_.cache_path / subject.dst_path).parent_path());
  fs::remove_all((test_config.fixture_path / subject.dst_path).parent_path());
}