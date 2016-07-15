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
    config_t const& config,
    release_manifest const& rm
  )
  : operation(config, rm),
    logger("op_update"),
    patched_(false),
    basis_length(0),
    delta_length(0),
    patched_length(0)
  {
  }

  update_operation::~update_operation() {
  }

  string_t update_operation::tostring() {
    std::ostringstream s;
    s << "update basis[" << basis << ']'
      << " using[" << delta << ']';
    return s.str();
  }

  STAGE_RC update_operation::stage() {
    auto file_manager = config_.file_manager;

    basis_path_     = (config_.root_path / basis).make_preferred();
    cache_dir_      = path_t(config_.cache_path / rm_.id / basis).make_preferred().parent_path();
    signature_path_ = (cache_dir_ / basis).make_preferred().filename().string() + ".signature";
    delta_path_     = (cache_dir_ / basis).make_preferred().filename().string() + ".delta";
    patched_path_   = (cache_dir_ / basis).make_preferred().filename().string() + ".patched";

    // basis must exist
    if (!file_manager->is_readable(basis_path_)) {
      error()
        << "basis file does not exist at: " << basis_path_;

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

    if (file_manager->stat_filesize(basis_path_) != basis_length) {
      error()
        << "Length mismatch: "
        << file_manager->stat_filesize(basis_path_) << " to " << basis_length
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
    if (!config_.downloader->fetch(delta, delta_path_, delta_checksum, delta_length)) {
      throw invalid_resource(delta);
    }

    // create the signature
    debug() << "generating signature for " << basis_path_ << " out to " << signature_path_;

    rs_result rc = encoder_.signature(basis_path_.c_str(), signature_path_.c_str());
    if (rc != RS_DONE) {
      error() << "Generating of signature for file " << basis_path_ << " has failed. librsync rc: " << rc;
      return STAGE_INTERNAL_ERROR;
    }

    return STAGE_OK;
  }

  STAGE_RC update_operation::deploy() {
    auto file_manager = config_.file_manager;

    debug() << "patching file " << basis_path_ << " using delta " << delta_path_ << " out to " << patched_path_;
    rs_result rc = encoder_.patch(basis_path_.c_str(), delta_path_.c_str(), patched_path_.c_str());

    if (rc != RS_DONE) {
      error()
        << "Patching file " << basis_path_ << " using patch " << delta_path_
        <<" has failed. librsync rc: " << rc;

      return STAGE_INTERNAL_ERROR;
    }

    hasher::digest_rc digest = config_.hasher->hex_digest(patched_path_);
    if (digest != patched_checksum) {
      error()
        << "Checksum mismatch: "
        << digest.digest << " vs " << patched_checksum
        << " in file " << patched_path_;

      return STAGE_FILE_INTEGRITY_MISMATCH;
    }

    if (file_manager->stat_filesize(patched_path_) != patched_length) {
      error()
        << "Length mismatch: "
        << file_manager->stat_filesize(patched_path_) << " to " << patched_length
        << " in file " << patched_path_;

      return STAGE_FILE_INTEGRITY_MISMATCH;
    }

    // swap the files
    debug() << "Swapping files: " << basis_path_ << " & " << patched_path_;

    const path_t temp_path(path_t(patched_path_.string() + ".tmp").make_preferred());

    file_manager->move(patched_path_, temp_path);
    file_manager->move(basis_path_, patched_path_);
    file_manager->move(temp_path, basis_path_);

    patched_ = true;

    return STAGE_OK;
  }

  void update_operation::rollback() {
    auto file_manager = config_.file_manager;

    // make sure the basis still exists in cache
    if (patched_) {

      if (!file_manager->exists(patched_path_)) {
        throw invalid_state("Basis no longer exists in cache, can not rollback!");
      }

      // the file should be there.. but just in case
      if (file_manager->exists(basis_path_)) {
        file_manager->remove_file(basis_path_);
      }

      file_manager->move(patched_path_, basis_path_);
    }

    cleanup();
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

  }

}