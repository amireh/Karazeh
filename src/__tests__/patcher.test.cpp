#include "catch.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/patcher.hpp"
#include "karazeh/path_resolver.hpp"
#include "test_utils.hpp"

using namespace kzh;
using namespace Catch::Matchers;

TEST_CASE("Patcher", "[Patcher_JSON]") {
  config_t              config_(sample_config);
  path_resolver         path_resolver_;
  file_manager          file_manager_;

  path_resolver_.resolve(test_config.fixture_path / "sample_application/0.1.2");
  config_.host = test_config.server_host;
  config_.root_path = path_resolver_.get_root_path();
  config_.cache_path = path_resolver_.get_cache_path();

  downloader  downloader_(config_, *config_.file_manager);
  patcher     subject(config_, *config_.file_manager, downloader_);
}
