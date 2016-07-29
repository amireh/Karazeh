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
  // end of fixtures

  const string_t delta_url("junk junk junk");

  update_operation subject(config, manifest,
    (config.root_path / "old_file.txt").make_preferred(),
    delta_url
  );

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
        uint64_t,
        int* const
       ) {
        REQUIRE(url == delta_url);
        REQUIRE(checksum == delta_checksum);
        return false;
      });

      REQUIRE_THROWS_AS(subject.stage(), kzh::invalid_resource);
    }

    SECTION("It generates a signature of the basis file") {
      When(FI_DOWNLOADER_FETCH(downloader_spy)).AlwaysDo([&](
        string_t const &url,
        path_t const &out,
        string_t const& checksum,
        uint64_t,
        int* const
       ) {
        string_t delta_contents;
        file_manager.load_file(config.root_path / "old_file.txt.delta", delta_contents);
        test_utils::create_file(out, delta_contents);
        return true;
      });

      REQUIRE(subject.stage() == kzh::STAGE_OK);

      // assert the contents, this wouldn't have been necessary were we able to
      // spy on the operation's delta_encoder instance, but oh well:
      REQUIRE(
        hasher.hex_digest(
          config.cache_path / manifest.id / "7457e972627b746a8819aca7424b6b25" / "signature"
        ).digest == hasher.hex_digest(
          // the one we've generated in the fixture stage
          config.root_path / "old_file.txt.signature"
        ).digest
      );
    }
  } // Staging

  SECTION("#deploy()") {
  } // Deplying

  SECTION("#rollback()") {
  }

  SECTION("#commit()") {
  }
}
