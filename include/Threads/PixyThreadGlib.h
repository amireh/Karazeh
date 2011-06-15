//

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

#ifndef H_PixyThreadless_H
#define H_PixyThreadless_H

#include "glibmm.h"
#include "Patcher.h"

namespace Pixy {

  template <class T>
  class Thread {
    public:
    Thread() {};

    Thread(T& inT) {
      doWork();
    };

    Thread(T* inT) {
      doWork();
    };

    virtual ~Thread() {
      g_return_if_fail(mWorker == 0);

      mWorker = 0;
    };

    private:

    void doWork() {
      mWorker = Glib::Thread::create(sigc::mem_fun(Patcher::getSingleton(), &Patcher::operator()), true);
      mWorker = 0;
    }

    Thread(const Thread& rhs);
    Thread& operator=(const Thread& rhs);

    Glib::Thread* mWorker;
  };

}

#endif // H_PixyThreadless_H
