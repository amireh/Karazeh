#include "catch.hpp"
#include "test_utils.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/path_resolver.hpp"

using namespace kzh;

TEST_CASE("PathResolver") {
  path_resolver subject;
  subject.resolve(test_config.fixture_path / "sample_application/0.1.2");

  SECTION("resolving paths") {
    REQUIRE("0.1.2" == subject.get_root_path().filename());

    REQUIRE(
      path_t(subject.get_root_path() / ".kzh/cache").make_preferred().string() ==
      subject.get_cache_path().string()
    );
  }

  SECTION("overriding paths") {
    subject.resolve("./tmp_path");

    REQUIRE( "tmp_path" == subject.get_root_path().filename() );
    REQUIRE(
      path_t("./tmp_path/.kzh/cache").make_preferred().string() ==
      path_t(subject.get_cache_path()).make_preferred().string()
    );
  }
}
