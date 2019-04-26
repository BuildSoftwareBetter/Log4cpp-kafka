/*
 * Copyright 2002, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2002, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */


#if !defined(h_738a42b1_1502_4483_948a_a69e7bbbee6a)
#define h_738a42b1_1502_4483_948a_a69e7bbbee6a

#include <string>
#include <map>
#include <memory>
#include "Portability.hh"
#include "Appender.hh"
#include "FactoryParams.hh"

namespace log4cpp
{
   class LOG4CPP_EXPORT AppendersFactory
   {
      public:
         typedef FactoryParams params_t;
         typedef std::auto_ptr<Appender> (*create_function_t)(const params_t& params);

         // 单例模式，获取AppenderFactory对象
         static AppendersFactory& getInstance();

         // 注册类型创建者及创建方式
         void registerCreator(const std::string& class_name, create_function_t create_function);

         // 创建appender
         std::auto_ptr<Appender> create(const std::string& class_name, const params_t& params);

         // 判断类型是否注册
         bool registered(const std::string& class_name) const;

      private:
         AppendersFactory(){};

         // 类型及创建方式集合
         typedef std::map<std::string, create_function_t> creators_t;

         // 迭代器
         typedef creators_t::const_iterator const_iterator;

         // 
         creators_t creators_;
   };
}

#endif // h_738a42b1_1502_4483_948a_a69e7bbbee6a
