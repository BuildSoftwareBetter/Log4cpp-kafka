/*
 * Properties.hh
 *
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_PROPERTIES_HH
#define _LOG4CPP_PROPERTIES_HH

#include "PortabilityImpl.hh"
#include <string>
#include <iostream>
#include <map>

namespace log4cpp
{

/* 属性类，key-value

    从指定的文件中读取key-value对
    每一行“=”左边为key，右边为value
    */
class Properties : public std::map<std::string, std::string>
{
  public:
    Properties();
    virtual ~Properties();

    // 加载
    virtual void load(std::istream &in);
    virtual void save(std::ostream &out);

    virtual int getInt(const std::string &property, int defaultValue);
    virtual bool getBool(const std::string &property, bool defaultValue);
    virtual std::string getString(const std::string &property,
                                  const char *defaultValue);

  protected:
      // 处理${}引用
    virtual void _substituteVariables(std::string &value);
};
} // namespace log4cpp

#endif // _LOG4CPP_PROPERTIES_HH
