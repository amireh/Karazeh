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

#ifndef H_KARAZEH_FILE_MANAGER_H
#define H_KARAZEH_FILE_MANAGER_H

#include <curl/curl.h>
#include <boost/filesystem.hpp>
#include "binreloc/binreloc.h"
#include "karazeh_export.h"
#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/hasher.hpp"

namespace kzh {
  class KARAZEH_EXPORT file_manager : public logger {
  public:

    file_manager();
    virtual ~file_manager();

    /** Loads the content of a file stream into memory */
    virtual bool load_file(std::ifstream &fs, string_t& out_buf) const;

    /** Loads the content of a file found at @path into memory */
    virtual bool load_file(string_t const& path, string_t& out_buf) const;
    virtual bool load_file(path_t const& path, string_t& out_buf) const;

    virtual bool remove_file(path_t const&) const;
    virtual bool remove_directory(path_t const&) const;

    virtual bool exists(path_t const&) const;

    virtual bool is_empty(path_t const&) const;
    virtual bool is_directory(path_t const&) const;

    /** Checks if the resource at the given path exists, is a file, and is readable. */
    virtual bool is_readable(path_t const &path) const;
    virtual bool is_readable(string_t const &path) const;

    /** Checks if the resource at the given path exists, is a file, and is writable. */
    virtual bool is_writable(path_t const &path) const;
    virtual bool is_writable(string_t const &path) const;

    virtual bool move(path_t const&, path_t const&) const;

    /**
     * Creates a directory indicated by the given path,
     * while creating all necessary ancestor directories (similar to mkdir -p)
     *
     * Returns false if the directories couldn't be created, and the cause
     * will be logged.
     */
    virtual bool create_directory(path_t const& path) const;
    virtual bool ensure_directory(path_t const& path) const;

    /**
     * Enables the executable permission flag for systems that support it.
     *
     * The file will have a permission mask equal to 0711
     *
     * Returns false if the permissions couldn't be modified.
     */
    virtual bool make_executable(path_t const&) const;

    virtual uint64_t stat_filesize(path_t const&) const;
    virtual uint64_t stat_filesize(std::ifstream&) const;
  };

} // end of namespace kzh

#endif
