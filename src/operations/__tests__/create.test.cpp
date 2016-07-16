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

#define STUB_MOVE ConstOverloadedMethod(spy, move, bool(path_t const&, path_t const&))
#define STUB_REMOVE_FILE ConstOverloadedMethod(spy, remove_file, bool(path_t const&))
#define STUB_REMOVE_DIRECTORY ConstOverloadedMethod(spy, remove_directory, bool(path_t const&))
#define STUB_MAKE_EXECUTABLE ConstOverloadedMethod(spy, make_executable, bool(path_t const&))
#define STUB_IS_READABLE ConstOverloadedMethod(spy, is_readable, bool(path_t const&))
#define STUB_IS_WRITABLE ConstOverloadedMethod(spy, is_writable, bool(path_t const&))
#define STUB_HEX_DIGEST ConstOverloadedMethod(hasherSpy, hex_digest, hasher::digest_rc(const path_t&))

TEST_CASE("create_operation") {
  config_t          config(kzh::sample_config);
  kzh::file_manager      file_manager;
  kzh::downloader        downloader(kzh::sample_config, file_manager);
  kzh::release_manifest  manifest;

  downloader.set_retry_count(0);

  config.downloader = &downloader;
  config.file_manager = &file_manager;

  manifest.id = "somethingsomething";

  create_operation subject(
    config,
    manifest
  );

  subject.dst_path = "CreateOperationTest/bar.txt";
  subject.src_uri  = "/hash_me.txt";
  subject.src_checksum = "f1eb970aeb2e380593480ed76070acbe";
  subject.src_size = 24;

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
      Mock<kzh::file_manager> spy(file_manager);
      When(STUB_IS_READABLE).AlwaysDo([&](const path_t &path) {
        return path == destPath;
      });

      THEN("It aborts") {
        REQUIRE(subject.deploy() == STAGE_FILE_EXISTS);
      }
    }

    WHEN("The destination is not writable") {
      Mock<kzh::file_manager> spy(file_manager);
      When(STUB_IS_WRITABLE).AlwaysDo([&](const path_t &path) {
        return path != destPath;
      });

      THEN("It aborts") {
        REQUIRE(subject.deploy() == STAGE_UNAUTHORIZED);
      }
    }

    WHEN("The cache source is not writable") {
      Mock<kzh::file_manager> spy(file_manager);
      When(STUB_IS_WRITABLE).AlwaysDo([&](const path_t &path) {
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
      subject.is_executable = true;

      Mock<kzh::file_manager> spy(file_manager);
      Fake(STUB_MAKE_EXECUTABLE);

      REQUIRE(subject.deploy() == STAGE_OK);

      THEN("It sets the executable bit") {
        Verify(STUB_MAKE_EXECUTABLE)
          .Exactly(Once)
        ;
      }
    }
  }

  SECTION("Rolling back") {
    const path_t cachePath(config.cache_path / manifest.id / subject.dst_path);
    const path_t destPath(config.root_path / subject.dst_path);

    WHEN("It has been staged") {
      REQUIRE(subject.stage() == STAGE_OK);

      THEN("It removes the directory it has created") {
        Mock<kzh::file_manager> spy(file_manager);
        Fake(STUB_REMOVE_DIRECTORY);

        subject.rollback();

        Verify(STUB_REMOVE_DIRECTORY).Exactly(Once);
      }
    }

    WHEN("It has been deployed") {
      REQUIRE(subject.stage() == STAGE_OK);
      REQUIRE(subject.deploy() == STAGE_OK);

      THEN("It moves the file back into staging") {
        Mock<kzh::file_manager> spy(file_manager);
        Fake(STUB_MOVE);

        subject.rollback();

        Verify(STUB_MOVE).Exactly(1);
      }
    }

    WHEN("It has neither been staged nor deployed") {
      Mock<kzh::file_manager> spy(file_manager);

      Fake(STUB_MOVE);
      Fake(STUB_REMOVE_DIRECTORY);

      THEN("It does nothing") {
        subject.rollback();

        Verify(STUB_MOVE).Exactly(0);
        Verify(STUB_REMOVE_DIRECTORY).Exactly(0);
      }
    }
  }

  SECTION("Committing") {
    WHEN("It has been deployed") {
      THEN("It does nothing") {}
    }

    WHEN("It has been rolled-back") {
      Mock<kzh::file_manager> spy(file_manager);
      Fake(STUB_REMOVE_FILE);

      REQUIRE(subject.stage() == STAGE_OK);
      REQUIRE(subject.deploy() == STAGE_OK);

      subject.rollback();

      THEN("It removes the staged file") {
        subject.commit();

        Verify(STUB_REMOVE_FILE).Exactly(1);
      }

      THEN("It does not remove the staged file if it finds the checksum not to match") {
        Mock<md5_hasher> hasherSpy;
        config.hasher = &hasherSpy.get();

        When(STUB_HEX_DIGEST).AlwaysDo([&](const path_t&) {
          hasher::digest_rc rc;

          rc.valid = true;
          rc.digest = "asdf";

          return rc;
        });

        subject.commit();

        Verify(STUB_REMOVE_FILE).Exactly(0);
      }
    }
  }

  fs::remove_all(sample_config.cache_path / "CreateOperationTest");
  fs::remove_all(test_config.fixture_path / "CreateOperationTest");
}

#undef STUB_MOVE
#undef STUB_REMOVE_FILE
#undef STUB_REMOVE_DIRECTORY
#undef STUB_MAKE_EXECUTABLE
#undef STUB_IS_READABLE
#undef STUB_IS_WRITABLE
#undef STUB_HEX_DIGEST