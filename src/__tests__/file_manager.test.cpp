#include "catch.hpp"
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/file_manager.hpp"

using namespace kzh;

TEST_CASE("FileManager") {
  file_manager subject;

  SECTION("checking_file_read_permissions") {
    REQUIRE(subject.is_readable(test_config.fixture_path / "permissions/readable_file.txt"));
  }

  SECTION("checking_file_write_permissions") {
    REQUIRE_FALSE(subject.is_writable(test_config.fixture_path / "permissions/unwritable_file.txt"));
  }

  SECTION("checking_directory_read_permissions") {
    path_t p(test_config.fixture_path / "permissions/readable_dir");

    REQUIRE(subject.is_readable(p));
  }

  SECTION("checking_directory_write_permissions") {
    REQUIRE_FALSE(subject.is_writable(test_config.fixture_path / "permissions/unwritable_dir"));
  }

  SECTION("removing_unwritable_file") {
    path_t p(test_config.fixture_path / "permissions/unwritable_dir/unwritable_file.txt");

    REQUIRE_FALSE(subject.remove_file(p));
  }

  SECTION("removing_unwritable_directory") {
    path_t path(test_config.fixture_path / "permissions/unwritable_dir/unwritable_empty_dir");

    REQUIRE_FALSE(subject.create_directory(path));
  }

  SECTION("creating_directories") {
    path_t p(test_config.fixture_path / "permissions/readable_dir");

    REQUIRE(subject.exists(p));
    REQUIRE(subject.is_writable(p));

    REQUIRE(subject.create_directory(p / "created_by_kzh_test"));
    REQUIRE(subject.remove_directory(p / "created_by_kzh_test"));
  }

  SECTION("creating_directories_inside_unwritable_directory") {
    path_t p(test_config.fixture_path / "permissions/unwritable_dir");

    REQUIRE_FALSE(subject.is_writable(p));
    REQUIRE_FALSE(subject.create_directory(p / "created_by_kzh_test"));
    REQUIRE_FALSE(subject.exists(p / "created_by_kzh_test"));
  }

  SECTION("loading_a_local_file_from_stream") {
    path_t p(test_config.fixture_path / "hash_me.txt");
    string_t buf;

    std::ifstream fh(p.string().c_str());
    REQUIRE(fh.is_open());
    REQUIRE(fh.good());

    subject.load_file(fh, buf);
    REQUIRE("CALCULATE MY HEX DIGEST\n" == buf);
  }

  SECTION("statting_filesize") {
    REQUIRE(24 == subject.stat_filesize(test_config.fixture_path / "hash_me.txt"));
  }
}
