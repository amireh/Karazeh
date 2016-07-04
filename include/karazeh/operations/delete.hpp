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

#ifndef H_KARAZEH_OPERATION_DELETE_H
#define H_KARAZEH_OPERATION_DELETE_H

#include "karazeh/operation.hpp"
#include "karazeh/logger.hpp"
 
namespace kzh {
  
  class delete_operation : public operation, protected logger {
  public:
    delete_operation(resource_manager&, release_manifest&);
    virtual ~delete_operation();

    /**
     * Checks whether the source to be removed exists.
     * 
     * Returns STAGE_OK on success, otherwise an error indicated by the return code,
     * see karazeh/operation.hpp for a complete listing.
     */
    virtual STAGE_RC stage();

    /** 
     * Moves the file or directory at a given path to Karazeh's cache.
     * The entry is purged from the cache if the patch was successful
     * (no rollback required).
     */
    virtual STAGE_RC deploy();

    /**
     * Moves the item that was deleted from its place in the cache to its original
     * path.
     */
    virtual void rollback();

    virtual void commit();

    virtual string_t tostring();

    string_t dst_path;

  private:
    path_t dst_dir_;
    path_t cache_dir_;
    path_t cache_path_;
    
    bool deleted_;
  };

} // end of namespace kzh

#endif
