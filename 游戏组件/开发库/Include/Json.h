#ifndef JSON_HEAD_FILE
#define JSON_HEAD_FILE
#pragma once

//�������ļ�
#include "rapidjson/document.h"
#include "CommonServiceHead.h"

/////////////////////////////////////////////////////////////////
//JSON ��
class COMMON_SERVICE_CLASS CJson
{
	//��Ա����
public:
	rapidjson::Document					m_JsonDoc;			//�ĵ�����

	//��������
public:
	//���캯��
	CJson();
	//��������
	~CJson();

	//���ܺ���
public:
	//��ȡJSON
	bool Read(LPCTSTR lpJsonString);
	//��ȡ�ļ�
	bool ReadFromFile(LPCTSTR lpFile);
	//д���ļ�
	bool WriteToFile(LPCTSTR lpFile);
	//д�뻺��
	bool WriteToBuffer(LPTSTR lpOutBuf, WORD wBufLen);

	//���ܺ���
public:
	//��ȡ����
	int GetValueInt(LPCTSTR lpKey);
	//��ȡ����
	bool GetValueBool(LPCTSTR lpKey);
	//��ȡ����
	double GetValueDouble(LPCTSTR lpKey);
	//��ȡ����
	SCORE GetValueInt64(LPCTSTR lpKey);
	//��ȡ����
	LPTSTR GetValueString(LPCTSTR lpKey, LPTSTR lpOutBuf, WORD wBufLen);
	//��ȡ����
	void GetValueObject(LPCTSTR lpObj, LPTSTR lpOutBuf, WORD wBufLen);

	//���ܺ���
public:
	//��������
	bool SetKeyValue(LPCTSTR lpKey, int nValue);
	//��������
	bool SetKeyValue(LPCTSTR lpKey, int64_t nValue);
	//��������
	bool SetKeyValue(LPCTSTR lpKey, bool nValue);
	//��������
	bool SetKeyValue(LPCTSTR lpKey, double nValue);
	//��������
	bool SetKeyValue(LPCTSTR lpKey, LPCTSTR lpValue);

	//���ܺ���
public:
	//�������
	bool AddMember(LPCTSTR lpKey, int nValue);
	//�������
	bool AddMember(LPCTSTR lpKey, int64_t nValue);
	//�������
	bool AddMember(LPCTSTR lpKey, bool nValue);
	//�������
	bool AddMember(LPCTSTR lpKey, double nValue);
	//�������
	bool AddMember(LPCTSTR lpKey, LPCTSTR lpValue);

	//���ܺ���
public:
	//�������
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, int nValue);
	//�������
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, int64_t nValue);
	//�������
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, bool nValue);
	//�������
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, double nValue);
	//�������
	bool AddMember(LPCTSTR lpObj, LPCTSTR lpKey, LPCTSTR lpValue);

	//���ܺ���
public:
	//�������
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, int nValue);
	//�������
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, int64_t nValue);
	//�������
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, bool nValue);
	//�������
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, double nValue);
	//�������
	bool AddArrayMember(LPCTSTR lpArray, WORD wIndex, LPCTSTR lpKey, LPCTSTR lpValue);

	//���ܺ���
public:
	//ɾ����Ա
	bool DeleteALL();
	//ɾ����Ա
	bool DeleteMember(LPCTSTR lpMember);
	//�����Ա
	void EraseMember(rapidjson::Value::ConstMemberIterator pos);
	//�����Ա
	void EraseMember(rapidjson::Value::ConstMemberIterator first, rapidjson::Value::ConstMemberIterator last);

	//���ܺ���
public:
	//��Ա����
	DWORD GetMemberCounts();
	//��ȡ��Ա
	rapidjson::Value::MemberIterator GetMemberFirst();
	//��ȡ��Ա
	rapidjson::Value::MemberIterator GetMemberEnd();
};

/////////////////////////////////////////////////////////////////
#endif

