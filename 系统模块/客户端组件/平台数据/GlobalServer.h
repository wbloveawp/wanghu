#ifndef GLOBAL_SERVER_HEAD_FILE
#define GLOBAL_SERVER_HEAD_FILE

#pragma once

#include "PlatformDataHead.h"

//////////////////////////////////////////////////////////////////////////
//ע�����

//��¼����
#define REG_GOBAL_OPTION			TEXT("Option\\Golbal")				//ȫ����Ϣ

//////////////////////////////////////////////////////////////////////////

//��ַ����
enum enNetworkKind
{
	NetworkKind_Default,			//Ĭ�ϵ�ַ
	NetworkKind_DianXin,			//���ŵ�ַ
	NetworkKind_WangTong,			//��ͨ��ַ
};

//��ַ��Ϣ
struct tagServerItem
{
	WORD							wServerPort;						//����˿�
	DWORD							dwServerAddr;						//�����ַ
	TCHAR							szServerHost[LEN_SERVER];			//������Ϣ
};

//////////////////////////////////////////////////////////////////////////

//ȫ�ַ�����
class PLATFORM_DATA_CLASS CGlobalServer
{
	//��������
public:
	//���캯��
	CGlobalServer();
	//��������
	virtual ~CGlobalServer();

	//ȫ����Ϣ
public:
	//������Ŀ
	WORD GetItemCount();

	//��·����
public:
	//��������
	enNetworkKind GetNetworkKind();
	//��������
	VOID SetNetworkKind(enNetworkKind NetworkKind);

	//�ϴε�¼
public:
	//�ϴε�¼
	VOID SetLastServerName(LPCTSTR pszLastServer);
	//�ϴε�¼
	VOID GetLastServerName(TCHAR szLastServer[LEN_SERVER]);

	//������Ϣ
public:
	//��������
	VOID GetServerItemName(WORD wIndex, TCHAR szResult[LEN_SERVER]);
	//������Ϣ
	bool GetServerItemInfo(LPCTSTR pszServerName, enNetworkKind NetworkKind, tagServerItem & ServerItem);

	//д�뺯��
public:
	//д������
	VOID WriteServerItemInfo(LPCTSTR pszServerName, enNetworkKind NetworkKind, tagServerItem & ServerItem);

	//д�뺯��
protected:
	//��ȡ��ֵ
	VOID WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nValue);
	//д������
	VOID WriteFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, LPCTSTR pszString);

	//��ȡ����
protected:
	//��ȡ��ֵ
	UINT ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault);
	//��ȡ����
	VOID ReadFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount);
};

//////////////////////////////////////////////////////////////////////////

#endif