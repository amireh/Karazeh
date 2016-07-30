#include <boost/filesystem.hpp>
#include "catch.hpp"
#include "fakeit.hpp"
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"
#include "karazeh/operations/update.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/release_manifest.hpp"

TEST_CASE("update_operation") {
  using namespace kzh;
  using fakeit::Mock;
  using fakeit::When;

  config_t config(kzh::sample_config);
  kzh::release_manifest manifest;
  kzh::md5_hasher hasher;
  kzh::file_manager file_manager;
  kzh::downloader downloader(config, file_manager);
  kzh::delta_encoder encoder;

  Mock<kzh::file_manager> file_manager_spy(file_manager);
  Mock<kzh::md5_hasher>   hasher_spy(hasher);
  Mock<kzh::downloader>   downloader_spy(downloader);

  config.root_path = (test_config.temp_path / "update_operation_test").make_preferred();
  config.file_manager = &file_manager;
  config.hasher = &hasher;
  config.downloader = &downloader;

  manifest.id = "some manifest id";

  // setup fixtures
  const path_t   file_path(config.root_path / "old_file.txt");
  const path_t   updated_file_path(config.root_path / "new_file.txt");
  const url_t    delta_url("junk junk junk");

  test_utils::create_file(config.root_path / "old_file.txt", "Hello");
  test_utils::create_file(config.root_path / "new_file.txt", "Hello World!");

  encoder.signature(
    config.root_path / "old_file.txt",
    config.root_path / "old_file.txt.signature"
  );

  encoder.delta(
    config.root_path / "old_file.txt.signature",
    config.root_path / "new_file.txt",
    config.root_path / "old_file.txt.delta"
  );

  const string_t delta_checksum(hasher.hex_digest(config.root_path / "old_file.txt.delta").digest);

  auto serve_delta_file = [&]() {
    When(FI_DOWNLOADER_FETCH(downloader_spy)).AlwaysDo(
      [&](string_t const &url, path_t const & out, string_t const& checksum, int* const) {
        REQUIRE(url == delta_url);

        string_t delta_contents;
        file_manager.load_file(config.root_path / "old_file.txt.delta", delta_contents);
        test_utils::create_file(out, delta_contents);

        return true;
      }
    );
  };

  const path_t cache_path(config.cache_path / manifest.id / "0");

  // end of fixtures

  update_operation subject(0, config, manifest, file_path, delta_url);

  subject.basis_checksum = "8b1a9953c4611296a827abf8c47804d7";
  subject.delta_checksum = delta_checksum;
  subject.patched_checksum = "ed076287532e86365e841e92bfc50d8c";

  SECTION("#stage()") {
    WHEN("The basis file is not readable...") {
      When(FI_FILE_MANAGER_IS_READABLE(file_manager_spy)).AlwaysDo([&](const path_t &path) {
        return path != subject.basis_path();
      });

      THEN("It aborts") {
        REQUIRE(subject.stage() == kzh::STAGE_FILE_MISSING);
      }
    }

    WHEN("The basis file checksum mismatches") {
      When(FI_HASHER_HEX_DIGEST(hasher_spy)).AlwaysDo([&](const path_t &path) {
        kzh::hasher::digest_rc rc;

        if (path == subject.basis_path()) {
          rc.valid = false;
          rc.digest = "lololol";
        }
        else {
          rc.valid = true;
          rc.digest = "asdfasdf";
        }

        return rc;
      });

      THEN("It aborts") {
        REQUIRE(subject.stage() == kzh::STAGE_FILE_INTEGRITY_MISMATCH);
      }
    }

    SECTION("It attempts to download the delta file") {
      When(FI_DOWNLOADER_FETCH(downloader_spy)).AlwaysDo([&](
        string_t const &url,
        path_t const &out,
        string_t const& checksum,
        int* const
       ) {
        REQUIRE(url == delta_url);
        REQUIRE(checksum == delta_checksum);
        return false;
      });

      REQUIRE_THROWS_AS(subject.stage(), kzh::invalid_resource);
    }

    SECTION("It generates a signature of the basis file") {
      serve_delta_file();

      REQUIRE(subject.stage() == kzh::STAGE_OK);

      // assert the contents, this wouldn't have been necessary were we able to
      // spy on the operation's delta_encoder instance, but oh well:
      REQUIRE(
        hasher.hex_digest(
          config.cache_path / manifest.id / "0" / "signature"
        ).digest == hasher.hex_digest(
          // the one we've generated in the fixture stage
          config.root_path / "old_file.txt.signature"
        ).digest
      );
    }
  } // Staging

  SECTION("#deploy()") {
    SECTION("It works") {
      serve_delta_file();

      REQUIRE(subject.stage() == kzh::STAGE_OK);
      REQUIRE(subject.deploy() == kzh::STAGE_OK);

      REQUIRE(
        hasher.hex_digest(file_path).digest ==
        hasher.hex_digest(updated_file_path).digest
      );
    }

    WHEN("Patching fails...") {
      When(FI_DOWNLOADER_FETCH(downloader_spy)).AlwaysDo(
        [&](string_t const &url, path_t const & out, string_t const& checksum, int* const) {
          REQUIRE(url == delta_url);
          test_utils::create_file(out, "junk delta junk");
          return true;
        }
      );

      THEN("It aborts") {
        REQUIRE(subject.stage() == kzh::STAGE_OK);
        REQUIRE(subject.deploy() == kzh::STAGE_ENCODING_ERROR);
      }
    }

    WHEN("The patched file checksum mismatches") {
      serve_delta_file();
      kzh::md5_hasher functional_hasher;

      When(FI_HASHER_HEX_DIGEST(hasher_spy)).AlwaysDo([&](const path_t &path) {
        if (path.string().find("/patched") != string_t::npos) {
          kzh::hasher::digest_rc rc;

          rc.valid = false;
          rc.digest = "lololol";

          return rc;
        }
        else {
          return functional_hasher.hex_digest(path);
        }
      });

      THEN("It aborts") {
        REQUIRE(subject.stage() == kzh::STAGE_OK);
        REQUIRE(subject.deploy() == kzh::STAGE_FILE_INTEGRITY_MISMATCH);
      }
    }
  } // Deplying

  SECTION("#rollback()") {
    WHEN("Subject has not been staged yet...") {
      subject.rollback();

      THEN("It is a no-nop") {
        REQUIRE(hasher.hex_digest(file_path).digest == subject.basis_checksum);
      }
    }

    WHEN("Subject has been staged but not yet deployed...") {
      serve_delta_file();

      REQUIRE(subject.stage() == kzh::STAGE_OK);

      subject.rollback();

      THEN("It is a no-nop") {
        REQUIRE(hasher.hex_digest(file_path).digest == subject.basis_checksum);
      }
    }

    WHEN("Subject has been deployed but the basis has been lost...") {
      serve_delta_file();

      REQUIRE(subject.stage() == kzh::STAGE_OK);
      REQUIRE(subject.deploy() == kzh::STAGE_OK);

      test_utils::remove_file(cache_path / "patched");

      subject.rollback();

      THEN("It is a no-op") {
        REQUIRE(hasher.hex_digest(file_path).digest == subject.patched_checksum);
      }
    }

    WHEN("Subject has been deployed but the basis has been tampered with...") {
      serve_delta_file();

      REQUIRE(subject.stage() == kzh::STAGE_OK);
      REQUIRE(subject.deploy() == kzh::STAGE_OK);

      test_utils::remove_file(cache_path / "patched");
      test_utils::create_file(cache_path / "patched", "trolololol");

      subject.rollback();

      THEN("It is a no-op") {
        REQUIRE(hasher.hex_digest(file_path).digest == subject.patched_checksum);
      }
    }

    WHEN("Subject has been deployed but the destination is occupied with something unexpected") {
      kzh::md5_hasher functional_hasher;

      test_utils::create_file(file_path, "LOL! TROLLED! LOL");
      auto digest = config.hasher->hex_digest(file_path);

      REQUIRE(functional_hasher.hex_digest(file_path).digest == digest.digest);
      subject.rollback();

      THEN("It leaves it as it is") {
        REQUIRE(functional_hasher.hex_digest(file_path).digest == digest.digest);
      }
    }

    WHEN("Subject has been deployed...") {
      serve_delta_file();

      REQUIRE(subject.stage() == kzh::STAGE_OK);
      REQUIRE(subject.deploy() == kzh::STAGE_OK);
      REQUIRE(hasher.hex_digest(file_path).digest == subject.patched_checksum);

      subject.rollback();

      THEN("It restores the original file") {
        REQUIRE(hasher.hex_digest(file_path).digest == subject.basis_checksum);
      }
    }
  }

  SECTION("#commit()") {
    WHEN("Subject has not been staged yet...") {
      THEN("It is a no-op") {
        subject.commit();
      }
    }
  }
}
