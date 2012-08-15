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

#include "karazeh/patcher.hpp"

namespace kzh {

  std::ostream& operator<<(std::ostream& s, release_manifest* rm) {
    s << "release[" << rm->tag << "] => " << rm->uri;
  }

  patcher::patcher(resource_manager& rmgr)
  : logger("patcher"),
    rmgr_(rmgr)
  {

  }

  patcher::~patcher() {
    while (!identifiers_.empty()) {
      delete identifiers_.back();
      identifiers_.pop_back();
    }

    while (!rmanifests_.empty()) {
      delete rmanifests_.back();
      rmanifests_.pop_back();
    }


  }

  bool patcher::identify(string_t const& in_manifest_uri) {
    string_t manifest_uri(rmgr_.host_address() + in_manifest_uri);
    string_t manifest_xml;
    
    if (!rmgr_.get_remote(manifest_uri, manifest_xml)) {
      throw invalid_resource(manifest_uri);
    }

    using namespace tinyxml2;

    /** Parse the manifest */
    int xml_rc = XML_NO_ERROR;
    xml_rc = vmanifest_.Parse(manifest_xml.c_str());
    if (xml_rc != XML_NO_ERROR) {
      error() << "Unable to parse version manifest, xml error rc: " << xml_rc;
      throw invalid_manifest("Version manifest could not be parsed.");
    }

    XMLElement* root = vmanifest_.RootElement();

    /** Now we build the identity map, a list of files to be checksummed
        to identify the application's version */
    XMLElement *identity_node = root->FirstChildElement("identity");
    if (!identity_node || identity_node->NoChildren()) {
      error() << "Version manifest is missing the identity map, or it's an empty one!";
      throw invalid_manifest("Identity map is either missing or has no entries.");
    }
  
    identity_node = identity_node->FirstChildElement();
    string_t checksums = "";
    while (identity_node) {
      identifier *id = new identifier();
      id->filepath = (rmgr_.root_path() / identity_node->FirstChild()->Value());
      id->checksum = "";

      identifiers_.push_back(id);

      /** verify that the file exists and is readable */
      if (!rmgr_.is_readable(id->filepath)) {
        throw integrity_violation("Identity file " + id->filepath.string() + " is not readable.");
      }

      /** calculate the checksum */
      string_t buf;
      rmgr_.load_file(id->filepath.string(), buf);
      hasher::digest_rc rc = hasher::instance()->hex_digest(buf);
      if (!rc.valid) {
        throw integrity_violation("Identity file " + id->filepath.string() + " could not be digested.");
      }

      id->checksum = rc.digest;
      debug() << "Identity file: " << id->filepath.string() << "#" << id->checksum;
      checksums += id->checksum;

      identity_node = identity_node->NextSiblingElement();
    }
    debug() << checksums;
    version_ = hasher::instance()->hex_digest(checksums).digest;
    info() << "Version: " << version_;

    return true;
  }

  bool patcher::is_identified() const {
    return !version_.empty();
  }

  identity_t const& patcher::version() const {
    return version_;
  }

  bool patcher::is_update_available() {
    using namespace tinyxml2;

    /** Make sure the version has been identified in identify() */
    if (!is_identified()) {
      throw invalid_state("Version must be identified before attempting to check for patch availability.");
    }

    assert(!vmanifest_.Error());

    XMLElement *root = vmanifest_.RootElement();

    /** Go through each release manifest entry and attempt
      * to locate our version.
      */
    XMLElement *rm_el = root->FirstChildElement("release");
    bool our_version_located = false;
    while (rm_el) {
      rm_el = rm_el->ToElement();

      /** checksum attribute is required */
      if (!rm_el->Attribute("checksum"))
        throw invalid_manifest("Version manifest <release> entry is missing required 'checksum' attribute!");
      /** so is the uri attribute */
      if (!rm_el->Attribute("uri") && !rm_el->Attribute("initial"))
        throw invalid_manifest("Version manifest <release> entry is missing required 'uri' attribute!");
        
      release_manifest *rm = new release_manifest();
      rm->checksum = string_t(rm_el->Attribute("checksum"));

      /** the initial release has no manifest */
      if (!rm_el->Attribute("initial"))
        rm->uri = rm_el->Attribute("uri");

      /** tag is optional */
      if (rm_el->Attribute("tag")) {
        rm->tag = rm_el->Attribute("tag");
      } else { /** print a warning if it's not set */
        warn() 
          << "Version manifest <release uri='" << rm->uri << "'"
          << "> entry does not have a 'tag' attribute!";
      }

      if (our_version_located) {
        /** append to the list of new releases to be applied later */
        new_releases_.push_back(rm);
      } else {
        debug() << "checking if " << rm->checksum << " is our version";
        if (rm->checksum == version_) {
          our_version_located = true;
        }
      }

      rmanifests_.push_back(rm);
      rm_el = rm_el->NextSiblingElement("release");
    }

    if (!our_version_located) {
      throw integrity_violation("Unable to locate current application version " + version_);
    }

    info() << "There are " << new_releases_.size() << " new releases available.";

    return !new_releases_.empty();
  }

  void patcher::apply_next_update() {
    if (new_releases_.empty()) {
      throw invalid_state("apply_next_update() requires at least one new release to apply!");
    }

    release_manifest *next_update = new_releases_.front();

    info() << "applying update: " << next_update;
  }
}