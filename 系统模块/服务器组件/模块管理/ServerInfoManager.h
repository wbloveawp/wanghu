#ifndef SERVER_INFO_MANAGER_HEAD_FILE
#define SERVER_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagGameServerInfo *> CGameServerInfoArray;

//��������
typedef CMap<WORD,WORD,tagGameServerInfo *,tagGameServerInfo *> CGameServerInfoMap;

//////////////////////////////////////////////////////////////////////////

//ģ������
class MODULE_MANAGER_CLASS CServerInfoBuffer
{
	//��������
public:
	CGameServerInfoMap				m_GameServerInfoMap;				//ģ������
	CGameServerInfoArray			m_GameServerInfoArray;				//ģ������

	//��������
public:
	//���캯��
	CServerInfoBuffer();
	//��������
	virtual ~CServerInfoBuffer();

	//������
public:
	//��������
	bool ResetServerInfo();
	//ɾ������
	bool DeleteServerInfo(WORD wServerID);
	//��������
	bool InsertServerInfo(tagGameServerInfo * pGameServerInfo);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetServerInfoCount();
	//��������
	tagGameServerInfo * SearchServerInfo(WORD wServerID);

	//�ڲ�����
private:
	//��������
	tagGameServerInfo * CreateServerInfo();
};


//////////////////////////////////////////////////////////////////////////

//ģ����Ϣ
class MODULE_MANAGER_CLASS CServerInfoManager
{
	//��������
public:
	//���캯��
	CServerInfoManager();
	//��������
	virtual ~CServerInfoManager();

	//���ط���
public:
	//���ط���
	bool LoadGameServerInfo(WORD wServerID, tagGameServerInfo & GameServerResult);
	//���ط���
	bool LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer);

	//ע�����
public:
	//ɾ������
	bool DeleteGameServer(WORD wServerID);
	//���뷿��
	bool InsertGameServer(tagCreateServerInfo * pCreateServerInfo, tagGameServerInfo & GameServerResult);
	//�޸ķ���
	bool ModifyGameServer(tagCreateServerInfo * pCreateServerInfo, tagGameServerInfo & GameServerResult);

	//�ڲ�����
private:
	//��������
	bool ConnectPlatformDB(CDataBaseHelper & PlatformDBModule);
	//��ȡ����
	bool ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameServerInfo & GameServerResult);
};

//////////////////////////////////////////////////////////////////////////

#endif