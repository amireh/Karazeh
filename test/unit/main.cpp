#include "karazeh/karazeh.hpp"
#include "karazeh/logger.hpp"
#include <gtest/gtest.h>

class kzh_unit_test_env : public ::testing::Environment {
 public:
  virtual ~kzh_unit_test_env() {}
  // Override this to define how to set up the environment.
  virtual void SetUp() {
    kzh::logger::mute();
  }
  // Override this to define how to tear down the environment.
  virtual void TearDown() {
    
  }
};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new kzh_unit_test_env);
  return RUN_ALL_TESTS();  
}