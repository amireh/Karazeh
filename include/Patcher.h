#ifndef H_Patcher_H
#define H_Patcher_H

#include "Pixy.h"
#include <vector>
#include <boost/thread.hpp>

namespace Pixy {

typedef enum {
  CREATE,
  DELETE,
  MODIFY
} PATCHOP;

class Patcher {

  struct PatchEntry;
  public:
    ~Patcher( void );
    
	  static Patcher* getSingletonPtr();
	  static Patcher& getSingleton();
		
		void registerEntry(std::string src, std::string dest, PATCHOP op);
		bool doPatch(void(*callback)(int));
		
	protected:
	  std::vector<PatchEntry*> mEntries;
	  
  private:
	  Patcher();
	  Patcher(const Patcher&) {}
	  Patcher& operator=(const Patcher&);
	  
    static Patcher *__instance;
    log4cpp::Category* mLog;

    struct PatchEntry {
      /*
       * PATCHOP op:
       *  1) CREATE creates a file locally from a given remote file URL
       *  2) MODIFY patches a file locally from a given remote diff URL
       *  3) DELETE deletes a file locally 
       */
      PATCHOP op;
      /* 
       * std::string src:
       *  1) in the case of CREATE, it represents the relative URL from which the dest will be created
       *  2) in the case of MODIFY, it represents the local path of the file to be patched
       *  3) in the case of DELETE, it represents the local path of the file to be deleted
       */
      std::string src; 
      /* 
       * std::string dest:
       *  1) in the case of CREATE, it represents the path at which the file will be created
       *  2) in the case of MODIFY, it represents the relative URL to the diff file to be patched
       *  3) in the case of DELETE, this field is discarded
       */ 
      std::string dest;
    };
    
};
  
};

#endif
