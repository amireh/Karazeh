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

#ifndef H_KARAZEH_PATCHER_H
#define H_KARAZEH_PATCHER_H

#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include "karazeh/resource_manager.hpp"
#include "tinyxml2/tinyxml2.h"
#include <map>
#include <vector>
 
namespace kzh {
  using tinyxml2::XMLDocument;

  typedef string_t identity_t;
  class patcher : protected logger {
  public:
    
    patcher();
    virtual ~patcher();

    /**
     * Retrieves the manifest from one of the assigned patch servers
     * and attempts to identify the application's current version.
     *
     * Returns true if the version has been identified and is valid,
     * false otherwise.
     *
     * @throw kzh::invalid_resource if the manifest could not be retrieved
     * @throw kzh::invalid_manifest if the manifest could not be parsed
     */
    bool identify(string_t const& version_manifest_url);

    /**
     * Determines whether an update is available. True if a patch is pending,
     * and false if the application is up to date.
     *
     * @throw kzh::unidentified if the application version has not been identified
     */
    bool is_update_available();

  protected:
    typedef std::vector<string_t> identity_files_t;

    identity_t version_;
    identity_files_t identity_files_;

    /** Whether the version has been correctly identified and is a valid one */
    bool is_identified() const;

  private:
    XMLDocument version_manifest_;
  };

} // end of namespace kzh

#endif
