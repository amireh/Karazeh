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

#ifndef H_KARAZEH_OPERATION_CREATE_H
#define H_KARAZEH_OPERATION_CREATE_H

#include "karazeh/operation.hpp"
#include "karazeh/logger.hpp"
 
namespace kzh {
  
  class create_operation : public operation, protected logger {
  public:
    create_operation(resource_manager&, release_manifest&);
    virtual ~create_operation();

    /**
     * Downloads the file from src_uri and stores it in the staging temporary
     * repository. If an integrity mismatch occurs, the file will be re-downloaded
     * up to resource_manager::download_retries times.
     *
     * The following conditions must be met for the staging to be successful:
     *
     * 1. No file must exist at dst_path
     * 2. Running user must have write permissions for dst_path
     * 3. Enough available space to hold src_size bytes
     * 
     * @throw kzh::invalid_resource if the file couldn't be DLed
     *
     * Returns STAGE_OK on success, otherwise an error indicated by the return code,
     * see karazeh/operation.hpp for a complete listing.
     */
    virtual STAGE_RC stage();
    virtual STAGE_RC deploy();
    virtual void rollback();

    virtual string_t tostring();

    string_t  src_checksum;
    uint64_t  src_size;
    string_t  src_uri;
    string_t  dst_path;
    bool      is_executable;

    void marked_for_deletion();

  private:
    path_t dst_dir_;
    path_t tmp_dir_;
    path_t tmp_path_;

    bool created_directory_;
    bool created_;
    bool marked_for_deletion_;
  };

} // end of namespace kzh

#endif
