#include <boost/filesystem.hpp>
#include "catch.hpp"
#include "fakeit.hpp"
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"
#include "karazeh/operations/create.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/release_manifest.hpp"

namespace fs = boost::filesystem;

TEST_CASE("create_operation") {
  using namespace kzh;
  using fakeit::Mock;
  using fakeit::When;
  using fakeit::Fake;
  using fakeit::Verify;
  using fakeit::Once;

  config_t                config(kzh::sample_config);
  kzh::file_manager       file_manager;
  kzh::downloader         downloader(kzh::sample_config, file_manager);
  kzh::release_manifest   manifest;
  create_operation        subject(config, manifest);

  // spies
  Mock<kzh::file_manager> file_manager_spy(file_manager);
  Mock<md5_hasher>        hasher_spy;

  // <setup>
  config.downloader = &downloader;
  config.file_manager = &file_manager;

  downloader.set_retry_count(0);

  manifest.id = "somethingsomething";

  subject.dst_path = "CreateOperationTest/bar.txt";
  subject.src_uri  = "/hash_me.txt";
  subject.src_checksum = "f1eb970aeb2e380593480ed76070acbe";
  // </setup>

  SECTION("Staging...") {
    WHEN("The destination directory doesn't already exist") {
      THEN("It creates it") {
        REQUIRE(subject.stage() == STAGE_OK);

        REQUIRE(file_manager.exists(
          (config.cache_path / manifest.id / subject.dst_path).parent_path()
        ));
      }
    }

    WHEN("The cache directory does not exist...") {
      config.cache_path = (test_config.temp_path / "some_custom_cache").make_preferred();

      THEN("it creates it") {
        REQUIRE(subject.stage() == STAGE_OK);
        REQUIRE(file_manager.is_directory(
          config.cache_path / "somethingsomething/CreateOperationTest"
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
      config.cache_path = test_config.fixture_path / "permissions/unwritable_dir";

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
    const path_t cachePath(config.cache_path / manifest.id / subject.dst_path);
    const path_t destPath(config.root_path / subject.dst_path);

    REQUIRE(subject.stage() == STAGE_OK);

    WHEN("The destination is occupied") {
      When(FI_FILE_MANAGER_IS_READABLE(file_manager_spy)).AlwaysDo([&](const path_t &path) {
        return path == destPath;
      });

      THEN("It aborts") {
        REQUIRE(subject.deploy() == STAGE_FILE_EXISTS);
      }
    }

    WHEN("The destination is not writable") {
      When(FI_FILE_MANAGER_IS_WRITABLE(file_manager_spy)).AlwaysDo([&](const path_t &path) {
        return path != destPath;
      });

      THEN("It aborts") {
        REQUIRE(subject.deploy() == STAGE_UNAUTHORIZED);
      }
    }

    WHEN("The cache source is not writable") {
      When(FI_FILE_MANAGER_IS_WRITABLE(file_manager_spy)).AlwaysDo([&](const path_t &path) {
        return path != cachePath;
      });

      THEN("It aborts") {
        REQUIRE(subject.deploy() == STAGE_UNAUTHORIZED);
      }
    }

    WHEN("All looks good") {
      REQUIRE(subject.deploy() == STAGE_OK);

      THEN("It installs the staged file at the destination") {
        REQUIRE(file_manager.exists(destPath));
        REQUIRE(config.hasher->hex_digest(destPath) == subject.src_checksum);
      }

      THEN("It removes the staged file") {
        REQUIRE_FALSE(file_manager.exists(cachePath));
      }
    }

    WHEN("It is marked as an executable") {
      Fake(FI_FILE_MANAGER_MAKE_EXECUTABLE(file_manager_spy));

      subject.is_executable = true;

      REQUIRE(subject.deploy() == STAGE_OK);

      THEN("It sets the executable bit") {
        Verify(FI_FILE_MANAGER_MAKE_EXECUTABLE(file_manager_spy)).Exactly(Once);
      }
    }
  }

  SECTION("Rolling back") {
    const path_t cachePath(config.cache_path / manifest.id / subject.dst_path);
    const path_t destPath(config.root_path / subject.dst_path);

    WHEN("It has been staged") {
      REQUIRE(subject.stage() == STAGE_OK);

      THEN("It removes the directory it has created") {
        Fake(FI_FILE_MANAGER_REMOVE_DIRECTORY(file_manager_spy));

        subject.rollback();

        Verify(FI_FILE_MANAGER_REMOVE_DIRECTORY(file_manager_spy)).Exactly(Once);
      }
    }

    WHEN("It has been deployed") {
      REQUIRE(subject.stage() == STAGE_OK);
      REQUIRE(subject.deploy() == STAGE_OK);

      THEN("It moves the file back into staging") {
        Fake(FI_FILE_MANAGER_MOVE(file_manager_spy));

        subject.rollback();

        Verify(FI_FILE_MANAGER_MOVE(file_manager_spy)).Exactly(1);
      }
    }

    WHEN("It has neither been staged nor deployed") {
      Fake(FI_FILE_MANAGER_MOVE(file_manager_spy));
      Fake(FI_FILE_MANAGER_REMOVE_DIRECTORY(file_manager_spy));

      THEN("It does nothing") {
        subject.rollback();

        Verify(FI_FILE_MANAGER_MOVE(file_manager_spy)).Exactly(0);
        Verify(FI_FILE_MANAGER_REMOVE_DIRECTORY(file_manager_spy)).Exactly(0);
      }
    }
  }

  SECTION("Committing") {
    WHEN("It has been deployed") {
      THEN("It does nothing") {}
    }

    WHEN("It has been rolled-back") {
      Fake(FI_FILE_MANAGER_REMOVE_FILE(file_manager_spy));

      REQUIRE(subject.stage() == STAGE_OK);
      REQUIRE(subject.deploy() == STAGE_OK);

      subject.rollback();

      THEN("It removes the staged file") {
        subject.commit();

        Verify(FI_FILE_MANAGER_REMOVE_FILE(file_manager_spy)).Exactly(1);
      }

      THEN("It does not remove the staged file if it finds the checksum not to match") {
        config.hasher = &hasher_spy.get();

        When(FI_HASHER_HEX_DIGEST(hasher_spy)).AlwaysDo([&](const path_t&) {
          hasher::digest_rc rc;

          rc.valid = true;
          rc.digest = "asdf";

          return rc;
        });

        subject.commit();

        Verify(FI_FILE_MANAGER_REMOVE_FILE(file_manager_spy)).Exactly(0);
      }
    }
  }

  fs::remove_all(sample_config.cache_path / "CreateOperationTest");
  fs::remove_all(test_config.fixture_path / "CreateOperationTest");
}