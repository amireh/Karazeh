#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/file_manager.hpp"

namespace kzh {
  namespace fs = boost::filesystem;

  class file_manager_test : public ::testing::Test {
  protected:
    virtual void SetUp() {
      subject = new file_manager();
    }

    virtual void TearDown() {
      delete subject;
    }

    file_manager *subject;
  };

  TEST_F(file_manager_test, checking_file_read_permissions) {
    ASSERT_TRUE(subject->is_readable(test_config.fixture_path / "permissions/readable_file.txt"));
  }

  TEST_F(file_manager_test, checking_file_write_permissions) {
    ASSERT_FALSE(subject->is_writable(test_config.fixture_path / "permissions/unwritable_file.txt"));
  }

  TEST_F(file_manager_test, checking_directory_read_permissions) {
    path_t p(test_config.fixture_path / "permissions/readable_dir");

    ASSERT_TRUE(subject->is_readable(p));
  }

  TEST_F(file_manager_test, checking_directory_write_permissions) {
    ASSERT_FALSE(subject->is_writable(test_config.fixture_path / "permissions/unwritable_dir"));
  }

  TEST_F(file_manager_test, removing_unwritable_file) {
    path_t p(test_config.fixture_path / "permissions/unwritable_dir/unwritable_file.txt");

    ASSERT_FALSE(subject->remove_file(p));
  }

  TEST_F(file_manager_test, removing_unwritable_directory) {
    path_t path(test_config.fixture_path / "permissions/unwritable_dir/unwritable_empty_dir");

    ASSERT_FALSE(subject->create_directory(path));
  }

  TEST_F(file_manager_test, creating_directories) {
    path_t p(test_config.fixture_path / "permissions/readable_dir");

    ASSERT_TRUE(subject->exists(p));
    ASSERT_TRUE(subject->is_writable(p));

    ASSERT_TRUE(subject->create_directory(p / "created_by_kzh_test"));
    ASSERT_TRUE(subject->remove_directory(p / "created_by_kzh_test"));
  }

  TEST_F(file_manager_test, creating_directories_inside_unwritable_directory) {
    path_t p(test_config.fixture_path / "permissions/unwritable_dir");

    ASSERT_FALSE(subject->is_writable(p));
    ASSERT_FALSE(subject->create_directory(p / "created_by_kzh_test"));
    ASSERT_FALSE(subject->exists(p / "created_by_kzh_test"));
  }

  TEST_F(file_manager_test, loading_a_local_file_from_stream) {
    path_t p(test_config.fixture_path / "hash_me.txt");
    string_t buf;

    std::ifstream fh(p.string().c_str());
    ASSERT_TRUE(fh.is_open() && fh.good());

    subject->load_file(fh, buf);
    EXPECT_EQ("CALCULATE MY HEX DIGEST\n", buf);
  }

  TEST_F(file_manager_test, statting_filesize) {
    ASSERT_EQ(24, subject->stat_filesize(test_config.fixture_path / "hash_me.txt"));
  }
}
