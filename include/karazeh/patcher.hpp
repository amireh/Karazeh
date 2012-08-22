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

#ifndef H_KARAZEH_PATCHER_H
#define H_KARAZEH_PATCHER_H

#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/resource_manager.hpp"
#include "karazeh/hasher.hpp"
#include "karazeh/release_manifest.hpp"
#include "karazeh/operation.hpp"
#include "karazeh/operations/create.hpp"
#include "karazeh/operations/delete.hpp"
#include "karazeh/operations/update.hpp"
#include "karazeh/utility.hpp"
#include "tinyxml2/tinyxml2.h"
#include <map>
#include <vector>
#include <boost/foreach.hpp>
 
namespace kzh {
  using tinyxml2::XMLDocument;
  using tinyxml2::XMLNode;

  typedef string_t identity_t;

  class patcher : protected logger {
  public:
    
    /** Given resource manager must have the paths resolved, 
      * see resource_manager::resolve_paths()
      */
    patcher(resource_manager&);
    virtual ~patcher();

    /**
     * Retrieves the version manifest from one of the assigned patch servers
     * and attempts to identify the application's current version.
     *
     * Returns true if the version has been identified, false otherwise.
     *
     * @throw invalid_resource if the manifest could not be retrieved
     * @throw invalid_manifest if:
     *        1. the manifest could not be parsed
     *        2. no <release initial="true"> was found
     *        3. <release> "identity" attribute points to an undefined identity list
     * @throw missing_children if:
     *        1. the manifest is empty
     *        2. <identity> node is empty
     * @throw missing_attribute if:
     *        1. <identity> tag missing the "name" attribute
     *        2. <release> is missing "checksum" attribute
     *        3. <release> is missing "identity" attribute
     * @throw manifest_error if:
     *        1. an identity file is unreadable
     *        2. an identity file couldn't be checksummed
     * @throw missing_child if:
     *        1. manifest has no <release> children
     *
     * @note Identity lists are built and populated in this method.
     */
    bool identify(string_t const& version_manifest_url);
    bool identify(XMLDocument& version_manifest);

    /** The application current version, if identified */
    identity_t const& version() const;

    /**
     * Determines whether an update is available. True if a patch(es) is/are pending,
     * and false if the application is up to date.
     *
     * @throw invalid_state       identify() has not been called or wasn't successful
     * @throw invalid_manifest    one of the release manifest entries lacked a required attribute
     * @throw integrity_violation the application version could not be located in the version manifest
     * @throw missing_attribute   if a <release> is missing any of "checksum", "tag", or "uri" attributes
     */
    bool is_update_available();

    /** The number of available updates obtained in the last identify() call. */
    uint32_t nr_available_updates() const;

    /**
     *
     * @throw invalid_state     if no new releases are pending
     * @throw invalid_resource  if the release manifest couldn't be DLed
     * @throw missing_node      if <release> isn't defined
     * @throw missing_children  if <release> has no operations/empty
     * @throw invalid_manifest  if:
     *        1. the release manifest is empty
     *        2. the release manifest has no <release> node
     *        3. the release manifest's <release> node has no children (operations)
     * @throw missing_child if:
     *        1. <create> has no <source> or <target> children
     *        2. <delete> has no <target> child
     * @throw missing_attribute if:
     *        1. <create>/<source> has no "checksum" or "size" attributes
     *
     * Returns true if the patch was successfully applied, false otherwise.
     */
    bool apply_next_update();

  protected:

    struct identity_file {
      path_t    filepath;
      string_t  checksum;
    };

    struct identity_list {
    public:

      identity_list(string_t const& name);
      ~identity_list();

      typedef std::vector<identity_file*> identity_files_t;

      string_t          name;
      identity_files_t  files;
      string_t          checksum;
    };

    
    typedef std::map<string_t, identity_list*> identity_lists_t;
    typedef std::vector<release_manifest*> rmanifests_t;

    resource_manager  &rmgr_;
    
    /** 
     * Points to the identity checksum resolved in identify(),
     * being empty indicates the version hasn't been identified.
     */
    identity_t        version_;

    /** The identity list constructs built from the version manifest. */
    identity_lists_t  identity_lists_;

    /** The version manifest document */
    XMLDocument       vmanifest_;

    /** All the release manifests, and the ones marked for applying. */
    rmanifests_t      rmanifests_, new_releases_;

    /** 
     * Used by error reporting helpers. This is set everytime
     * a method is parsing / handling a manifest.
     */
    string_t          current_manifest_uri_;

    /** Whether the version has been correctly identified and is a valid one */
    bool is_identified() const;

    /**
     * Checks whether the given node has the specified attribute @name, and 
     * will compare its value if @value isn't empty (and the attribute exists).
     *
     * @throw kzh::missing_attribute when the attribute isn't set
     * @throw kzh::invalid_attribute when the attribute is set but doesn't match the expected value
     */
    void ensure_has_attribute(const XMLNode* const node, string_t const& name, string_t const& value = "") const;
    void ensure_has_child(const XMLNode* const parent, string_t const& child_name) const;
    void ensure_has_children(const XMLNode* const) const;

    /**
     * Will attempt to log the cause of the XML parser error using
     * tinyxml2::XMLDocument::GetErrorStr1() and
     * tinyxml2::XMLDocument::GetErrorStr2() if they're available.
     *
     * @throw kzh::invalid_manifest unconditionally
     */
    void report_parser_error(int xml_rc, XMLDocument&) const;
  private:
  };

} // end of namespace kzh

#endif
