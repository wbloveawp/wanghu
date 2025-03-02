#ifndef WH_JWT_HEAD_FILE
#define WH_JWT_HEAD_FILE

#pragma once

#include "WHJson.h"
#include "CommonServiceHead.h"

using namespace std;
using namespace WHJson;

//////////////////////////////////////////////////////////////////////////////////
//类型声明
class CWHJwt;
class CWHClaims;

//类型定义
typedef CWHAutoPtr<CWHJwt>			CWHJwtPtr;
typedef CWHAutoPtr<CWHClaims>		CWHClaimsPtr;

//////////////////////////////////////////////////////////////////////////////////

class COMMON_SERVICE_CLASS CWHClaims : public CHandleBase
{
	//变量定义
protected:	
	CJsonValueObjPtr				m_Claims;						//Json对象

	//函数定义
public:
	//构造函数
	CWHClaims();
	//构造函数
	explicit CWHClaims(const string& sJsonData, bool bBase64 = false);

	//访问函数
public:
	//检查函数
	bool Has(const string& key);
	//删除函数
	void Del(const string& key);	
	//获取函数
	template<typename T>
	T Get(const string& key);
	//校验函数
	template<typename T>
	bool Check(const string& key, T& value);
	//设置函数
	template<typename T>
	void Set(const string& key, const T& value);	

	//功能函数
public:
	//获取内容
	string GetContent();
	//编码函数
	string Base64Encode();
};

//////////////////////////////////////////////////////////////////////////////////


//JWT
class COMMON_SERVICE_CLASS CWHJwt : public CHandleBase
{
	//变量定义
private:
	string							m_sSign;
	CWHClaimsPtr					m_ClaimsPtr;	

	//函数定义
private:
	//构造函数
	CWHJwt(CWHClaimsPtr claimsPtr, const string& Sign);

	//成员函数
public:
	//校验函数
	bool Verify();	
	//获取对象
	CWHClaims & Claims() { return *(m_ClaimsPtr.get()); }

	//静态函数
public:
	//签名函数
	static string Sign(CWHClaims& claims);
	//签名函数
	static string Sign(const string& sData);	
	//解析函数
	static CWHJwtPtr Parse(const string& sToken);	
};

//////////////////////////////////////////////////////////////////////////////////

//获取函数
template<typename T>
T CWHClaims::Get(const string& key)
{
	if (!m_Claims)
	{
		throw exception("数据格式错误！");
	}

	T v;
	CWHJsonInput::readJson(v, m_Claims->value[key], false);
	return v;
}

//校验函数
template<typename T>
bool CWHClaims::Check(const string& key, T& value)
{
	//校验对象
	if (!m_Claims) return false;

	T v;
	CWHJsonInput::readJson(v, m_Claims->value[key], false);
	return v == value;
}

//设置函数
template<typename T>
void CWHClaims::Set(const string& key, const T& value)
{
	if (!key.empty())
	{
		m_Claims->value[key] = CWHJsonOutput::writeJson(value);
	}
}

//////////////////////////////////////////////////////////////////////////////////

#endif