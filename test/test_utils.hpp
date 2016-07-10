#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"

namespace kzh {
  typedef struct {
    string_t server_host;
    path_t   fixture_path;
    path_t   temp_path;
  } test_config_t;

  extern test_config_t test_config;
  extern config_t      sample_config;
}
