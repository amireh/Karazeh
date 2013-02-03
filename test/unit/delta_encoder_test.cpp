#include <gtest/gtest.h>
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/delta_encoder.hpp"
#include "test_helper.hpp"

#ifdef ASSERT_THROW
  #undef ASSERT_THROW
#endif

#define ASSERT_THROW KZH_ASSERT_THROW

namespace kzh {
  
  class delta_encoder_test : public ::testing::Test {
  protected:
    virtual void SetUp() {
      
      archive_010 = path_t(KZH_FIXTURE_PATH / "archives/q3_archive.0.1.0.tar");
      archive_012 = path_t(KZH_FIXTURE_PATH / "archives/q3_archive.0.1.2.tar");
      sig_checksum = string_t("5e4e9bfefd881d6a46572d30f1f93345");
      delta_checksum = string_t("12e73f0e4c38d1051bdf5452dea2ac10");
      target_checksum = string_t("e48ead8578ab2459989cd4de4fcba992");
    }
    
    virtual void TearDown() {
    }
    
    static void SetUpTestCase() {
      rmgr_ = new resource_manager(KZH_SERVER_ADDRESS.string());
      rmgr_->override_paths(KZH_FIXTURE_PATH);     
      
      sig_path    = path_t(rmgr_->cache_path() / "q3_archive.0.1.0.signature");
      delta_path  = path_t(rmgr_->cache_path() / "q3_archive.0.1.0-0.1.2.patch");
      target_path = path_t(rmgr_->cache_path() / "q3_archive.0.1.2.patched.tar");
    }
    
    static void TearDownTestCase() {
      namespace fs = boost::filesystem;
      fs::remove(sig_path);
      fs::remove(delta_path);
      fs::remove(target_path);
      delete rmgr_;
    }
    
    path_t            archive_010;
    path_t            archive_012;
    static path_t     sig_path;
    static path_t     delta_path;
    static path_t     target_path;
    string_t          sig_checksum;
    string_t          delta_checksum;
    string_t          target_checksum;
    static resource_manager  *rmgr_;
    delta_encoder     encoder_;
  };
  
  path_t     delta_encoder_test::sig_path;
  path_t     delta_encoder_test::delta_path;
  path_t     delta_encoder_test::target_path;
  resource_manager* delta_encoder_test::rmgr_;

  TEST_F(delta_encoder_test, signature_generation) {
    // generate sig
    ASSERT_TRUE(rmgr_->is_readable(archive_010))
      << "basis for signature is missing! path: " << archive_010;
    rs_result rc;
    ASSERT_NO_THROW(rc = encoder_.signature(archive_010.c_str(), sig_path.c_str()));
    ASSERT_EQ(RS_DONE, rc);
    
    // verify the checksum
    std::ifstream sig(sig_path.c_str());
    hasher::digest_rc drc = hasher::instance()->hex_digest(sig);
    sig.close();
  }
  
  TEST_F(delta_encoder_test, delta_generation) {
    ASSERT_TRUE(rmgr_->is_readable(sig_path));
    ASSERT_TRUE(rmgr_->is_readable(archive_012));
    ASSERT_TRUE(rmgr_->is_writable(delta_path));
    
    rs_result rc;
    ASSERT_NO_THROW(rc = encoder_.delta(sig_path.c_str(), archive_012.c_str(), delta_path.c_str()));

    ASSERT_EQ(RS_DONE, rc);

    std::ifstream delta(delta_path.c_str());
    hasher::digest_rc drc = hasher::instance()->hex_digest(delta);
    delta.close();
    ASSERT_EQ(delta_checksum, drc.digest);
  }
 
  TEST_F(delta_encoder_test, target_patching) {
    ASSERT_TRUE(rmgr_->is_readable(delta_path));
    ASSERT_TRUE(rmgr_->is_readable(archive_010));
    ASSERT_TRUE(rmgr_->is_writable(target_path));
        
    rs_result rc;
    ASSERT_NO_THROW(rc = encoder_.patch(archive_010.c_str(), delta_path.c_str(), target_path.c_str()));
    ASSERT_EQ(RS_DONE, rc);

    std::ifstream target(target_path.c_str());
    hasher::digest_rc drc = hasher::instance()->hex_digest(target);
    target.close();

    ASSERT_EQ(target_checksum, drc.digest);
  }
  
} // namespace kzh