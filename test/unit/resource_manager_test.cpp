#include <gtest/gtest.h>
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/resource_manager.hpp"
#include "karazeh/hashers/md5_hasher.hpp"

namespace kzh {
  class resource_manager_test : public ::testing::Test {
    protected:
      virtual void SetUp() {
        rmgr_ = new resource_manager(host_);
        
        hasher_ = new md5_hasher();
        hasher::assign_hasher(hasher_);        
      }
      
      virtual void TearDown() {
        delete rmgr_;
        delete hasher_;
      }
      
      static void SetUpTestCase() {
        host_ = "http://localhost:9393";
      }
      
      resource_manager *rmgr_;
      md5_hasher       *hasher_;
      
      static string_t host_;
  };
  
  string_t resource_manager_test::host_;

  TEST_F(resource_manager_test, resolving_paths) {
    using utility::split;
    
    ASSERT_NO_THROW(rmgr_->resolve_paths());
    EXPECT_EQ( "unit", split(rmgr_->root_path().string(), '/').back() );
    EXPECT_EQ( "bin", split(rmgr_->bin_path().string(), '/').back() );
    
    string_t cache_path = rmgr_->root_path().string() + "/.kzh/cache";
    EXPECT_EQ( cache_path, rmgr_->cache_path().string() );
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
  
}

// resource_manager rmgr("http://localhost:9333");
//     md5_hasher h;
//     hasher::assign_hasher(&h);

//     string_t buf;

//     stage("Loading a remote resource");
//     {
//       soft_assert("loading remote resource from http://localhost:9333/version.xml",
//                   rmgr.get_remote("/version.xml", buf));
//     }

//     stage("Loading a non existent file");
//     {
//       soft_assert("loading a remote resource that doesn't exist",
//                   !rmgr.get_remote("/some_non_existent_file.xml", buf));
//     }

//     stage("Querying an unreachable server");
//     {
//       soft_assert("loading a remote resource from an unreachable server",
//                   !rmgr.get_remote("http://localhost:12345/some_non_existent_file.xml", buf));
//     }

//     stage("Retry-able downloads");
//     {
//       soft_assert("Verifying integrity against an incorrect checksum",
//                   !rmgr.get_remote("/current/data/hash_me.txt", "./downloads_test.tmp", "invalid_dummy_checksum"));
      
//       soft_assert("Verifying integrity against an incorrect filesize",
//                   !rmgr.get_remote(
//                     "/current/data/hash_me.txt", 
//                     "./downloads_test.tmp", 
//                     "f1eb970aeb2e380593480ed76070acbe", 
//                     32 /* it is 24 */));

//     }
