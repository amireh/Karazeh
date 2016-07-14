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
  patcher::patcher(config_t const& config, file_manager const& fmgr, downloader const& downloader)
  : logger("patcher"),
    downloader_(downloader),
    config_(config),
    file_manager_(fmgr)
  {
  }

  patcher::~patcher() {
  }

  bool patcher::apply_update(const release_manifest& release) {
    // info() << "applying update: " << next_update;


    // Perform the staging step for all operations
    // info() << "Staging...";

    // // create the cache directory for this release
    // file_manager_.create_directory(config_.cache_path / next_update->checksum);

    // bool staging_failure = false;
    // for (operations_t::iterator op_itr = patch.operations.begin();
    //   op_itr != patch.operations.end();
    //   ++op_itr) {

    //   STAGE_RC rc = (*op_itr)->stage();
    //   if (rc != STAGE_OK) {
    //     error() << "An operation failed to stage, patch will not be applied.";
    //     error() << (*op_itr)->tostring();
    //     debug() << "STAGE_RC: " << rc;
    //     staging_failure = true;
    //     break;
    //   }
    // }

    // // TODO: an option to keep the data that has been downloaded would be nice

    // if (staging_failure) {
    //   // rollback any changes if the staging failed
    //   info() << "Rolling back all changes.";

    //   for (operations_t::iterator op_itr = patch.operations.begin();
    //     op_itr != patch.operations.end();
    //     ++op_itr)
    //   {
    //     (*op_itr)->rollback();
    //   }

    //   fs::remove_all(config_.cache_path / next_update->checksum);

    //   return false;
    // }

    // // Commit the patch
    // info() << "Comitting...";

    // bool deploy_failure = false;
    // for (operations_t::iterator op_itr = patch.operations.begin();
    //   op_itr != patch.operations.end();
    //   ++op_itr) {

    //   STAGE_RC rc = (*op_itr)->deploy();
    //   if (rc != STAGE_OK) {
    //     error() << "An operation failed to deploy, patch will not be applied.";
    //     error() << (*op_itr)->tostring();
    //     debug() << "STAGE_RC: " << rc;
    //     deploy_failure = true;
    //     break;
    //   }
    // }

    // if (deploy_failure) {
    //   // rollback any changes if the deploy failed
    //   info() << "Rolling back all changes.";

    //   for (operations_t::iterator op_itr = patch.operations.begin();
    //     op_itr != patch.operations.end();
    //     ++op_itr)
    //   {
    //     (*op_itr)->rollback();
    //   }

    //   fs::remove_all(config_.cache_path / next_update->checksum);

    //   return false;
    // } else {
    //   info() << "patch applied successfully, committing and purging the cache...";
    //   // BOOST_FOREACH(operation* op, patch.operations) {
    //   //   op->commit();
    //   // }
    //   for (operations_t::iterator op_itr = patch.operations.begin();
    //     op_itr != patch.operations.end();
    //     ++op_itr)
    //   {
    //     (*op_itr)->commit();
    //   }

    //   fs::remove_all(config_.cache_path / next_update->checksum);
    // }

    // return true;
  }
}