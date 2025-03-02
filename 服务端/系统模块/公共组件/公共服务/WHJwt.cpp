#include "StdAfx.h"
#include "WHJwt.h"
#include "WHBase64.h"
#include "WHService.h"
#include "DataEncrypt.h"

//////////////////////////////////////////////////////////////////////////////////
//���嶨��
#define LEN_BUFFER			1024

//��Կ����
LPCSTR WHJWT_KEY = "31A4BA93C4EF4844875CF1907C8FE6FEDC7AC5F5942F44ADAE22CD7CAD3E48B728ACE6EE185C46A5AE15D29CE7151DFD";



//////////////////////////////////////////////////////////////////////////////////

//ת������
string String2Utf8(const string& str)
{
	//���㳤��
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	//���㳤��
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	//ת������
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	//��ӽ�����
	pBuf[nLen] = 0;

	//������
	std::string strResult(pBuf);

	//�ͷ���Դ
	SafeDeleteArray(pwBuf);
	SafeDeleteArray(pBuf);

	return strResult;
}

//ת������
string Utf82String(const string& str)
{
	//���㳤��
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	//���㳤��
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	//����ת��
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	//��ӽ�����
	pBuf[nLen] = 0;

	//������
	std::string strResult(pBuf);

	//�ͷ���Դ
	SafeDeleteArray(pwBuf);
	SafeDeleteArray(pBuf);

	return strResult;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CWHClaims::CWHClaims()
{
	m_Claims = new CJsonValueObj;	
}

//���캯��
CWHClaims::CWHClaims(const string& sJsonData, bool bBase64)
{
	if (bBase64)
	{
		//Base64����
		CHAR szResult[LEN_BUFFER];
		CWHBase64::Decode(sJsonData.c_str(), sJsonData.size(), szResult, CountArray(szResult));

		//����ת��
		string sResult;
		CWHService::Utf82GB2312(szResult, sResult);

		auto ValuePtr = CWHJson::getValue(sResult);
		if (ValuePtr->getType() != eJsonTypeObj)
		{
			throw exception("Json��ʽ����");
		}

		m_Claims = CJsonValueObjPtr::dynamicCast(ValuePtr);
	}
	else
	{
		auto ValuePtr = CWHJson::getValue(sJsonData);
		if (ValuePtr->getType() != eJsonTypeObj)
		{
			throw exception("Json��ʽ����");
		}

		m_Claims = CJsonValueObjPtr::dynamicCast(ValuePtr);
	}
}

//��麯��
bool CWHClaims::Has(const string& key)
{
	if (!m_Claims)
	{
		return false;
	}

	return m_Claims->value.find(key) == m_Claims->value.end();
}

//ɾ������
void CWHClaims::Del(const string& key)
{
	if (m_Claims)
	{
		m_Claims->value.erase(key);
	}	
}

//��ȡ����
string CWHClaims::GetContent()
{
	//��ȡJson
	string sJson;
	CWHJson::writeValue(m_Claims, sJson);

	return sJson;
}

//���뺯��
string CWHClaims::Base64Encode()
{
	//����ת��
	string sContent;
	CWHService::GB23122Utf8(GetContent().c_str(), sContent);

	//Base64����
	CHAR szResult[LEN_BUFFER];
	CWHBase64::Encode(sContent.c_str(), sContent.size(), szResult, CountArray(szResult));

	return string(szResult);
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CWHJwt::CWHJwt(CWHClaimsPtr claimsPtr, const string& sSign) :	
	m_sSign(sSign),
	m_ClaimsPtr(claimsPtr)	
{
}

//У�麯��
bool CWHJwt::Verify()
{	
	//��������
	CHAR szSrcData[LEN_BUFFER];
	_snprintf_s(szSrcData, CountArray(szSrcData), "%s%s", m_ClaimsPtr->GetContent().c_str(), WHJWT_KEY);

	//ת������
	string sSrcDataUtf8;
	CWHService::GB23122Utf8(szSrcData, sSrcDataUtf8);

	//��������
	CHAR szSignData[LEN_MD5];
	CMD5Encrypt::EncryptData(sSrcDataUtf8.c_str(), szSignData);

	return m_sSign.compare(szSignData)==0;
}

//ǩ������
string CWHJwt::Sign(CWHClaims& claims)
{
	//��������
	CHAR szSrcData[LEN_BUFFER];
	_snprintf_s(szSrcData, CountArray(szSrcData), "%s%s", claims.GetContent().c_str(), WHJWT_KEY);

	//��ȡ����
	string sSrcDataUtf8;
	CWHService::GB23122Utf8(szSrcData, sSrcDataUtf8);

	//��������
	CHAR szSignData[LEN_MD5];
	CMD5Encrypt::EncryptData(sSrcDataUtf8.c_str(), szSignData);

	//��������
	CHAR szToken[LEN_BUFFER];
	_snprintf_s(szToken, CountArray(szToken), "%s.%s", claims.Base64Encode().c_str(), szSignData);

	return string(szToken);	
}

//ǩ������
string CWHJwt::Sign(const string& sData)
{
	//�������
	CWHClaims Claims(sData);	

	return Sign(Claims);
}
	
//��������
CWHJwtPtr CWHJwt::Parse(const string& sToken)
{
	//��������
	size_t Pos = sToken.find('.', 0);	
	if (Pos == string::npos)
	{
		throw runtime_error("Token��ʽ����!");
	}
	
	//��ȡ����
	string sData = sToken.substr(0, Pos);	
	string sSign = sToken.substr(Pos + 1, sToken.length());

	//��������
	CWHJwt* pWHJwt = NULL;
	try 
	{
		//����Claims
		CWHClaimsPtr ClaimsPtr = new CWHClaims(sData, true);

		//����WHJWT
		pWHJwt = new CWHJwt(ClaimsPtr, sSign);
	}
	catch (...)
	{
		throw;
	}

	return pWHJwt;
}

//////////////////////////////////////////////////////////////////////////////////
