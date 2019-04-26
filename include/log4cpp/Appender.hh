/*
 * Appender.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_APPENDER_HH
#define _LOG4CPP_APPENDER_HH

#include <log4cpp/Portability.hh>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log4cpp/Priority.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/LoggingEvent.hh>
#include <log4cpp/threading/Threading.hh>

namespace log4cpp {
    class LOG4CPP_EXPORT Filter;
    
    /**
     *  Implement this interface for your own strategies for printing log
     *  statements.
     * 
     *  实现当前接口来自定义日志输出
     *  日志输出工具
     **/
    class LOG4CPP_EXPORT Appender {
		friend class Category;  // 友元类
        public:
        
        /**
         * Get a pointer to an exitsing Appender.
         * @param name The name of the Appender to return.
         * @returns a pointer to an existing Appender, or NULL if no appender
         * with the specfied name exists.
         * 
         * 获取指定名字的appender指针
         **/
        static Appender* getAppender(const std::string& name);
        
        /**
         * Call reopen() on all existing Appenders.
         * @returns true if all Appenders returned true on their reopen() call.
         * 
         * 重新打开所有appender
         **/
        static bool reopenAll();
        
        /**
         * Call reopen() on all existing Appenders.
         * @returns true if all Appenders returned true on their reopen() call.
         * 
         * 关闭全部appender
         **/
        static void closeAll();
        
        protected:
        /**
         * Constructor for Appender. Will only be used in getAppender() (and
         * in derived classes of course).
         * @param name The name of this Appender.
         * 
         * 构造函数
         **/
        Appender(const std::string& name);
        
        public:
        /**
         * Destructor for Appender.
         * 
         * 析构函数
         **/
        virtual ~Appender();
        
        /**
         * Log in Appender specific way.
         * @param event  The LoggingEvent to log.
         * 
         * 添加日志记录到当前appender
         **/
        virtual void doAppend(const LoggingEvent& event) = 0;
        
        /**
         * Reopens the output destination of this Appender, e.g. the logfile
         * or TCP socket.
         * @returns false if an error occured during reopening, true otherwise.
         * 
         * 重新打开当前appender
         **/
        virtual bool reopen() = 0;
        
        /**
         * Release any resources allocated within the appender such as file
         * handles, network connections, etc.
         * 
         * 关闭当前appender
         **/
        virtual void close() = 0;
        
        /**
         * Check if the appender uses a layout.
         *
         * @returns true if the appender implementation requires a layout.
         * 
         * 确认当前appender是否使用了一个layout
         **/
        virtual bool requiresLayout() const = 0;
        
        /**
         * Set the Layout for this appender.
         * @param layout The layout to use.
         * 
         * 设置layout
         **/
        virtual void setLayout(Layout* layout) = 0;
        
        /**
         * Get the name of this appender. The name identifies the appender.
         * @returns the name of the appender.
         * 
         * 获取appender名称
         **/
        inline const std::string& getName() const { return _name; };
        
        /**
         * Set the threshold priority of this Appender. The Appender will not
         * appender LoggingEvents with a priority lower than the threshold.
         * Use Priority::NOTSET to disable threshold checking.
         * @param priority The priority to set.
         * 
         * 设置优先级阈值，appender不会附加低于阈值的日志记录
         **/
        virtual void setThreshold(Priority::Value priority) = 0;
        
        /**
         * Get the threshold priority of this Appender.
         * @returns the threshold
         * 
         * 获取阈值
         **/
        virtual Priority::Value getThreshold() = 0;
        
        /**
         * Set a Filter for this appender.
         * 
         * 设置过滤器
         **/
        virtual void setFilter(Filter* filter) = 0;
        
        /**
         * Get the Filter for this appender.
         * @returns the filter, or NULL if no filter has been set.
         * 
         * 获取日志过滤器
         **/
        virtual Filter* getFilter() = 0;
        
		private:

        // 日志appender集合
        typedef std::map<std::string, Appender*> AppenderMap;

        // 获取进程内的appender集合
        static AppenderMap& _getAllAppenders();

        // 删除进程内全部appender
        static void _deleteAllAppenders();

        // 删除指定appender集合
		static void _deleteAllAppendersWOLock(std::vector<Appender*> &appenders);

        // 添加appender
        static void _addAppender(Appender* appender);

        // 移除指定appender
        static void _removeAppender(Appender* appender);

        // 当前appender名字
        const std::string _name;

		public:
		class AppenderMapStorage {
		public:
			Appender::AppenderMap* _allAppenders;	// single shared instance, nifty-counter defensed
	        threading::Mutex _appenderMapMutex;	// mutex protecting map from multiple thread access 

			AppenderMapStorage();
			~AppenderMapStorage();
		};
		class LOG4CPP_EXPORT AppenderMapStorageInitializer {
		public:
			AppenderMapStorageInitializer();
			~AppenderMapStorageInitializer();
		};
		private:
        static AppenderMapStorage &_appenderMapStorageInstance;
    };

    // appender初始化工具
	static Appender::AppenderMapStorageInitializer appenderMapStorageInitializer; // static initializer for every translation unit

    // appender集合
    typedef std::set<Appender *> AppenderSet;

}

#endif // _LOG4CPP_APPENDER_HH
