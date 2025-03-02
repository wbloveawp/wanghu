#include "StdAfx.h"
#include "WHJwt.h"
#include "WHBase64.h"
#include "WHService.h"
#include "DataEncrypt.h"

//////////////////////////////////////////////////////////////////////////////////
//缓冲定义
#define LEN_BUFFER			1024

//秘钥定义
LPCSTR WHJWT_KEY = "31A4BA93C4EF4844875CF1907C8FE6FEDC7AC5F5942F44ADAE22CD7CAD3E48B728ACE6EE185C46A5AE15D29CE7151DFD";



//////////////////////////////////////////////////////////////////////////////////

//转换函数
string String2Utf8(const string& str)
{
	//计算长度
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	//计算长度
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	//转换编码
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	//添加结束符
	pBuf[nLen] = 0;

	//构造结果
	std::string strResult(pBuf);

	//释放资源
	SafeDeleteArray(pwBuf);
	SafeDeleteArray(pBuf);

	return strResult;
}

//转换函数
string Utf82String(const string& str)
{
	//计算长度
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	//计算长度
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	//编码转换
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	//添加结束符
	pBuf[nLen] = 0;

	//构造结果
	std::string strResult(pBuf);

	//释放资源
	SafeDeleteArray(pwBuf);
	SafeDeleteArray(pBuf);

	return strResult;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CWHClaims::CWHClaims()
{
	m_Claims = new CJsonValueObj;	
}

//构造函数
CWHClaims::CWHClaims(const string& sJsonData, bool bBase64)
{
	if (bBase64)
	{
		//Base64解码
		CHAR szResult[LEN_BUFFER];
		CWHBase64::Decode(sJsonData.c_str(), sJsonData.size(), szResult, CountArray(szResult));

		//编码转换
		string sResult;
		CWHService::Utf82GB2312(szResult, sResult);

		auto ValuePtr = CWHJson::getValue(sResult);
		if (ValuePtr->getType() != eJsonTypeObj)
		{
			throw exception("Json格式错误！");
		}

		m_Claims = CJsonValueObjPtr::dynamicCast(ValuePtr);
	}
	else
	{
		auto ValuePtr = CWHJson::getValue(sJsonData);
		if (ValuePtr->getType() != eJsonTypeObj)
		{
			throw exception("Json格式错误！");
		}

		m_Claims = CJsonValueObjPtr::dynamicCast(ValuePtr);
	}
}

//检查函数
bool CWHClaims::Has(const string& key)
{
	if (!m_Claims)
	{
		return false;
	}

	return m_Claims->value.find(key) == m_Claims->value.end();
}

//删除函数
void CWHClaims::Del(const string& key)
{
	if (m_Claims)
	{
		m_Claims->value.erase(key);
	}	
}

//获取内容
string CWHClaims::GetContent()
{
	//获取Json
	string sJson;
	CWHJson::writeValue(m_Claims, sJson);

	return sJson;
}

//编码函数
string CWHClaims::Base64Encode()
{
	//编码转换
	string sContent;
	CWHService::GB23122Utf8(GetContent().c_str(), sContent);

	//Base64编码
	CHAR szResult[LEN_BUFFER];
	CWHBase64::Encode(sContent.c_str(), sContent.size(), szResult, CountArray(szResult));

	return string(szResult);
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CWHJwt::CWHJwt(CWHClaimsPtr claimsPtr, const string& sSign) :	
	m_sSign(sSign),
	m_ClaimsPtr(claimsPtr)	
{
}

//校验函数
bool CWHJwt::Verify()
{	
	//构造数据
	CHAR szSrcData[LEN_BUFFER];
	_snprintf_s(szSrcData, CountArray(szSrcData), "%s%s", m_ClaimsPtr->GetContent().c_str(), WHJWT_KEY);

	//转换编码
	string sSrcDataUtf8;
	CWHService::GB23122Utf8(szSrcData, sSrcDataUtf8);

	//加密数据
	CHAR szSignData[LEN_MD5];
	CMD5Encrypt::EncryptData(sSrcDataUtf8.c_str(), szSignData);

	return m_sSign.compare(szSignData)==0;
}

//签名函数
string CWHJwt::Sign(CWHClaims& claims)
{
	//构造数据
	CHAR szSrcData[LEN_BUFFER];
	_snprintf_s(szSrcData, CountArray(szSrcData), "%s%s", claims.GetContent().c_str(), WHJWT_KEY);

	//获取内容
	string sSrcDataUtf8;
	CWHService::GB23122Utf8(szSrcData, sSrcDataUtf8);

	//加密数据
	CHAR szSignData[LEN_MD5];
	CMD5Encrypt::EncryptData(sSrcDataUtf8.c_str(), szSignData);

	//令牌数据
	CHAR szToken[LEN_BUFFER];
	_snprintf_s(szToken, CountArray(szToken), "%s.%s", claims.Base64Encode().c_str(), szSignData);

	return string(szToken);	
}

//签名函数
string CWHJwt::Sign(const string& sData)
{
	//构造对象
	CWHClaims Claims(sData);	

	return Sign(Claims);
}
	
//解析函数
CWHJwtPtr CWHJwt::Parse(const string& sToken)
{
	//变量定义
	size_t Pos = sToken.find('.', 0);	
	if (Pos == string::npos)
	{
		throw runtime_error("Token格式错误!");
	}
	
	//截取数据
	string sData = sToken.substr(0, Pos);	
	string sSign = sToken.substr(Pos + 1, sToken.length());

	//变量定义
	CWHJwt* pWHJwt = NULL;
	try 
	{
		//创建Claims
		CWHClaimsPtr ClaimsPtr = new CWHClaims(sData, true);

		//构造WHJWT
		pWHJwt = new CWHJwt(ClaimsPtr, sSign);
	}
	catch (...)
	{
		throw;
	}

	return pWHJwt;
}

//////////////////////////////////////////////////////////////////////////////////
