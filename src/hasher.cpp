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

#include "karazeh/hasher.hpp"
#include "karazeh/logger.hpp"

namespace kzh {

  const hasher* hasher::hasher_instance_ = NULL;

  hasher::~hasher() {
    if (hasher::instance() == this) {
      hasher::assign_hasher(NULL);
    }
  }

  void hasher::assign_hasher(hasher const* h) {
    hasher_instance_ = h;

    if (h) {
      logger l("hasher"); l.info() << "will be using " << h->name() << " for hashing";
    }
  }

  hasher const* const hasher::instance() {
    if (!hasher_instance_)
      throw uninitialized("No Hasher instance has been assigned!");

    return hasher_instance_;
  }

  string_t const& hasher::name() const {
    return name_;
  }
}