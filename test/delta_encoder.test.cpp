#include <gtest/gtest.h>
#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"
#include "karazeh/delta_encoder.hpp"
#include "karazeh/path_resolver.hpp"
#include "karazeh/file_manager.hpp"
#include "test_utils.hpp"

namespace kzh {
  namespace fs = boost::filesystem;

  class delta_encoder_test : public ::testing::Test {
  protected:
    virtual void SetUp() {
      path_resolver_.resolve(test_config.fixture_path);

      config_.root_path = path_resolver_.get_root_path();
      config_.cache_path = path_resolver_.get_cache_path();

      fs::create_directories(config_.cache_path);

      archive_011 = path_t(config_.root_path / "sample_application/0.1.1/data/common.tar");
      archive_012 = path_t(config_.root_path / "sample_application/0.1.2/data/common.tar");

      sig_path    = config_.cache_path / "archive_v0.1.1.signature";
      sig_checksum = string_t("e7f2c9bea103241c398b885f768d0b8b");

      delta_path  = config_.cache_path / "archive_v0.1.1-0.1.2.delta";
      delta_checksum = string_t("b02c5026a9e24d0cdefa19641077ca91");

      target_path = config_.cache_path / "archive_v0.1.2.tar";
      target_checksum = string_t("72eda360361e155ad8eabd07f07fa017");
    }

    virtual void TearDown() {
      fs::remove(sig_path);
      fs::remove(delta_path);
      fs::remove(target_path);
      fs::remove(config_.cache_path);
    }

    static void TearDownTestCase() {
    }

    path_t            archive_011;
    path_t            archive_012;
    path_t            sig_path;
    path_t            delta_path;
    path_t            target_path;
    string_t          sig_checksum;
    string_t          delta_checksum;
    string_t          target_checksum;
    path_resolver     path_resolver_;
    file_manager      file_manager_;
    config_t          config_;
    delta_encoder     encoder_;
  };

  TEST_F(delta_encoder_test, signature_generation) {
    rs_result rc;

    ASSERT_TRUE(file_manager_.is_readable(archive_011))
      << "basis for signature is missing! path: " << archive_011;

    // generate sig
    rc = encoder_.signature(archive_011.c_str(), sig_path.c_str());
    ASSERT_EQ(RS_DONE, rc);

    // verify the checksum
    std::ifstream sig(sig_path.c_str());
    hasher::digest_rc drc = hasher::instance()->hex_digest(sig);
    sig.close();
  }

  TEST_F(delta_encoder_test, delta_generation) {
    rs_result rc;

    ASSERT_NO_THROW(rc = encoder_.signature(archive_011.c_str(), sig_path.c_str()));
    ASSERT_EQ(RS_DONE, rc);

    ASSERT_TRUE(file_manager_.is_readable(sig_path));
    ASSERT_TRUE(file_manager_.is_readable(archive_012));
    ASSERT_TRUE(file_manager_.is_writable(delta_path));

    ASSERT_NO_THROW(rc = encoder_.delta(sig_path.c_str(), archive_012.c_str(), delta_path.c_str()));
    ASSERT_EQ(RS_DONE, rc);

    std::ifstream delta(delta_path.c_str());
    hasher::digest_rc drc = hasher::instance()->hex_digest(delta);
    delta.close();
    ASSERT_EQ(delta_checksum, drc.digest);
  }

  TEST_F(delta_encoder_test, target_patching) {
    rs_result rc;

    // create signature
    ASSERT_NO_THROW(rc = encoder_.signature(archive_011.c_str(), sig_path.c_str()));
    ASSERT_EQ(RS_DONE, rc);

    // create delta patch
    ASSERT_NO_THROW(rc = encoder_.delta(sig_path.c_str(), archive_012.c_str(), delta_path.c_str()));
    ASSERT_EQ(RS_DONE, rc);

    ASSERT_TRUE(file_manager_.is_readable(delta_path));
    ASSERT_TRUE(file_manager_.is_readable(archive_011));
    ASSERT_TRUE(file_manager_.is_writable(target_path));

    // apply patch
    ASSERT_NO_THROW(rc = encoder_.patch(archive_011.c_str(), delta_path.c_str(), target_path.c_str()));
    ASSERT_EQ(RS_DONE, rc);

    std::ifstream target(target_path.c_str());
    hasher::digest_rc drc = hasher::instance()->hex_digest(target);
    target.close();

    ASSERT_EQ(target_checksum, drc.digest);
  }

} // namespace kzh