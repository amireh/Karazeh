#include "catch.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/patcher.hpp"
#include "karazeh/path_resolver.hpp"
#include "test_utils.hpp"
#include "tinyxml2/tinyxml2.h"

using namespace kzh;
using namespace Catch::Matchers;

TEST_CASE("Patcher - Manifest validation", "[Patcher]") {
  config_t              config_;
  path_resolver         path_resolver_;
  file_manager          file_manager_;
  tinyxml2::XMLDocument manifest_doc_;

  path_resolver_.resolve(test_config.fixture_path / "sample_application/0.1.2");
  config_.host = test_config.server_host;
  config_.root_path = path_resolver_.get_root_path();
  config_.cache_path = path_resolver_.get_cache_path();

  downloader  downloader_(file_manager_, config_);
  patcher     subject(config_, file_manager_, downloader_);

  auto load_manifest = [&](string_t const& p, tinyxml2::XMLDocument& doc) -> bool {
    string_t manifest_xml;

    downloader_.fetch(config_.host + "/manifests/" + p, manifest_xml);

    return doc.Parse(manifest_xml.c_str()) == tinyxml2::XML_SUCCESS;
  };

  GIVEN("A bad URI to a manifest...") {
    THEN("it throws") {
      REQUIRE_THROWS_AS(
        subject.identify(config_.host + "/manifests/non_existent_manifest.xml"),
        kzh::invalid_resource
      );
    }
  }

  GIVEN("An empty manifest...") {
    THEN("it throws") {
      REQUIRE_THROWS_AS(
        subject.identify(config_.host + "/manifests/empty.xml"),
        kzh::missing_children
      );
    }
  }

  GIVEN("No <identity /> list") {
    REQUIRE(load_manifest("invalid_vm_missing_ilist.xml", manifest_doc_));

    THEN("it throws") {
      REQUIRE_THROWS_WITH(
        subject.identify(manifest_doc_),
        Equals("no identity list defined.", Catch::CaseSensitive::No)
      );
    }
  }

  GIVEN("A manifest with an identity list missing the name parameter") {
    REQUIRE(load_manifest("invalid_vm_missing_ilist_name.xml", manifest_doc_));

    THEN("it throws") {
      REQUIRE_THROWS_WITH(
        subject.identify(manifest_doc_),
        Contains("missing required attribute 'name'", Catch::CaseSensitive::No)
      );
    }
  }

  GIVEN("A manifest with an identity list with no files") {
    REQUIRE(load_manifest("invalid_vm_missing_ilist_files.xml", manifest_doc_));

    THEN("it throws") {
      REQUIRE_THROWS_WITH(
        subject.identify(manifest_doc_),
        Contains("node <identity name=\"general\"> must have <file> nodes to define identity files", Catch::CaseSensitive::No)
      );
    }
  }

  GIVEN("A manifest with an identity list missing <release /> nodes") {
    REQUIRE(load_manifest("invalid_vm_missing_releases.xml", manifest_doc_));

    THEN("it throws") {
      REQUIRE_THROWS_WITH(
        subject.identify(manifest_doc_),
        Contains("missing a required child <release>", Catch::CaseSensitive::No)
      );
    }
  }

  GIVEN("A manifest with no initial <release />") {
    REQUIRE(load_manifest("invalid_vm_missing_initial_release.xml", manifest_doc_));

    THEN("it throws") {
      REQUIRE_THROWS_WITH(
        subject.identify(manifest_doc_),
        Contains("missing initial release node", Catch::CaseSensitive::No)
      );
    }
  }

  GIVEN("A <release /> tag that points to an undefined identity list...") {
    REQUIRE(load_manifest("invalid_vm_undefined_ilist.xml", manifest_doc_));

    THEN("it throws") {
      REQUIRE_THROWS_WITH(
        subject.identify(manifest_doc_),
        Contains("<release> node points to an undefined identity list", Catch::CaseSensitive::No)
      );
    }
  }

  GIVEN("A <release /> with no [identity]") {
    REQUIRE(load_manifest("invalid_vm_unmapped_release.xml", manifest_doc_));

    THEN("it throws") {
      REQUIRE_THROWS_WITH(
        subject.identify(manifest_doc_),
        Contains("missing required attribute 'identity'", Catch::CaseSensitive::No)
      );
    }
  }
}
