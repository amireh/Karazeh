#include "karazeh/karazeh.hpp"
#include "karazeh/settings.hpp"

namespace kzh {
  int karazeh_init(int argc, char** argv) {
    settings::set_defaults();

	return 0;
  }
  
  int karazeh_cleanup() {
    return 0;
  }
}