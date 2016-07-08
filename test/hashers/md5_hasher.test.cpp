#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/hashers/md5_hasher.hpp"

namespace kzh {
  TEST(md5_hasher_test, calculating_digest) {
    md5_hasher h;

    hasher::digest_rc drc = h.hex_digest(KZH_FIXTURE_PATH / "hash_me.txt");

    ASSERT_TRUE(drc.valid);
    ASSERT_EQ(string_t("f1eb970aeb2e380593480ed76070acbe"), drc.digest);
  }
}