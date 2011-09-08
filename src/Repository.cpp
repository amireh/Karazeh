/*
 *  COpyright (c) 2011 Ahmad Amireh <ahmad@amireh.net>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  cOpy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, cOpy, modify, merge, publish, distribute, sublicense,
 *  and/or sell cOpies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above cOpyright notice and this permission notice shall be included in
 *  all cOpies or substantial portions of the Software.
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

#include "Repository.h"
#include "Pixy.h"

namespace Pixy {

	Repository::Repository(Version inVersion) : mVersion(inVersion) {
	  mVersion = inVersion;
	  mLog = new log4cpp::FixedContextCategory(PIXY_LOG_CATEGORY,"Repository " + mVersion.Value);
		mLog->infoStream() << "constructed";

    mSize = 0;
    mArchive = 0;

  }

	Repository::~Repository() {

		mLog->infoStream() << "destroyed";

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


  PatchEntry* const
  Repository::registerEntry(PATCHOP Op,
                            std::string Local,
                            std::string Remote,
                            std::string Temp,
                            std::string Checksum
                            )
  {
    mLog->debugStream() << "registering entry of type " <<
      ( (Op == P_CREATE) ? "CREATE" : (Op == P_MODIFY)
      ? "MODIFY" : (Op == P_ARCHIVE) ? "ARCHIVE" : "DELETE" )
      << " with: \n\t\tLocal: " << Local << "\n\t\tdest: " << Remote
      << "\n\t\tTemp: " << Temp;

    PatchEntry *lEntry = new PatchEntry();

    lEntry->Op = Op;
    lEntry->Local = Local;
    lEntry->Remote = Remote;
    lEntry->Temp = Temp;
    lEntry->Checksum = Checksum;
    lEntry->Repo = this;

    mEntries.push_back(lEntry);
    return lEntry;
  }

  std::vector<PatchEntry*>
  Repository::getEntries(PATCHOP inOp) {

    std::vector<PatchEntry*> entries;
    std::vector<PatchEntry*>::const_iterator _itr;
    for (_itr = mEntries.begin(); _itr != mEntries.end(); ++_itr) {
      if ((*_itr)->Op == inOp)
        entries.push_back((*_itr));
    }

    return entries;
  }

  const std::vector<PatchEntry*>&
  Repository::getEntries() {
    return mEntries;
  }

  const Version& Repository::getVersion() {
    return mVersion;
  }

  long long Repository::getSize() {
    return mSize;
  }
  void Repository::setSize(long long inSize) {
    mSize = inSize;

  }

  void Repository::setArchive(PatchEntry* inArchive) {
    mArchive = inArchive;
  }
  PatchEntry* const Repository::getArchive() {
    return mArchive;
  }
  bool Repository::isArchived() {
    return mArchive != NULL;
  }

};
