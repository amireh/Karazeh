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

#ifndef H_KARAZEH_RELEASE_MANIFEST_H
#define H_KARAZEH_RELEASE_MANIFEST_H

#include "karazeh/karazeh.hpp"
#include "karazeh_export.h"

namespace kzh {
  typedef string_t identity_t;

  struct KARAZEH_EXPORT release_manifest {
    identity_t  checksum;
    string_t    tag;
    string_t    uri;

    string_t tostring() const;
    friend std::ostream& operator<<(std::ostream&, release_manifest*);
  };

} // end of namespace kzh

#endif
