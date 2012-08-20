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

#include "karazeh/operations/create.hpp"
#include "karazeh/hasher.hpp"
 
namespace kzh {

  create_operation::create_operation(resource_manager& rmgr, release_manifest& rm)
  : operation(rmgr, rm),
    logger("op_create"),
    created_directory_(false),
    created_(false),
    marked_for_deletion_(false),
    is_executable(false)
  {

  }

  create_operation::~create_operation() {
  }

  void create_operation::marked_for_deletion() {
    marked_for_deletion_ = true;
  }

  STAGE_RC create_operation::stage() {
    cache_path_ = path_t(rmgr_.cache_path() / rm_.checksum / dst_path);
    cache_dir_ = cache_path_.parent_path();
    dst_dir_ = (rmgr_.root_path() / dst_path).parent_path();

    if (settings::is_enabled("-v")) {
      indent();
      debug() << "Caching path: "<< cache_path_;
      debug() << "Caching dir: "<< cache_dir_;
      debug() << "Dest dir: "<< dst_dir_;
      deindent();
    }

    // Prepare our staging directory
    if (!rmgr_.create_directory(cache_dir_)) {
      error() << "Unable to create caching directory: " << cache_dir_;
      return STAGE_UNAUTHORIZED;
    }

    // Prepare our destination directory, if necessary
    if (!boost::filesystem::is_directory(dst_dir_)) {
      if (!rmgr_.create_directory(dst_dir_)) {
        error() << "Unable to create destination dir: " << dst_dir_;
        return STAGE_UNAUTHORIZED;
      }

      created_directory_ = true;
    }

    // Make sure the destination is free
    if (rmgr_.is_readable(rmgr_.root_path() / dst_path) && !marked_for_deletion_) {
      error() << "Destination is occupied: " << dst_path;

      return STAGE_FILE_EXISTS;
    }

    // Can we write to the destination?
    if (!rmgr_.is_writable(rmgr_.root_path() / dst_path)) {
      error() << "Destination isn't writable: " << rmgr_.root_path() / dst_path;
      return STAGE_UNAUTHORIZED;
    }

    // Can we write to the staging destination?
    if (!rmgr_.is_writable(cache_path_)) {
      error() << "The cache isn't writable: " << cache_path_;
      return STAGE_UNAUTHORIZED;
    }
    
    if (!rmgr_.get_remote(src_uri, cache_path_, src_checksum, src_size)) {
      throw invalid_resource(src_uri);
    }

    return STAGE_OK;
  }

  STAGE_RC create_operation::deploy() {
    using boost::filesystem::rename;
    
    // Make sure the destination is free
    if (rmgr_.is_readable(rmgr_.root_path() / dst_path)) {
      error() << "Destination is occupied: " << dst_path;
      return STAGE_FILE_EXISTS;
    }

    // Can we write to the destination?
    if (!rmgr_.is_writable(rmgr_.root_path() / dst_path)) {
      error() << "Destination isn't writable: " << rmgr_.root_path() /dst_path;
      return STAGE_UNAUTHORIZED;
    }

    // Can we write to the staging destination?
    if (!rmgr_.is_writable(cache_path_)) {
      error() << "Temp isn't writable: " << cache_path_;
      return STAGE_UNAUTHORIZED;
    }

    // Move the staged file to the destination
    info() << "Creating " << rmgr_.root_path() / dst_path;
    rename(cache_path_, rmgr_.root_path() / dst_path);
    created_ = true;

    // validate integrity
    std::ifstream fh((rmgr_.root_path() / dst_path).string().c_str());
    hasher::digest_rc rc = hasher::instance()->hex_digest(fh);    
    fh.close();

    if (rc != src_checksum) {
      error() << "Created file integrity mismatch: " << rc.digest << " vs " << src_checksum;
      return STAGE_FILE_INTEGRITY_MISMATCH;
    }

    if (is_executable) {
      rmgr_.make_executable(rmgr_.root_path() / dst_path);
    }

    return STAGE_OK;
  } 

  void create_operation::rollback() {
    using boost::filesystem::is_empty;
    using boost::filesystem::remove;

    if (created_) {
      remove(rmgr_.root_path() / dst_path);
    }

    // If we were responsible for creating the directory of our dst_path
    // we need to remove it and all its ancestors if they're empty
    if (created_directory_) {
      path_t curr_dir = dst_dir_;
      while (is_empty(curr_dir)) {
        remove(curr_dir);
        curr_dir = curr_dir.parent_path();
      }
    }

    return;
  }

  string_t create_operation::tostring() {
    std::ostringstream s;
    s << "create from[" << this->src_uri << ']'
      << " to[" << this->dst_path << ']'
      << " checksum[" << this->src_checksum << ']'
      << " size[" << this->src_size << ']';
    return s.str();
  }
}