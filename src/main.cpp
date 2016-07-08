#include "karazeh/patcher.hpp"
#include "karazeh/path_resolver.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/settings.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

int main(int argc, char** argv) {
  using kzh::string_t;

  string_t root_path;

  kzh::logger::enable_timestamps(false);
  kzh::logger logger("test");
  kzh::path_resolver path_resolver;
  kzh::file_manager file_manager;
  kzh::config_t config;
  kzh::md5_hasher hasher;

  if (argc > 1) {
    for (int i = 0; i < argc; ++i) {
      string_t arg = argv[i];

      if (arg == "-r" && argc > i) {
        root_path = string_t(argv[++i]);
      }
      else if (arg == "-v") {
        config.verbose = true;
      }
    }
  }

  path_resolver.resolve(root_path);

  // create the folders if they doesn't exist
  fs::create_directory(path_resolver.get_cache_path());

  config.root_path = path_resolver.get_root_path();
  config.cache_path = path_resolver.get_cache_path();
  config.host = "http://localhost:9393";
  config.hasher = &hasher;

  kzh::downloader downloader(file_manager, config);
  kzh::patcher patcher(config, file_manager, downloader);

  if (patcher.identify(config.host + "/version.xml")) {
    if (patcher.is_update_available()) {
      if (patcher.apply_next_update()) {
        logger.info() << "Successfully updated";
      } else {
        logger.error() << "Wasn't able to update.";
      }
    }
  } // identify()
  else {
    logger.error() << "unable to identify version";
  }

  return 0;
}