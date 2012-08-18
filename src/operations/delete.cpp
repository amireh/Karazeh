/*
 *  Copyright (c) 2011-2012 Ahmad Amireh <kandie@mxvt.net>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#include "karazeh/operations/delete.hpp"
#include "karazeh/hasher.hpp"
 
namespace kzh {

  delete_operation::delete_operation(resource_manager& rmgr, release_manifest& rm)
  : operation(rmgr, rm),
    logger("op_delete"),
    deleted_(false)
  {

  }

  delete_operation::~delete_operation() {

  }

  STAGE_RC delete_operation::stage() {
    dst_dir_ = (rmgr_.root_path() / dst_path).parent_path();
    cache_path_ = rmgr_.cache_path() / rm_.checksum / dst_path;
    cache_dir_ = cache_path_.parent_path();

    indent();
      debug() << "Dst dir: " << dst_dir_;
      debug() << "Cache path: " << cache_path_;
      debug() << "Cache dir: " << cache_dir_;
    deindent();

    // Make sure the destination exists
    if (!boost::filesystem::exists(rmgr_.root_path() / dst_path)) {
      error() << "Destination does not exist: " << dst_path;

      return STAGE_FILE_MISSING;
    }

    if (!rmgr_.create_directory(cache_dir_)) {
      error() << "Unable to create caching directory: " << cache_dir_;

      return STAGE_UNAUTHORIZED;
    }

    return STAGE_OK;
  }

  STAGE_RC delete_operation::deploy() {
    using boost::filesystem::rename;
    
    // Make sure the destination exists
    if (!boost::filesystem::exists(rmgr_.root_path() / dst_path)) {
      error() << "Destination does not exist: " << rmgr_.root_path() / dst_path;

      return STAGE_FILE_MISSING;
    }

    // Move the staged file to the destination
    info() << "Deleting " << rmgr_.root_path() / dst_path;
    rename(rmgr_.root_path() / dst_path, rmgr_.cache_path() / rm_.checksum / dst_path);
    deleted_ = true;

    return STAGE_OK;
  } 

  void delete_operation::rollback() {
    using boost::filesystem::is_empty;
    using boost::filesystem::remove;

    if (deleted_) {
      try {
        rename(rmgr_.cache_path() / rm_.checksum / dst_path, rmgr_.root_path() / dst_path);
      } catch (boost::filesystem::filesystem_error &e) {
        error() << "Cached file could not be found! Can not rollback!!";
        // TODO: handle rollback failures
      }
    }

    return;
  }

  string_t delete_operation::tostring() {
    std::ostringstream s;
    s << "delete from[" << this->dst_path << ']';
    return s.str();
  }
}