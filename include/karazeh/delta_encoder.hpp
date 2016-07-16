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

#ifndef H_KARAZEH_DELTA_ENCODER_H
#define H_KARAZEH_DELTA_ENCODER_H

#include "karazeh_export.h"
#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/file_manager.hpp"

extern "C" {
  #include "librsync-2.0.0/src/librsync.h"
  #include "librsync-2.0.0/src/fileutil.h"
}

namespace kzh {

  /**
   * @class delta_encoder
   * @brief
   * librsync implementation of delta encoding, used in rdiff.
   */
  class KARAZEH_EXPORT delta_encoder : public logger
  {
  public:

    delta_encoder();
    virtual ~delta_encoder();

    /**
     * Generates the signature from the basis file.
     *
     * @param basis path to the file you want to generate the signature for
     * @param signature path to where the signature file should be stored
     *
     * @return the status of rs_sig_file() (@see man rdiff)
     *
     * @throw kzh::invalid_resource if basis does not exist or is unreadable
     * @throw kzh::invalid_state if signature is not writable
     */
    rs_result signature(path_t const& basis, path_t const& signature);

    /**
     * Generates a delta patch based on the given signature and the new file.
     *
     * @param signature the signature of the basis file, generatable using signature()
     * @param new_file  the new version of basis
     * @param delta     path to where the delta file will be stored
     *
     * @return the status of rs_delta_file() (@see man rdiff)
     *
     * @throw kzh::invalid_resource if basis does not exist or is unreadable
     * @throw kzh::invalid_resource if delta does not exist or is unreadable
     * @throw kzh::invalid_state if target destination is unwritable
     */
    rs_result delta(path_t const& signature, path_t const& new_file, path_t const& delta);

    /**
     * Applies a patch on the basis file and stores it somewhere else.
     *
     * @param basis the base file to patch
     * @param delta the delta to patch the basis with, generatable using delta()
     * @param target the destination where the patched file will be stored
     *
     * @return the status of rs_patch_file() (@see man rdiff)
     *
     * @throw kzh::invalid_resource if basis does not exist or is unreadable
     * @throw kzh::invalid_resource if delta does not exist or is unreadable
     * @throw kzh::invalid_state if target destination is unwritable
     */
    rs_result patch(path_t const& basis, path_t const& delta, path_t const& target);

  protected:
    /// used for validating paths and file permissions
    file_manager file_manager_;
  };

} // end of namespace kzh

#endif
