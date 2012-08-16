#include "karazeh/patcher.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "karazeh/settings.hpp"

int main(int argc, char** argv) {
  using namespace kzh;

  string_t root_path;
  logger::enable_timestamps(false);
  if (argc > 1) {
    for (int i = 0; i < argc; ++i) {
      string_t arg = argv[i];
      if (arg == "-r" && argc > i) {
        root_path = string_t(argv[++i]);
      }
      else if (arg == "-v") {
        settings::enable("-v");
      }
      else if (arg == "-vv") {
        settings::enable("-vv");
      }
    }
  }

  logger l("test");

  resource_manager rmgr("http://localhost:9333");
  rmgr.resolve_paths(root_path);

  md5_hasher my_hasher;
  hasher::assign_hasher(&my_hasher);

  patcher p(rmgr);
  if (p.identify("/version.xml")) {
    if (p.is_update_available()) {
      if (p.apply_next_update()) {
        l.info() << "Successfully updated";
      } else {
        l.error() << "Wasn't able to update.";
      }
    }
  }

  return 0;
}