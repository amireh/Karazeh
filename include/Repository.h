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

#ifndef H_Repository_H
#define H_Repository_H

#include "Pixy.h"
#include "PatchEntry.h"
#include "PatchVersion.h"
#include "PixyUtility.h"
#include "PixyExceptions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

using std::ostream;
namespace log4cpp { class Category; }
namespace Pixy {

  /*! \class Repository
   * \brief
   *  A repository represents the state of the application at one *version*.
   *  It is a collection of Entries that define what changed in said version.
   *
   *  \note
   *  The Patcher acts as the manager and interface to all repositories.
   */

  class Repository {

    public:
      Repository(Version inVersion);
      virtual ~Repository();

      PatchEntry* const
      registerEntry(PATCHOP op,
                    std::string local,
                    std::string remote = "",
                    std::string temp = "",
                    std::string checksum = "");

      /*! \brief
       *  Returns all the entries registered in this repository.
       */
      const std::vector<PatchEntry*>& getEntries();

      /*! \brief
       *  Returns all entries belonging to the given operation.
       */
      std::vector<PatchEntry*> getEntries(PATCHOP op);

      Version const& getVersion();

      pbigint_t getSize();
      void setSize(pbigint_t inSize);

      void setArchive(PatchEntry* inArchive);
      PatchEntry* const getArchive();
      bool isArchived();

    protected:
      std::vector<PatchEntry*> mEntries;
      log4cpp::Category* mLog;
      Version mVersion;

      long long mSize;
      PatchEntry* mArchive;

    private:
      // Repositores can not be copied
      Repository(const Repository&);
      Repository& operator=(const Repository&);
  };

};

#endif
