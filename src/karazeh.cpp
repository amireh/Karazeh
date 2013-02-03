#include "karazeh/karazeh.hpp"
#include "karazeh/settings.hpp"

namespace kzh {
  int karazeh_init(int argc, char** argv) {
    settings::set_defaults();
  }
  
  int karazeh_cleanup() {
    
  }
}