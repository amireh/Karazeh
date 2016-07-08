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

#include "karazeh/logger.hpp"
#include <iomanip>

namespace kzh {

  static char levels[] = { 'D','I','N','W','E','A','C' };
  static char threshold = 'D';

  ostringstream logger::sink;
  ostream*      logger::out = &std::cout;
  bool          logger::with_timestamps = true;
  string_t      logger::app_name = "";
  int           logger::indent_level = 0;
  bool          logger::silenced = false;

  void logger::mute() {
    silenced = true;
  }
  void logger::unmute() {
    silenced = false;
  }

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
    if (silenced)
      return sink;

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
