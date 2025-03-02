#ifndef SERVICE_PARAMETER_HEAD_FILE
#define SERVICE_PARAMETER_HEAD_FILE

#pragma once

#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//�������
class SERVICE_FRAME_CLASS CServiceParameter
{
	//��������
public:
	//���캯��
	CServiceParameter();
	//��������
	virtual ~CServiceParameter();

	//���Ķ˿�
public:
	//��ȡ�˿�
	WORD GetCenterPort();
	//���ö˿�
	bool SetCenterPort(WORD wCenterPort);

	//��������
public:
	//��������
	bool GetServerName(TCHAR szServerName[LEN_SERVER]);
	//��������
	bool SetServerName(TCHAR szServerName[LEN_SERVER]);

	//�����ַ
public:
	//�����ַ
	bool GetServiceAddress(tagAddressInfo & AddressInfo);
	//�����ַ
	bool SetServiceAddress(const tagAddressInfo & AddressInfo);

	//���ĵ�ַ
public:
	//���ĵ�ַ
	bool GetCenterAddress(tagAddressInfo & AddressInfo);
	//���ĵ�ַ
	bool SetCenterAddress(const tagAddressInfo & AddressInfo);

	//��ѯ����
public:
	//��Ϣ���ݿ�
	bool GetMessageDBInfo(tagDataBaseParameter& DataBaseParameter);
	//ƽ̨���ݿ�
	bool GetPlatformDBInfo(tagDataBaseParameter & DataBaseParameter);
	//�ʻ����ݿ�
	bool GetAccountsDBInfo(tagDataBaseParameter & DataBaseParameter);
	//������ݿ�
	bool GetGameGoldDBInfo(tagDataBaseParameter & DataBaseParameter);
	//�������ݿ�
	bool GetGameMatchDBInfo(tagDataBaseParameter& DataBaseParameter);
	//Լս���ݿ�
	bool GetGameBattleDBInfo(tagDataBaseParameter& DataBaseParameter);
	//��λ���ݿ�
	bool GetGameRankingDBInfo(tagDataBaseParameter& DataBaseParameter);

	//���ú���
public:
	//��Ϣ���ݿ�
	bool SetMessageDBInfo(const tagDataBaseParameter& DataBaseParameter);
	//ƽ̨���ݿ�
	bool SetPlatformDBInfo(const tagDataBaseParameter & DataBaseParameter);
	//�ʻ����ݿ�
	bool SetAccountsDBInfo(const tagDataBaseParameter & DataBaseParameter);
	//������ݿ�
	bool SetGameGoldDBInfo(const tagDataBaseParameter & DataBaseParameter);
	//�������ݿ�
	bool SetGameMatchDBInfo(const tagDataBaseParameter& DataBaseParameter);
	//Լս���ݿ�
	bool SetGameBattleDBInfo(const tagDataBaseParameter& DataBaseParameter);
	//��λ���ݿ�
	bool SetGameRankingDBInfo(const tagDataBaseParameter& DataBaseParameter);

	//�ڲ�����
private:
	//��ȡ��Ϣ
	bool ReadDataBaseInfo(LPCTSTR pszSubName, tagDataBaseParameter & DataBaseParameter);
	//д����Ϣ
	bool WriteDataBaseInfo(LPCTSTR pszSubName, const tagDataBaseParameter & DataBaseParameter);
	//д������
	bool WriteEncryptString(LPCTSTR pszAppName, LPCTSTR pszName, LPCTSTR pszString);
	//д������
	bool WriteNumber(LPCTSTR pszAppName, LPCTSTR pszName,int nNum);

	//��ȡ����
	bool ReadEncryptString(LPCTSTR pszAppName, LPCTSTR pszKeyName, LPTSTR pszString, UINT uMaxCount);
	//��ȡ����
	UINT ReadNumber(LPCTSTR pszAppName, LPCTSTR pszKeyName);
};

//////////////////////////////////////////////////////////////////////////

#endif