#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/path_resolver.hpp"

namespace kzh {
  namespace fs = boost::filesystem;

  class path_resolver_test : public ::testing::Test {
  protected:
    virtual void SetUp() {
      ASSERT_NO_THROW(subject.resolve(test_config.fixture_path / "sample_application/0.1.2"));
    }

    virtual void TearDown() {
    }

    path_resolver subject;
  };

  TEST_F(path_resolver_test, resolving_paths) {
    using utility::split;

    EXPECT_EQ(
      "0.1.2",
      split(subject.get_root_path().string(), '/').back()
    );

    EXPECT_EQ(
      subject.get_root_path().string() + "/.kzh/cache",
      subject.get_cache_path().string()
    );
  }

  TEST_F(path_resolver_test, overriding_paths) {
    using utility::split;

    ASSERT_NO_THROW(subject.resolve("./tmp_path"));

    EXPECT_EQ( "tmp_path", subject.get_root_path().filename() );
    EXPECT_EQ(
      path_t("./tmp_path/.kzh/cache").make_preferred().string(),
      subject.get_cache_path().string()
    );
  }
}
