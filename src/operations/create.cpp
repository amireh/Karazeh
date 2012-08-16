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
    created_directory_(false)
  {

  }

  create_operation::~create_operation() {

  }

  STAGE_RC create_operation::stage() {
    tmp_path_ = path_t(rmgr_.tmp_path() / rm_.checksum / dst_path);
    debug() << "Temp staging path: "<< tmp_path_;
    
    tmp_dir_ = tmp_path_.parent_path();
    debug() << "Temp staging dir: "<< tmp_dir_;

    dst_dir_ = (rmgr_.root_path() / dst_path).parent_path();
    debug() << "Dest dir: "<< dst_dir_;

    // Prepare our staging directory
    if (!rmgr_.create_directory(tmp_dir_)) {
      error() << "Unable to create temp directory: " << tmp_dir_;
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
    if (!rmgr_.is_writable(tmp_path_)) {
      error() << "Temp isn't writable: " << tmp_path_;
      return STAGE_UNAUTHORIZED;
    }

    // Download the file
    // std::ofstream staged_file(tmp_path_.string().c_str());
    // if (!staged_file.is_open() || !staged_file.good()) {
    //   return STAGE_UNAUTHORIZED;
    // }

    // bool downloaded = false;
    // for (int i = 0; i < rmgr_.retry_amount() + 1; ++i) {
    //   if (rmgr_.get_remote(src_uri, staged_file)) {
    //     // validate integrity
    //     staged_file.close();
    //     std::ifstream staged_file_for_reading(tmp_path_.string().c_str());
    //     if (hasher::instance()->hex_digest(staged_file_for_reading).digest == src_checksum) {
    //       downloaded = true;
    //       staged_file_for_reading.close();
    //       info() << "downloaded properly!";
    //       break;
    //     } else {
    //       notice()
    //         << "Downloaded file integrity mismatch: "
    //         <<  hasher::instance()->hex_digest(staged_file_for_reading).digest
    //         << " vs " << src_checksum;
    //     }

    //     staged_file_for_reading.close();
    //   }
      
    //   notice() << "retrying for the " << i+1 << " time";
    //   staged_file.close();
    //   staged_file.open(tmp_path_.string().c_str(), std::ios_base::trunc);
    // }
    
    if (!rmgr_.get_remote(src_uri, tmp_path_, src_checksum)) {
      // staged_file.close();
      throw invalid_resource(src_uri);
    }

    return STAGE_OK;
  }

  void create_operation::commit() {
    return;
  }

  void create_operation::rollback() {
    using boost::filesystem::is_empty;
    using boost::filesystem::remove;

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