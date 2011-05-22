#include "PixyLogLayout.h"
#include "log4cpp/Priority.hh"
#include "log4cpp/FactoryParams.hh"
#ifdef LOG4CPP_HAVE_SSTREAM
#include <sstream>
#endif

namespace Pixy {

  PixyLogLayout::PixyLogLayout() {
	  fTimestampsOn = false;
	  fVanilla = false;
  }
  
  PixyLogLayout::~PixyLogLayout() {
  }

  std::string PixyLogLayout::format(const LoggingEvent& event) {

	if (fVanilla)
		return event.message;
		
	
	std::ostringstream message;        
	
      const std::string& priorityName = Priority::getPriorityName(event.priority);
	if (fTimestampsOn)
		message << event.timeStamp.getSeconds() << " ";
      
	// start off with priority
	message << priorityName	<< "\t| ";
	
	// append NDC
	if (event.ndc != "")
		message << event.ndc << ": ";
	
	message << event.message << "\n";
	
      return message.str();
  }

  std::auto_ptr<Layout> create__layout(const FactoryParams& params)
  {
	return std::auto_ptr<Layout>(new PixyLogLayout);
  }

  void PixyLogLayout::setTimestamps(bool fLogTimestamps) {
	  fTimestampsOn = fLogTimestamps;
  }

  void PixyLogLayout::setVanilla(bool inVanilla) {
	  fVanilla = inVanilla;
  }

}
