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
 
#ifndef H_PixyLogLayout_H
#define H_PixyLogLayout_H

#include <log4cpp/FixedContextCategory.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Portability.hh>
#include <log4cpp/Layout.hh>
#include <memory>

using namespace log4cpp;
namespace Pixy {
	
  /**
  * PixyLogLayout is a simple fixed format Layout implementation. 
  **/
  class LOG4CPP_EXPORT PixyLogLayout : public Layout {
    public:
      PixyLogLayout();
      virtual ~PixyLogLayout();

      /**
      * Formats the LoggingEvent in PixyLogLayout style:<br>
      * "timeStamp priority category ndc: message"
      **/
      virtual std::string format(const LoggingEvent& event);

      /**
      * Controls whether to append timestamps to messages or not.
      */
      virtual void setTimestamps(bool fLogTimestamps);

      /* doesnt format the message */
      virtual void setVanilla(bool inVanilla);
    protected:
      bool fTimestampsOn;
      bool fVanilla;
  };        
}

#endif // END OF H_PixyLogLayout_H
