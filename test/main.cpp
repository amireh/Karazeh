#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/path_resolver.hpp"
#include "test_utils.hpp"
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <cstdlib>

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

static std::string get_env_var(std::string const& key, std::string const& default_value) {
  const char *val = getenv(key.c_str());

  if (val != NULL) {
    return std::string(val);
  }
  else {
    return default_value;
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new kzh::unit_test_env(argc, argv));

  kzh::path_resolver path_resolver;
  kzh::path_t base_path = kzh::path_t(get_env_var("ROOT", "")).make_preferred();
  kzh::string_t host(get_env_var("HOST", "http://localhost:9393"));

  path_resolver.resolve(base_path);
  
  kzh::test_config.fixture_path = (path_resolver.get_root_path() / "test/fixture").make_preferred();
  kzh::test_config.server_host = host;


  std::cout << "Fixture path: " << kzh::test_config.fixture_path.string() << "\n";
  std::cout << "Server host: " << kzh::test_config.server_host << "\n";

  return RUN_ALL_TESTS();
}