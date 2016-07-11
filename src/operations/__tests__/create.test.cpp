#include "catch.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"
#include "karazeh/operations/create.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/release_manifest.hpp"
#include "test_utils.hpp"
#include <boost/filesystem.hpp>
#include "fakeit.hpp"

using namespace kzh;
using namespace fakeit;

namespace fs = boost::filesystem;

TEST_CASE("create_operation") {
  config_t          config_(kzh::sample_config);
  file_manager      file_manager_;
  md5_hasher        md5_hasher_;
  downloader        downloader_(kzh::sample_config, file_manager_);
  release_manifest  manifest_;

  downloader_.set_retry_count(0);

  manifest_.checksum = "somethingsomething";

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

  SECTION("Staging...") {
    WHEN("The destination directory doesn't already exist") {
      THEN("It creates it") {
        REQUIRE(subject.stage() == STAGE_OK);

        REQUIRE(file_manager_.exists(
          (config_.cache_path / manifest_.checksum / subject.dst_path).parent_path()
        ));
      }
    }

    WHEN("The cache directory does not exist...") {
      config_.cache_path = (test_config.temp_path / "some_custom_cache").make_preferred();

      THEN("it creates it") {
        REQUIRE(subject.stage() == STAGE_OK);
        REQUIRE(file_manager_.is_directory(
          config_.cache_path / "somethingsomething/CreateOperationTest"
        ));
      }
    }

    WHEN("The destination is already occupied") {
      subject.dst_path = "/hash_me.txt";

      THEN("it aborts") {
        REQUIRE(subject.stage() == STAGE_FILE_EXISTS);
      }
    }

    WHEN("The destination is already occupied but is also marked for deletion") {
      subject.dst_path = "/hash_me.txt";
      subject.marked_for_deletion();

      THEN("it works") {
        REQUIRE(subject.stage() == STAGE_OK);
      }
    }

    WHEN("The destination is not writable") {
      subject.dst_path = "/permissions/unwritable_dir/foo.txt";

      THEN("it aborts") {
        REQUIRE(subject.stage() == STAGE_UNAUTHORIZED);
      }
    }

    WHEN("The cache destination is not writable") {
      config_.cache_path = test_config.fixture_path / "permissions/unwritable_dir";

      THEN("it aborts") {
        REQUIRE(subject.stage() == STAGE_UNAUTHORIZED);
      }
    }

    WHEN("The download fails") {
      subject.src_uri = "/akljhasdklfjhasdlfkhjasdf";

      THEN("it aborts") {
        REQUIRE_THROWS_AS(
          subject.stage(),
          invalid_resource
        );
      }
    }
  } // Staging

  SECTION("Deploying") {
    const path_t cachePath(config_.cache_path / manifest_.checksum / subject.dst_path);
    const path_t destPath(config_.root_path / subject.dst_path);

    REQUIRE(subject.stage() == STAGE_OK);

    WHEN("The destination is occupied") {
      Mock<file_manager> spy(file_manager_);
      When(ConstOverloadedMethod(spy, is_readable, bool(path_t const&)))
        .AlwaysDo([&](const path_t &path) {
          return path == destPath;
        })
      ;

      THEN("It aborts") {
        REQUIRE(subject.deploy() == STAGE_FILE_EXISTS);
      }
    }

    WHEN("The destination is not writable") {
      Mock<file_manager> spy(file_manager_);
      When(ConstOverloadedMethod(spy, is_writable, bool(path_t const&)))
        .AlwaysDo([&](const path_t &path) {
          return path == destPath ? false : true;
        })
      ;

      THEN("It aborts") {
        REQUIRE(subject.deploy() == STAGE_UNAUTHORIZED);
      }
    }

    WHEN("The cache source is not writable") {
      Mock<file_manager> spy(file_manager_);
      When(ConstOverloadedMethod(spy, is_writable, bool(path_t const&)))
        .AlwaysDo([&](const path_t &path) {
          return path == cachePath ? false : true;
        })
      ;

      THEN("It aborts") {
        REQUIRE(subject.deploy() == STAGE_UNAUTHORIZED);
      }
    }

    WHEN("All looks good") {
      REQUIRE(subject.deploy() == STAGE_OK);

      THEN("It installs the staged file at the destination") {
        REQUIRE(file_manager_.exists(destPath));
        REQUIRE(config_.hasher->hex_digest(destPath) == subject.src_checksum);
      }

      THEN("It removes the staged file") {
        REQUIRE_FALSE(file_manager_.exists(cachePath));
      }
    }

    WHEN("It is marked as an executable") {
      subject.is_executable = true;

      Mock<file_manager> spy(file_manager_);
      Fake(ConstOverloadedMethod(spy, make_executable, bool(path_t const&)));

      REQUIRE(subject.deploy() == STAGE_OK);

      THEN("It sets the executable bit") {
        Verify(ConstOverloadedMethod(spy, make_executable, bool(path_t const&)))
          .Exactly(Once)
        ;
      }
    }
  }

  fs::remove_all(sample_config.cache_path / "CreateOperationTest");
  fs::remove_all(test_config.fixture_path / "CreateOperationTest");
}