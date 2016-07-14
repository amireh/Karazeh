/**
 * karazeh -- the library for patching software
 *
 * Copyright (C) 2011-2016 by Ahmad Amireh <ahmad@amireh.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef H_KARAZEH_VERSION_MANIFEST_H
#define H_KARAZEH_VERSION_MANIFEST_H

#include <map>
#include <vector>
#include "json11/json11.hpp"
#include "karazeh_export.h"
#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"
#include "karazeh/hasher.hpp"
#include "karazeh/release_manifest.hpp"
#include "karazeh/file_manager.hpp"
#include "karazeh/downloader.hpp"
#include "karazeh/operation.hpp"
#include "karazeh/operations/create.hpp"
#include "karazeh/operations/delete.hpp"
#include "karazeh/operations/update.hpp"

namespace kzh {
  typedef json11::Json JSON;

  using std::vector;
  using std::map;

  class KARAZEH_EXPORT version_manifest {
  public:
    explicit version_manifest(config_t const&);
    virtual ~version_manifest();

    /**
     * A convenience method for downloading a JSON manifest from a remote server
     * and parsing it. This implicitly calls #load_from_string() and #parse().
     *
     * @throw kzh::invalid_resource
     *        If the resource at the supplied URI could not be downloaded.
     */
    void load_from_uri(string_t const& uri);

    /**
     * A convenience method for parsing a JSON manifest from a string. This
     * implicitly calls #parse().
     *
     * This function is equivalent to manually parsing JSON into an object and
     * feeding it to parse.
     *
     * @throw kzh::invalid_manifest
     *        If the supplied JSON string is malformed or could not be parsed
     *        for some reason.
     */
    void load_from_string(string_t const& raw_json);

    /**
     * Populate the version manifest object from a JSON manifest.
     *
     * This method tracks the identity lists defined in the manifest as well as
     * the release definitions.
     *
     * @throw kzh::invalid_manifest
     *        - if the manifest could not be parsed
     *        - if the manifest has no identity lists
     *        - if the manifest has no releases
     *        - if an identity list construct has no "name"
     *        - if an identity list construct has no "files" or an empty "files"
     *          list
     *        - if a release construct is missing the "id" attribute
     *        - if a release construct is missing the "identity" attribute
     *        - if a release construct's "identity" attribute points to an
     *          undefined identity list
     *
     * @throw std::domain_error
     *        - if an identity file is unreadable
     *
     * This function is NOT idempotent.
     * This function has implicit side-effects on the tracked identity list and
     * release containers.
     */
    void parse(JSON const&);

    void parse_release(JSON const&);

    /**
     * @return The number of tracked releases for *all* identities.
     */
    int get_release_count() const;

    /**
     * @return The number of tracked releases for a certain identity.
     */
    int get_release_count(string_t const& identity) const;

    /**
     * Attempt to identify the current application version.
     *
     * @return A string representing the release "id", and an empty one if the
     * version could not be identified.
     *
     * @throw std::domain_error
     *        - if an identity file is unreadable
     *        - if an identity file could not be checksummed
     */
    string_t get_current_version() const;

    /**
     * @return A list of release IDs that need to be applied to transition the
     * application to the latest version.
     */
    vector<string_t> get_available_updates(const string_t& current_version) const;

    /**
     * @warn If no such release could be found, the behavior is undefined.
     */
    release_manifest const* get_release(const string_t&) const;

  private:
    struct identity_list_t {
      inline explicit identity_list_t(string_t const& name) : name(name) {};
      inline ~identity_list_t() {
        files.clear();
      };

      string_t name;
      vector<path_t> files;
    };

    map<string_t, identity_list_t> identity_lists_;
    vector<release_manifest*>      releases_;
    config_t                       const &config_;

    release_manifest const* find_or_create_release(JSON const&);
    operation* parse_operation(release_manifest const&, JSON const&) const;
  };

} // end of namespace kzh

#endif
