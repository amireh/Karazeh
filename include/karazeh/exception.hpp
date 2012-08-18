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

  /** Thrown when the patcher was unable to parse the version or release manifest. */
  class integrity_violation : public internal_error {
  public:
    inline integrity_violation(const std::string& s)
    : internal_error(s)
    { }
  };


  /** Thrown when an un-initialized component has been called to do some
    * work without calling its setup routine first.
    */
  class uninitialized : public internal_error {
  public:
    inline uninitialized(const std::string& s)
    : internal_error(s)
    { }
  };

  /** Thrown when an operation depends on another which has not bee
    * completed successfully (or at all).
    */
  class invalid_state : public internal_error {
  public:
    inline invalid_state(const std::string& s)
    : internal_error(s)
    { }
  };


  class manifest_error : public std::runtime_error {
  public:
    inline manifest_error(std::string const& s, std::string const& prefix = "")
    : std::runtime_error("Manifest error: " + prefix + ": " + s)
    { }
  };

  /** Thrown when an XML node in any manifest is missing a required attribute */
  class missing_attribute : public manifest_error {
  public:
    inline 
    missing_attribute(std::string const& node, 
                      std::string const& attribute,
                      std::string const& xml_file = "")
    : manifest_error(
      std::string("Missing required attribute '" + attribute + "' in <" + node + ">"), xml_file)
    { }
  };

  class invalid_attribute : public manifest_error {
  public:
    inline 
    invalid_attribute(std::string const& node, 
                      std::string const& attribute,
                      std::string const& actual,
                      std::string const& expected,
                      std::string const& xml_file = "")
    : manifest_error(
      std::string("Unexpected value '" + actual + "' of attribute '" 
        + attribute + "', was expecting '" + expected + "'. Node: <" + node + ">"), xml_file)
    { }
  };

  /** Thrown when an XML node in any manifest is missing a required child */
  class missing_node : public manifest_error {
  public:
    inline 
    missing_node(std::string const& parent, 
                 std::string const& child,
                 std::string const& xml_file = "")
    : manifest_error(
      std::string("Parent node <" + parent + "> is missing a required child <" + child + ">"), xml_file)
    { }
  };

  /** Thrown when an XML node has no children when it should */
  class missing_children : public manifest_error {
  public:
    inline 
    missing_children(std::string const& node, 
                     std::string const& xml_file = "")
    : manifest_error(
      std::string("Node <" + node + "> is empty!"), xml_file)
    { }
  };

} // end of namespace kzh

#endif // H_KARAZEH_EXCEPTION_H