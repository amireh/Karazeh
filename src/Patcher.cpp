#include "Patcher.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>

namespace Pixy {
	Patcher* Patcher::__instance;
	
	Patcher::Patcher() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Patcher");
		mLog->infoStream() << "firing up";
		
  }
	
	Patcher::~Patcher() {
		
		mLog->infoStream() << "shutting down";
		
		PatchEntry* lEntry = 0;
		while (!mEntries.empty()) {
		  lEntry = mEntries.back();
		  mEntries.pop_back();
		  delete lEntry;
		}
		lEntry = 0;
		
		if (mLog)
		  delete mLog;
	}
	
	Patcher* Patcher::getSingletonPtr() {
		if( !__instance ) {
		    __instance = new Patcher();
		}
		
		return __instance;
	}
	
	Patcher& Patcher::getSingleton() {
		return *getSingletonPtr();
	}
	
  void Patcher::registerEntry(std::string src, std::string dest, PATCHOP op) {
    mLog->infoStream() << "Registering patch entry of type " <<
      ( (op == CREATE) ? "CREATE" : (op == MODIFY) ? "MODIFY" : "DELETE" )
      << " with src: " << src << " and dest: " << dest;
     
    PatchEntry *lEntry = new PatchEntry();
    lEntry->op = op; lEntry->src = src; lEntry->dest = dest; 
    mEntries.push_back(lEntry);
    lEntry = 0;
  }
};
