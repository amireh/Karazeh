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

#include "karazeh/operations/delete.hpp"
#include "karazeh/release_manifest.hpp"

namespace kzh {
  namespace fs = boost::filesystem;

  delete_operation::delete_operation(
    config_t const& config,
    file_manager const& file_manager,
    downloader const& downloader,
    release_manifest const& rm
  )
  : operation(config, file_manager, downloader, rm),
    logger("op_delete"),
    deleted_(false)
  {
  }

  delete_operation::~delete_operation() {
  }

  STAGE_RC delete_operation::stage() {
    dst_dir_ = (config_.root_path / dst_path).parent_path();
    cache_path_ = config_.cache_path / rm_.id / "deleted" / dst_path;
    cache_dir_ = cache_path_.parent_path();

    indent();
      debug() << "Dst dir: " << dst_dir_;
      debug() << "Cache path: " << cache_path_;
      debug() << "Cache dir: " << cache_dir_;
    deindent();

    // Make sure the destination exists
    if (!fs::exists(config_.root_path / dst_path)) {
      error() << "Destination does not exist: " << dst_path;

      return STAGE_FILE_MISSING;
    }

    // Make sure the cache destination is _free_
    if (fs::exists(cache_path_)) {
      error() << "Caching destination exists!" << cache_path_;

      return STAGE_FILE_EXISTS;
    }

    if (!file_manager_.create_directory(cache_dir_)) {
      error() << "Unable to create caching directory: " << cache_dir_;

      return STAGE_UNAUTHORIZED;
    }

    return STAGE_OK;
  }

  STAGE_RC delete_operation::deploy() {
    using fs::rename;

    // Make sure the destination exists
    if (!fs::exists(config_.root_path / dst_path)) {
      error() << "Destination does not exist: " << config_.root_path / dst_path;

      return STAGE_FILE_MISSING;
    }

    // Make sure the cache destination is _free_
    if (fs::exists(cache_path_)) {
      error() << "Caching destination exists!" << cache_path_;

      return STAGE_FILE_EXISTS;
    }

    // Move the staged file to the destination
    info()
      << "Moving " << config_.root_path / dst_path << " to "
      << cache_path_;

    rename(config_.root_path / dst_path, cache_path_);

    deleted_ = true;

    return STAGE_OK;
  }

  void delete_operation::rollback() {
    using fs::is_empty;
    using fs::remove;

    if (deleted_) {
      try {
        rename(cache_path_, config_.root_path / dst_path);
      } catch (fs::filesystem_error &e) {
        error() << "Cached file could not be found! Can not rollback!! Cause: " << e.what();
        // TODO: handle rollback failures
      }
    }

    return;
  }

  void delete_operation::commit() {
    debug() << "Deleting " << cache_path_ << "...";
    fs::remove_all(cache_path_);
    debug() << "Truly deleted " << cache_path_;
  }

  string_t delete_operation::tostring() {
    std::ostringstream s;
    s << "delete from[" << this->dst_path << ']';
    return s.str();
  }
}