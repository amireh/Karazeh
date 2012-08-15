#include "karazeh/patcher.hpp"

int main(int argc, char** argv) {
  using namespace kzh;

  logger::enable_timestamps(false);
  if (argc > 1) {
    for (int i = 0; i < argc; ++i) {
      string_t arg = argv[i];
      if (arg == "-r" && argc > i) {
        resource_manager rmgr;
        rmgr.resolve_paths(string_t(argv[++i]));
      }
    }
  }

  patcher p;
  if (p.identify("http://localhost:9333/version.xml")) {
    p.is_update_available();    
  }

  return 0;
}