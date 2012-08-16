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

#ifndef H_KARAZEH_OPERATION_H
#define H_KARAZEH_OPERATION_H

#include "karazeh/karazeh.hpp"
#include "karazeh/resource_manager.hpp"
#include "karazeh/release_manifest.hpp"
 
namespace kzh {
  
  enum STAGE_RC {
    STAGE_OK = 0,

    /** Running user lacks the required permission to read/write */
    STAGE_UNAUTHORIZED,

    /** Not enough free space to hold a file to be downloaded */
    STAGE_OUT_OF_SPACE,

    /** Destination at which a file is to be created or moved is occupied */
    STAGE_FILE_EXISTS,

    /** A file to be updated, renamed, or deleted is missing */
    STAGE_FILE_MISSING,

    /** Checksum of a file to be updated does not match the expected checksum */
    STAGE_FILE_INTEGRITY_MISMATCH
  };

  class operation {
  public:
    inline operation(resource_manager& rmgr, release_manifest& rm)
    : rmgr_(rmgr),
      rm_(rm)
    {}

    inline virtual ~operation() {}

    virtual STAGE_RC stage() = 0;
    virtual STAGE_RC commit() = 0;
    virtual void rollback() = 0;

    inline virtual string_t tostring() { return ""; }

  protected:
    resource_manager  &rmgr_;
    release_manifest  &rm_;
  };

} // end of namespace kzh

#endif
