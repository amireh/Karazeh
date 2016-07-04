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

#ifndef H_KARAZEH_UTILITY_H
#define H_KARAZEH_UTILITY_H

#include "karazeh/karazeh.hpp"
#include "karazeh/exception.hpp"
#include "tinyxml2/tinyxml2.h"
#include <algorithm>
 
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
    using namespace tinyxml2;

    std::ostringstream s;
    s << (with_brackets ? "<" : "") << node->Value();
    const XMLAttribute *a = node->ToElement()->FirstAttribute();
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

  inline static
  string_t sanitize(string_t const& s) {
    string_t out(s);
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    std::replace(out.begin(),   out.end(), ' ', '_');    
    return out;
  }
} // namespace utility
} // namespace kzh

#endif
