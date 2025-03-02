#ifndef WH_JWT_HEAD_FILE
#define WH_JWT_HEAD_FILE

#pragma once

#include "WHJson.h"
#include "CommonServiceHead.h"

using namespace std;
using namespace WHJson;

//////////////////////////////////////////////////////////////////////////////////
//��������
class CWHJwt;
class CWHClaims;

//���Ͷ���
typedef CWHAutoPtr<CWHJwt>			CWHJwtPtr;
typedef CWHAutoPtr<CWHClaims>		CWHClaimsPtr;

//////////////////////////////////////////////////////////////////////////////////

class COMMON_SERVICE_CLASS CWHClaims : public CHandleBase
{
	//��������
protected:	
	CJsonValueObjPtr				m_Claims;						//Json����

	//��������
public:
	//���캯��
	CWHClaims();
	//���캯��
	explicit CWHClaims(const string& sJsonData, bool bBase64 = false);

	//���ʺ���
public:
	//��麯��
	bool Has(const string& key);
	//ɾ������
	void Del(const string& key);	
	//��ȡ����
	template<typename T>
	T Get(const string& key);
	//У�麯��
	template<typename T>
	bool Check(const string& key, T& value);
	//���ú���
	template<typename T>
	void Set(const string& key, const T& value);	

	//���ܺ���
public:
	//��ȡ����
	string GetContent();
	//���뺯��
	string Base64Encode();
};

//////////////////////////////////////////////////////////////////////////////////


//JWT
class COMMON_SERVICE_CLASS CWHJwt : public CHandleBase
{
	//��������
private:
	string							m_sSign;
	CWHClaimsPtr					m_ClaimsPtr;	

	//��������
private:
	//���캯��
	CWHJwt(CWHClaimsPtr claimsPtr, const string& Sign);

	//��Ա����
public:
	//У�麯��
	bool Verify();	
	//��ȡ����
	CWHClaims & Claims() { return *(m_ClaimsPtr.get()); }

	//��̬����
public:
	//ǩ������
	static string Sign(CWHClaims& claims);
	//ǩ������
	static string Sign(const string& sData);	
	//��������
	static CWHJwtPtr Parse(const string& sToken);	
};

//////////////////////////////////////////////////////////////////////////////////

//��ȡ����
template<typename T>
T CWHClaims::Get(const string& key)
{
	if (!m_Claims)
	{
		throw exception("���ݸ�ʽ����");
	}

	T v;
	CWHJsonInput::readJson(v, m_Claims->value[key], false);
	return v;
}

//У�麯��
template<typename T>
bool CWHClaims::Check(const string& key, T& value)
{
	//У�����
	if (!m_Claims) return false;

	T v;
	CWHJsonInput::readJson(v, m_Claims->value[key], false);
	return v == value;
}

//���ú���
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