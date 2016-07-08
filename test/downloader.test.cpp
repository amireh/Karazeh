#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/downloader.hpp"

namespace kzh {
  namespace fs = boost::filesystem;

  class downloader_test : public ::testing::Test {
  protected:
    virtual void SetUp() {
      config_.host = test_config.server_host;
      subject = new downloader(file_manager_, config_);
    }

    virtual void TearDown() {
      delete subject;

      if (fs::exists("./downloads_test.tmp")) {
        fs::remove("./downloads_test.tmp");
      }
    }

    static void SetUpTestCase() {
    }

    downloader *subject;
    config_t config_;
    file_manager file_manager_;
  };

  TEST_F(downloader_test, loading_remote_resources) {
    string_t buf;
    std::ostringstream s;

    EXPECT_TRUE(subject->fetch("/version.xml", buf));
    EXPECT_TRUE(subject->fetch("/version.xml", s));
    EXPECT_EQ(buf, s.str());

    EXPECT_FALSE(subject->fetch("/version_woohoohaha.xml", buf));
  }

  // TEST_F(downloader_test, querying_unreachable_host) {
  //   string_t buf;
  //   subject->rebase("http://localhost.3456:9123");
  //   EXPECT_FALSE(subject->fetch("/version.xml", buf));

  //   subject->rebase("http://localhost.3456:91234123"); // OOB port
  //   EXPECT_FALSE(subject->fetch("/version.xml", buf));
  // }

  TEST_F(downloader_test, should_respect_expected_checksum) {
    int nr_retries = -1;

    subject->set_retry_count(3);

    ASSERT_FALSE(subject->
      fetch("/hash_me.txt",
        "./downloads_test.tmp",
        "dummy_checksum",
        0,
        &nr_retries));

    EXPECT_EQ(nr_retries, 3);

    // should not download more than once, since the checksum
    // is correct
    nr_retries = -1;
    ASSERT_TRUE(
      subject->fetch(
        "/hash_me.txt",
        "./downloads_test.tmp",
        "f1eb970aeb2e380593480ed76070acbe", // MD5
        0,
        &nr_retries));

    EXPECT_EQ(nr_retries, 0);
  }

  TEST_F(downloader_test, should_respect_expected_filesize) {
    int nr_retries = -1;
    subject->set_retry_count(3);

    ASSERT_FALSE(
      subject->fetch(
        "/hash_me.txt",
        "./downloads_test.tmp",
        "f1eb970aeb2e380593480ed76070acbe",
        32, /* it is 24 */
        &nr_retries));

    EXPECT_EQ(nr_retries, 3);

    nr_retries = -1;
    ASSERT_TRUE(
      subject->fetch(
        "/hash_me.txt",
        "./downloads_test.tmp",
        "f1eb970aeb2e380593480ed76070acbe",
        24,
        &nr_retries));

    EXPECT_EQ(nr_retries, 0);
  }
}
