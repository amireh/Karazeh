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
#include "karazeh/utility.hpp"
#include "tinyxml2/tinyxml2.h"
#include <map>
#include <vector>
 
namespace kzh {
  using tinyxml2::XMLDocument;

  typedef string_t identity_t;

  class patcher : protected logger {
  public:
    
    /** Given resource manager must have the paths resolved, 
      * see resource_manager::resolve_paths()
      */
    patcher(resource_manager&);
    virtual ~patcher();

    /**
     * Retrieves the manifest from one of the assigned patch servers
     * and attempts to identify the application's current version.
     *
     * Returns true if the version has been identified and is valid,
     * false otherwise.
     *
     * @throw kzh::invalid_resource if the manifest could not be retrieved
     * @throw kzh::invalid_manifest if the manifest could not be parsed
     */
    bool identify(string_t const& version_manifest_url);

    /** The application current version, if identified */
    identity_t const& version() const;

    /**
     * Determines whether an update is available. True if a patch(es) is/are pending,
     * and false if the application is up to date.
     *
     * @throw kzh::invalid_state:
     *  => identify() has not been called or wasn't successful
     * @throw kzh::invalid_manifest:
     *  => one of the release manifest entries lacked a required attribute
     * @throw kzh::integrity_violation:
     *  => the application version could not be located in the version manifest
     */
    bool is_update_available();

    /**
     *
     * @throw kzh::invalid_state if no new releases are pending
     * @throw kzh::invalid_resource if the release manifest couldn't be DLed
     * @throw kzh::invalid_manifest if:
     *  => 1. the release manifest is empty
     *  => 2. the release manifest has no <release> node
     *  => 3. the release manifest's <release> node has no children (operations)
     *
     * Returns true if the patch was successfully applied, false otherwise.
     */
    bool apply_next_update();

  protected:
    struct identifier {
      path_t    filepath;
      string_t  checksum;
    };
    
    typedef std::vector<identifier*> identifiers_t;
    typedef std::vector<release_manifest*> rmanifests_t;

    resource_manager  &rmgr_;
    identity_t        version_;
    identifiers_t     identifiers_;
    XMLDocument       vmanifest_;
    rmanifests_t      rmanifests_, new_releases_;

    /** Whether the version has been correctly identified and is a valid one */
    bool is_identified() const;

  private:
  };

} // end of namespace kzh

#endif
