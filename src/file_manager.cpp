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

#include "karazeh/file_manager.hpp"

namespace kzh {
  namespace fs = boost::filesystem;

  file_manager::file_manager() : logger("file_manager")
  {
  }

  file_manager::~file_manager() {
  }

  bool file_manager::load_file(std::ifstream &fs, string_t& out_buf) const
  {
    if (!fs.is_open() || !fs.good()) return false;

    while (fs.good())
      out_buf.push_back(fs.get());

    out_buf.erase(out_buf.size()-1,1);

    return true;
  }

  bool file_manager::load_file(string_t const& path, string_t& out_buf) const
  {
    std::ifstream fs(path.c_str());
    return load_file(fs, out_buf);
  }

  bool file_manager::load_file(path_t const& path, string_t& out_buf) const
  {
    std::ifstream fs(path.string().c_str());
    return load_file(fs, out_buf);
  }

  bool file_manager::is_readable(string_t const& resource) const
  {
    // boost::system::error_code ec;
    // fs::file_status result = fs::status(resource, ec);

    // if (ec != boost::system::error_code()) {
    //   return false;
    // }

    // return (
    //   (
    //     result.type() == fs::regular_file ||
    //     result.type() == fs::directory_file
    //   ) &&
    //   result.permissions() & fs::owner_read
    // );

    using fs::path;
    using fs::exists;
    using fs::is_regular_file;
    using fs::is_directory;

    path fp(resource);

    if (exists(fp)) {
      if (is_directory(fp)) {
        try {
          for (fs::directory_iterator it(fp); it != fs::directory_iterator(); ++it) {
            break;
          }

          return true;
        }
        catch (fs::filesystem_error& e) {
          error() << e.what();
          return false;
        }
      }
      else {
        std::ifstream fs(resource.c_str());
        bool readable = fs.is_open() && fs.good();
        fs.close();
        return is_regular_file(fp) && readable;
      }
    }

    return false;
  }

  bool file_manager::is_readable(path_t const& resource) const
  {
    return is_readable(path_t(resource).make_preferred().string());
  }

  bool file_manager::is_writable(string_t const& resource) const
  {
    using fs::path;
    using fs::exists;
    using fs::is_regular_file;

    try {
      path fp(resource);
      if (exists(fp)) {

        if (is_directory(fp)) {
          return is_writable(fp / "__karazeh_internal_directory_check__");
        }

        // it already exists, make sure we don't overwrite it
        std::ofstream fs(resource.c_str(), std::ios_base::app);
        bool writable = fs.is_open() && fs.good() && !fs.fail();
        fs.close();

        return is_regular_file(fp) && writable;
      } else {

        // try creating a file and write to it
        std::ofstream fs(resource.c_str(), std::ios_base::app);
        bool writable = fs.is_open() && fs.good() && !fs.fail();
        fs << "This was generated automatically by Karazeh and should have been deleted.";
        fs.close();

        if (exists(fp)) {
          // delete the file
          fs::remove(fp);
        }

        return writable;
      }
    } catch (...) {
      // something bad happened, it is most likely unwritable
      return false;
    }

    return false;
  }

  bool file_manager::is_writable(path_t const& resource) const
  {
    return is_writable(path_t(resource).make_preferred().string());
  }

  bool file_manager::is_directory(path_t const& path) const
  {
    return is_readable(path) && fs::is_directory(path);
  }

  bool file_manager::create_directory(path_t const& path) const
  {
    try {
      fs::create_directories(path);
    }
    catch (fs::filesystem_error &e) {
      error()
        << "Unable to create directory chain @ " << path
        << ". Cause: " << e.what();

      return false;
    }
    catch (std::exception &e) {
      error() << "Unknown error while creating directory chain @ " << path;
      error() << "Possible cause: " << e.what();

      return false;
    }

    return true;
  }

  bool file_manager::ensure_directory(path_t const& path) const
  {
    if (is_directory(path)) {
      return true;
    }
    else {
      return create_directory(path);
    }
  }

  bool file_manager::make_executable(path_t const& p) const
  {
    using namespace fs;

    try {
      permissions(p, owner_all | group_exe | others_exe);
    } catch (fs::filesystem_error &e) {
      error() << "Unable to modify permissions of file: " << p;
      return false;
    }

    return true;
  }

  uint64_t file_manager::stat_filesize(std::ifstream& in) const
  {
    in.seekg(0,std::ifstream::end);
    uint64_t size = in.tellg();
    in.seekg(0);

    return size;
  }

  uint64_t file_manager::stat_filesize(path_t const& p) const
  {
    std::ifstream fp(p.string().c_str(), std::ios_base::binary);
    if (!fp.is_open() || !fp.good())
      return 0;

    uint64_t size = stat_filesize(fp);

    fp.close();

    return size;
  }

  bool file_manager::remove_file(const path_t& path) const {
    if (!is_writable(path)) {
      return false;
    }

    try {
      fs::remove(path);
    }
    catch (fs::filesystem_error &e) {
      return false;
    }

    return true;
  }

  bool file_manager::remove_directory(const path_t& path) const {
    if (!is_readable(path)) {
      return false;
    }

    try {
      fs::remove_all(path);
    }
    catch (fs::filesystem_error &e) {
      return false;
    }

    return true;
  }

  bool file_manager::exists(const path_t& path) const {
    try {
      return fs::exists(path);
    }
    catch (fs::filesystem_error &e) {
      return false;
    }
  }
}