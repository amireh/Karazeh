/*
 *  Copyright (C) 1999, 2000, 2001 by Martin Pool <mbp@samba.org>,
 *                2011-2012 Ahmad Amireh <ahmad@amireh.net>
 *  
 *  This implementation was adapted from librsync:
 *    librsync -- the library for network deltas
 *    Website: http://librsync.sourceforge.net/
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

#ifndef H_KARAZEH_DELTA_ENCODER_H
#define H_KARAZEH_DELTA_ENCODER_H

#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/resource_manager.hpp"

extern "C" {
  #include "librsync-0.9.7/librsync.h"
  #include "librsync-0.9.7/fileutil.h"
}

namespace kzh {

  /**
   * @class delta_encoder
   * @brief
   * librsync implementation of delta encoding, used in rdiff.
   */
  class delta_encoder : public logger
  {
  public:
    
    delta_encoder();
    virtual ~delta_encoder();

    /** 
     * Generates the signature from the basis file.
     */
    rs_result signature(path_t const& basis, path_t const& signature);

    /**
     * Generates a patch based on the given signature and the new file.
     */
    rs_result delta(path_t const& signature, path_t const& new_file, path_t const& delta);
    
    /**
     * Applies a patch on the basis file and stores it somewhere else.
     */
    rs_result patch(path_t const& basis, path_t const& delta, path_t const& patched_file);
  };

} // end of namespace kzh

#endif
