#ifndef H_KARAZEH_EXCEPTION_H
#define H_KARAZEH_EXCEPTION_H

#include <exception>
#include <string>
#include <stdexcept>

namespace kzh {

  /** 
   * Thrown when an argument passed to utility::convertTo<> is not a number
   * and thus can not be converted.
   */
  class bad_conversion : public std::runtime_error {
  public:
    inline bad_conversion(const std::string& s)
    : std::runtime_error(s)
    { }
  };

  class internal_error : public std::runtime_error {
  public:
    inline internal_error(const std::string& s)
    : std::runtime_error(s)
    { }

    inline bool is_internal() { return true; }
  };

  /** 
   * Thrown when the resource manager was unable to retrieve a remote file,
   * the message will contain the URI of the resource.
   */
  class invalid_resource : public internal_error {
  public:
    inline invalid_resource(const std::string& uri)
    : internal_error(uri)
    { }
  };

  /** Thrown when the patcher was unable to parse the version or release manifest. */
  class invalid_manifest : public internal_error {
  public:
    inline invalid_manifest(const std::string& s)
    : internal_error(s)
    { }
  };


} // end of namespace kzh

#endif // H_KARAZEH_EXCEPTION_H