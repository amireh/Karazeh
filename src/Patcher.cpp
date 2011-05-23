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
 
#include "Patcher.h"
#include "Downloader.h"
#include "Launcher.h"

namespace Pixy {
	Patcher* Patcher::__instance;
	
	Patcher::Patcher() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Patcher");
		mLog->infoStream() << "firing up";
		
		mCurrentVersion = Version(std::string(PIXY_APP_VERSION));
		mPatchListPath = std::string(PROJECT_TEMP_DIR) + "patchlist.txt";
  }
	
	Patcher::~Patcher() {
		
		mLog->infoStream() << "shutting down";
		
		Repository* lRepo = 0;
		while (!mRepos.empty()) {
		  lRepo = mRepos.back();
		  mRepos.pop_back();
		  delete lRepo;
		}
		lRepo = 0;
		
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

	bool Patcher::validateVersion() {
	  
	  Launcher::getSingleton().evtValidateStarted();
	  
	  // download the patch list
	  try {
	    Downloader::getSingleton().fetchPatchList(mPatchListPath);
	  } catch (BadPatchURL& e) {
	    mLog->errorStream()
	      << "could not retrieve patch list, aborting validation. Cause: " 
	      << e.what();
	    return false;
	  }

    /*
     * Find out the latest version
     */
    mPatchList.open(mPatchListPath.c_str());

    if (!mPatchList.is_open() || !mPatchList.good()) {
      mLog->errorStream() << "could not read patch list!";
      throw new BadFileStream("unable to read patch list!");
    }
    
    bool needPatch = false;
    std::string line;
    
    getline(mPatchList,line);
    mTargetVersion = Version(line);
    if (mCurrentVersion != mTargetVersion) {
      mLog->infoStream() 
        << "Version mismatch, patch is due from " 
        << mCurrentVersion.Value << " to " << mTargetVersion.Value;
        
      needPatch = true;
    } else {
      mLog->infoStream() << "Application is up to date.";
    }

    mPatchList.close();
    
    Launcher::getSingleton().evtValidateComplete(needPatch);
    
    return true;
	};
	
	void Patcher::buildRepositories() {
    using std::string;
    
    string line;
    mPatchList.open(mPatchListPath.c_str());

    if (!mPatchList.is_open()) {
      mLog->errorStream() << "could not read patch list!";
      throw new BadFileStream("unable to read patch list!");
    }
    	
    /* we need to locate our current version in the patch list, and track all
     * the entries in between
     */
    bool located = false;
    while ( !located && mPatchList.good() )
    {
      getline(mPatchList,line);
      
      // determine what kind of entry it is:
      if (line == "" || line == "-") { // 1) it's trash
        continue;
      
      } else if (line.substr(0, 7) == "VERSION") { // 2) it's a version signature
        if (Version(line) == mCurrentVersion) {
          // we're done parsing, this is our current version
          located = true;
          mLog->debugStream() << "found our version, no more files to parse";
          break;
        } else {
          // this is another version we need to patch up to
          mLog->debugStream() << "New version found: creating a repository..." << line;
          
          // create a new repository for this version
          Repository *lRepo = new Repository(Version(line));
          mRepos.push_back(lRepo);
          lRepo = 0;
          
          
          continue;
        }
      }
      
      // 3) it's an entry, let's parse it
      // __DEBUG__
      std::cout << "Line: " << line << "\n";
      fflush(stdout);
      
      // make sure the line has at least 2 fields
      std::vector<std::string> elements = Utility::split(line.c_str(), ' ');
      if (elements.size() < 2) {
        mLog->errorStream() << "malformed line: '" << line << "', skipping";
        continue;
      }
      
      // parse the operation type
      PATCHOP op;
      if (elements[0] == "C")
        op = CREATE;
      else if (elements[0] == "D")
        op = DELETE;
      else if (elements[0] == "M")
        op = MODIFY;
      else {
        mLog->errorStream() << "undefined operation symbol: " << elements[0];
        continue;
      }
      
      // add the entries to our Patcher's list for processing
      switch(op) {
        case CREATE:
        case MODIFY:
          mRepos.back()->registerEntry(elements[1], elements[2], op);
          break;
        case DELETE:
          mRepos.back()->registerEntry(elements[1], "", op);
          break;
      }
      
    } // patchlist file parsing loop
    
    mPatchList.close();
    
    // this really shouldn't happen
    if (!located) {
      mLog->warnStream() << "possible file or local version corruption: could not locate our version in patch list";
      throw new BadVersion("could not locate our version in patch list " + mCurrentVersion.Value);
    }
    
	};
	
	bool Patcher::doPatch(void(*callback)(int)) {
	
	  if (callback)
	    (*callback)(0);
	    
	  return true;
	}
	
};
