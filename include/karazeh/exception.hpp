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

#ifndef H_KARAZEH_EXCEPTION_H
#define H_KARAZEH_EXCEPTION_H

#include <exception>
#include <string>
#include <stdexcept>
#include "karazeh_export.h"

namespace kzh {
  /**
   * Thrown when the resource manager was unable to retrieve a remote file,
   * the message will contain the URI of the resource.
   */
  class invalid_resource : public std::runtime_error {
  public:
    inline invalid_resource(const std::string& uri)
    : std::runtime_error("invalid resource: " + uri)
    { }
  };

  /**
   * Thrown when a version or release manifest could not be parsed due to
   * either malformed JSON or to invalid data.
   */
  class invalid_manifest : public std::runtime_error {
  public:
    inline invalid_manifest(const std::string& s)
    : std::runtime_error(s)
    { }
  };

  /** Thrown when an operation depends on another which has not bee
    * completed successfully (or at all).
    */
  class invalid_state : public std::runtime_error {
  public:
    inline invalid_state(const std::string& s)
    : std::runtime_error(s)
    { }
  };

  class manifest_error : public std::runtime_error {
  public:
    inline manifest_error(std::string const& s, std::string const& prefix = "")
    : std::runtime_error("Manifest error: " + prefix + ": " + s)
    { }
  };

} // end of namespace kzh

#endif // H_KARAZEH_EXCEPTION_H