/*
 * Properties.cpp
 *
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "Properties.hh"
#include <cstdlib>
#include "StringUtil.hh"

namespace log4cpp
{

//
Properties::Properties()
{
}

Properties::~Properties()
{
}

// 使用输入流获取属性值
void Properties::load(std::istream &in)
{
    clear();

    std::string fullLine, command;
    std::string leftSide, rightSide;
    char line[256];
    std::string::size_type length;
    bool partiallyRead(false); // fix for bug#137, for strings longer than 256 chars

    while (in)
    {
        // 读取一行数据
        if (in.getline(line, 256) || !in.bad())
        {
            // either string is read fully or only partially (logical but not read/write error)
            if (partiallyRead)
                fullLine.append(line);
            else
                fullLine = line;
            partiallyRead = (in.fail() && !in.bad());
            if (partiallyRead && !in.eof())
            {
                in.clear(in.rdstate() & ~std::ios::failbit);
                continue; // to get full line
            }
        }
        else
        {
            break;
        }
        /* if the line contains a # then it is a comment
               if we find it anywhere other than the beginning, then we assume 
               there is a command on that line, and it we don't find it at all
               we assume there is a command on the line (we test for valid 
               command later) if neither is true, we continue with the next line

               根据#位置，移除注释信息
            */
        length = fullLine.find('#');
        if (length == std::string::npos)
        {
            command = fullLine;
        }
        else if (length > 0)
        {
            command = fullLine.substr(0, length);
        }
        else
        {
            continue;
        }

        // check the command and handle it
        // 查找=出现的第一个位置
        length = command.find('=');
        if (length != std::string::npos)
        {
            leftSide = StringUtil::trim(command.substr(0, length));                            // = 左边，key
            rightSide = StringUtil::trim(command.substr(length + 1, command.size() - length)); // = 右边，value

            // 若value为引用则替换
            _substituteVariables(rightSide);
        }
        else
        {
            continue; // 没找到
        }

        /* handle the command by determining what object the left side
               refers to and setting the value given on the right
               ASSUMPTIONS:  
               1. first object given  on left side is "log4j" or "log4cpp"
               2. all class names on right side are ignored because we
               probably cannot resolve them anyway.
            */

        // strip off the "log4j" or "log4cpp"
        // 移除开头的log4j和log4cpp
        length = leftSide.find('.');
        if (leftSide.substr(0, length) == "log4j" ||
            leftSide.substr(0, length) == "log4cpp")
            leftSide = leftSide.substr(length + 1);

        // add to the map of properties
        insert(value_type(leftSide, rightSide));
    }
}

void Properties::save(std::ostream &out)
{
    for (const_iterator i = begin(); i != end(); ++i)
    {
        out << (*i).first << "=" << (*i).second << std::endl;
    }
}

int Properties::getInt(const std::string &property, int defaultValue)
{
    const_iterator key = find(property);
    return (key == end()) ? defaultValue : std::atoi((*key).second.c_str());
}

bool Properties::getBool(const std::string &property, bool defaultValue)
{
    const_iterator key = find(property);
    return (key == end()) ? defaultValue : ((*key).second == "true");
}

std::string Properties::getString(const std::string &property,
                                  const char *defaultValue)
{
    const_iterator key = find(property);
    return (key == end()) ? std::string(defaultValue) : (*key).second;
}

// 替换值，若value为${}格式，则从环境变量和当前key-value中查找替换
void Properties::_substituteVariables(std::string &value)
{
    std::string result;

    std::string::size_type left = 0;

    // 查找${
    std::string::size_type right = value.find("${", left);
    if (right == std::string::npos)
    {
        // bail out early for 99% of cases
        return;
    }

    while (true)
    {
        // 取${的左边数据
        result += value.substr(left, right - left);
        if (right == std::string::npos)
        {
            break;
        }

        left = right + 2;
        right = value.find('}', left);
        if (right == std::string::npos)
        {
            // no close tag, use string literally 没有关闭符号，相当于移除字符串内的${符号
            result += value.substr(left - 2);
            break;
        }
        else
        {
            // 取${}中间的值为key
            const std::string key = value.substr(left, right - left);
            if (key == "${")
            {
                result += "${";
            }
            else
            {
                // 从环境变量中查找
                char *env_value = std::getenv(key.c_str());
                if (env_value)
                {
                    result += env_value;
                }
                else
                {
                    // 从当前已有的key-value中查找
                    const_iterator it = find(key);
                    if (it == end())
                    {
                        // not found assume empty;
                    }
                    else
                    {
                        result += (*it).second;
                    }
                }
            }
            left = right + 1;
        }

        right = value.find("${", left);
    }

    value = result;
}
} // namespace log4cpp
