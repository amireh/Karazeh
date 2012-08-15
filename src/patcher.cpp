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

  patcher::patcher()
  : logger("patcher") {

  }

  patcher::~patcher() {

  }

  bool patcher::identify(string_t const& manifest_uri) {
    string_t manifest_xml;
    resource_manager rmgr;
    
    if (!rmgr.get_remote(manifest_uri, manifest_xml)) {
      throw invalid_resource(manifest_uri);
    }

    using namespace tinyxml2;

    /** Parse the manifest */
    int xml_rc = XML_NO_ERROR;
    xml_rc = version_manifest_.Parse(manifest_xml.c_str());
    if (xml_rc != XML_NO_ERROR) {
      error() << "Unable to parse version manifest, xml error rc: " << xml_rc;
      throw invalid_manifest("Version manifest could not be parsed.");
    }

    XMLElement* root = version_manifest_.RootElement();

    /** Now we build the identity map, a list of files to be checksummed
        to identify the application's version */
    XMLElement *identity_node = root->FirstChildElement("identity");
    if (!identity_node || identity_node->NoChildren()) {
      error() << "Version manifest is missing the identity map, or it's an empty one!";
      throw invalid_manifest("Identity map is either missing or has no entries.");
    }
  
    identity_node = identity_node->FirstChildElement();
    while (identity_node) {
      identity_files_.push_back(identity_node->FirstChild()->Value());
      identity_node = identity_node->NextSiblingElement();

      debug() << "Identity file: " << identity_files_.back();
      
      /** verify that the file exists and is readable */
      
    }
  }

  bool patcher::is_identified() const {
    return !version_.empty();
  }

  bool patcher::is_update_available() {
    /** Download the version manifest */
    string_t manifest;
    string_t manifest_uri("http://localhost:9333/version.xml");
    resource_manager rmgr;
    
    if (!rmgr.get_remote(manifest_uri, manifest)) {
      throw invalid_resource(manifest_uri);
    }

    using namespace tinyxml2;

    /** Parse the manifest */
    int xml_rc = XML_NO_ERROR;
    XMLDocument doc;
    xml_rc = doc.Parse(manifest.c_str());
    if (xml_rc != XML_NO_ERROR) {
      error() << "Unable to parse version manifest, xml error rc: " << xml_rc;
      throw invalid_manifest("version manifest");
    }

    XMLElement* root = doc.RootElement();


    XMLNode *refspec = root->FirstChild();

    typedef std::map<string_t, string_t> rm_t;
    rm_t release_manifests;
    while (refspec) {
      release_manifests.insert(
        std::make_pair(refspec->ToElement()->Attribute("version"),
                       refspec->ToElement()->Attribute("url")));
      refspec = refspec->NextSibling();
    }

    for (rm_t::const_iterator rm = release_manifests.begin(); rm != release_manifests.end(); ++rm) {
      info() << rm->first << " => " << rm->second;
    }
    return false;
  }
}