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

#ifndef H_KARAZEH_TEST_H
#define H_KARAZEH_TEST_H

#include "karazeh/logger.hpp"

#include <string>
#include <iostream>
#include <cassert>
#include <exception>
#include <stdexcept>

namespace kzh {

  typedef int result_t;

  class test_forced_teardown : std::runtime_error {
  public:
    inline
    test_forced_teardown(string_t const& msg)
    : std::runtime_error(msg) { }
  };
  class test : public logger {
  public:

    enum {
      passed,
      failed
    };

    explicit test(std::string in_name)
    : name_(in_name),
      result_(failed),
      logger(in_name + "_test")
    {
      enable_timestamps(false);
      info() << "starting";
    }

    virtual ~test()
    {
      stop();
    }

    /**
     * Override with the actual test implementation.
     */
    virtual result_t run(int argc, char** argv)=0;
    virtual void stop()
    {
      info() << "tearing down";
    }

    /** Override this if you need any functionality to be called within the program's main() */
    inline virtual void main(int, char **) {

    };

    inline virtual void report(result_t rc) {
      info() << "finished: " << status_to_string(rc);
    }

  protected:

    /**
     * Marks the beginning of a new stage in the test identified by @name.
     *
     * If you pass @timed=true, the stage will be stopwatched and the time
     * elapsed (in ms) inside it will be displayed when the test is over.
     *
     * The result of the stage is determined by the results of all assertions
     * occuring inside it.
     */
    void stage(string_t, bool) {

    }

    /**
     * Asserts the given condition and registers it for display in the
     * result dump as @state.
     *
     * If the condition is false, the test will NOT abort.
     */
    bool soft_assert(string_t explanation, bool condition) {
      // TODO: track state
      if (!condition) {
        error() << explanation << " \033[0;31m[ FAILED ]\033[0m";
      } else {
        notice() << explanation << " \033[0;32m[ PASSED ]\033[0m";
      }

      return condition;
    }

    /**
     * Asserts the given condition and registers it for display in the
     * result dump as @state.
     *
     * If the condition is false, the test WILL abort.
     */
    bool hard_assert(string_t explanation, bool condition) {
      soft_assert(explanation, condition);

      if (!condition)
        throw test_forced_teardown(explanation);

      return condition;
    }

    std::string name_;
    result_t result_;

    inline string_t status_to_string(result_t rc) {
      if (rc == passed) {
        return "\033[0;32mSUCCESS\033[0m";
      } else {
        return "\033[0;31mFAILURE\033[0m";
      }      
    }
  private:
    test(const test&);
    test& operator=(const test&);

    struct stage_t {
      string_t        name;
      bool            timed;
      unsigned long   elapsed_ms;
      result_t        result;
    };
  };
}
#endif
