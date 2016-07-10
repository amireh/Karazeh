/**
 * karazeh -- the library for patching software
 *
 * Copyright (C) 2011-2016 by Ahmad Amireh <ahmad@amireh.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef H_KARAZEH_UTILITY_H
#define H_KARAZEH_UTILITY_H

#include "karazeh/karazeh.hpp"
#include "karazeh/exception.hpp"
#include "tinyxml2/tinyxml2.h"
#include <algorithm>
#include <sstream>

namespace kzh {
namespace utility {
  static const string_t tinyxml2_errors[] = {
    "XML_NO_ATTRIBUTE",
    "XML_WRONG_ATTRIBUTE_TYPE",
    "XML_ERROR_FILE_NOT_FOUND",
    "XML_ERROR_FILE_COULD_NOT_BE_OPENED",
    "XML_ERROR_FILE_READ_ERROR",
    "XML_ERROR_ELEMENT_MISMATCH",
    "XML_ERROR_PARSING_ELEMENT",
    "XML_ERROR_PARSING_ATTRIBUTE",
    "XML_ERROR_IDENTIFYING_TAG",
    "XML_ERROR_PARSING_TEXT",
    "XML_ERROR_PARSING_CDATA",
    "XML_ERROR_PARSING_COMMENT",
    "XML_ERROR_PARSING_DECLARATION",
    "XML_ERROR_PARSING_UNKNOWN",
    "XML_ERROR_EMPTY_DOCUMENT",
    "XML_ERROR_MISMATCHED_ELEMENT",
    "XML_ERROR_PARSING",
    "XML_CAN_NOT_CONVERT_TEXT",
    "XML_NO_TEXT_NODE"
  };

  inline static
  string_t tinyxml2_ec_to_string(int ec) {
    std::ostringstream s;
    s << tinyxml2_errors[ec-1] << " (" << ec << ')';
    return s.str();
  }

  // helper; converts an integer-based type to a string
  template<typename T>
  inline static void convert(string_t const& s, T& inValue,
            bool fail_if_leftovers = true)
  {
    std::istringstream _buffer(s);
    char c;
    if (!(_buffer >> inValue) || (fail_if_leftovers && _buffer.get(c)))
      throw bad_conversion(s);
  }

  template<typename T>
  inline static T convert_to(const std::string& s, bool fail_if_leftovers = true)
  {
    T _value;
    convert(s, _value, fail_if_leftovers);
    return _value;
  }

  inline static uint64_t tonumber(string_t const& s) {
    return convert_to<uint64_t>(s);
  }

  inline static string_t
  dump_node(const tinyxml2::XMLNode* const node, bool with_brackets = true) {
    std::ostringstream s;
    s << (with_brackets ? "<" : "") << node->Value();
    const tinyxml2::XMLAttribute *a = node->ToElement()->FirstAttribute();
    while (a) {
      s << ' ' << a->Name() << "=\"" << a->Value() << '"';
      a = a->Next();
    }
    s << (with_brackets ? " />" : "");

    return s.str();
  }

  /* splits a string s using the delimiter delim */
  typedef std::vector<string_t> partitioned_string_t;
  inline static
  partitioned_string_t split(const string_t &s, char delim = '\n') {
    partitioned_string_t elems;
    std::stringstream ss(s);
    string_t item;
    while(std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
    return elems;
  }
} // namespace utility
} // namespace kzh

#endif
