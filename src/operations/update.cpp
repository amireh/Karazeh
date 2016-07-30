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

#include "karazeh/operations/update.hpp"
#include "karazeh/release_manifest.hpp"

namespace kzh {
  update_operation::update_operation(
    int id,
    config_t const& config,
    release_manifest const& release,
    path_t   const& in_basis_path,
    string_t const& in_delta_url
  )
  : operation(id, config, release),
    logger("op_update"),
    patched_(false),

    basis_path_(in_basis_path),
    delta_url_(in_delta_url),

    signature_path_(cache_dir_ / "signature"),
    delta_path_(cache_dir_ / "delta"),
    patched_path_(cache_dir_ / "patched")
  {
  }

  update_operation::~update_operation() {
  }

  string_t update_operation::tostring() {
    std::ostringstream s;
    s << "update basis[" << basis_path_ << ']'
      << " using[" << delta_path_ << ']';
    return s.str();
  }

  STAGE_RC update_operation::stage() {
    auto file_manager = config_.file_manager;

    // basis must exist
    if (!file_manager->is_readable(basis_path_)) {
      error() << "basis file does not exist at: " << basis_path_;

      return STAGE_FILE_MISSING;
    }

    // basis checksum check
    hasher::digest_rc digest = config_.hasher->hex_digest(basis_path_);

    if (digest != basis_checksum) {
      error()
        << "Basis file checksum mismatch: "
        << digest.digest << " vs " << basis_checksum
        << " in file " << basis_path_;

      return STAGE_FILE_INTEGRITY_MISMATCH;
    }

    // prepare our cache directory, if necessary
    if (!file_manager->is_directory(cache_dir_)) {
      if (!file_manager->create_directory(cache_dir_)) {
        error() << "Unable to create cache directory: " << cache_dir_;
        return STAGE_UNAUTHORIZED;
      }
    }

    // TODO: free space checks, need at least 2x basis file size + delta size

    // get the delta patch
    if (!config_.downloader->fetch(delta_url_, delta_path_, delta_checksum)) {
      throw invalid_resource(delta_url_);
    }

    // create the signature
    debug() << "generating signature for " << basis_path_ << " out to " << signature_path_;

    rs_result rc = encoder_.signature(basis_path_, signature_path_);

    if (rc != RS_DONE) {
      error() << "Generating of signature for file " << basis_path_ << " has failed. librsync rc: " << rc;
      return STAGE_ENCODING_ERROR;
    }

    return STAGE_OK;
  }

  STAGE_RC update_operation::deploy() {
    auto file_manager = config_.file_manager;

    debug() << "patching file " << basis_path_ << " using delta " << delta_path_ << " out to " << patched_path_;

    if (
      !file_manager->is_readable(basis_path_) ||
      !file_manager->is_readable(delta_path_)
    ) {
      return STAGE_INVALID_STATE;
    }

    rs_result rc = encoder_.patch(basis_path_.c_str(), delta_path_.c_str(), patched_path_.c_str());

    if (rc != RS_DONE) {
      error()
        << "Patching file " << basis_path_ << " using patch " << delta_path_
        <<" has failed. librsync rc: " << rc;

      return STAGE_ENCODING_ERROR;
    }

    hasher::digest_rc digest = config_.hasher->hex_digest(patched_path_);

    if (digest != patched_checksum) {
      error()
        << "Checksum mismatch: "
        << digest.digest << " vs " << patched_checksum
        << " in file " << patched_path_;

      return STAGE_FILE_INTEGRITY_MISMATCH;
    }

    const path_t temp_path(path_t(patched_path_.string() + ".tmp").make_preferred());

    // move the patched file to a temporary location until we can move it over
    // to the destination:
    file_manager->move(patched_path_, temp_path);

    // free the destination; move the old file to where the patched file was so
    // that we can roll back if necessary:
    file_manager->move(basis_path_, patched_path_); // !! repository side effect !!

    // finally, move over the patched file to where the old file was:
    file_manager->move(temp_path, basis_path_); // !! repository side effect !!

    patched_ = true;

    return STAGE_OK;
  }

  void update_operation::rollback() {
    auto file_manager = config_.file_manager;
    auto hasher       = config_.hasher;

    // did we patch the file? keep in mind that if we did:
    //
    // - patched_path_ would point to the **original** file
    // - basis_path_ would point to the **patched** file
    //
    // to avoid confusion, we'll gonna rename the references to "original" and
    // "patched":
    const path_t    &original_path(patched_path_);
    const string_t  &original_checksum(basis_checksum);
    const path_t    &modified_path(basis_path_);
    const string_t  &modified_checksum(patched_checksum);

    if (!file_manager->is_readable(modified_path)) {
      return (void)STAGE_INVALID_STATE;
    }

    auto checksum = hasher->hex_digest(modified_path);

    if (checksum == modified_checksum) {
      // make sure the original still exists
      if (!file_manager->exists(original_path)) {
        error() << "Basis no longer exists in cache, can not rollback!";

        return (void)STAGE_INVALID_STATE;
      }
      else if (hasher->hex_digest(original_path) != original_checksum) {
        error() << "Basis file seems to have changed, can not rollback!";

        return (void)STAGE_INVALID_STATE;
      }

      // remove the modified file:
      file_manager->remove_file(modified_path);

      // and move the original file back in its place:
      file_manager->move(original_path, modified_path);
    }
  }

  void update_operation::commit() {
    cleanup();
  }

  void update_operation::cleanup() {
    auto file_manager = config_.file_manager;

    // delete the delta patch
    if (file_manager->exists(delta_path_)) {
      file_manager->remove_file(delta_path_);
    }

    if (file_manager->exists(signature_path_)) {
      file_manager->remove_file(signature_path_);
    }

    if (file_manager->exists(patched_path_)) {
      file_manager->remove_file(patched_path_);
    }
  }
}