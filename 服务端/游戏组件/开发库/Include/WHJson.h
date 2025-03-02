�?ifndef WHJSON_HEAD_FILE
#define WHJSON_HEAD_FILE

#include <map>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <unordered_map>

#include "CommonServiceHead.h"

using namespace std;

namespace WHJson
{

///////////////////////////////////////////////////////////////////////////////////////////

//Json类型
enum eJsonType
{
	eJsonTypeString,
	eJsonTypeNum,
	eJsonTypeObj,
	eJsonTypeArray,
	eJsonTypeBoolean
};

//类型声明
class CJsonValue;
class CJsonValueString;
class CJsonValueNum;
class CJsonValueObj;
class CJsonValueArray;
class CJsonValueBoolean;

//类型定义
typedef CWHAutoPtr<CJsonValue> CJsonValuePtr;
typedef CWHAutoPtr<CJsonValueNum> CJsonValueNumPtr;
typedef CWHAutoPtr<CJsonValueObj> CJsonValueObjPtr;
typedef CWHAutoPtr<CJsonValueArray> CJsonValueArrayPtr;
typedef CWHAutoPtr<CJsonValueString> CJsonValueStringPtr;
typedef CWHAutoPtr<CJsonValueBoolean> CJsonValueBooleanPtr;

///////////////////////////////////////////////////////////////////////////////////////////

//类型基类
class CJsonValue : public CHandleBase
{
public:
	virtual ~CJsonValue() {}
	virtual eJsonType getType()=NULL;	
};

///////////////////////////////////////////////////////////////////////////////////////////
//字符串类�?
class CJsonValueString : public CJsonValue
{
public:
	CJsonValueString(const string & s):value(s)
	{
	}
	CJsonValueString()
	{
	}

	eJsonType getType()
	{
		return eJsonTypeString;
	}
	virtual ~CJsonValueString()
	{
	}

public:
	string value;
};

///////////////////////////////////////////////////////////////////////////////////////////
//数字类型
class CJsonValueNum : public CJsonValue
{
public:
	CJsonValueNum(double d,bool b=false):value(d),isInt(b)
	{
	}
	CJsonValueNum()
	{
		isInt=false;
		value=0.0f;
	}
	eJsonType getType()
	{
		return eJsonTypeNum;
	}
	virtual ~CJsonValueNum(){}

public:
	bool isInt;
	double value;	
};

///////////////////////////////////////////////////////////////////////////////////////////

//对象类型
class CJsonValueObj: public CJsonValue
{
public:
	eJsonType getType()
	{
		return eJsonTypeObj;
	}
	const CJsonValuePtr &get(const char *name)
	{
		auto it = value.find(name);

		if(it == value.end())
		{
			char s[64];
			snprintf(s, sizeof(s), "get obj error(key is not exists)[key:%s]", name);
			throw exception(s); 
		}

		return it->second;
	}

	virtual ~CJsonValueObj(){}

public:
	unordered_map<string,CJsonValuePtr> value;
};

///////////////////////////////////////////////////////////////////////////////////////////

//数组类型
class CJsonValueArray: public CJsonValue
{
public:
	eJsonType getType()
	{
		return eJsonTypeArray;
	}
	void push_back(const CJsonValuePtr & p)
	{
		value.push_back(p);
	}
	virtual ~CJsonValueArray(){}

public:
	vector<CJsonValuePtr> value;
};

///////////////////////////////////////////////////////////////////////////////////////////

//布尔类型
class CJsonValueBoolean : public CJsonValue
{
public:
	eJsonType getType()
	{
		return eJsonTypeBoolean;
	}
	bool getValue()
	{
		return value;
	}
	virtual ~CJsonValueBoolean(){}

public:
	bool value;
};

///////////////////////////////////////////////////////////////////////////////////////////

//Json读取�?
class CBufferJsonReader
{
	/*buffer*/
	const char *    _buf;		///< 缓冲�?
	/*buffer length*/
	size_t       _buf_len;	///< 缓冲区长�?
	/*current location*/
	size_t       _cur;		///< 当前位置

public:

	CBufferJsonReader () :_buf(NULL),_buf_len(0), _cur(0) {}

	void reset() { _cur = 0;}

	void setBuffer(const char * buf, size_t len)
	{
		_buf = buf;
		_buf_len = len;
		_cur = 0;
	}

	void setBuffer(const std::vector<char> &buf)
	{
		_buf = buf.data();
		_buf_len = buf.size();
		_cur = 0;
	}

	size_t getCur()
	{
		return _cur;
	}

	const char * getPoint()
	{
		return _buf+_cur;
	}

	char read()
	{
		check();
		_cur ++;
		return *(_buf+_cur-1);
	}

	char get()
	{
		check();
		return *(_buf+_cur);
	}

	char getBack()
	{
		assert(_cur>0);
		return *(_buf+_cur-1);
	}

	void back()
	{
		assert(_cur>0);
		_cur--;
	}

	void check()
	{
		if (_cur + 1 > _buf_len)
		{
			char s[64];
			snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)_buf_len);
			throw exception(s);
		}
	}

	bool hasEnd()
	{
		return _cur >= _buf_len;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////

class CJsonWriteOstream
{
public:
  static void writeValue(const CJsonValuePtr& p, ostream& ostr);

  //写入函数
private:
  static void writeString(const CJsonValueStringPtr& p, ostream& ostr);
  static void writeString(const string& s, ostream& ostr);
  static void writeNum(const CJsonValueNumPtr& p, ostream& ostr);
  static void writeObj(const CJsonValueObjPtr& p, ostream& ostr);
  static void writeArray(const CJsonValueArrayPtr& p, ostream& ostr);
  static void writeBoolean(const CJsonValueBooleanPtr& p, ostream& ostr);
};

///////////////////////////////////////////////////////////////////////////////////////////

//Json分析�?
class COMMON_SERVICE_CLASS CWHJson
{
public:
	//json类型到字符串的转�?
	static string writeValue(const CJsonValuePtr & p);
	static void writeValue(const CJsonValuePtr & p, string& ostr);
	static void writeValue(const CJsonValuePtr & p, vector<char>& buf);

	//json字符串到json结构的转�?
	static CJsonValuePtr getValue(const string & str);
  static CJsonValuePtr getValue(const vector<char>& buf);
private:
	//string 类型到json字符�?
	static void writeString(const CJsonValueStringPtr & p, string& ostr);
	static void writeString(const string & s, string& ostr);

	//num 类型到json字符�?
	static void writeNum(const CJsonValueNumPtr & p, string& ostr);

	//obj 类型到json字符�?
	static void writeObj(const CJsonValueObjPtr & p, string& ostr);

	//array 类型到json字符�?
	static void writeArray(const CJsonValueArrayPtr & p, string& ostr);

	//boolean 类型到json字符�?
	static void writeBoolean(const CJsonValueBooleanPtr & p, string& ostr);

	//读取json的类�?
	static CJsonValuePtr getValue(CBufferJsonReader & reader);
	static CJsonValueObjPtr getObj(CBufferJsonReader & reader);
	static CJsonValueArrayPtr getArray(CBufferJsonReader & reader);
	static CJsonValueStringPtr getString(CBufferJsonReader & reader,char head='\"');
	static CJsonValueNumPtr getNum(CBufferJsonReader & reader,char head);
	static CJsonValueBooleanPtr getBoolean(CBufferJsonReader & reader,char c);
	static CJsonValuePtr getNull(CBufferJsonReader & reader,char c);
	static uint32_t getHex(CBufferJsonReader & reader);
	static bool isspace(char c);
};

///////////////////////////////////////////////////////////////////////////////////////////

class COMMON_SERVICE_CLASS CWHJsonInput
{
public:
  static void readJson(bool& b, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeBoolean)
    {
      b = CJsonValueBooleanPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'bool' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(char& c, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      c = (char)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'char' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(unsigned char& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = (unsigned char)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'unsigned char' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(short& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = (short)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'short' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(unsigned short& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = (unsigned short)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'unsigned short' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(long& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = (long)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'long' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(unsigned long& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = (unsigned long)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'unsigned long' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(int& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = (int)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'int' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(unsigned int& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = (unsigned int)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'unsigned int' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(long long& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = (long long)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'long long' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(float& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = (float)CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'float' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(double& n, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeNum)
    {
      n = CJsonValueNumPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'double' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(std::string& s, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeString)
    {
      s = CJsonValueStringPtr::dynamicCast(p)->value;
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'string' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  static void readJson(char* buf, const unsigned int bufLen, unsigned int& readLen, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeString)
    {
      CJsonValueStringPtr pString = CJsonValueStringPtr::dynamicCast(p);
      if ((unsigned int)pString->value.size() > bufLen)
      {
        char s[128];
        snprintf(s, sizeof(s), "invalid char * size, size: %u", (unsigned int)pString->value.size());
        throw exception(s);
      }
      memcpy(buf, pString->value.c_str(), pString->value.size());
      readLen = (unsigned int)pString->value.size();
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'char *' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<string, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<string, V> pr;
        pr.first = iter->first;
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<bool, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<bool, V> pr;
        pr.first = CWHCommon::strto<bool>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<char, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<char, V> pr;
        pr.first = CWHCommon::strto<int>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }
  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<unsigned char, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<unsigned char, V> pr;
        pr.first = CWHCommon::strto<unsigned char>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }
  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<short, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<short, V> pr;
        pr.first = CWHCommon::strto<short>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<unsigned short, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<unsigned short, V> pr;
        pr.first = CWHCommon::strto<unsigned short>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<int, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<int, V> pr;
        pr.first = CWHCommon::strto<int>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<unsigned int, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<unsigned int, V> pr;
        pr.first = CWHCommon::strto<unsigned int>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<long long, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<long long, V> pr;
        pr.first = CWHCommon::strto<long long>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<float, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<float, V> pr;
        pr.first = CWHCommon::strto<float>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<double, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeObj)
    {
      CJsonValueObjPtr pObj = CJsonValueObjPtr::dynamicCast(p);
      auto iter = pObj->value.begin();
      for (; iter != pObj->value.end(); ++iter)
      {
        std::pair<double, V> pr;
        pr.first = CWHCommon::strto<double>(iter->first);
        readJson(pr.second, iter->second);
        m.insert(pr);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'map' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  template<typename K, typename V, typename Cmp, typename Alloc>
  static void readJson(std::map<K, V, Cmp, Alloc>& m, const CJsonValuePtr& p, bool isRequire = true)
  {
    char s[128];
    snprintf(s, sizeof(s), "map key is not Basic type. map key is only string|bool|num");
    throw exception(s);
  }

  template<typename T, typename Alloc>
  static void readJson(std::vector<T, Alloc>& v, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeArray)
    {
      CJsonValueArrayPtr pArray = CJsonValueArrayPtr::dynamicCast(p);
      v.resize(pArray->value.size());
      for (size_t i = 0; i < pArray->value.size(); ++i)
      {
        readJson(v[i], pArray->value[i]);
      }
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'vector' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }

  /// 读取结构数组
  template<typename T>
  static void readJson(T* v, const unsigned int len, unsigned int& readLen, const CJsonValuePtr& p, bool isRequire = true)
  {
    if (NULL != p.get() && p->getType() == eJsonTypeArray)
    {
      CJsonValueArrayPtr pArray = CJsonValueArrayPtr::dynamicCast(p);
      if (pArray->value.size() > len)
      {
        char s[128];
        snprintf(s, sizeof(s), "read 'T *' invalid size, size: %u", (uint32_t)pArray->value.size());
        throw exception(s);
      }
      for (size_t i = 0; i < pArray->value.size(); ++i)
      {
        readJson(v[i], pArray->value[i]);
      }
      readLen = pArray->value.size();
    }
    else if (isRequire)
    {
      char s[128];
      snprintf(s, sizeof(s), "read 'T *' type mismatch, get type: %d.", (p.get() ? p->getType() : 0));
      throw exception(s);
    }
  }
};

///////////////////////////////////////////////////////////////////////////////////////////

class COMMON_SERVICE_CLASS CWHJsonOutput
{
public:
  static CJsonValueBooleanPtr writeJson(bool b)
  {
    CJsonValueBooleanPtr p = new CJsonValueBoolean();
    p->value = b;
    return p;
  }

  static CJsonValueNumPtr writeJson(char n)
  {
    return (new CJsonValueNum(n, true));
  }

  static CJsonValueNumPtr writeJson(unsigned char n)
  {
    return (new CJsonValueNum(n, true));
  }

  static CJsonValueNumPtr writeJson(short n)
  {
    return (new CJsonValueNum(n, true));
  }

  static CJsonValueNumPtr writeJson(unsigned short n)
  {
    return (new CJsonValueNum(n, true));
  }

  static CJsonValueNumPtr writeJson(int n)
  {
    return (new CJsonValueNum(n, true));
  }

  static CJsonValueNumPtr writeJson(unsigned int n)
  {
    return (new CJsonValueNum(n, true));
  }

  static CJsonValueNumPtr writeJson(unsigned long n)
  {
    return (new CJsonValueNum(n, true));
  }

  static CJsonValueNumPtr writeJson(long long n)
  {
    return (new CJsonValueNum((double)n, true));
  }

  static CJsonValueNumPtr writeJson(float n)
  {
    return (new CJsonValueNum(n));
  }

  static CJsonValueNumPtr writeJson(double n)
  {
    return (new CJsonValueNum(n));
  }

  static CJsonValueStringPtr writeJson(const std::string& s)
  {
    return (new CJsonValueString(s));
  }

  static CJsonValueStringPtr writeJson(const char* buf, const unsigned int len)
  {
    return (new CJsonValueString(string(buf, len)));
  }

  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<string, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<string, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[i->first] = writeJson(i->second);
    }
    return pObj;
  }

  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<bool, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<bool, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr(i->first)] = writeJson(i->second);
    }
    return pObj;
  }

  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<char, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<char, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr((int)i->first)] = writeJson(i->second);
    }
    return pObj;
  }

  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<unsigned char, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<unsigned char, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr(i->first)] = writeJson(i->second);
    }
    return pObj;
  }

  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<short, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<short, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr(i->first)] = writeJson(i->second);
    }
    return pObj;
  }
  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<unsigned short, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<unsigned short, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr(i->first)] = writeJson(i->second);
    }
    return pObj;
  }
  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<int, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<int, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr(i->first)] = writeJson(i->second);
    }
    return pObj;
  }
  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<unsigned int, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<unsigned int, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr(i->first)] = writeJson(i->second);
    }
    return pObj;
  }

  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<long long, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<long long, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr(i->first)] = writeJson(i->second);
    }
    return pObj;
  }

  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<float, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<float, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr(i->first)] = writeJson(i->second);
    }
    return pObj;
  }

  template<typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<double, V, Cmp, Alloc>& m)
  {
    CJsonValueObjPtr pObj = new JsonValueObj();
    typedef typename std::map<double, V, Cmp, Alloc>::const_iterator IT;
    for (IT i = m.begin(); i != m.end(); ++i)
    {
      pObj->value[CWHCommon::tostr(i->first)] = writeJson(i->second);
    }
    return pObj;
  }

  template<typename K, typename V, typename Cmp, typename Alloc>
  static CJsonValueObjPtr writeJson(const std::map<K, V, Cmp, Alloc>& m)
  {
    char s[128];
    snprintf(s, sizeof(s), "map key is not Basic type. map key is only string|bool|num");
    throw exception(s);
  }

  template<typename T, typename Alloc>
  static CJsonValueArrayPtr writeJson(const std::vector<T, Alloc>& v)
  {
    CJsonValueArrayPtr pArray = new JsonValueArray();
    typedef typename std::vector<T, Alloc>::const_iterator IT;
    for (IT i = v.begin(); i != v.end(); ++i)
      pArray->value.push_back(writeJson(*i));
    return pArray;
  }

  template<typename T>
  static CJsonValueArrayPtr writeJson(const T* v, const unsigned int len)
  {
    CJsonValueArrayPtr pArray = new JsonValueArray();
    for (size_t i = 0; i < len; ++i)
    {
      pArray->value.push_back(writeJson(v[i]));
    }
    return pArray;
  }
};

///////////////////////////////////////////////////////////////////////////////////////////
}

#endif
