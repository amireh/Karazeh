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

#ifndef H_KARAZEH_OPERATION_UPDATE_H
#define H_KARAZEH_OPERATION_UPDATE_H

#include "karazeh/operation.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/delta_encoder.hpp"
#include "karazeh_export.h"

namespace kzh {

  class KARAZEH_EXPORT update_operation : public operation, protected logger {
  public:
    update_operation(config_t const&, file_manager const&, downloader&, release_manifest&);
    virtual ~update_operation();

    /**
     * Verifies the source's existence and its integrity, then
     * computes the source's signature, and downloads the delta
     * file.
     *
     * Returns STAGE_OK on success, otherwise an error indicated by the return code,
     * see karazeh/operation.hpp for a complete listing.
     */
    virtual STAGE_RC stage();

    /**
     * Applies the delta patch on a clone of the source,
     * then swaps the source into the cache with the patched
     * version.
     */
    virtual STAGE_RC deploy();

    /**
     * Swaps the (now-cached) original source with the patched one.
     */
    virtual void rollback();

    /**
     * Removes the delta patch file and the cached source.
     */
    virtual void commit();

    virtual string_t tostring();

    string_t basis; /** Path to the file to patch */
    string_t basis_checksum;
    uint64_t basis_length;
    string_t delta; /** URI of the delta patch file */
    string_t delta_checksum;
    uint64_t delta_length;
    string_t patched_checksum; /* Checksum of the file post-patching (the new one) */
    uint64_t patched_length; /* Size (in bytes) of the patched file */

  private:
    /** Fully qualified path to the basis file */
    path_t basis_path_;

    /** Path to where the signature will be generated */
    path_t signature_path_;

    /** Path to where the delta will be downloaded */
    path_t delta_path_;

    /** Path to where the patched version of the basis will be stored */
    path_t patched_path_;

    /** Where cached data will be */
    path_t cache_dir_;

    delta_encoder encoder_;

    void cleanup();

    bool patched_;
  };

} // end of namespace kzh

#endif
