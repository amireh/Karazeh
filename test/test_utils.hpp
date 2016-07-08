#include "karazeh/karazeh.hpp"
#include "karazeh/utility.hpp"
#include <gtest/internal/gtest-internal.h>

namespace kzh {
  typedef struct {
    string_t server_host;
    path_t   fixture_path;
  } test_config_t;

  extern test_config_t test_config;
}

static std::string keywords(std::string const& s) {
  using ::kzh::utility::partitioned_string_t;
  using ::kzh::string_t;

  string_t out;
  partitioned_string_t parts = ::kzh::utility::split(s, ' ');

  for (partitioned_string_t::const_iterator i = parts.begin(); i != parts.end(); ++i) {
    out += (*i) + ".*";
  }

  return out.substr(0, out.length() - 2);
}
// TODO: use internal RE instead of gtest's

#define ASSERT_THROW_WITH(statement, expected_exception, expected_msg) \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
  if (::testing::internal::ConstCharPtr gtest_msg = "") { \
    bool gtest_caught_expected = false; \
    try { \
      GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); \
    } \
    catch (expected_exception const& e) { \
      using namespace ::testing::internal; \
      std::string errmsg = e.what(); \
      std::transform(errmsg.begin(), errmsg.end(), errmsg.begin(), ::tolower); \
      std::string expmsg(expected_msg); \
      std::transform(expmsg.begin(), expmsg.end(), expmsg.begin(), ::tolower); \
      gtest_caught_expected = errmsg.find(expmsg) != std::string::npos; \
      if (!gtest_caught_expected) { \
        std::ostringstream m; \
        m << "Expected: " #statement " throws an exception of type " \
            #expected_exception " with message '" \
          << expmsg \
          << "'.\n Actual: it throws a different message: '" \
          << errmsg << "'."; \
        gtest_msg.value = strdup(m.str().c_str()); \
        goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
      } \
    } \
    catch (std::exception const& e) { \
      std::ostringstream m; \
      m << \
          "Expected: " #statement " throws an exception of type " \
          #expected_exception ".\n  Actual: it throws a different type with message: " \
          << e.what() \
          << "."; \
      gtest_msg.value = strdup(m.str().c_str()); \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
    } \
    if (!gtest_caught_expected) { \
      gtest_msg.value = \
          "Expected: " #statement " throws an exception of type " \
          #expected_exception ".\n  Actual: it throws nothing."; \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
    } \
  } else \
    GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__): \
      GTEST_FATAL_FAILURE_(gtest_msg.value)

#define KZH_ASSERT_THROW(statement, expected_exception) \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
  if (::testing::internal::ConstCharPtr gtest_msg = "") { \
    bool gtest_caught_expected = false; \
    try { \
      GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); \
    } \
    catch (expected_exception const&) { \
      gtest_caught_expected = true; \
    } \
    catch (std::exception const& e) { \
      std::ostringstream m; \
      m << \
          "Expected: " #statement " throws an exception of type " \
          #expected_exception ".\n  Actual: it throws a different type with message: " \
          << e.what() \
          << "."; \
      gtest_msg.value = strdup(m.str().c_str()); \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
    } \
    if (!gtest_caught_expected) { \
      gtest_msg.value = \
          "Expected: " #statement " throws an exception of type " \
          #expected_exception ".\n  Actual: it throws nothing."; \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
    } \
  } else \
    GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__): \
      GTEST_FATAL_FAILURE_(gtest_msg.value)


#ifdef ASSERT_THROW
  #undef ASSERT_THROW
#endif

#define ASSERT_THROW KZH_ASSERT_THROW
