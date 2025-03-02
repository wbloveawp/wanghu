#ifndef WHCOMMON_HEAD_FILE
#define WHCOMMON_HEAD_FILE

#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class CWHCommon
{
public:
    //字符串转化成T型
    template<typename T> 
    static T strto(const string &sStr);

    //字符串转化成T型
    template<typename T> 
    static T strto(const string &sStr, const string &sDefault);

    //T型转换成字符串
    template<typename T> 
    inline static string tostr(const T &t)
    {
        ostringstream sBuffer;
        sBuffer << t;
        return sBuffer.str();
    }   
};

namespace p
{
    template<typename D>
    struct strto1
    {
        D operator()(const string &sStr)
        {
            string s = "0";

            if(!sStr.empty())
            {
                s = sStr;
            }

            istringstream sBuffer(s);

            D t;
            sBuffer >> t;

            return t;
        }
    };

    template<>
    struct strto1<char>
    {
        char operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return sStr[0];
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned char>
    {
        unsigned char operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return (unsigned char)sStr[0];
            }
            return 0;
        }
    };

    template<>
    struct strto1<short>
    {
        short operator()(const string &sStr)
        {
            if (!sStr.empty()) {
                if (sStr.find("0x") == 0) {
                    return (short) ::strtol(sStr.c_str(), NULL, 16);
                }
                else {
                    return atoi(sStr.c_str());
                }
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned short>
    {
        unsigned short operator()(const string &sStr)
        {
            if (!sStr.empty()) {
                if (sStr.find("0x") == 0) {
                    return (unsigned short) ::strtoul(sStr.c_str(), NULL, 16);
                }
                else {
                    return (unsigned short) strtoul(sStr.c_str(), NULL, 10);
                }
            }
            return 0;
        }
    };

    template<>
    struct strto1<int>
    {
        int operator()(const string &sStr)
        {
            if (!sStr.empty()) {
                if (sStr.find("0x") == 0) {
                    return ::strtol(sStr.c_str(), NULL, 16);
                }
                else {
                    return atoi(sStr.c_str());
                }
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned int>
    {
        unsigned int operator()(const string &sStr)
        {
            if (!sStr.empty()) {
                if (sStr.find("0x") == 0) {
                    return ::strtoul(sStr.c_str(), NULL, 16);
                }
                else {
                    return strtoul(sStr.c_str(), NULL, 10);
                }
            }
            return 0;
        }
    };

    template<>
    struct strto1<long>
    {
        long operator()(const string &sStr)
        {
            if (!sStr.empty()) {
                if (sStr.find("0x") == 0) {
                    return ::strtol(sStr.c_str(), NULL, 16);
                }
                else {
                    return atol(sStr.c_str());
                }
            }
            return 0;
        }
    };

    template<>
    struct strto1<long long>
    {
        long long operator()(const string &sStr)
        {
            if (!sStr.empty()) {
                if (sStr.find("0x") == 0) {
                    return ::strtoll(sStr.c_str(), NULL, 16);
                }
                else {
                    return atoll(sStr.c_str());
                }
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned long>
    {
        unsigned long operator()(const string &sStr)
        {
            if (!sStr.empty()) {
                if (sStr.find("0x") == 0) {
                    return ::strtoul(sStr.c_str(), NULL, 16);
                }
                else {
                    return strtoul(sStr.c_str(), NULL, 10);
                }
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned long long>
    {
        unsigned long long operator()(const string& sStr)
        {
            if (!sStr.empty()) {
                if (sStr.find("0x") == 0) {
                    return ::strtoull(sStr.c_str(), NULL, 16);
                }
                else {
                    return strtoull(sStr.c_str(), NULL, 10);
                }
            }
            return 0;
        }
    };

    template<>
    struct strto1<float>
    {
        float operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return (float) atof(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<double>
    {
        double operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atof(sStr.c_str());
            }
            return 0;
        }
    };

    template<typename D>
    struct strto2
    {
        D operator()(const string &sStr)
        {
            istringstream sBuffer(sStr);

            D t;
            sBuffer >> t;

            return t;
        }
    };

    template<>
    struct strto2<string>
    {
        string operator()(const string &sStr)
        {
            return sStr;
        }
    };
}

template<typename T>
T CWHCommon::strto(const string &sStr)
{
    using strto_type = typename std::conditional<std::is_arithmetic<T>::value, p::strto1<T>, p::strto2<T>>::type;

    return strto_type()(sStr);
}

template<typename T>
T CWHCommon::strto(const string &sStr, const string &sDefault)
{
    string s;

    if(!sStr.empty())
    {
        s = sStr;
    }
    else
    {
        s = sDefault;
    }

    return strto<T>(s);
}

template <>
string CWHCommon::tostr<bool>(const bool& t)
{
    char buf[2];
    buf[0] = t ? '1' : '0';
    buf[1] = '\0';
    return string(buf);
}

template <>
string CWHCommon::tostr<char>(const char& t)
{
    char buf[2];
    snprintf(buf, 2, "%c", t);
    return string(buf);
}

template <>
string CWHCommon::tostr<unsigned char>(const unsigned char& t)
{
    char buf[2];
    snprintf(buf, 2, "%c", t);
    return string(buf);
}

template <>
string CWHCommon::tostr<short>(const short& t)
{
    char buf[16];
    snprintf(buf, 16, "%d", t);
    return string(buf);
}

template <>
string CWHCommon::tostr<unsigned short>(const unsigned short& t)
{
    char buf[16];
    snprintf(buf, 16, "%u", t);
    return string(buf);
}

template <>
string CWHCommon::tostr<int>(const int& t)
{
    char buf[16];
    snprintf(buf, 16, "%d", t);
    return string(buf);
}

template <>
string CWHCommon::tostr<unsigned int>(const unsigned int& t)
{
    char buf[16];
    snprintf(buf, 16, "%u", t);
    return string(buf);
}

template <>
string CWHCommon::tostr<long>(const long& t)
{
    char buf[32];
    snprintf(buf, 32, "%ld", t);
    return string(buf);
}

template <>
string CWHCommon::tostr<long long>(const long long& t)
{
    char buf[32];
    snprintf(buf, 32, "%lld", t);
    return string(buf);
}


template <>
string CWHCommon::tostr<unsigned long>(const unsigned long& t)
{
    char buf[32];
    snprintf(buf, 32, "%lu", t);
    return string(buf);
}

template<>
string CWHCommon::tostr<unsigned long long>(const unsigned long long& t)
{
    char buf[32];
    snprintf(buf, 32, "%llu", t);
    return string(buf);
}

template <>
string CWHCommon::tostr<float>(const float& t)
{
    //C++11 to_string，默认保留后面6位小数
    string s = std::to_string(t);

    //去掉无效0, eg. 1.0300 -> 1.03;1.00 -> 1
    bool bFlag = false;
    int pos = int(s.size() - 1);
    for (; pos > 0; --pos)
    {
        if (s[pos] == '0')
        {
            bFlag = true;
            if (s[pos - 1] == '.')
            {
                //-2为了去掉"."号
                pos -= 2;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (bFlag)
        s = s.substr(0, pos + 1);

    return s;
}

template <>
string CWHCommon::tostr<double>(const double& t)
{
    //C++11 to_string，默认保留后面6位小数
    string s = std::to_string(t);
    //去掉无效0, eg. 1.0300 -> 1.03;1.00 -> 1
    bool bFlag = false;
    int pos = int(s.size() - 1);
    for (; pos > 0; --pos)
    {
        if (s[pos] == '0')
        {
            bFlag = true;
            if (s[pos - 1] == '.')
            {
                //-2为了去掉"."号
                pos -= 2;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (bFlag)
        s = s.substr(0, pos + 1);

    return s;
}

template <>
string CWHCommon::tostr<long double>(const long double& t)
{
    char buf[32];
    snprintf(buf, 32, "%Lf", t);
    string s(buf);

    //去掉无效0, eg. 1.0300 -> 1.03;1.00 -> 1
    bool bFlag = false;
    int pos = int(s.size() - 1);
    for (; pos > 0; --pos)
    {
        if (s[pos] == '0')
        {
            bFlag = true;
            if (s[pos - 1] == '.')
            {
                //-2为了去掉"."号
                pos -= 2;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (bFlag)
        s = s.substr(0, pos + 1);

    return s;

}

template <>
string CWHCommon::tostr<std::string>(const std::string& t)
{
    return t;
}

#endif

