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

#ifndef H_KARAZEH_RELEASE_MANIFEST_H
#define H_KARAZEH_RELEASE_MANIFEST_H

#include "karazeh/karazeh.hpp"
 
namespace kzh {
  typedef string_t identity_t;

  struct release_manifest {
    identity_t  checksum;
    string_t    tag;
    string_t    uri;

    string_t tostring() const;
    friend std::ostream& operator<<(std::ostream&, release_manifest*);
  };

  class manifest_error : public std::runtime_error {
  public:
    inline manifest_error(std::string const& s, release_manifest* rm)
    : std::runtime_error("Manifest error: " + rm->tostring() + ": " + s)
    { }
  };

  /** Thrown when an XML node in any manifest is missing a required attribute */
  class missing_attribute : public manifest_error {
  public:
    inline 
    missing_attribute(release_manifest* rm, 
                      std::string const& node, 
                      std::string const& attribute)
    : manifest_error(
      string_t("Missing required attribute '" + attribute + "' in <" + node + ">")
      ,rm)
    { }
  };

  /** Thrown when an XML node in any manifest is missing a required child */
  class missing_node : public manifest_error {
  public:
    inline 
    missing_node(release_manifest* rm, 
                 std::string parent, 
                 std::string child)
    : manifest_error(
      string_t("Parent node <" + parent + "> is missing a required child <" + child + ">"),rm)
    { }
  };

} // end of namespace kzh

#endif
