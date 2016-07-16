#include "catch.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/patcher.hpp"
#include "karazeh/path_resolver.hpp"
#include "karazeh/version_manifest.hpp"
#include "test_utils.hpp"
#include <boost/filesystem.hpp>

using namespace kzh;
using namespace Catch::Matchers;

TEST_CASE("Patcher", "[Patcher_JSON]") {
  string_t original_host(sample_config.host);
  path_t   original_root_path(sample_config.root_path);
  path_t   original_cache_path(sample_config.cache_path);

  kzh::path_resolver         path_resolver;
  kzh::config_t const& config(sample_config);

  path_resolver.resolve(test_config.fixture_path / "sample_application/current");

  sample_config.host          = test_config.server_host;
  sample_config.root_path     = path_resolver.get_root_path();
  sample_config.cache_path    = path_resolver.get_cache_path();

  version_manifest version(config);

  patcher subject(config);

  SECTION("#apply_update()") {
    sample_config.host = sample_config.host + "/sample_application";

    test_utils::copy_directory(
      test_config.fixture_path / "sample_application/0.1.0",
      config.root_path
    );

    version.load_from_uri(config.host + "/manifests/version.json");
    version.load_release_from_uri(config.host + "/manifests/release__0.1.1.json");

    auto release = version.get_release("ebb5dcbf784e0ef2fe6c37dae8d52722");

    REQUIRE(release);
    REQUIRE(subject.apply_update(*release) == STAGE_OK);
  }

  config.file_manager->remove_directory(sample_config.root_path);

  sample_config.host          = original_host;
  sample_config.root_path     = original_root_path;
  sample_config.cache_path    = original_cache_path;
}
