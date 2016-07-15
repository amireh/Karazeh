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

#ifndef H_KARAZEH_PATCHER_H
#define H_KARAZEH_PATCHER_H

#include "karazeh_export.h"
#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/config.hpp"
#include "karazeh/downloader.hpp"
#include "karazeh/file_manager.hpp"
#include "karazeh/release_manifest.hpp"

namespace kzh {
  class KARAZEH_EXPORT patcher : protected logger {
  public:

    /** Given resource manager must have the paths resolved,
      * see downloader::resolve_paths()
      */
    patcher(config_t const&);
    virtual ~patcher();

    /**
     *
     * @throw invalid_state     if no new releases are pending
     * @throw invalid_resource  if the release manifest couldn't be DLed
     * @throw missing_node      if <release> isn't defined
     * @throw missing_children  if <release> has no operations/empty
     * @throw invalid_manifest  if:
     *        1. the release manifest is empty
     *        2. the release manifest has no <release> node
     *        3. the release manifest's <release> node has no children (operations)
     * @throw missing_child if:
     *        1. <create> has no <source> or <target> children
     *        2. <delete> has no <target> child
     * @throw missing_attribute if:
     *        1. <create>/<source> has no "checksum" or "size" attributes
     *
     * Returns true if the patch was successfully applied, false otherwise.
     */
    STAGE_RC apply_update(release_manifest const&);

  private:
    config_t const &config_;
  };

} // end of namespace kzh

#endif
