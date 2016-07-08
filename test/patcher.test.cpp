#include <gtest/gtest.h>
#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include "karazeh/patcher.hpp"
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
      rmgr_ = new resource_manager(KZH_SERVER_ADDRESS.string());
      patcher_ = new patcher(*rmgr_);

      rmgr_->override_paths(KZH_FIXTURE_PATH / "ktest" / "current");
    }

    virtual void TearDown() {
      delete patcher_;
      delete rmgr_;
    }

    static void SetUpTestCase() {
    }

    virtual bool load_manifest(path_t const& p, XMLDocument& doc) {
      using namespace tinyxml2;

      string_t manifest_xml;
      rmgr_->get_resource((KZH_SERVER_ADDRESS / "manifests" / p).string(), manifest_xml);

      int xml_rc = XML_SUCCESS;
      xml_rc = doc.Parse(manifest_xml.c_str());
      if (xml_rc != XML_SUCCESS) {
        return false;
      }

      return true;
    }

    resource_manager  *rmgr_;
    patcher           *patcher_;
  };

  TEST_F(patcher_test, loading_a_nonexistent_manifest) {
    ASSERT_THROW(
      patcher_->identify((KZH_SERVER_ADDRESS / "manifests/non_existent_manifest.xml").string()),
      kzh::invalid_resource);
  }

  TEST_F(patcher_test, loading_an_empty_manifest) {
    ASSERT_THROW(
      patcher_->identify((KZH_SERVER_ADDRESS / "manifests/empty.xml").string()),
      kzh::missing_children);
  }

  TEST_F(patcher_test, manifest_missing_identity_list) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_ilist.xml", m));
    ASSERT_THROW_WITH(
      patcher_->identify(m),
      kzh::invalid_manifest,
      "no identity list defined");
  }

  TEST_F(patcher_test, manifest_invalid_identity_list_missing_name) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_ilist_name.xml", m));
    ASSERT_THROW_WITH(
      patcher_->identify(m),
      kzh::manifest_error,
      "missing required attribute 'name'");
  }

  TEST_F(patcher_test, manifest_invalid_identity_list_missing_files) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_ilist_files.xml", m));
    ASSERT_THROW_WITH(
      patcher_->identify(m),
      kzh::manifest_error,
      "must have file nodes");
  }

  TEST_F(patcher_test, manifest_invalid_identity_list_missing_releases) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_releases.xml", m));
    ASSERT_THROW_WITH(
      patcher_->identify(m),
      kzh::missing_node,
      "missing child release");
  }

  TEST_F(patcher_test, manifest_invalid_identity_list_missing_initial_release) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_missing_initial_release.xml", m));
    ASSERT_THROW_WITH(
      patcher_->identify(m),
      kzh::invalid_manifest,
      "missing initial release node");
  }

  TEST_F(patcher_test, manifest_identity_list_undefined) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_undefined_ilist.xml", m));
    ASSERT_THROW_WITH(
      patcher_->identify(m),
      kzh::invalid_manifest,
      "<release> node points to an undefined identity list");
  }

  TEST_F(patcher_test, manifest_unmapped_release) {
    XMLDocument m;
    ASSERT_TRUE(load_manifest("invalid_vm_unmapped_release.xml", m));
    ASSERT_THROW_WITH(
      patcher_->identify(m),
      kzh::missing_attribute,
      "missing required attribute identity");
  }
} // namespace kzh
