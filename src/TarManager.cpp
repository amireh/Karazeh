#include "TarManager.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <memory>

namespace Pixy {

  TarManager::TarManager() {

  }

  TarManager::~TarManager() {

  }

  int
  TarManager::extract( const char* in, const char* out ) {
    FILE* fin = fopen(in, "rb");
    if (!fin) {
      std::cerr << "couldn't open tar archive @ " << in << ", aborting\n";
      return 0;
    }

    TarHeader h;
    //fread(&h, sizeof(char), 512, fin);
    //memcpy(&h, fin, 512);
    //std::cout << "file name : " << h.Name << "\n";

    fclose(fin);
  };
}
