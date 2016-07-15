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

#include "karazeh/patcher.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace kzh {
  patcher::patcher(config_t const& config)
  : logger("patcher"),
    config_(config)
  {
  }

  patcher::~patcher() {
  }

  STAGE_RC patcher::apply_update(const release_manifest& release) {
    auto file_manager = config_.file_manager;

    const path_t staging_path(config_.cache_path / release.id);
    const auto rollback = [&](STAGE_RC rc) -> STAGE_RC {
      // rollback any changes if the staging failed
      info() << "Rolling back all changes.";

      for (auto op : release.operations) {
        op->rollback();
      }

      file_manager->remove_directory(staging_path);

      return rc;
    };

    info() << "Applying update: <<" << release.tag << ">>";
    info() << "Update has " << release.operations.size() << " operations to be applied.";
    info() << "Staging...";

    // create the cache directory for this release
    file_manager->create_directory(staging_path);

    for (auto op : release.operations) {
      STAGE_RC rc = op->stage();

      if (rc != STAGE_OK) {
        error() << "An operation failed to stage, patch will not be applied.";
        error() << op->tostring();
        debug() << "STAGE_RC: " << rc;

        // TODO: an option to keep the data that has been downloaded would be nice
        return rollback(rc);
      }
    }

    // Commit the patch
    info() << "Deploying...";

    for (auto op : release.operations) {
      STAGE_RC rc = op->deploy();

      if (rc != STAGE_OK) {
        error() << "An operation failed to deploy, patch will not be applied.";
        error() << op->tostring();
        debug() << "STAGE_RC: " << rc;

        return rollback(rc);
      }
    }

    info() << "All operations staged and deployed, now to clean artifacts...";

    for (auto op : release.operations) {
      op->commit();
    }

    file_manager->remove_directory(staging_path);

    info() << "Patch applied successfully.";

    return STAGE_OK;
  }
}