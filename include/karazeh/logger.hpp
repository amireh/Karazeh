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

#ifndef H_KARAZEH_LOGGER_H
#define H_KARAZEH_LOGGER_H

#include "karazeh.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

namespace kzh {

  using std::ostream;
  using std::ostringstream;

  struct logstream;
  class logger {
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

    /** Message will be prefixed with [D] */
    logstream debug();
    /** Message will be prefixed with [I] */
    logstream info();
    /** Message will be prefixed with [N] */
    logstream notice();
    /** Message will be prefixed with [W] */
    logstream warn();
    /** Message will be prefixed with [E] */
    logstream error();
    /** Message will be prefixed with [A] */
    logstream alert();
    /** Message will be prefixed with [C] */
    logstream crit();

    logstream plain();

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
    ostream& log(char lvl);
    string_t context_;

    static ostringstream  sink;
    static ostream*       out;
    static bool           with_timestamps;
    static bool           with_app_name;
    static string_t       app_name;
    static int            indent_level;

    string_t uuid_prefix_;
  }; // end of logger class
  
  struct logstream {
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
