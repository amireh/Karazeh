#include <gtest/gtest.h>
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/patcher.hpp"
#include "karazeh/path_resolver.hpp"
#include "test_utils.hpp"
#include "tinyxml2/tinyxml2.h"

#ifdef ASSERT_THROW
  #undef ASSERT_THROW
#endif

#define ASSERT_THROW KZH_ASSERT_THROW

namespace kzh {

  class patcher_test : public ::testing::Test {
  protected:
    virtual void SetUp() {
      path_resolver_.resolve(test_config.fixture_path / "ktest/current");

      config_.host = test_config.server_host;
      config_.root_path = path_resolver_.get_root_path();
      config_.cache_path = path_resolver_.get_cache_path();

      downloader_ = new downloader(file_manager_, config_);
      subject = new patcher(config_, file_manager_, *downloader_);
    }

    virtual void TearDown() {
      delete subject;
      delete downloader_;
    }

    virtual bool load_manifest(string_t const& p, XMLDocument& doc) {
      using namespace tinyxml2;

      string_t manifest_xml;
      downloader_->fetch(config_.host + "/manifests/" + p, manifest_xml);

      int xml_rc = XML_SUCCESS;
      xml_rc = doc.Parse(manifest_xml.c_str());
      if (xml_rc != XML_SUCCESS) {
        return false;
      }

      return true;
    }

    config_t          config_;
    patcher           *subject;
    path_resolver     path_resolver_;
    file_manager      file_manager_;
    downloader        *downloader_;
  };

  TEST_F(patcher_test, loading_a_nonexistent_manifest) {
    ASSERT_THROW(
      subject->identify(config_.host + "/manifests/non_existent_manifest.xml"),
      kzh::invalid_resource
    );
  }

  TEST_F(patcher_test, loading_an_empty_manifest) {
    ASSERT_THROW(
      subject->identify(config_.host + "/manifests/empty.xml"),
      kzh::missing_children
    );
  }

  TEST_F(patcher_test, manifest_missing_identity_list) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_ilist.xml", m));
    ASSERT_THROW_WITH(
      subject->identify(m),
      kzh::invalid_manifest,
      "no identity list defined");
  }

  TEST_F(patcher_test, manifest_invalid_identity_list_missing_name) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_ilist_name.xml", m));
    ASSERT_THROW_WITH(
      subject->identify(m),
      kzh::manifest_error,
      "missing required attribute 'name'");
  }

  TEST_F(patcher_test, manifest_invalid_identity_list_missing_files) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_ilist_files.xml", m));
    ASSERT_THROW_WITH(
      subject->identify(m),
      kzh::manifest_error,
      "must have file nodes");
  }

  TEST_F(patcher_test, manifest_invalid_identity_list_missing_releases) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_releases.xml", m));
    ASSERT_THROW_WITH(
      subject->identify(m),
      kzh::missing_node,
      "missing child release");
  }

  TEST_F(patcher_test, manifest_invalid_identity_list_missing_initial_release) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_initial_release.xml", m));
    ASSERT_THROW_WITH(
      subject->identify(m),
      kzh::invalid_manifest,
      "missing initial release node");
  }

  TEST_F(patcher_test, manifest_identity_list_undefined) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_undefined_ilist.xml", m));
    ASSERT_THROW_WITH(
      subject->identify(m),
      kzh::invalid_manifest,
      "<release> node points to an undefined identity list");
  }

  TEST_F(patcher_test, manifest_unmapped_release) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_unmapped_release.xml", m));
    ASSERT_THROW_WITH(
      subject->identify(m),
      kzh::missing_attribute,
      "missing required attribute identity");
  }
} // namespace kzh
