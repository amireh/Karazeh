#include "karazeh/patcher.hpp"

int main(int argc, char** argv) {
  using namespace kzh;

  logger::enable_timestamps(false);
  
  patcher p;
  if (p.identify("http://localhost:9333/version.xml")) {
    p.is_update_available();    
  }

  return 0;
}