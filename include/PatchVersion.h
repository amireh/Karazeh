
#ifndef H_PatchVersion_H
#define H_PatchVersion_H

#include <string>
#include <sstream>
#include "PixyUtility.h"
#include "PixyExceptions.h"

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
    inline friend std::ostream& operator<<(std::ostream& stream, Version& version) {
      stream << version.toNumber();
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

}
#endif
