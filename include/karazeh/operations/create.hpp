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

#ifndef H_KARAZEH_OPERATION_CREATE_H
#define H_KARAZEH_OPERATION_CREATE_H

#include "karazeh_export.h"
#include "karazeh/operation.hpp"
#include "karazeh/logger.hpp"

namespace kzh {

  class KARAZEH_EXPORT create_operation : public operation, protected logger {
  public:
    create_operation(int id, config_t const&, release_manifest const&);
    virtual ~create_operation();

    /**
     * Downloads the file from src_uri and stores it in the staging temporary
     * repository. If an integrity mismatch occurs, the file will be re-downloaded
     * up to resource_manager::download_retries times.
     *
     * The following conditions must be met for the staging to be successful:
     *
     * 1. No file must exist at dst_path
     * 2. Running user must have write permissions for dst_path
     * 3. Enough available space to hold src_size bytes
     *
     * @throw kzh::invalid_resource if the file couldn't be DLed
     *
     * Returns STAGE_OK on success, otherwise an error indicated by the return code,
     * see karazeh/operation.hpp for a complete listing.
     */
    virtual STAGE_RC stage();
    virtual STAGE_RC deploy();
    virtual void rollback();
    virtual void commit();

    virtual string_t tostring();

    string_t  src_checksum;
    string_t  src_uri;
    string_t  dst_path;
    bool      is_executable;

    void marked_for_deletion();
  protected:
    bool has_deployed() const;
    path_t get_destination() const;

  private:
    path_t cache_path_;

    bool marked_for_deletion_;
  };

} // end of namespace kzh

#endif
