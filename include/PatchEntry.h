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

#ifndef H_PatchEntry_H
#define H_PatchEntry_H

#include <string>

namespace Pixy {

typedef enum {
  P_CREATE, //! creates a file locally from a given remote file URL
  P_DELETE, //! deletes a file locally
  P_MODIFY, //! patches a file locally from a given remote diff URL
  P_RENAME,
  P_ARCHIVE
} PATCHOP;

class Repository;
struct PatchEntry {
  inline PatchEntry() { Repo = 0; };
  /*! \brief
   *  convenience constructor
   */
  inline
  PatchEntry(PATCHOP inOp,
             std::string inLocal,
             std::string inRemote = "",
             std::string inChecksum = "",
             Repository* inRepo = 0) {
    Local = inLocal;
    Remote = inRemote;
    Op = inOp;
    Repo = inRepo;
    Checksum = inChecksum;
  }
  inline ~PatchEntry() { Repo = 0; }

  // see ENUM PATCHOP
  PATCHOP Op;

  /*
   * std::string Local:
   *  1) in the case of CREATE, it represents the relative URL from which the dest will be created
   *  2) in the case of MODIFY, it represents the local path of the file to be patched
   *  3) in the case of DELETE, it represents the local path of the file to be deleted
   */
  std::string Local;
  /*
   * std::string Remote:
   *  1) in the case of CREATE, it represents the path at which the file will be created
   *  2) in the case of MODIFY, it represents the relative URL to the diff file to be patched
   *  3) in the case of DELETE, this field is discarded
   */
  std::string Remote;

  // only used in the case of MODIFY, it represents where the diff patch was downloaded to
  std::string Temp;

  // a handle to the repository this entry belongs to
  Repository* Repo;

  /*
   * MD5 checksum used to verify the integrity of downloaded files.
   * In the case of MODIFY entries, the checksum is that of the downloaded diff file,
   * and in the case of CREATE, it's of the downloaded file.
   */
  std::string Checksum;

  pbigint_t Filesize; // in bytes

};

};

#endif // H_PatchEntry_H
