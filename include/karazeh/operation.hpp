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

#ifndef H_KARAZEH_OPERATION_H
#define H_KARAZEH_OPERATION_H

#include "karazeh_export.h"
#include "karazeh/karazeh.hpp"
#include "karazeh/config.hpp"
#include "karazeh/downloader.hpp"
#include "karazeh/file_manager.hpp"

namespace kzh {
  struct release_manifest;

  enum STAGE_RC {
    STAGE_OK = 0,
    /** Running user lacks the required permission to read/write */
    STAGE_UNAUTHORIZED,
    /** Not enough free space to hold a file to be downloaded */
    STAGE_OUT_OF_SPACE,
    /** Destination at which a file is to be created or moved is occupied */
    STAGE_FILE_EXISTS,
    /** A file to be updated, renamed, or deleted is missing */
    STAGE_FILE_MISSING,
    /** Checksum of a file to be updated does not match the expected checksum */
    STAGE_FILE_INTEGRITY_MISMATCH,

    /** Probably a Karazeh bug */
    STAGE_INTERNAL_ERROR
  };

  class KARAZEH_EXPORT operation {
  public:
    inline operation(config_t const& config, release_manifest const& rm)
    : config_(config),
      rm_(rm)
    {}

    inline virtual ~operation() {}

    /**
     * An operation gets "staged" in order to verify whether all
     * its required conditions are fulfilled in order to be deployted.
     *
     * Usually, operations verify permissions, existence (or lack of)
     * required files, and the downloading of resources while staging.
     *
     * If an operation returns anything other than STAGE_OK,
     * all registered patch operations will be called to rollback().
     */
    virtual STAGE_RC stage() = 0;

    /**
     * When all operations are staged, they are called to deploy
     * their changes and do whatever is necessary now that they
     * have their resources.
     *
     * Failure at the deploy stage will incur a rollback() on all
     * the patch operations that have been deployted and/or staged.
     */
    virtual STAGE_RC deploy() = 0;

    /**
     * Rolling-back ALL the changes that have been made to the
     * repository while stage()ing or deploy()ing.
     *
     * Use internal flags or members to keep track of the changes
     * you make if necessary. rollback() might be invoked after
     * operation::stage(), operation::deploy(), or neither!
     */
    virtual void rollback() = 0;

    /**
     * Purging is handy for operations that use the Karazeh cache
     * while deploying or staging. When an operation is called to
     * commit, it is guaranteed that the patch has been rolled out
     * successfully and any transient data will _not_ be needed
     * (no rollback will be invoked).
     *
     * The cache is used for operations that require storing some
     * data in a safe place only for the use of rollbacks if required,
     * but should otherwise be discarded.
     */
    inline virtual void commit() {};

    /** Used internally for exceptions and logging */
    inline virtual string_t tostring() { return ""; }

  protected:
    config_t const& config_;
    release_manifest const&rm_;
  };

} // end of namespace kzh

#endif
