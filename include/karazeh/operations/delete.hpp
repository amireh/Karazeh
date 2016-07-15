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

#ifndef H_KARAZEH_OPERATION_DELETE_H
#define H_KARAZEH_OPERATION_DELETE_H

#include "karazeh/operation.hpp"
#include "karazeh/logger.hpp"
#include "karazeh_export.h"

namespace kzh {

  // TODO: require target checksum
  class KARAZEH_EXPORT delete_operation : public operation, protected logger {
  public:
    delete_operation(config_t const&, release_manifest const&);
    virtual ~delete_operation();

    /**
     * Checks whether the source to be removed exists.
     *
     * Returns STAGE_OK on success, otherwise an error indicated by the return code,
     * see karazeh/operation.hpp for a complete listing.
     */
    virtual STAGE_RC stage();

    /**
     * Moves the file or directory at a given path to Karazeh's cache.
     * The entry is purged from the cache if the patch was successful
     * (no rollback required).
     */
    virtual STAGE_RC deploy();

    /**
     * Moves the item that was deleted from its place in the cache to its original
     * path.
     */
    virtual void rollback();

    virtual void commit();

    virtual string_t tostring();

    string_t dst_path;

  private:
    path_t dst_dir_;
    path_t cache_dir_;
    path_t cache_path_;

    bool deleted_;
  };

} // end of namespace kzh

#endif
