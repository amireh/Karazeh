#include "karazeh/patcher.hpp"
#include "karazeh/path_resolver.hpp"
#include "karazeh/version_manifest.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using kzh::string_t;

static int apply_updates(kzh::config_t&);

int main(int argc, char** argv) {

  kzh::config_t config;

  if (argc > 1) {
    for (int i = 0; i < argc; ++i) {
      string_t arg = argv[i];

      if (arg == "-r" && argc > i) {
        config.root_path = string_t(argv[++i]);
      }
      else if (arg == "-h") {
        config.host = string_t(argv[++i]);
      }
      else if (arg == "-v") {
        config.verbose = true;
      }
    }
  }

  try {
    return apply_updates(config);
  }
  catch (std::exception &e) {
    std::cerr << "Patching failed! Details:" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
}

int apply_updates(kzh::config_t &config) {
  kzh::logger::enable_timestamps(false);
  kzh::logger logger("test");
  kzh::path_resolver path_resolver;
  kzh::file_manager file_manager;
  kzh::md5_hasher hasher;
  kzh::downloader downloader(config, file_manager);

  path_resolver.resolve(config.root_path);

  // create the cache folders if they doesn't exist
  fs::create_directories(path_resolver.get_cache_path());

  config.root_path = path_resolver.get_root_path();
  config.cache_path = path_resolver.get_cache_path();

  if (config.host.empty()) {
    config.host = "http://localhost:9393";
  }

  config.hasher = &hasher;
  config.file_manager = &file_manager;
  config.downloader = &downloader;

  kzh::patcher patcher(config);
  kzh::version_manifest version_manifest(config);

  version_manifest.load_from_uri(config.host + "/manifests/version.json");

  const string_t current_version(version_manifest.get_current_version());

  if (current_version.empty()) {
    logger.error() << "Unable to identify current version!";
    return 1;
  }

  logger.info() << "Current version: " << current_version;

  auto available_updates = version_manifest.get_available_updates(current_version);

  if (available_updates.size() > 0) {
    logger.info() << available_updates.size() << " updates are available.";

    for (auto release_id : available_updates) {
      auto release = version_manifest.get_release(release_id);

      if (patcher.apply_update(*release) != kzh::STAGE_OK) {
        logger.error() << "Update " << release << " could not be applied!";
        return 1;
      }
    }
  }
  else {
    logger.info() << "No updates available, ciao!";
  }

  return 0;
}