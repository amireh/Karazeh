/*
 *  Copyright (c) 2011 Ahmad Amireh <ahmad@amireh.net>
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
 
#ifndef H_Patcher_H
#define H_Patcher_H

#include "Pixy.h"
#include "EventListener.h"
#include "Repository.h"
#include <list>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

namespace Pixy {

typedef enum {
  NO_SUCH_FILE,
  FILE_ALREADY_EXISTS,
  FILE_ALREADY_PATCHED,
  CANNOT_DELETE,
  CANNOT_CREATE,
  CANNOT_MODIFY
} PATCHERROR;

typedef enum {

} PATCHNOTICE;

class Patcher : public EventListener {
  typedef void (Patcher::*t_proc)(PatchEntry*, bool);
  typedef std::map<PATCHOP, t_proc> t_procmap;
  public:
    ~Patcher( void );
    
	  static Patcher* getSingletonPtr();
	  static Patcher& getSingleton();
		
		void update();
		
	  /*! 
	   * \brief
	   *  Validates current application version against the latest version
	   *  tracked in the remote server patch list. 
	   *
	   *  If there's a version mismatch,
	   *  it tracks all the remote changes, and registers them with the Patcher
	   *  in case the Launcher decides to update the app.
	   *
	   * Return:
	   *  true if version is up to date, false if a patch is due
	   *
	   * \warn
	   *  MUST be called before fetchPatchData()
	   */
	  bool validateVersion();
	  
	protected:
	  std::list<Repository*> mRepos;
    std::string mPatchListPath;
    std::ifstream mPatchList;
    Version mCurrentVersion, mTargetVersion;
    
    bool evtDoPatch(Event* inEvt);
    
    void buildRepositories();
    
    t_procmap mProcessors;
    
    void processCreate(PatchEntry* inEntry, bool fCommit);
    void processDelete(PatchEntry* inEntry, bool fCommit);
    void processModify(PatchEntry* inEntry, bool fCommit);
    
    /*! \brief
     *  Validates the PatchEntries in the repository: if there's a CREATE
     *  operation and the file already exists, or if there's a DELETE operation
     *  when the file doesn't exist, we cannot commit this patch; it's invalid.
     */
    bool preprocess(Repository* inRepo);
    
    void updateVersion();
    
  private:
	  Patcher();
	  Patcher(const Patcher&) {}
	  Patcher& operator=(const Patcher&);
	  
    static Patcher *__instance;
    log4cpp::Category* mLog;
};
  
};

#endif
