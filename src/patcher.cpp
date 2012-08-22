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

  patcher::identity_list::identity_list(string_t const& n)
  : name(n) {

  }
  
  patcher::identity_list::~identity_list() {
    while (!files.empty()) {
      delete files.back();
      files.pop_back();
    }
  }

  patcher::patcher(resource_manager& rmgr)
  : logger("patcher"),
    rmgr_(rmgr)
  {

  }

  patcher::~patcher() {
    for (identity_lists_t::iterator ilist = identity_lists_.begin();
      ilist != identity_lists_.end();
      ++ilist) {
      delete ilist->second;
    }

    identity_lists_.clear();

    while (!rmanifests_.empty()) {
      delete rmanifests_.back();
      rmanifests_.pop_back();
    }
  }

  bool patcher::identify(string_t const& manifest_uri) {
    string_t manifest_xml;

    current_manifest_uri_ = "version[" + manifest_uri + "]";
    
    if (!rmgr_.get_resource(manifest_uri, manifest_xml)) {
      throw invalid_resource(manifest_uri);
    }

    using namespace tinyxml2;

    /** Parse the manifest */
    int xml_rc = XML_NO_ERROR;
    xml_rc = vmanifest_.Parse(manifest_xml.c_str());
    if (xml_rc != XML_NO_ERROR) {
      // abort
      report_parser_error(xml_rc, vmanifest_);
    }

    return identify(vmanifest_);
  }

  bool patcher::identify(XMLDocument& doc) {
    using namespace tinyxml2;

    XMLElement* root = doc.RootElement();

    ensure_has_children(root);

    /** Now we parse all defined identity lists; files to be checksummed
        to identify the application's version */

    // ilist stands for identity_list
    // ifile stands for identity_file

    XMLElement *ilist_node = root->FirstChildElement("identity");

    if (!ilist_node) {
      error() << "Version manifest is missing identity lists";
      throw invalid_manifest("No identity list defined.");
    }

    while (ilist_node) {

      ensure_has_attribute(ilist_node, "name");
      ensure_has_children(ilist_node);

      identity_list *ilist = new identity_list(ilist_node->Attribute("name"));
      identity_lists_.insert(std::make_pair(ilist->name, ilist));
    
      XMLElement *ifile_node = ilist_node->FirstChildElement();
      while (ifile_node) {
        identity_file *ifile = new identity_file();
        ifile->filepath = (rmgr_.root_path() / ifile_node->GetText());
        ifile->checksum = "";

        ilist->files.push_back(ifile);

        /** verify that the file exists and is readable */
        if (!rmgr_.is_readable(ifile->filepath)) {
          throw manifest_error("Identity file " + ifile->filepath.string() + " is not readable.");
        }

        /** calculate the checksum */
        std::ifstream fh(ifile->filepath.string().c_str());
        hasher::digest_rc rc = hasher::instance()->hex_digest(fh);
        if (!rc.valid) {
          throw manifest_error("Identity file " + ifile->filepath.string() + " could not be digested.");
        }

        ifile->checksum = rc.digest;
        debug() << "ifile[" << ifile->filepath.string() << "] => " << ifile->checksum;
        ilist->checksum += ifile->checksum;

        ifile_node = ifile_node->NextSiblingElement();
      }

      ilist->checksum = hasher::instance()->hex_digest(ilist->checksum).digest;
      debug() << "ilist[" << ilist->name << "] => " << ilist->checksum;
      
      ilist_node = ilist_node->NextSiblingElement("identity");
    }

    // Now that the identity lists are built, we run through all the release entries
    // and attempt to find one whose checksum matches that of the identity list it 
    // points to
    ensure_has_child(root, "release");

    XMLElement *release_node = root->FirstChildElement("release");
    bool current_release_found = false;
    bool initial_release_found = false;
    while (release_node) {
      ensure_has_attribute(release_node, "identity");
      ensure_has_attribute(release_node, "checksum");

      string_t ilist_name = release_node->Attribute("identity");
      if (identity_lists_.find(ilist_name) == identity_lists_.end()) {
        error() << "<release> node points to an undefined identity list '" << ilist_name << "'!";
        throw invalid_manifest("<release> node points to an undefined identity list " + ilist_name);
      }

      identity_list* ilist = identity_lists_.find(ilist_name)->second;
      
      string_t release_checksum = release_node->Attribute("checksum");
      // debug() << "checking " << release_checksum  << " vs " << ilist->checksum;

      // is this our version?
      if (!current_release_found && release_checksum == ilist->checksum) {
        version_ = release_checksum;
        current_release_found = true;
        debug() << "Current release located: " << utility::dump_node(release_node);
      }

      if (release_node->Attribute("initial")) {
        debug() << "Intial release found: " << utility::dump_node(release_node);
        initial_release_found = true;
      }

      release_node = release_node->NextSiblingElement("release");
    }

    if (!initial_release_found) {
      throw invalid_manifest("No initial release entry defined.");
    }

    return current_release_found;
  }

  bool patcher::is_identified() const {
    return !version_.empty();
  }

  identity_t const& patcher::version() const {
    return version_;
  }

  uint32_t patcher::nr_available_updates() const {
    return new_releases_.size();
  }

  bool patcher::is_update_available() {
    using namespace tinyxml2;

    /** Make sure the version has been identified in identify() */
    if (!is_identified()) {
      throw invalid_state("Version must be identified before attempting to check for patch availability.");
    }

    assert(!vmanifest_.Error());

    XMLElement *root = vmanifest_.RootElement();

    // TODO: make sure there's an initial release entry

    /** Go through each release manifest entry and attempt
      * to locate our version.
      */
    XMLElement *rm_node = root->FirstChildElement("release");
    bool our_version_located = false;
    while (rm_node) {
      rm_node = rm_node->ToElement();

      // checksum attribute is required
      ensure_has_attribute(rm_node, "checksum");
      // so is the tag attribute
      ensure_has_attribute(rm_node, "tag");    
      // so is the uri attribute, unless it's the initial release
      if (!rm_node->Attribute("initial")) {
        ensure_has_attribute(rm_node, "uri");
      }

      release_manifest *rm = new release_manifest();
      rm->checksum = rm_node->StringAttribute("checksum");
      rm->tag = rm_node->Attribute("tag");
      if (rm_node->Attribute("uri"))
        rm->uri = rm_node->Attribute("uri");

      if (our_version_located) {
        // append to the list of new releases to be applied later
        new_releases_.push_back(rm);
      } else {
        debug() << "checking if " << rm->checksum << " is our version";

        if (rm->checksum == version_) {
          our_version_located = true;
          indent(); debug() << "yep, it is"; deindent();
        }
      }

      rmanifests_.push_back(rm);
      rm_node = rm_node->NextSiblingElement("release");
    }

    if (!our_version_located) {
      throw integrity_violation("Unable to locate current application version " + version_);
    }


    info() << "There are " << new_releases_.size() << " new releases available.";

    return !new_releases_.empty();
  }

  bool patcher::apply_next_update() {
    if (new_releases_.empty()) {
      throw invalid_state("apply_next_update() requires at least one new release to apply!");
    }

    release_manifest *next_update = new_releases_.front();

    info() << "applying update: " << next_update;

    // Download the manifest and parse it
    string_t rm_xml;
    
    if (!rmgr_.get_remote(next_update->uri, rm_xml)) {
      throw invalid_resource(next_update->uri);
    }

    current_manifest_uri_ = next_update->uri;

    using namespace tinyxml2;

    // Parse the manifest
    XMLDocument rm_doc;
    int xml_rc = XML_NO_ERROR;
    xml_rc = rm_doc.Parse(rm_xml.c_str());
    if (xml_rc != XML_NO_ERROR) {
      report_parser_error(xml_rc, rm_doc);
    }

    typedef std::vector<operation*> operations_t;
    std::map<path_t, bool> marked_for_deletion;

    struct patch_t {
      operations_t      operations;

      inline ~patch_t() {
        while (!operations.empty()) {
          delete operations.back();
          operations.pop_back();
        }        
      }
    } patch;

    // Go through each operation node and build up the patch
    XMLElement *root = rm_doc.RootElement();
    if (!root) {
      throw invalid_manifest("Release manifest seems to be empty!");
    }

    XMLElement *release = root->FirstChildElement("release");
    if (!release) {
      throw missing_node("karazeh", "release", next_update->tostring());
    }

    ensure_has_children(release);

    XMLElement* op_node = release->FirstChildElement();
    while (op_node) {
      using utility::tonumber;

      debug() << op_node->Value();
      if (strcmp(op_node->Value(), "create") == 0) {
        // <create> operations have two nodes: <source> and <destination>

        // <source checksum="a-z0-9" size="0-9">PATH</source>
        ensure_has_child(op_node, "source");
        ensure_has_child(op_node, "destination");

        XMLElement *src_node = op_node->FirstChildElement("source");

        ensure_has_attribute(src_node, "checksum");
        ensure_has_attribute(src_node, "size");

        // <destination>PATH</destination>
        XMLElement *dst_node = op_node->FirstChildElement("destination");

        create_operation* op = new create_operation(rmgr_, *next_update);
        op->src_checksum = src_node->Attribute("checksum");
        op->src_size = utility::tonumber(src_node->Attribute("size"));
        op->src_uri = src_node->GetText();
        op->dst_path = dst_node->GetText();
        op->is_executable = op_node->Attribute("executable") != NULL;

        if (marked_for_deletion.find(op->dst_path) != marked_for_deletion.end())
          op->marked_for_deletion();

        debug() << op->tostring();

        patch.operations.push_back(op);
      } // <create>

      else if (strcmp(op_node->Value(), "update") == 0) {
        
        ensure_has_child(op_node, "basis");
        ensure_has_child(op_node, "delta");

        XMLElement *basis_node = op_node->FirstChildElement("basis");

        ensure_has_attribute(basis_node, "pre-checksum");
        ensure_has_attribute(basis_node, "post-checksum");
        ensure_has_attribute(basis_node, "pre-size");
        ensure_has_attribute(basis_node, "post-size");

        XMLElement *delta_node = op_node->FirstChildElement("delta");

        ensure_has_attribute(delta_node, "checksum");
        ensure_has_attribute(delta_node, "size");

        // TODO: verify that nodes aren't empty

        update_operation *op = new update_operation(rmgr_, *next_update);
        op->basis = basis_node->GetText();
        op->basis_checksum = basis_node->Attribute("pre-checksum");
        op->basis_length = tonumber(basis_node->Attribute("pre-size"));
        op->patched_checksum = basis_node->Attribute("post-checksum");
        op->patched_length = tonumber(basis_node->Attribute("post-size"));
        op->delta = delta_node->GetText();
        op->delta_checksum = delta_node->Attribute("checksum");
        op->delta_length = tonumber(delta_node->Attribute("size"));

        debug() << op->tostring();

        patch.operations.push_back(op);

      } // <update>
      else if (strcmp(op_node->Value(), "rename") == 0) {
        notice() << "rename operations are not yet implemented";
      } // <rename>
      else if (strcmp(op_node->Value(), "delete") == 0) {
        // <delete> operations have a single node: <target>

        ensure_has_child(op_node, "target");

        // <target>PATH</target>
        XMLElement *tgt_node = op_node->FirstChildElement("target");

        delete_operation* op = new delete_operation(rmgr_, *next_update);
        op->dst_path = tgt_node->GetText();

        debug() << op->tostring();

        patch.operations.push_back(op);
        marked_for_deletion.insert(std::make_pair(op->dst_path, true));
      } // <delete>

      op_node = op_node->NextSiblingElement();
    }

    // Perform the staging step for all operations
    info() << "Staging...";

    // create the cache directory for this release
    rmgr_.create_directory(rmgr_.cache_path() / next_update->checksum);

    bool staging_failure = false; 
    for (operations_t::iterator op_itr = patch.operations.begin();
      op_itr != patch.operations.end();
      ++op_itr) {

      STAGE_RC rc = (*op_itr)->stage();
      if (rc != STAGE_OK) {
        error() << "An operation failed to stage, patch will not be applied.";
        debug() << "STAGE_RC: " << rc;
        staging_failure = true;
        break;
      }
    }

    // TODO: an option to keep the data that has been downloaded would be nice

    if (staging_failure) {
      // rollback any changes if the staging failed
      info() << "Rolling back all changes.";

      for (operations_t::iterator op_itr = patch.operations.begin();
        op_itr != patch.operations.end();
        ++op_itr)
      {
        (*op_itr)->rollback();
      }

      fs::remove_all(rmgr_.cache_path() / next_update->checksum);
      
      return false;
    }
    
    // Commit the patch
    info() << "Comitting...";

    bool deploy_failure = false; 
    for (operations_t::iterator op_itr = patch.operations.begin();
      op_itr != patch.operations.end();
      ++op_itr) {

      STAGE_RC rc = (*op_itr)->deploy();
      if (rc != STAGE_OK) {
        error() << "An operation failed to deploy, patch will not be applied.";
        debug() << "STAGE_RC: " << rc;
        deploy_failure = true;
        break;
      }
    }

    if (deploy_failure) {
      // rollback any changes if the deploy failed
      info() << "Rolling back all changes.";

      for (operations_t::iterator op_itr = patch.operations.begin();
        op_itr != patch.operations.end();
        ++op_itr)
      {
        (*op_itr)->rollback();
      }

      fs::remove_all(rmgr_.cache_path() / next_update->checksum);
      
      return false;
    } else {
      info() << "patch applied successfully, committing and purging the cache...";
      // BOOST_FOREACH(operation* op, patch.operations) {
      //   op->commit();
      // }
      for (operations_t::iterator op_itr = patch.operations.begin();
        op_itr != patch.operations.end();
        ++op_itr)
      {
        (*op_itr)->commit();
      }

      fs::remove_all(rmgr_.cache_path() / next_update->checksum);
    }
 
    return true;
  }

  void patcher::ensure_has_attribute(const tinyxml2::XMLNode* const node, string_t const& name, string_t const& value) const {
    using utility::dump_node;
    using namespace tinyxml2;

    const XMLElement *el = node->ToElement();

    if (!el->Attribute(name.c_str())) {
      error() << dump_node(el) << " is missing a required attribute " << name;
      throw missing_attribute(dump_node(el, false), name, current_manifest_uri_);
    }

    if (!value.empty()) {
      if (!el->Attribute(name.c_str(), value.c_str())) {
        error() 
          << dump_node(el) 
          << " attribute " << name << " has an unexpected value " 
          << el->Attribute(name.c_str()) << ", expected: " << value;

        throw invalid_attribute(dump_node(el, false), 
          name, 
          value, 
          el->Attribute(name.c_str()), 
          current_manifest_uri_);
      }
    }
  }
  void patcher::ensure_has_child(const XMLNode* const node, string_t const& child_name) const {
    using utility::dump_node;
    using namespace tinyxml2;

    if (!node->FirstChildElement(child_name.c_str())) {
      error() << dump_node(node) << " is missing a child node named '" << child_name << '\'';
      throw missing_node(dump_node(node, false), child_name, current_manifest_uri_);
    }
  }

 void patcher::ensure_has_children(const XMLNode* const node) const {
    using utility::dump_node;
    using namespace tinyxml2;

    if (node->NoChildren()) {
      error() << dump_node(node) << " is empty!";
      throw missing_children(dump_node(node, false), current_manifest_uri_);
    }
  }

  void patcher::report_parser_error(int xml_rc, XMLDocument& doc) const {
    error() << "Unable to parse document, xml error: "
            << utility::tinyxml2_ec_to_string(xml_rc);
    if (doc.GetErrorStr1())
      error() << "tinyxml2 error string#1: " << doc.GetErrorStr1();
    if (doc.GetErrorStr2())
      error() << "tinyxml2 error string#2: " << doc.GetErrorStr2();

    throw invalid_manifest("Manifest[" + current_manifest_uri_ + "] could not be parsed. See log for more info.");
  }
}