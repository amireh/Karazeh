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

#ifdef __OBJC__
#include <Cocoa/Cocoa.h>
#endif

extern int bspatch(const char* src, const char* dest, const char* diff);

namespace Pixy {
	Patcher* Patcher::__instance = NULL;

	Patcher::Patcher() {
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY, "Patcher");
		mLog->infoStream() << "firing up";

		fValidated = false;
		fPatched = false;

		using boost::filesystem::exists;
    using boost::filesystem::path;
		using std::fstream;

		// get our current version, and set a default one
		mCurrentVersion = Version(std::string(PIXY_APP_VERSION));
		fstream res;
    path resPath = path(Launcher::getSingleton().getBinPath() + "/" + std::string(PIXY_RESOURCE));
		if (!exists(resPath)) {
      res.open(resPath.c_str(), fstream::out);

      if (!res.is_open() || !res.good()) {

		  } else {
		    res << PIXY_APP_VERSION;
		    res.close();
		  }

		} else {
      res.open(resPath.c_str(), fstream::in);

		  if (res.is_open() && res.good()) {
		    std::string v;
		    getline(res, v);
		    mCurrentVersion = Version(v);
		    res.close();
		  } else {
		    // TODO: inject renderer with error: missing data file
		  }
		}

		mLog->infoStream() << "Application version: " << mCurrentVersion.Value;
		mPatchScriptPath = path(Launcher::getSingleton().getTempPath() + "/" + std::string("/patch.txt")).string();

		mProcessors.insert(std::make_pair<PATCHOP, t_proc>(P_CREATE, &Patcher::processCreate));
		mProcessors.insert(std::make_pair<PATCHOP, t_proc>(P_DELETE, &Patcher::processDelete));
		mProcessors.insert(std::make_pair<PATCHOP, t_proc>(P_MODIFY, &Patcher::processModify));
		mProcessors.insert(std::make_pair<PATCHOP, t_proc>(P_RENAME, &Patcher::processRename));

    mRenderer = 0;
  }

	Patcher::~Patcher() {
    mRenderer = 0;

		mLog->infoStream() << "shutting down";

		Repository* lRepo = 0;
		while (!mRepos.empty()) {
		  lRepo = mRepos.back();
		  mRepos.pop_back();
		  delete lRepo;
		}
		lRepo = 0;

    mProcessors.clear();

#ifndef PIXY_PERSISTENT
    // TODO: boost error checking
    boost::filesystem::remove_all(Launcher::getSingleton().getTempPath());
#endif

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

  void Patcher::update() {
  }

	void Patcher::validate() {
	  if (fValidated)
	    return;

    if (!mRenderer)
      mRenderer = Launcher::getSingleton().getRenderer();

	  mLog->debugStream() << "validating version";

    mRenderer->injectValidateStarted();

	  // download the patch list
	  try {
	    Downloader::getSingleton()._fetchPatchScript(mPatchScriptPath);
	  } catch (BadPatchURL& e) {
	    mLog->errorStream()
	      << "could not retrieve patch list, aborting validation. Cause: "
	      << e.what();

      mRenderer->injectUnableToConnect();
	    return;
	  }

    // find the latest version
    std::ifstream mPatchScript;
    mPatchScript.open(mPatchScriptPath.c_str());

    if (!mPatchScript.is_open() || !mPatchScript.good()) {
      mLog->errorStream() << "could not read patch list!";
      throw BadFileStream("unable to read patch list!");
    }

    bool needPatch = false;
    bool versionFound = false;
    std::string line;
    while (!versionFound) {
      getline(mPatchScript, line);
      if (line.find("VERSION") != std::string::npos)
        versionFound = true;
    }
    mTargetVersion = Version(line);
    if (mCurrentVersion != mTargetVersion) {
      mLog->infoStream()
        << "Version mismatch, patch is due from "
        << mCurrentVersion.Value << " to " << mTargetVersion.Value;

      needPatch = true;
    } else {
      mLog->infoStream() << "Application is up to date.";
    }

    mPatchScript.close();

    fValidated = true;

    return mRenderer->injectValidateComplete( needPatch, mTargetVersion );
	};

	void Patcher::buildRepositories() {
    using std::string;
    using boost::filesystem::path;

    mLog->debugStream() << "building repositories";

    string line;
    std::ifstream mPatchScript;
    mPatchScript.open(mPatchScriptPath.c_str());

    if (!mPatchScript.is_open()) {
      mLog->errorStream() << "could not read patch list!";
      throw BadFileStream("unable to read patch list!");
    }

    /* we need to locate our current version in the patch list, and track all
     * the entries in between
     */
    bool located = false;
    while ( !located && mPatchScript.good() )
    {
      getline(mPatchScript,line);

      // determine what kind of script entry it is:
      if (line == "" || line == "-") { // 1) garbage
        continue;

      //std::cout << "Line: " << line << "\n";
      } else if (line.find("VERSION") != string::npos) { // 2) a version signature
        if (Version(line) == mCurrentVersion) {
          // we're done parsing, this is our current version
          located = true;
          mLog->debugStream() << "found our version, no more files to parse";
          break;
        } else {
          // this is another version we need to patch up to
          mLog->debugStream() << "New version found: creating a repository... " << line;

          // create a new repository for this version
          Repository *lRepo = new Repository(Version(line));
          mRepos.push_front(lRepo);
          lRepo = 0;

          continue;
        }
      }

      Repository *lRepo = mRepos.front();

      // 3) an entry, let's parse it
#ifdef DEBUG
      std::cout << "Line: " << line << "\n";
      fflush(stdout);
#endif

      std::vector<std::string> elements = Utility::split(line.c_str(), ' ');

      if (!validateLine(elements, line)) {
        continue;
      };

      PATCHOP op;
      try {
        op = opFromChar(elements[0].c_str());
      } catch (std::runtime_error& e) {
        mLog->errorStream() << e.what() << " from " << elements[0];
        continue;
      }

      // "local" entry paths need to be adjusted to reflect the project root
      // TODO: use tokens in patch script to manually specify project root
      // and special paths
      elements[1] = path(Launcher::getSingleton().getRootPath() + "/" + elements[1]).string();

      // construct the temp path for C and M operations since these need to be staged
      using boost::filesystem::path;
      path lTempPath;
      if (op == P_CREATE || op == P_MODIFY) {
        lTempPath = path(
          Launcher::getSingleton().getTempPath() + "/" +
          lRepo->getVersion().PathValue + "/" +
          path(elements[2]).filename().string()
        );

        if (!exists(lTempPath.parent_path()))
         create_directory(lTempPath.parent_path());
      }

      switch(op) {
        case P_CREATE:
        case P_MODIFY:
          lRepo->registerEntry(op, elements[1], elements[2], lTempPath.string(), elements[3]);
          break;
        case P_DELETE:
          lRepo->registerEntry(op, elements[1]);
          break;
        case P_RENAME:
          elements[2] = path(Launcher::getSingleton().getRootPath() + "/" + elements[2]).string(); // we assume the path is relative to the root
          lRepo->registerEntry(op, elements[1], elements[2]);
          break;
      }

      lRepo = 0;
    } // patchlist file parsing loop

    mPatchScript.close();

    // this really shouldn't happen
    if (!located) {
      mLog->warnStream() << "possible file or local version corruption: could not locate our version in patch list";
      throw BadVersion("could not locate our version in patch list " + mCurrentVersion.Value);
    }

    mLog->debugStream() << "repositories built";
	};

	bool Patcher::validateLine(std::vector<std::string>& elements, std::string line) {

    // minimum script entry (DELETE)
    if (elements.size() < 2) {
      mLog->errorStream() << "malformed line: '" << line << "', skipping";
      return false;
    }

    // parse the operation type and make sure the required fields exist
    if (elements[0] == "C") {
      if (elements.size() < 4) {
        // CREATE entries must have at least 4 fields
        mLog->errorStream() << "malformed CREATE line: '" << line << "', skipping";
        return false;
      }
    } else if (elements[0] == "M") {
      if (elements.size() < 4) {
        // MODIFY entries must have at least 4 fields
        mLog->errorStream() << "malformed MODIFY line: '" << line << "', skipping";
        return false;
      }
    } else if (elements[0] == "R") {
      if (elements.size() < 3) {
        // RENAME entries must have at least 3 fields
        mLog->errorStream() << "malformed RENAME line: '" << line << "', skipping";
        return false;
      }
    } else {
      mLog->errorStream()
        << "undefined operation symbol: "
        << elements[0] << " from line " << line;
      return false;
    }

	  return true;
	};

	bool Patcher::preprocess(Repository* inRepo) {
	  std::vector<PatchEntry*> entries = inRepo->getEntries();
    bool success = true;
    std::vector<PatchEntry*>::const_iterator entry;
    for (entry = entries.begin(); entry != entries.end() && success; ++entry)
      try {
        (this->*mProcessors[(*entry)->Op])((*entry), false);
      } catch (FileDoesNotExist& e) {
        // TODO: inject renderer with the error        
        mRenderer->injectPatchFailed("A required file does not exist! Possible application corruption, please re-install.", inRepo->getVersion());
        success = false;
        mLog->errorStream() << e.what();
      } catch (FileAlreadyCreated& e) {
        mRenderer->injectPatchFailed("A file already exists! Possible application corruption, please re-install.", inRepo->getVersion());
        success = false;
        mLog->errorStream() << e.what();
      }

    return success;
	};

#ifdef __OBJC__
	void Patcher::operator()() {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    if (!fValidated) {
      validate();
      [pool release];
      return;
    }
    
    // Cocoa renderer will crash if we don't sleep for some time here because
    // this generates a race condition with displaying NSAlert sheets
    sleep(1);
    patch();
    [pool release];
	}
#else
	void Patcher::operator()() {
    if (!fValidated)
      return (void)validate();

    patch();
	}
#endif

	void Patcher::patch() {
	  if (fPatched)
	    return;

	  if (!fValidated) // this really shouldn't happen, but just guard against it
	    validate();

	  try {
	    buildRepositories();
	  } catch (BadVersion& e) {
      mRenderer->injectPatchFailed( std::string("Bad application version"), mCurrentVersion );
      mLog->errorStream() << e.what();
      return;
	  } catch (BadFileStream& e) {
      mRenderer->injectPatchFailed( std::string("Bad file stream"), mCurrentVersion );
      mLog->errorStream() << e.what();
      return;
	  }

	  mLog->infoStream() << "Total patches: " << mRepos.size();

	  bool success = true;
	  std::list<Repository*>::const_iterator repo;
	  for (repo = mRepos.begin(); repo != mRepos.end(); ++repo) {
	    mLog->infoStream() << "----";
	    mLog->infoStream() << "Patching to " << (*repo)->getVersion().Value;

      mRenderer->injectPatchStarted((*repo)->getVersion());

	    // validate the repository
	    if (!preprocess(*repo)) {
	      // we cannot patch
        mRenderer->injectPatchFailed( std::string("Bad application data, have you manually modified the application's files?"), (*repo)->getVersion() );

	      success = false;
	      break;
	    }

	    // download the patch files
	    Downloader::getSingleton()._fetchRepository(*repo);

	    // TODO: perform a check before committing any changes so we can rollback (DONE)
	    std::vector<PatchEntry*> entries = (*repo)->getEntries();
	    std::vector<PatchEntry*>::const_iterator entry;
	    for (entry = entries.begin(); entry != entries.end(); ++entry)
        (this->*mProcessors[(*entry)->Op])((*entry), true);

      mLog->infoStream()
        << "Application successfully patched to "
        << (*repo)->getVersion().Value;

      this->updateVersion((*repo)->getVersion());

      mRenderer->injectPatchComplete( mCurrentVersion );
	  }

	  if (success) {
      mRenderer->injectApplicationPatched( mCurrentVersion );
	  }


	  // we're done, let's clean up.. delete the whole tmp directory
	  // TODO: boost error checking
	  //mLog->infoStream() << "cleaning up temp";
	  //boost::filesystem::remove_all(PROJECT_TEMP_DIR);

	  fPatched = true;
	  return;
	}

	void Patcher::processCreate(PatchEntry* inEntry, bool fCommit) {
	  using boost::filesystem::exists;
	  using boost::filesystem::is_directory;
	  using boost::filesystem::create_directory;
	  using boost::filesystem::path;
	  using boost::filesystem::rename;
	  using boost::filesystem::copy_file;

	  // TODO: boost error checking

	  path local(inEntry->Local);
	  path temp(inEntry->Temp);

	  // make sure the file doesn't already exist, if it does.. abort
	  if (exists(local)) {
	    mLog->errorStream() << "file to be created already exists! " << local << " aborting...";
	    throw FileAlreadyCreated("Could not create file!", inEntry);
	  }

	  if (!fCommit)
	    return;

	  mLog->debugStream() << "creating a file " << local;

	  // create the parent directory if it doesn't exist
	  if (!is_directory(local.parent_path()))
	    create_directory(local.parent_path());

	  // now we move the file

	  // TODO: fix this, we shouldn't be copying, we should be moving
	  // bug #1: attempting to create 1+ files from the same remote source
	  // will result in undefined behaviour as the remote file is stored in the
	  // temp location and it's being removed on the first creation
	  //rename(temp, local);
	  copy_file(temp,local);
	};

	void Patcher::processDelete(PatchEntry* inEntry, bool fCommit) {
	  using boost::filesystem::exists;
	  using boost::filesystem::is_directory;
	  using boost::filesystem::create_directory;
	  using boost::filesystem::path;

	  path local(inEntry->Local);

	  // make sure the file exists!
	  if (!exists(local)) {
	    mLog->errorStream() << "no such file to delete! " << local;
	    throw FileDoesNotExist("Could not delete file!", inEntry);
	  }

    if (!fCommit)
	    return;

	  mLog->debugStream() << "deleting a file " << local;

	  // TODO: boost error checking
    if (is_directory(local))
      boost::filesystem::remove_all(local);
    else
      boost::filesystem::remove(local);
	};

	void Patcher::processModify(PatchEntry* inEntry, bool fCommit) {
	  using boost::filesystem::exists;
	  using boost::filesystem::path;
	  using boost::filesystem::rename;
	  using boost::filesystem::copy_file;

	  if (!exists(inEntry->Local)) {
	    mLog->errorStream() << "No such file to modify! " << inEntry->Local;
      throw FileDoesNotExist("No such file to modify!", inEntry);
	  }

	  if (!fCommit)
	    return;

	  mLog->debugStream() << "modifying a file " << inEntry->Local;

	  // TODO: boost error checking
	  // __DEBUG__
	  std::string tmp = inEntry->Temp;
	  tmp += ".foobar";


	  //patch(inEntry->Local.c_str(), inEntry->Local.c_str(), inEntry->Temp.c_str());
	  copy_file(inEntry->Local, tmp);
	  bspatch(tmp.c_str(), tmp.c_str(), inEntry->Temp.c_str());
	  //remove(path(inEntry->Temp));
	  remove(path(inEntry->Local));
	  rename(tmp, inEntry->Local);


	  // TODO: check if the file was already patched
	};

	void Patcher::processRename(PatchEntry* inEntry, bool fCommit) {
	  using boost::filesystem::exists;
	  using boost::filesystem::is_directory;
	  using boost::filesystem::create_directory;
	  using boost::filesystem::path;
	  using boost::filesystem::rename;
	  using boost::filesystem::copy_file;

	  // TODO: boost error checking

	  path dest(inEntry->Local);
	  path src(inEntry->Remote);

    // the source must exist and the destination must not
	  if (!exists(src)) {
	    mLog->errorStream() << "file to be moved doesn't exist! " << src << " aborting...";
	    throw FileDoesNotExist("Could not move file!", inEntry);
	  }

	  if (exists(dest)) {
	    mLog->errorStream() << "destination of rename already exists! " << dest << " aborting...";
	    throw FileAlreadyCreated("Could not move file!", inEntry);
	  }

	  if (!fCommit)
	    return;

	  // create the parent directory if it doesn't exist
	  if (!is_directory(dest.parent_path()))
	    create_directory(dest.parent_path());

	  mLog->debugStream() << "renaming a file " << src << " to " << dest;

	  rename(src, dest);
	};

	void Patcher::updateVersion(Version const& inVersion) {
    using std::fstream;
    using boost::filesystem::path;

    mCurrentVersion = Version(inVersion);

	  fstream res;
    std::string resPath = path(Launcher::getSingleton().getBinPath() + "/" + std::string(PIXY_RESOURCE)).string();
	  res.open(resPath.c_str(), fstream::out);
	  if (!res.is_open() || !res.good()) {
	    mLog->errorStream() << "couldn't open version resource file " << PIXY_RESOURCE << ", aborting version update";
	    return;
	  }
	  res.write(mCurrentVersion.Value.c_str(), mCurrentVersion.Value.size());
	  res.close();

	};

	PATCHOP Patcher::opFromChar(const char* inC) {

	  if (*inC == 'M')
	    return P_MODIFY;
	  else if (*inC == 'C')
	    return P_CREATE;
	  else if (*inC == 'D')
	    return P_DELETE;
	  else if (*inC == 'R')
	    return P_RENAME;
	  else {
	    throw std::runtime_error("Invalid OP character!");
	  }
	}
};
