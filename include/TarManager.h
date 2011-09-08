/* Credit goes to: Brendan Long <http://supremerule.net> */

#ifndef H_TarManager_H
#define H_TarManager_H

#include <list>

namespace Pixy {

  struct TarFile;
  class TarManager {
    public:
      TarManager();
      ~TarManager();

      int
      extract( const char* in, const char* out );

    private:
      struct TarHeader {
        char Name[100];
        char Mode[8];
        char OwnerID[8];
        char GroupID[8];
        char Size[12];
        char LastModTime[12];
        char Checksum[8];
        char Type[1];
        char LinkName[100];
      };

      struct TarFile {
        TarHeader Header;
        char* Contents;
        char* Filepath;
      };

      std::list<TarFile*> mFiles;

      int octalStringToInt(char *string, unsigned int size) {
        unsigned int output = 0;
        while(size > 0){
          output = output*8 + *string - '0';
          string++;
          size--;
        }
        return output;
      }
  };

}
#endif
