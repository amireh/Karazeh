#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/resource_manager.hpp"

namespace kzh {
  namespace fs = boost::filesystem;

  class resource_manager_test : public ::testing::Test {
    protected:
      virtual void SetUp() {
        rmgr_ = new resource_manager(KZH_SERVER_ADDRESS.string());
        rmgr_->override_paths(KZH_FIXTURE_PATH / "ktest/current");
      }

      virtual void TearDown() {
        delete rmgr_;

        if (fs::exists("./downloads_test.tmp")) {
          fs::remove("./downloads_test.tmp");
        }
      }

      static void SetUpTestCase() {
      }

      resource_manager *rmgr_;

  };

  TEST_F(resource_manager_test, resolving_paths) {
    using utility::split;

    ASSERT_NO_THROW(rmgr_->resolve_paths());
    EXPECT_EQ( "current", split(rmgr_->root_path().string(), '/').back() );
    EXPECT_EQ( "bin",     split(rmgr_->bin_path().string(), '/').back() );

    string_t cache_path = rmgr_->root_path().string() + "/.kzh/cache";
    EXPECT_EQ( cache_path, rmgr_->cache_path().string() );
  }

  TEST_F(resource_manager_test, overriding_paths) {
    using utility::split;

    ASSERT_NO_THROW(rmgr_->override_paths("./tmp_path"));
    EXPECT_EQ( "tmp_path",                        rmgr_->root_path().filename() );
    EXPECT_EQ( rmgr_->root_path() / "bin",        rmgr_->bin_path() );
    EXPECT_EQ( rmgr_->root_path() / ".kzh/cache", rmgr_->cache_path() );

    settings::set("bin_path",     "moo");
    settings::set("cache_path",   ".kzh/krack");

    ASSERT_NO_THROW(rmgr_->override_paths("./tmp_path"));
    EXPECT_EQ( "tmp_path",                                  rmgr_->root_path().filename() );
    EXPECT_EQ((rmgr_->root_path() / "moo").string(),        rmgr_->bin_path().string() );
    EXPECT_EQ((rmgr_->root_path() / ".kzh/krack").string(), rmgr_->cache_path().string() );

    fs::remove_all(rmgr_->root_path());
  }

  TEST_F(resource_manager_test, loading_remote_resources) {
    string_t buf;
    std::ostringstream s;

    EXPECT_TRUE(rmgr_->get_remote("/version.xml", buf));
    EXPECT_TRUE(rmgr_->get_remote("/version.xml", s));
    EXPECT_EQ(buf, s.str());

    EXPECT_FALSE(rmgr_->get_remote("/version_woohoohaha.xml", buf));
  }

  TEST_F(resource_manager_test, querying_unreachable_host) {
    string_t buf;
    rmgr_->rebase("http://localhost.3456:9123");
    EXPECT_FALSE(rmgr_->get_remote("/version.xml", buf));

    rmgr_->rebase("http://localhost.3456:91234"); // OOB port
    EXPECT_FALSE(rmgr_->get_remote("/version.xml", buf));
  }

  TEST_F(resource_manager_test, should_respect_expected_checksum) {
    int nr_retries = -1;

    rmgr_->set_retry_count(3);

    ASSERT_FALSE(rmgr_->
      get_remote("/hash_me.txt",
        "./downloads_test.tmp",
        "dummy_checksum",
        0,
        &nr_retries));

    EXPECT_EQ(nr_retries, 3);

    // should not download more than once, since the checksum
    // is correct
    nr_retries = -1;
    ASSERT_TRUE(
      rmgr_->get_remote(
        "/hash_me.txt",
        "./downloads_test.tmp",
        "f1eb970aeb2e380593480ed76070acbe", // MD5
        0,
        &nr_retries));

    EXPECT_EQ(nr_retries, 0);
  }

  TEST_F(resource_manager_test, should_respect_expected_filesize) {
    int nr_retries = -1;
    rmgr_->set_retry_count(3);

    ASSERT_FALSE(
      rmgr_->get_remote(
        "/hash_me.txt",
        "./downloads_test.tmp",
        "f1eb970aeb2e380593480ed76070acbe",
        32, /* it is 24 */
        &nr_retries));

    EXPECT_EQ(nr_retries, 3);

    nr_retries = -1;
    ASSERT_TRUE(
      rmgr_->get_remote(
        "/hash_me.txt",
        "./downloads_test.tmp",
        "f1eb970aeb2e380593480ed76070acbe",
        24,
        &nr_retries));

    EXPECT_EQ(nr_retries, 0);
  }

  TEST_F(resource_manager_test, checking_file_read_permissions) {
    ASSERT_TRUE(rmgr_->is_readable(fixture_path / "permissions/readable_file.txt"));
  }

  TEST_F(resource_manager_test, checking_file_write_permissions) {
    ASSERT_FALSE(rmgr_->is_writable(fixture_path / "permissions/unwritable_file.txt"));
  }

  TEST_F(resource_manager_test, checking_directory_read_permissions) {
    fs::path p(fixture_path / "permissions/readable_dir");

    ASSERT_TRUE(fs::is_directory(p));
    ASSERT_TRUE(rmgr_->is_readable(p));
  }

  TEST_F(resource_manager_test, checking_directory_write_permissions) {
    ASSERT_FALSE(rmgr_->is_writable(fixture_path / "permissions/unwritable_dir"));
  }

  TEST_F(resource_manager_test, removing_unwritable_file) {
    fs::path p(fixture_path / "permissions/unwritable_dir/unwritable_file.txt");
    ASSERT_THROW(fs::remove(p), fs::filesystem_error);
  }

  TEST_F(resource_manager_test, removing_unwritable_directory) {
    fs::path p(fixture_path / "permissions/unwritable_dir/unwritable_empty_dir");
    ASSERT_FALSE(fs::exists(p));
    ASSERT_FALSE(fs::remove_all(p));
  }

  TEST_F(resource_manager_test, creating_directories) {
    fs::path p(fixture_path / "permissions/readable_dir");
    ASSERT_TRUE(fs::exists(p));
    ASSERT_TRUE(fs::is_directory(p));

    ASSERT_TRUE(rmgr_->create_directory(p / "created_by_kzh_test"));
    ASSERT_TRUE(fs::remove_all(p / "created_by_kzh_test"));

    p = fs::path(fixture_path / "permissions/unwritable_dir");
    ASSERT_FALSE(rmgr_->create_directory(p / "created_by_kzh_test"));
    ASSERT_FALSE(fs::exists(p / "created_by_kzh_test"));
  }

  TEST_F(resource_manager_test, loading_a_local_file_from_stream) {
    path_t p(fixture_path / "hash_me.txt");
    string_t buf;

    std::ifstream fh(p.string().c_str());
    ASSERT_TRUE(fh.is_open() && fh.good());

    rmgr_->load_file(fh, buf);
    EXPECT_EQ("CALCULATE MY HEX DIGEST\n", buf);
  }

  TEST_F(resource_manager_test, loading_a_resource_agnostically) {
    string_t lbuf, rbuf;

    ASSERT_TRUE(rmgr_->get_remote("/version.xml", lbuf));
    ASSERT_TRUE(rmgr_->get_resource(rmgr_->host_address() + "/version.xml", rbuf));
    ASSERT_EQ(lbuf, rbuf);

    lbuf.clear();
    rbuf.clear();

    fs::path p(fixture_path / "hash_me.txt");
    ASSERT_TRUE(rmgr_->load_file(p, lbuf));
    ASSERT_TRUE(rmgr_->get_resource(p.string(), rbuf));
    ASSERT_EQ(lbuf, rbuf);
  }

  TEST_F(resource_manager_test, statting_filesize) {
    ASSERT_EQ(24, rmgr_->stat_filesize(fixture_path / "hash_me.txt"));
  }
}
