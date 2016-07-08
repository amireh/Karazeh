#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/resource_manager.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>
#include <boost/filesystem/path.hpp>

namespace kzh {
  test_config_t test_config;

  class unit_test_env : public ::testing::Environment {
  public:
    unit_test_env(int argc, char** argv) {
      kzh::karazeh_init(argc, argv);
    }

    virtual ~unit_test_env() {}

    virtual void SetUp() {
      logger::mute();

      hasher_ = new md5_hasher();
      hasher::assign_hasher(hasher_);
    }

    virtual void TearDown() {
      delete hasher_;

      kzh::karazeh_cleanup();
    }

  protected:
    md5_hasher *hasher_;
  };
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new kzh::unit_test_env(argc, argv));

  kzh::resource_manager rmgr;

  rmgr.resolve_paths();

  kzh::test_config.fixture_path = rmgr.root_path() / "test/fixture";
  kzh::test_config.server_host = "http://localhost:9393";

  return RUN_ALL_TESTS();
}