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

extern "C" int patch(const char* src, const char* dest, const char* diff);

namespace Pixy {
	Patcher* Patcher::__instance;
	
	Patcher::Patcher() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Patcher");
		mLog->infoStream() << "firing up";
		
		mCurrentVersion = Version(std::string(PIXY_APP_VERSION));
		mPatchListPath = std::string(PROJECT_TEMP_DIR) + "patchlist.txt";
		
		mProcessors.insert(std::make_pair<PATCHOP, t_proc>(CREATE, &Patcher::processCreate));
		mProcessors.insert(std::make_pair<PATCHOP, t_proc>(DELETE, &Patcher::processDelete));
		mProcessors.insert(std::make_pair<PATCHOP, t_proc>(MODIFY, &Patcher::processModify));
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
      
      // make sure the line has at least 2 fields (DELETE case)
      std::vector<std::string> elements = Utility::split(line.c_str(), ' ');
      if (elements.size() < 2) {
        mLog->errorStream() << "malformed line: '" << line << "', skipping";
        continue;
      }
      
      // parse the operation type and make sure the required fields exist
      PATCHOP op;
      if (elements[0] == "C") {
        op = CREATE;
        if (elements.size() < 3) { // CREATE entries must have at least 3 fields
          mLog->errorStream() << "malformed CREATE line: '" << line << "', skipping";
          continue;
        }
      } else if (elements[0] == "D") {
        op = DELETE;
      } else if (elements[0] == "M") {
        op = MODIFY;
        if (elements.size() < 4) { // MODIFY entries must have at least 4 fields
          mLog->errorStream() << "malformed MODIFY line: '" << line << "', skipping";
          continue;
        }
      } else {
        mLog->errorStream() << "undefined operation symbol: " << elements[0];
        continue;
      }
      
      // "local" entry paths need to be adjusted to reflect the project root
      elements[1] = PROJECT_ROOT + elements[1];
      
      // add the entries to our Patcher's list for processing
      switch(op) {
        case CREATE:
          mRepos.back()->registerEntry(op, elements[1], elements[2]);
          break;
        case MODIFY:
          elements[3] = PROJECT_ROOT + elements[3];
          mRepos.back()->registerEntry(op, elements[1], elements[2], elements[3]);
          break;
        case DELETE:
          mRepos.back()->registerEntry(op, elements[1]);
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
	  
	  try {
	    buildRepositories();
	  } catch (BadVersion& e) {
	  
	  } catch (BadFileStream& e) {
	  
	  } catch (std::exception& e) {
	  
	  }
	    
	  mLog->infoStream() << "Patching " << mRepos.size() << " versions";
	  
	  std::vector<Repository*>::const_iterator repo;
	  for (repo = mRepos.begin(); repo != mRepos.end(); ++repo) {
	    // download the patch files
	    Downloader::getSingleton().fetchRepository(*repo);
	    
	    // process them
	    std::vector<PatchEntry*> entries = (*repo)->getEntries();
	    
	    std::vector<PatchEntry*>::const_iterator entry;
	    for (entry = entries.begin(); entry != entries.end(); ++entry)
	      (this->*mProcessors[(*entry)->Op])((*entry));
	    
	  }
	  
	  if (callback)
	    (*callback)(0);
	    
	  return true;
	}
	
	void Patcher::processCreate(PatchEntry* inEntry) {
	  mLog->infoStream() << "creating a file";
	
	};
	void Patcher::processDelete(PatchEntry* inEntry) {
	  using boost::filesystem::exists;
	  using boost::filesystem::is_directory;
	  using boost::filesystem::create_directory;
	  using boost::filesystem::path;

    path lPath = path(inEntry->Local);
	  mLog->infoStream() << "deleting a file";
	  // TODO: boost error checking
	  if (exists(lPath))
	    if (is_directory(lPath))
	      boost::filesystem::remove_all(lPath);
	    else
	      boost::filesystem::remove(lPath);
	  else {
	    mLog->errorStream() << "no such file to delete! " << lPath;
	  }
	};
	
	void Patcher::processModify(PatchEntry* inEntry) {
	  mLog->infoStream() << "modifying a file";
	  
	  patch(inEntry->Local.c_str(), inEntry->Local.c_str(), inEntry->Temp.c_str());
	};
};
