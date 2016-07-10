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

#ifndef H_KARAZEH_LOGGER_H
#define H_KARAZEH_LOGGER_H

#include "karazeh.hpp"
#include "karazeh_export.h"
#include <sstream>
#include <fstream>
#include <iostream>

namespace kzh {

  using std::ostream;
  using std::ostringstream;

  struct logstream;
  class KARAZEH_EXPORT logger {
  public:

    /**
     * Creates a new logger instance that can be used to spit out
     * messages using a stream interface.
     *
     * Loggers can either be manually instantiated or derived from.
     *
     * The context identifies the module that this logger instance
     * represents, ie: GameManager, db_adapter, UIEngine, etc.
     */
    logger(string_t context);
    virtual ~logger();

    /**
     * Assigns the threshold to use for filtering messages.
     * Messages logged with a level "below" the assigned threshold
     * will not be logged.
     *
     * Available level thresholds (ordered):
     * => D, I, N, W, E, A, C
     */
    static void set_threshold(char level);

    /**
     * Override the stream to append the messages to (defaults to std::cout)
     */
    static void set_stream(std::ostream*);

    /**
     * Messages will be prefixed with a timestamp of the following format:
     * => dd-mm-yyyy hh:mm::ss
     */
    static void enable_timestamps(bool on_or_off);

    /** All messages will be prefixed by the specified application name */
    static void set_app_name(string_t const& app_name);

    static void indent();
    static void deindent();

    static void mute();
    static void unmute();

    /** Message will be prefixed with [D] */
    logstream debug() const;
    /** Message will be prefixed with [I] */
    logstream info() const;
    /** Message will be prefixed with [N] */
    logstream notice() const;
    /** Message will be prefixed with [W] */
    logstream warn() const;
    /** Message will be prefixed with [E] */
    logstream error() const;
    /** Message will be prefixed with [A] */
    logstream alert() const;
    /** Message will be prefixed with [C] */
    logstream crit() const;

    logstream plain() const;

    /**
     * Subsequent messages will be prefixed with {IN_UUID}
     * positioned right after the [LEVEL] part.
     */
    void set_uuid_prefix(string_t const&);

    /** The assigned UUID prefix, if any */
    string_t const& uuid_prefix() const;

  protected:
    void rename_context(string_t const&);

  private:
    ostream& log(char lvl) const;
    string_t context_;

    static ostringstream  sink;
    static ostream*       out;
    static bool           with_timestamps;
    static bool           with_app_name;
    static bool           silenced;
    static string_t       app_name;
    static int            indent_level;

    string_t uuid_prefix_;
  }; // end of logger class

  struct KARAZEH_EXPORT logstream {
    logstream(std::ostream&);
    ~logstream();

    std::ostream &s;

    template<typename T>
    inline logstream& operator<<(T const& data) {
      s << data;
      return *this;
    }
  };

} // end of namespace kzh

#endif
