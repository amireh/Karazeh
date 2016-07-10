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

#ifndef H_KARAZEH_PATH_RESOLVER_H
#define H_KARAZEH_PATH_RESOLVER_H

#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/hasher.hpp"
#include <curl/curl.h>
#include <boost/filesystem.hpp>
#include "binreloc/binreloc.h"
#include "karazeh_export.h"

namespace kzh {
  class KARAZEH_EXPORT path_resolver : protected logger {
  public:
    path_resolver();
    virtual ~path_resolver();

    /**
     * The root path can be overridden at runtime via the -r option,
     * otherwise it is assumed to be KZH_DISTANCE_FROM_ROOT steps
     * above the directory that contains the running process.
     *
     * For example, if the running process is at bin/something
     * and KZH_DISTANCE_FROM_ROOT is set to 1, then the root will
     * be correctly set to `bin/..`
     *
     * All changes are deployted relative to the root path.
     */
    path_t const& get_root_path() const;

    /**
     * The "staging" directory used internally by Karazeh which
     * resides in `$ROOT/.kzh`.
     */
    path_t const& get_cache_path() const;

    /**
     * The directory of the running process.
     *
     * On Linux, it is located using binreloc (see deps/binreloc/binreloc.h)
     * On OS X, it is located using NSBundlePath() (see karazeh/utility.hpp)
     * On Windows, it is located using GetModuleFileName() (see resolve_paths())
     *
     * All the other paths are derived from the bin_path unless overridden.
     */
    void resolve(path_t root = "", bool verbose = false);

  private:
    path_t root_path_;
	path_t cache_path_;
  };

} // end of namespace kzh

#endif
