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
#include "karazeh_export.h"
 
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
    STAGE_FILE_INTEGRITY_MISMATCH,

    /** Probably a Karazeh bug */
    STAGE_INTERNAL_ERROR
  };

  class KARAZEH_EXPORT operation {
  public:
    inline operation(resource_manager& rmgr, release_manifest& rm)
    : rmgr_(rmgr),
      rm_(rm)
    {}

    inline virtual ~operation() {}

    /**
     * An operation gets "staged" in order to verify whether all
     * its required conditions are fulfilled in order to be deployted.
     *
     * Usually, operations verify permissions, existence (or lack of)
     * required files, and the downloading of resources while staging.
     *
     * If an operation returns anything other than STAGE_OK,
     * all registered patch operations will be called to rollback().
     */
    virtual STAGE_RC stage() = 0;

    /**
     * When all operations are staged, they are called to deploy
     * their changes and do whatever is necessary now that they
     * have their resources.
     *
     * Failure at the deploy stage will incur a rollback() on all
     * the patch operations that have been deployted and/or staged.
     */
    virtual STAGE_RC deploy() = 0;

    /**
     * Rolling-back ALL the changes that have been made to the
     * repository while stage()ing or deploy()ing.
     *
     * Use internal flags or members to keep track of the changes
     * you make if necessary. rollback() might be invoked after
     * operation::stage(), operation::deploy(), or neither!
     */
    virtual void rollback() = 0;

    /**
     * Purging is handy for operations that use the Karazeh cache
     * while deploying or staging. When an operation is called to
     * commit, it is guaranteed that the patch has been rolled out
     * successfully and any transient data will _not_ be needed
     * (no rollback will be invoked).
     *
     * The cache is used for operations that require storing some
     * data in a safe place only for the use of rollbacks if required,
     * but should otherwise be discarded.
     */
    inline virtual void commit() {};

    /** Used internally for exceptions and logging */
    inline virtual string_t tostring() { return ""; }

  protected:
    resource_manager  &rmgr_;
    release_manifest  &rm_;
  };

} // end of namespace kzh

#endif
