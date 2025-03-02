#ifndef DATACACHE_ENGINE_SINK_HEAD_FILE
#define DATACACHE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "map"
#include "redis++.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

using namespace redis;
using namespace lhchat;

//////////////////////////////////////////////////////////////////////////

//���ݿ⹳��
class CDataCacheEngineSink : public IDataBaseEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;			

	//�������
protected:
	CDataBaseAide					m_AccountsDBAide;					//��Ϣ���ݿ�	
	CDataBaseHelper					m_AccountsDBModule;					//��Ϣ���ݿ�

	//�������
protected:
	Redis *							m_pUserWClient;						//�������
	Redis *							m_pUserRClient;						//�������
	Redis *							m_pFriendClient;					//�������

	//�ű�����
protected:
	string							m_ScriptFriendList;					//�����б�

	//�������
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�

	//��������
public:
	//���캯��
	CDataCacheEngineSink();
	//��������
	virtual ~CDataCacheEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnDataBaseEngineStop(IUnknownEx * pIUnknownEx);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//��������
public:
	//ͬ����Ϣ
	bool OnRequestSyncUserInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ѯ��Ϣ
	bool OnRequestQueryUserInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ѯ������
	bool OnRequestQueryBlackList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ѯ�����б�
	bool OnRequestQueryFriendList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ѯ������Ϣ
	bool OnRequestQueryFriendInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ѯ������չ
	bool OnRequestQueryFriendExtras(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��������
public:
	//��������
	bool OnRequestBuildAccessToken(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��������
protected:
	//��ѯȨ��
	WORD QueryMasterOrder(UINT32 iUserID);
	//��ѯ��ʶ
	bool QueryUserID(vector<UINT32>& vGameID, vector<UINT32>& vUserID);
	//��ѯ��Ϣ
	bool QueryUserInfo(vector<UINT32>& vUserID,list<tagIMUserPublicInfo>& lUserPublicInfo);
	//�����û�
	bool CacheUserInfo(list<tagIMUserPublicInfo>& lIMUserPublicInfo, list<string>& lUserPublicInfo);	

	//��������
protected:		
	//����쳣
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif