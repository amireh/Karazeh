#include "catch.hpp"
#include "karazeh/karazeh.hpp"
#include "karazeh/version_manifest.hpp"
#include "karazeh/path_resolver.hpp"
#include "test_utils.hpp"

using namespace kzh;
using namespace Catch::Matchers;

TEST_CASE("VersionManifest") {
  config_t config(sample_config);
  config.root_path = test_config.fixture_path / "sample_application/0.1.2";

  version_manifest subject(config);

  // helper for loading test/fixture/sample_application/manifests/version.json
  // which we base most integration tests off of
  const auto load_functional_manifest = [&]() {
    string_t functional_manifest_string;

    REQUIRE(
      config.file_manager->load_file(
        test_config.fixture_path / "sample_application/manifests/version.json",
        functional_manifest_string
      )
    );

    subject.load_from_string(functional_manifest_string);
  };

  const auto parse_json = [&](string_t const& src) -> JSON {
    string_t parse_error;

    const JSON json = JSON::parse(src, parse_error);

    REQUIRE(parse_error.empty());

    return json;
  };

  SECTION("#parse()") {
    GIVEN("A manifest with no identity lists...") {
      THEN("it throws") {
        REQUIRE_THROWS_WITH(
          subject.load_from_string(R"VOGON(
          {
            "identities": [
            ],

            "releases": [{}]
          }
          )VOGON"),

          Equals("Version manifest must contain at least one identity list.", Catch::CaseSensitive::No)
        );
      }
    }

    GIVEN("An identity list missing the name parameter") {
      THEN("it throws") {
        REQUIRE_THROWS_WITH(
          subject.load_from_string(R"VOGON(
          {
            "identities": [
              {
              }
            ],

            "releases": [{}]
          }
          )VOGON"),
          Contains("Malformed entity: bad type for name", Catch::CaseSensitive::No)
        );
      }
    }

    GIVEN("An identity list with no files") {
      THEN("it throws") {
        REQUIRE_THROWS_WITH(
          subject.load_from_string(R"VOGON(
          {
            "identities": [
              {
                "name": "Vanilla",
                "files": []
              }
            ],

            "releases": [{}]
          }
          )VOGON"),
          Contains("Identity list (Vanilla) must contain at least one file entry.", Catch::CaseSensitive::No)
        );
      }
    }

    // Gonna relax this restriction since we want to be able to inject multiple
    // manifests into the same object (e.g. when loading from multiple sources).
    //
    // GIVEN("A manifest with no initial release entry") {
    //   THEN("it throws") {
    //     REQUIRE_THROWS_WITH(
    //       subject.load(R"VOGON(
    //       {
    //         "identities": [
    //           {
    //             "name": "Vanilla",
    //             "files": [ "bin/test" ]
    //           }
    //         ],

    //         "releases": [{
    //           "id": "asdf",
    //           "tag": "1.0.0",
    //           "identity": "Vanilla"
    //         }]
    //       }
    //       )VOGON"),
    //       Contains("Version manifest is missing an initial release.", Catch::CaseSensitive::No)
    //     );
    //   }
    // }

    GIVEN("A release pointing to an unknown identity list") {
      THEN("it throws") {
        REQUIRE_THROWS_WITH(
          subject.load_from_string(R"VOGON(
          {
            "identities": [{ "name": "Vanilla", "files": [ "bin/test" ] }],
            "releases": [{
              "id": "asdf",
              "tag": "1.0.0",
              "identity": "Foobar"
            }]
          }
          )VOGON"),
          Contains("Release (asdf) points to an undefined identity list (Foobar).", Catch::CaseSensitive::No)
        );
      }
    }
  }

  SECTION("#get_current_version()") {
    load_functional_manifest();

    REQUIRE(subject.get_release_count() == 3);

    SECTION("it works") {
      REQUIRE(
        subject.get_current_version() == "f265230773c54396fbf4da894127cfa8"
      );
    }
  }

  SECTION("#get_available_updates()") {
    load_functional_manifest();

    REQUIRE(subject.get_available_updates("bae3d8f9b767a12336768dacf72cb0de").size() == 2);
    REQUIRE(subject.get_available_updates("ebb5dcbf784e0ef2fe6c37dae8d52722").size() == 1);
    REQUIRE(subject.get_available_updates("f265230773c54396fbf4da894127cfa8").size() == 0);

    GIVEN("An invalid release ID") {
      THEN("It doesn't bork") {
        REQUIRE(subject.get_available_updates("asdfasdf").size() == 0);
      }
    }
  }

  SECTION("Parsing release manifests") {
    load_functional_manifest();

    SECTION("Parsing a \"create\" operation") {
      subject.parse_release(parse_json(
        R"VOGON({
          "id": "my fake release",
          "identity": "Base",
          "operations": [
            {
              "type": "create",
              "source": {
                "url": "/0.1.1/data/media/materials/programs/celshader.cg",
                "checksum": "3858f62230ac3c915f300c664312c63f",
                "size": 6
              },

              "destination": "/data/media/materials/programs/celshader.cg"
            }
          ]
        })VOGON"
      ));

      REQUIRE(subject.get_release("my fake release")->operations.size() == 1);

      auto op = static_cast<create_operation*>(
        subject.get_release("my fake release")->operations.front()
      );

      REQUIRE(op->src_uri == "/0.1.1/data/media/materials/programs/celshader.cg");
      REQUIRE(op->src_checksum == "3858f62230ac3c915f300c664312c63f");
      REQUIRE(op->src_size == 6);
      REQUIRE(op->dst_path == "/data/media/materials/programs/celshader.cg");
    }

    SECTION("Parsing an \"update\" operation") {
      subject.parse_release(parse_json(
        R"VOGON({
          "id": "my fake release",
          "identity": "Base",
          "operations": [
            {
              "type": "update",
              "basis": {
                "pre_checksum": "427fbbb5a80b517719defe07f7545686",
                "post_checksum": "72eda360361e155ad8eabd07f07fa017",
                "filepath": "/data/common.tar"
              },

              "delta": {
                "checksum": "b02c5026a9e24d0cdefa19641077ca91",
                "size": 60058,
                "url": "/patch_v0.1.1-v0.1.2/data_common.tar.delta"
              }
            }
          ]
        })VOGON"
      ));

      REQUIRE(subject.get_release("my fake release")->operations.size() == 1);

      auto op = static_cast<update_operation*>(
        subject.get_release("my fake release")->operations.front()
      );

      REQUIRE(op->basis == "/data/common.tar");
      REQUIRE(op->basis_checksum == "427fbbb5a80b517719defe07f7545686");
      REQUIRE(op->patched_checksum == "72eda360361e155ad8eabd07f07fa017");
      REQUIRE(op->delta == "/patch_v0.1.1-v0.1.2/data_common.tar.delta");
      REQUIRE(op->delta_checksum == "b02c5026a9e24d0cdefa19641077ca91");
      REQUIRE(op->delta_length == 60058);
    }

    SECTION("Parsing a \"delete\" operation") {
      subject.parse_release(parse_json(
        R"VOGON({
          "id": "my fake release",
          "identity": "Base",
          "operations": [
            {
              "type": "delete",
              "target": "/bin/test"
            }
          ]
        })VOGON"
      ));

      REQUIRE(subject.get_release("my fake release")->operations.size() == 1);

      auto op = static_cast<delete_operation*>(
        subject.get_release("my fake release")->operations.front()
      );

      REQUIRE(op->dst_path == "/bin/test");
    }
  }
}
