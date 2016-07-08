#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/hashers/md5_hasher.hpp"

namespace kzh {
  TEST(hasher_test, implicitly_unassigning_global_hasher_on_removal) {
    const hasher* const old_hasher(hasher::instance());

    hasher::assign_hasher(NULL);

    EXPECT_THROW(hasher::instance(), kzh::uninitialized);

    md5_hasher *h = new md5_hasher();
    hasher::assign_hasher(h);

    EXPECT_NO_THROW(hasher::instance());

    delete h;

    EXPECT_THROW(hasher::instance(), kzh::uninitialized);

    hasher::assign_hasher(old_hasher);
    EXPECT_NO_THROW(hasher::instance());
  }

}