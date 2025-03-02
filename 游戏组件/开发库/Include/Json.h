#ifndef JSON_HEAD_FILE
#define JSON_HEAD_FILE
#pragma once

//包含库文件
#include "rapidjson/document.h"
#include "CommonServiceHead.h"

/////////////////////////////////////////////////////////////////
//JSON 类
class COMMON_SERVICE_CLASS CJson
{
	//成员变量
public:
	rapidjson::Document					m_JsonDoc;			//文档变量

	//函数定义
public:
	//构造函数
	CJson();
	//析构函数
	~CJson();

	//功能函数
public:
	//读取JSON
	bool Read(LPCTSTR lpJsonString);
	//读取文件
	bool ReadFromFile(LPCTSTR lpFile);
	//写入文件
	bool WriteToFile(LPCTSTR lpFile);
	//写入缓存
	bool WriteToBuffer(LPTSTR lpOutBuf, WORD wBufLen);

	//功能函数
public:
	//获取数据
	int GetValueInt(LPCTSTR lpKey);
	//获取数据
	bool GetValueBool(LPCTSTR lpKey);
	//获取数据
	double GetValueDouble(LPCTSTR lpKey);
	//获取数据
	SCORE GetValueInt64(LPCTSTR lpKey);
	//获取数据
	LPTSTR GetValueString(LPCTSTR lpKey, LPTSTR lpOutBuf, WORD wBufLen);
	//获取对象
	void GetValueObject(LPCTSTR lpObj, LPTSTR lpOutBuf, WORD wBufLen);

	//功能函数
public:
	//设置数据
	bool SetKeyValue(LPCTSTR lpKey, int nValue);
	//设置数据
	bool SetKeyValue(LPCTSTR lpKey, int64_t nValue);
	//设置数据
	bool SetKeyValue(LPCTSTR lpKey, bool nValue);
	//设置数据
	bool SetKeyValue(LPCTSTR lpKey, double nValue);
	//设置数据
	bool SetKeyValue(LPCTSTR lpKey, LPCTSTR lpValue);

	//功能函数
public:
	//添加数据
	bool AddMember(LPCTSTR lpKey, int nValue);
	//添加数据
	bool AddMember(LPCTSTR lpKey, int64_t nValue);
	//添加数据
	bool AddMember(LPCTSTR lpKey, bool nValue);
	//添加数据
	bool AddMember(LPCTSTR lpKey, double nValue);
	//添加数据
	bool AddMember(LPCTSTR lpKey, LPCTSTR lpValue);

	//功能函数
public:
	//添加数据
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, int nValue);
	//添加数据
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, int64_t nValue);
	//添加数据
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, bool nValue);
	//添加数据
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, double nValue);
	//添加数据
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, LPCTSTR lpValue);

	//功能函数
public:
	//添加数据
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, int nValue);
	//添加数据
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, int64_t nValue);
	//添加数据
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, bool nValue);
	//添加数据
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, double nValue);
	//添加数据
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, LPCTSTR lpValue);

	//功能函数
public:
	//删除成员
	bool DeleteALL();
	//删除成员
	bool DeleteMember(LPCTSTR lpMember);
	//搽除成员
	void EraseMember(rapidjson::Value::ConstMemberIterator pos);
	//搽除成员
	void EraseMember(rapidjson::Value::ConstMemberIterator first, rapidjson::Value::ConstMemberIterator last);

	//功能函数
public:
	//成员数量
	DWORD GetMemberCounts();
	//获取成员
	rapidjson::Value::MemberIterator GetMemberFirst();
	//获取成员
	rapidjson::Value::MemberIterator GetMemberEnd();
};

/////////////////////////////////////////////////////////////////
#endif

