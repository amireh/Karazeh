#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/path_resolver.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

namespace kzh {
  test_config_t test_config;

  class unit_test_env : public ::testing::Environment {
  public:
    unit_test_env(int argc, char** argv) {
    }

    virtual ~unit_test_env() {}

    virtual void SetUp() {
      logger::mute();

      hasher_ = new md5_hasher();
      hasher::assign_hasher(hasher_);
    }

    virtual void TearDown() {
      delete hasher_;
    }

  protected:
    md5_hasher *hasher_;
  };
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new kzh::unit_test_env(argc, argv));

  kzh::path_resolver path_resolver;

  path_resolver.resolve();

  kzh::test_config.fixture_path = path_resolver.get_root_path() / "test/fixture";
  kzh::test_config.server_host = "http://localhost:9393";

  return RUN_ALL_TESTS();
}