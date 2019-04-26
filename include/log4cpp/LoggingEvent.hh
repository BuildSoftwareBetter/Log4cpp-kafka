/*
 * LoggingEvent.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_LOGGINGEVENT_HH
#define _LOG4CPP_LOGGINGEVENT_HH

#include <log4cpp/Portability.hh>
#include <string>

#include <log4cpp/Priority.hh>
#include <log4cpp/TimeStamp.hh>

/**
 * The top level namespace for all 'Log for C++' types and classes.
 **/
namespace log4cpp
{

/**
     * The internal representation of logging events. When a affirmative
     * logging decision is made a <code>LoggingEvent</code> instance is
     * created. This instance is passed around the different log4cpp
     * components.
     *
     * <p>This class is of concern to those wishing to extend log4cpp. 
     * 
     * 日志记录
     **/
struct LOG4CPP_EXPORT LoggingEvent
{
  public:
    /**
         * Instantiate a LoggingEvent from the supplied parameters.
         *
         * <p>Except <code>timeStamp</code> all the other fields of
         * <code>LoggingEvent</code> are filled when actually needed.
         * <p>
         * @param category The category of this event.
         * @param message  The message of this event.
         * @param ndc The nested diagnostic context of this event. 
         * @param priority The priority of this event.
         **/
    LoggingEvent(const std::string &category, const std::string &message,
                 const std::string &ndc, Priority::Value priority);

    /** The category name. 日志记录输出的category */
    const std::string categoryName;

    /** The application supplied message of logging event. 日志内容 */
    const std::string message;

    /** The nested diagnostic context (NDC) of logging event.  日志上下文*/
    const std::string ndc;

    /** Priority of logging event. 事件（等级） */
    Priority::Value priority;

    /** The name of thread in which this logging event was generated,
            e.g. the PID. 

            线程id
        */
    const std::string threadName;

    /** The number of seconds elapsed since the epoch 
            (1/1/1970 00:00:00 UTC) until logging event was created. 
            
            时间戳
            */
    TimeStamp timeStamp;
};
} // namespace log4cpp

#endif // _LOG4CPP_LOGGINGEVENT_HH
