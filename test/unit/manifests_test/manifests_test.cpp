/*
 *  Copyright (c) 2011-2012 Ahmad Amireh <kandie@mxvt.net>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */


#include "manifests_test/manifests_test.hpp"
#include "karazeh/patcher.hpp"
#include "karazeh/hashers/md5_hasher.hpp"
#include "tinyxml2/tinyxml2.h"

namespace kzh {
  
  using namespace tinyxml2;

  manifests_test::manifests_test() : test("manifests") {
  }

  manifests_test::~manifests_test() {
  }

  static bool load_manifest(path_t const& p, XMLDocument& doc) {
    resource_manager rm;

    string_t manifest_xml;
    rm.load_file(p, manifest_xml);

    int xml_rc = XML_NO_ERROR;
    xml_rc = doc.Parse(manifest_xml.c_str());
    if (xml_rc != XML_NO_ERROR) {
      return false;
    }

    return true;
  }

  int manifests_test::run(int argc, char** argv) {
    test::run(argc, argv);

    resource_manager rm("");
    rm.resolve_paths();

    md5_hasher h;
    hasher::assign_hasher(&h);

    stage("Non-existent manifest");
    {
      patcher p(rm);
      try {
        p.identify((fixture_path_ / "manifests/non_existent_manifest.xml").string());
      } catch(kzh::invalid_resource& e) {
        soft_assert("invalid_resource exception thrown", true);
      }
    }

    stage("Empty manifest");
    {
      patcher p(rm);
      try {
        p.identify((fixture_path_ / "manifests/empty.xml").string());
      } catch(kzh::missing_children& e) {
        soft_assert("missing_children exception thrown", true);
      }
    }

    stage("Missing Identity List");
    {
      XMLDocument m;
      hard_assert("loading manifest", load_manifest(fixture_path_ / "manifests/invalid_vm_missing_ilist.xml", m));

      patcher p(rm);
      try {
        p.identify(m);
      } catch(kzh::invalid_manifest& e) {
        soft_assert("invalid_manifest exception thrown", e.what() == string_t("No identity list defined."));
      }
    }

    stage("Invalid Identity List (missing name)");
    {
      XMLDocument m;
      hard_assert("loading manifest", load_manifest(fixture_path_ / "manifests/invalid_vm_missing_ilist_name.xml", m));

      patcher p(rm);
      try {
        p.identify(m);
      } catch(kzh::manifest_error& e) {
        soft_assert("manifest_error exception thrown", 
          e.what() == string_t("Manifest error: : Missing required attribute 'name' in <identity>"));
      }
    }

    stage("Invalid Identity List (missing files)");
    {
      XMLDocument m;
      hard_assert("loading manifest", load_manifest(fixture_path_ / "manifests/invalid_vm_missing_ilist_files.xml", m));

      patcher p(rm);
      try {
        p.identify(m);
      } catch(kzh::missing_node& e) {
        debug() << e.what();
        soft_assert("missing_node exception thrown", 
          e.what() == string_t("Manifest error: : Parent node <identity name=\"general\"> is missing a required child <file>"));
      }
    }

    stage("Missing releases");
    {
      patcher p(rm);
      try {
        p.identify((fixture_path_ / "manifests/invalid_vm_missing_releases.xml").string());
      } catch(kzh::missing_node& e) {
        debug() << e.what();
        soft_assert("missing_node exception thrown", 
          string_t(e.what()).find("Parent node <karazeh> is missing a required child <release>") != string_t::npos);
      }
    }

    stage("Missing initial release");
    {
      patcher p(rm);
      try {
        p.identify((fixture_path_ / "manifests/invalid_vm_missing_initial_release.xml").string());
      } catch(kzh::invalid_manifest& e) {
        debug() << e.what();
        soft_assert("invalid_manifest exception thrown", 
          e.what() == string_t("No initial release entry defined."));
      }
    }

    stage("Release points to undefined identity list");
    {
      patcher p(rm);
      try {
        p.identify((fixture_path_ / "manifests/invalid_vm_undefined_ilist.xml").string());
      } catch(kzh::invalid_manifest& e) {
        debug() << e.what();
        soft_assert("invalid_manifest exception thrown", 
          e.what() == string_t("<release> node points to an undefined identity list general_xxx"));
      }
    }
    stage("Release points to no identity list");
    {
      patcher p(rm);
      try {
        p.identify((fixture_path_ / "manifests/invalid_vm_unmapped_release.xml").string());
      } catch(kzh::missing_attribute& e) {
        debug() << e.what();
        soft_assert("missing_attribute exception thrown", 
          string_t(e.what()).find("Missing required attribute 'identity' in") != string_t::npos);
      }
    }

    return passed;
  }

}
