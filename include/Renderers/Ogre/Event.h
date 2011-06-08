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

#ifndef H_Event_H
#define H_Event_H

#include <vector>
#include <exception>
#include <map>
#include <string>
#include <iostream>
#include <stdint.h>

using std::ostream;
using std::map;
namespace Pixy
{

  /*! \class Event "Event.h" "include/Event.h"
   *  \brief
   *  Base Event object that is used and handled to represent game events.
   */
  class Event {
	  public:

		  //! default ctor
		  Event(const std::string inName);

		  //! destructor; resets evt state
		  virtual ~Event();

		  /*! \brief
		   *  Retrieves the name of this Event.
		   */
		  virtual std::string getName() const;

		  void _addHandler();
		  void _removeHandler();
		  uint8_t _getNrHandlers();

		  std::string getProperty(std::string inName) const;
		  virtual void setProperty(const std::string inName, const std::string inValue);
		  virtual bool hasProperty(const std::string& inName);

		  friend ostream& operator<<(ostream& outStream, Event* inEvt);

      virtual void setAny(void* any);
      virtual void* getAny();


	  protected:
		  typedef map< std::string, std::string > property_t;

		  std::string		mName;
		  uint8_t			  nrHandlers;
		  int				    nrProperties;
		  property_t		mProperties;
		  void         *mUserData;

		  virtual void reset();

	  private:
		  Event(const Event& src);
		  Event& operator=(const Event& rhs);
		  //virtual void clone(const Event& src);
	}; // class Event
} // end of namespace
#endif // H_Event_H
