/*
 *  Copyright (c) 2011 Ahmad Amireh <ahmad@amireh.net>
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

#ifndef H_Repository_H
#define H_Repository_H

#include "Pixy.h"
#include "PatchEntry.h"
#include "PixyUtility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>

using std::ostream;
namespace Pixy {

/*! \struct Version
 * \brief
 *  Every Repository has a version. Versioning scheme supported is:
 *  MAJOR.MINOR.BUILD
 */
struct Version {
  inline Version() { };
  /*! \brief
   *  Convenience constructor using a string. inValue MUST be in the form
   *  of "VERSION X.Y.Z" where X is Major, Y is Minor and Z is Build
   */
  inline Version(std::string inValue) {
    this->Value = inValue;
    std::string tmp = inValue.substr(8, inValue.size());
    std::vector<std::string> elems = Utility::split(tmp, '.');
    if (elems.size() != 3)
      throw new BadVersion("Invalid version scheme " + inValue);

    this->Major = atoi(elems[0].c_str());
    this->Minor = atoi(elems[1].c_str());
    this->Build = atoi(elems[2].c_str());
    this->PathValue = elems[0] + "_" + elems[1] + "_" + elems[2];

  };

  inline Version(int inMajor, int inMinor, int inBuild) {
    this->Major = inMajor;
    this->Minor = inMinor;
    this->Build = inBuild;
    std::stringstream ss;
    ss << "VERSION " << Major << "." << Minor << "." << Build;
    this->Value = ss.str();
    ss.str("");
    ss << Major << "_" << Minor << "_" << Build;
    this->PathValue = ss.str();
  };

  inline ~Version() { };
  // copy ctor
  inline Version(Version const& src) {
    clone(src);
  }
  inline Version& operator=(Version const& rhs) {
    if (this != &rhs) // prevent self assignment
      clone(rhs);

    return *this;
  }
  inline void clone(Version const& src) {
    this->Major = src.Major;
    this->Minor = src.Minor;
    this->Build = src.Build;
    this->Value = src.Value;
    this->PathValue = src.PathValue;
  }

  inline bool operator==(Version const& rhs) {
    return (Major == rhs.Major && Minor == rhs.Minor && Build == rhs.Build);
  }
  inline bool operator!=(Version const& rhs) {
    return (!(*this == rhs));
  }
  inline bool operator<(Version const& rhs) {
    if (*this == rhs)
      return false;

    return (Major < rhs.Major ||
      (Major == rhs.Major && Minor < rhs.Minor) ||
      (Major == rhs.Major && Minor == rhs.Minor && Build < rhs.Build));
  }
  inline bool operator>(Version const& rhs) {
    return (!(*this == rhs) && !(*this < rhs));
  }
  inline friend ostream& operator<<(ostream& stream, Version& version) {
    stream << version.Value;
    return stream;
  }
  inline const std::string toNumber() const {
    std::stringstream lNumber;
    lNumber << Major << "." << Minor << "." << Build;
    return lNumber.str();
  }

  int Major;
  int Minor;
  int Build;
  std::string Value;
  std::string PathValue;
};

/*! \class Repository
 * \brief
 *  A repository represents the state of the application at one *version*.
 *  It is a collection of Entries that define what changed in said version.
 *
 *  \note
 *  The Patcher acts as the manager and interface to all repositories.
 */
class Repository {

  public:
	  Repository(Version inVersion);
    virtual ~Repository();

    void
    registerEntry(PATCHOP op,
                  std::string local,
                  std::string remote = "",
                  std::string temp = "",
                  std::string checksum = "");

		/*! \brief
		 *  Returns all the entries registered in this repository.
		 */
		const std::vector<PatchEntry*>& getEntries();

		/*! \brief
		 *  Returns all entries belonging to the given operation.
		 */
		std::vector<PatchEntry*> getEntries(PATCHOP op);

		Version const& getVersion();

	protected:
	  std::vector<PatchEntry*> mEntries;
    log4cpp::Category* mLog;
    Version mVersion;

  private:
    // Repositores can not be copied
	  Repository(const Repository&);
	  Repository& operator=(const Repository&);
};

};

#endif
