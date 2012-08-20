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
#include "karazeh/resource_manager.hpp"

#include <string>
#include <iostream>
#include <cassert>
#include <exception>
#include <stdexcept>
#include <vector>

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
      passed = 1,
      failed = 0
    };

    explicit test(std::string in_name)
    : name_(in_name),
      result_(failed),
      logger(in_name + "_test"),
      stage_(NULL)
    {
      enable_timestamps(false);
      info() << "starting";

      fixture_path_ = path_t("../../fixture").make_preferred();
    }

    virtual ~test()
    {
      stop();

      if (!stages_.empty()) {
        int i = 1;
        plain() << "Status breakdown:";
        for (stages_t::const_iterator stage = stages_.begin();
          stage != stages_.end();
          ++stage, ++i)
        {
          logger::indent();
          plain() << "  Stage#" << i << " - " << (*stage)->title << ": " << status_to_string((*stage)->result);
          logger::deindent();
        }

        while (!stages_.empty()) {
          delete stages_.back();
          stages_.pop_back();
        }

      }
    }

    /**
     * Override with the actual test implementation.
     */
    virtual result_t run(int argc, char** argv) {
      if (argc > 1) {
        for (int i = 0; i < argc; ++i) {
          string_t arg(argv[i]);
          if (arg.find("--fixture-path") != string_t::npos) {
            fixture_path_ = path_t(arg.substr(arg.find("="))).make_preferred();
            debug() << "Fixture path was overridden: " << fixture_path_;
          }
        }
      }
    }
    virtual void stop()
    {
      if (stage_) {
        stages_.push_back(stage_);
        stage_ = NULL;
        deindent();
      }

      info() << "tearing down";
    }

    /** Override this if you need any functionality to be called within the program's main() */
    inline virtual void main(int, char **) {

    };

    inline virtual void report(result_t rc) {
      if (stages_.empty())
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
    void stage(string_t title, bool is_timed = false) {
      if (stage_) {
        // info() << "complete: " << status_to_string(stage_->result);
        deindent();
        stages_.push_back(stage_);
        stage_ = NULL;
      }

      stage_ = new stage_t();
      stage_->title = title;
      stage_->result = passed;
      stage_->timed = is_timed;

      logger::rename_context("");
      plain() << "\033[0;33mStage#" << stages_.size() + 1 << "\033[0m " << title << " --";
      logger::rename_context(name_ + "_test");

      logger::indent();
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

      if (stage_) {
        stage_->result = stage_->result && condition;
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

    inline string_t status_to_string(result_t rc) {
      if (rc == passed) {
        return "\033[0;32mSUCCESS\033[0m";
      } else {
        return "\033[0;31mFAILURE\033[0m";
      }      
    }
    
    string_t name_;
    result_t result_;
    path_t   fixture_path_;

  private:
    test(const test&);
    test& operator=(const test&);

    struct stage_t {
      string_t        title;
      bool            timed;
      unsigned long   elapsed_ms;
      result_t        result;
    };

    typedef std::vector<stage_t*> stages_t;
    stages_t stages_;
    stage_t* stage_;
  };
}
#endif
