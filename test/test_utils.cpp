#include "test_utils.hpp"

namespace kzh::utility {
  std::string keywords(std::string const& s) {
    string_t out;
    partitioned_string_t parts = split(s, ' ');
    for (partitioned_string_t::const_iterator i = parts.begin(); i != parts.end(); ++i) {
      out += (*i) + ".*";
    }

    return out.substr(0, out.length() - 2);
  }
}