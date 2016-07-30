#ifndef H_KARAZEH_TEST_UTILS_H
#define H_KARAZEH_TEST_UTILS_H

#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"

#define FI_FILE_MANAGER_MOVE(x) ConstOverloadedMethod(x, move, bool(path_t const&, path_t const&))
#define FI_FILE_MANAGER_REMOVE_FILE(x) ConstOverloadedMethod(x, remove_file, bool(path_t const&))
#define FI_FILE_MANAGER_REMOVE_DIRECTORY(x) ConstOverloadedMethod(x, remove_directory, bool(path_t const&))
#define FI_FILE_MANAGER_IS_READABLE(x) ConstOverloadedMethod(x, is_readable, bool(path_t const&))
#define FI_FILE_MANAGER_IS_WRITABLE(x) ConstOverloadedMethod(x, is_writable, bool(path_t const&))
#define FI_FILE_MANAGER_MAKE_EXECUTABLE(x) ConstOverloadedMethod(x, make_executable, bool(path_t const&))
#define FI_HASHER_HEX_DIGEST(x) ConstOverloadedMethod(x, hex_digest, hasher::digest_rc(const path_t&))
#define FI_DOWNLOADER_FETCH(x) ConstOverloadedMethod(x, fetch, bool(string_t const&, const path_t&, string_t const&, int* const))

namespace kzh {
  typedef struct {
    string_t server_host;
    path_t   fixture_path;
    path_t   temp_path;
  } test_config_t;

  extern test_config_t test_config;
  extern config_t      sample_config;

  namespace test_utils {
    bool copy_directory(path_t const& source, path_t const& destination);
    void create_file(path_t const&, string_t const&);
    void remove_file(path_t const&);
  }
}

#endif