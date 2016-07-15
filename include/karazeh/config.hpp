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

#ifndef H_KARAZEH_CONFIG_H
#define H_KARAZEH_CONFIG_H

#include "karazeh_export.h"
#include "karazeh/karazeh.hpp"

namespace kzh {
  class downloader;
  class file_manager;
  class hasher;

  typedef struct KARAZEH_EXPORT {
    string_t host;
    path_t root_path;
    path_t cache_path;
    kzh::hasher const* hasher;
    kzh::downloader const* downloader;
    kzh::file_manager const* file_manager;
    bool verbose;
  } config_t;

} // end of namespace kzh

#endif
