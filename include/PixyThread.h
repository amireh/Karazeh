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

#ifndef H_PixyThread_H
#define H_PixyThread_H
#define KARAZEH_USE_QT_THREADS
#include "Pixy.h"
#include <typeinfo>
#ifdef KARAZEH_USE_QT_THREADS
#include <QThread>
#else
#include <boost/thread.hpp>
#endif

namespace Pixy {

#ifdef KARAZEH_USE_QT_THREADS
template<class T>
class Thread : public QThread {
  public:

  Thread() {};
  void run() {
    (*mT)();
  }

  Thread(T& inT) {
    mT = &inT;
    this->start();
  };

  Thread(T* inT) {
    mT = inT;
    this->start();
  };
  virtual ~Thread() {
    mT = 0;
  };

  private:
  Thread(const Thread& rhs);
  Thread& operator=(const Thread& rhs);
  T* mT;
};
#else // use boost threads instead
template <class T>
class Thread {
  public:
  Thread() {};

  Thread(T& inT) {
    mWorker = boost::thread(boost::ref(inT));
  };

  Thread(T* inT) {
    mWorker = boost::thread(inT);
  };

  virtual ~Thread() {
    mWorker.detach();
  };

  private:
  Thread(const Thread& rhs);
  Thread& operator=(const Thread& rhs);

  boost::thread mWorker;
};
#endif // KARAZEH_USE_QT_THREADS

};

#endif
