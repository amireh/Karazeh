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

#include "karazeh/operations/create.hpp"
#include "karazeh/release_manifest.hpp"

namespace kzh {
  namespace fs = boost::filesystem;

  create_operation::create_operation(
    config_t const& config,
    release_manifest const& rm
  )
  : operation(config, rm),
    logger("op_create"),
    created_directory_(false),
    marked_for_deletion_(false),
    is_executable(false)
  {
  }

  create_operation::~create_operation() {
  }

  void create_operation::marked_for_deletion() {
    marked_for_deletion_ = true;
  }

  STAGE_RC create_operation::stage() {
    auto file_manager = config_.file_manager;

    cache_path_ = path_t(config_.cache_path / rm_.id / dst_path);
    cache_dir_ = cache_path_.parent_path();
    dst_dir_ = (config_.root_path / dst_path).parent_path();

    if (config_.verbose) {
      indent();

      debug() << "Caching path: "<< cache_path_;
      debug() << "Dest path: " << dst_path;

      deindent();
    }

    // Prepare our staging directory
    if (!file_manager->ensure_directory(cache_dir_)) {
      error() << "Unable to create caching directory: " << cache_dir_;
      return STAGE_UNAUTHORIZED;
    }

    // Prepare our destination directory, if necessary
    if (!file_manager->exists(dst_dir_)) {
      if (!file_manager->create_directory(dst_dir_)) {
        error() << "Unable to create destination dir: " << dst_dir_;
        return STAGE_UNAUTHORIZED;
      }

      created_directory_ = true;
    }

    // Make sure the destination is free
    if (file_manager->is_readable(config_.root_path / dst_path) && !marked_for_deletion_) {
      error() << "Destination is occupied: " << dst_path;

      return STAGE_FILE_EXISTS;
    }

    // Can we write to the destination?
    if (!file_manager->is_writable(config_.root_path / dst_path)) {
      error() << "Destination isn't writable: " << config_.root_path / dst_path;
      return STAGE_UNAUTHORIZED;
    }

    // Can we write to the staging destination?
    if (!file_manager->is_writable(cache_path_)) {
      error() << "The cache isn't writable: " << cache_path_;
      return STAGE_UNAUTHORIZED;
    }

    if (!config_.downloader->fetch(src_uri, cache_path_, src_checksum)) {
      throw invalid_resource(src_uri);
    }

    return STAGE_OK;
  }

  STAGE_RC create_operation::deploy() {
    auto file_manager = config_.file_manager;
    const path_t full_destination_path(config_.root_path / dst_path);

    // Make sure the destination is free
    if (file_manager->is_readable(full_destination_path)) {
      error() << "Destination is occupied: " << full_destination_path;
      return STAGE_FILE_EXISTS;
    }

    // Can we write to the destination?
    if (!file_manager->is_writable(full_destination_path)) {
      error() << "Destination isn't writable: " << full_destination_path;
      return STAGE_UNAUTHORIZED;
    }

    // Can we write to the staging destination?
    if (!file_manager->is_writable(cache_path_)) {
      error() << "Temp isn't writable: " << cache_path_;
      return STAGE_UNAUTHORIZED;
    }

    // Move the staged file to the destination
    info() << "Creating " << full_destination_path;
    file_manager->move(cache_path_, full_destination_path);

    // validate integrity
    hasher::digest_rc rc = config_.hasher->hex_digest(full_destination_path);

    if (rc != src_checksum) {
      error() << "Created file integrity mismatch: " << rc.digest << " vs " << src_checksum;
      return STAGE_FILE_INTEGRITY_MISMATCH;
    }

    if (is_executable) {
      debug() << "MARKING EXECUTABLE!\n";
      file_manager->make_executable(full_destination_path);
    }

    return STAGE_OK;
  }

  void create_operation::rollback() {
    auto file_manager = config_.file_manager;

    if (has_deployed()) {
      file_manager->move(get_destination(), cache_path_);
    }

    // If we were responsible for creating the directory of our dst_path
    // we need to remove it and all its ancestors if they're empty
    //
    // TODO: this might be better done by keeping track of it in the
    // cache so that the rollback becomes idempotent cross-runs
    if (created_directory_) {
      path_t dir = dst_dir_;

      while (file_manager->is_empty(dir)) {
        file_manager->remove_directory(dir);
        dir = dir.parent_path();
      }
    }
  }

  void create_operation::commit() {
    auto file_manager = config_.file_manager;
    auto hasher = config_.hasher;

    // in case of roll back, the source file will still be staged in the cache
    if (
      file_manager->exists(cache_path_) &&
      // just to be safe, double-check it's our own file!
      hasher->hex_digest(cache_path_) == src_checksum
    ) {
      file_manager->remove_file(cache_path_);
    }
  }

  string_t create_operation::tostring() {
    std::ostringstream s;

    s << "create:"
      << " URL='"         << this->src_uri << "'"
      << " destination='" << this->dst_path << "'"
      << " checksum="     << this->src_checksum << "'"
    ;

    return s.str();
  }

  bool create_operation::has_deployed() const {
    const path_t &destination(get_destination());

    auto file_manager = config_.file_manager;
    auto hasher = config_.hasher;

    return (
      file_manager->exists(destination) &&
      hasher->hex_digest(destination) == src_checksum
    );
  }

  path_t create_operation::get_destination() const {
    return config_.root_path / dst_path;
  }
}