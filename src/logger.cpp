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

#include "logger.hpp"
#include <iomanip>

namespace kzh {

  static char levels[] = { 'D','I','N','W','E','A','C' };
  static char threshold = 'D';

  ostringstream logger::sink;
  ostream*      logger::out = &std::cout;
  bool          logger::with_timestamps = true;
  string_t      logger::app_name = "";
  int           logger::indent_level = 0;

  void logger::set_threshold(char level) {
    threshold = level;
  }
  
  void logger::set_stream(std::ostream* s) {
    out = s;
  }
  
  void logger::enable_timestamps(bool f) { 
    with_timestamps = f;
  }

  void logger::set_app_name(string_t const& in_app_name) { 
    app_name = in_app_name;
  }

  void logger::indent() {
    ++indent_level;
  }

  void logger::deindent() {
    --indent_level;
  }

  logger::logger(string_t context)
  : context_(context)
  {
  }

  logger::~logger()
  {
  }

  ostream& logger::log(char lvl) const {
    bool enabled = false;
    for (int i = 0; i < 7; ++i)
      if (levels[i] == threshold) { enabled = true; break; }
      else if (levels[i] == lvl) break;

    if (!enabled)
      return sink;

    if (with_timestamps) {
      struct tm *pTime;
      time_t ctTime; time(&ctTime);
      pTime = localtime( &ctTime );
      std::ostringstream timestamp;
      timestamp
        << std::setw(2) << std::setfill('0') << pTime->tm_mon
        << '-' << std::setw(2) << std::setfill('0') << pTime->tm_mday
        << '-' << std::setw(4) << std::setfill('0') << pTime->tm_year + 1900
        << ' ' << std::setw(2) << std::setfill('0') << pTime->tm_hour
        << ":" << std::setw(2) << std::setfill('0') << pTime->tm_min
        << ":" << std::setw(2) << std::setfill('0') << pTime->tm_sec
        << " ";
      (*out) << timestamp.str();
    }

    if (!app_name.empty()) {
      (*out) << app_name << " ";
    }

    for (int i = 0; i < indent_level; ++i)
      (*out) << "  ";

    (*out) << "[" << lvl << "]" << uuid_prefix_ << " "
      << (context_.empty() ? "" : context_ + ": ");

    return *out;
  }

  void logger::set_uuid_prefix(string_t const& uuid) {
    uuid_prefix_ = " {" + uuid + "}";
  }
  string_t const& logger::uuid_prefix() const {
    return uuid_prefix_;
  }

  logstream logger::debug()  const { return logstream(log('D')); }
  logstream logger::info()   const { return logstream(log('I')); }
  logstream logger::notice() const { return logstream(log('N')); }
  logstream logger::warn()   const { return logstream(log('W')); }
  logstream logger::error()  const { return logstream(log('E')); }
  logstream logger::alert()  const { return logstream(log('A')); }
  logstream logger::crit()   const { return logstream(log('C')); }
  logstream logger::plain()  const { return logstream(*out); }

  logstream::logstream(std::ostream& in_s)
  : s(in_s) {}
  
  logstream::~logstream() {
    s << std::endl;
  }

  void logger::rename_context(string_t const& new_ctx) {
    context_ = new_ctx;
  }
}
