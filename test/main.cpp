#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/path_resolver.hpp"
#include "karazeh/file_manager.hpp"
#include "test_utils.hpp"
#include <cstdlib>

static std::string get_env_var(std::string const& key, std::string const& default_value = "");

kzh::test_config_t  kzh::test_config; // TEST GLOBAL
kzh::config_t       kzh::sample_config; // TEST GLOBAL

int main(int argc, char **argv) {
  int result;
  const bool verbose = get_env_var("VERBOSE") == "1";

  kzh::md5_hasher     hasher;
  kzh::file_manager   file_manager;

  kzh::path_resolver path_resolver;
  kzh::path_t base_path = kzh::path_t(get_env_var("ROOT", "")).make_preferred();
  kzh::string_t host(get_env_var("HOST", "http://localhost:9393"));

  path_resolver.resolve(base_path);

  kzh::test_config.fixture_path = (path_resolver.get_root_path() / "test/fixture").make_preferred();
  kzh::test_config.temp_path = (path_resolver.get_root_path() / "tmp").make_preferred();
  kzh::test_config.server_host = host;

  kzh::sample_config.host = kzh::test_config.server_host;
  kzh::sample_config.root_path = kzh::test_config.fixture_path;
  kzh::sample_config.cache_path = (kzh::sample_config.root_path / ".kzh/cache").make_preferred();
  kzh::sample_config.hasher = &hasher;
  kzh::sample_config.verbose = verbose;

  // ugh
  kzh::hasher::assign_hasher(&hasher);

  file_manager.ensure_directory(kzh::test_config.temp_path);
  file_manager.ensure_directory(kzh::sample_config.cache_path);

  if (verbose) {
    std::cout << "Fixture path: " << kzh::test_config.fixture_path.string() << "\n";
    std::cout << "Server host: " << kzh::test_config.server_host << "\n";
  }

  result = Catch::Session().run( argc, argv );

  kzh::hasher::assign_hasher(nullptr);
  file_manager.remove_directory(kzh::test_config.temp_path);
  file_manager.remove_directory(kzh::sample_config.cache_path.parent_path()); // the .kzh directory

  return result;
}

std::string get_env_var(std::string const& key, std::string const& default_value) {
  const char *val = getenv(key.c_str());

  if (val != NULL) {
    return std::string(val);
  }
  else {
    return default_value;
  }
}
