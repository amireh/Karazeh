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

//#include "Patcher.h"
#include <typeinfo>

#ifdef KARAZEH_THREAD_PROVIDER
#undef KARAZEH_THREAD_PROVIDER
#endif

// can use only one of these, hence the USE_THREADS flag

#if !defined KARAZEH_THREAD_PROVIDER && defined KARAZEH_THREADS_BOOST
#include "Threads/PixyThreadBoost.h"
#define KARAZEH_THREAD_PROVIDER
#endif

#if !defined KARAZEH_THREAD_PROVIDER && defined KARAZEH_THREADS_QT
#include "Threads/PixyThreadQt.h"
#define KARAZEH_THREAD_PROVIDER
#endif

#if !defined KARAZEH_THREAD_PROVIDER && defined KARAZEH_THREADS_TBB
#include "Threads/PixyThreadTBB.h"
#define KARAZEH_THREAD_PROVIDER
#endif

#if !defined KARAZEH_THREAD_PROVIDER && defined KARAZEH_THREADS_POCO
#include "Threads/PixyThreadPoco.h"
#define KARAZEH_THREAD_PROVIDER
#endif

#ifndef KARAZEH_THREAD_PROVIDER
#include "Threads/PixyThreadless.h"
#endif // ifndef KARAZEH_THREAD_PROVIDOR

#endif
