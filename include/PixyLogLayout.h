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
