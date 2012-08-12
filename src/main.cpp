#include "karazeh.hpp"
#include "logger.hpp"
#include "MurmurHash3/MurmurHash3.h"
#include <stdlib.h>
#include <time.h>
#include "hashers/md5_hasher.hpp"

int main(int argc, char** argv) {
  using namespace kzh;

  if (argc < 2) {
    std::cout << "Missing required argument: path to file to hash\n";
    return 1;
  }

  logger* l = new logger("main");

  string_t path(argv[1]);
  std::ifstream fh(path.c_str());
  hasher::digest_rc digest_rc = md5_hasher().hex_digest(fh);

  if (!digest_rc.valid) {
    delete l;
    return 1;
  }

  string_t digest = digest_rc.digest;
  fh.close();

  l->debug() << "Hash:" << digest;
  delete l;

  return 0;
}