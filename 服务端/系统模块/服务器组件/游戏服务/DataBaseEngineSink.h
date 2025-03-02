#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "DataBasePacket.h"
#include "GameServiceHead.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//���ݿ⹳��
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//��������
protected:
	INT_PTR							m_nSyncIndex;						//ͬ��ID

	//ϵͳ����
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//�������� 

	//��������
protected:
	DBO_GR_LogonFailure				m_LogonFailure;						//��¼ʧ��
	DBO_GR_LogonSuccess				m_LogonSuccess;						//��¼�ɹ�

	//��Ϸ���ݿ�
protected:
	CDataBaseAide					m_GameDBAide;						//��Ϸ���ݿ�
	CDataBaseHelper					m_GameDBModule;						//��Ϸ���ݿ�

	//��Ϸ���ݿ�
protected:
	CDataBaseAide					m_AccountsDBAide;					//�˺����ݿ�
	CDataBaseHelper					m_AccountsDBModule;					//�˺����ݿ�

	//ƽ̨���ݿ�
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

	//��չ�ӿ�
protected:
	IGameDataBaseSink *				m_pIGameDataBaseSink;				//��չ����
	IExtendDataBaseSink *			m_pIExtendDataBaseSink;				//��չ����

	//�������
protected:
	ITimerEngine *					m_pITimerEngine;					//��ʱ����
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�		
	CAttemperEngineSink *			m_pAttemperEngineSink;				//���ȹ���
	
	//��������
public:
	//���캯��
	CDataBaseEngineSink();
	//��������
	virtual ~CDataBaseEngineSink();

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
private:
	//���ػ���
	bool OnRequestLoadRobots(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��������
	bool OnRequestLoadConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��¼����
private:
	//������¼
	bool OnRequestLogonRobot(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�ֻ���¼
	bool OnRequestLogonMobile(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//���Ե�¼
	bool OnRequestLogonComputer(DWORD dwContextID, DWORD dwTokenID,VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//��Ϸ����
private:
	//¼���¼
	bool OnRequestRecordGameVideo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��Ϸ��¼
	bool OnRequestRecordDrawInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//д�ּ�¼
	bool OnRequestRecordDrawScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�û�Ȩ��
	bool OnRequestManageUserRight(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��������
	bool OnRequestSendGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��������
	bool OnRequestUpdateLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	

	//ϵͳ����
private:
	//�뿪����
	bool OnRequestLeaveService(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//��Ϸд��
	bool OnRequestWriteGameScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//�û��Ƹ�
	bool OnRequestWriteUserWealth(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//���͵ͱ�
	bool OnRequestLargessSubsidy(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//��������
private:
	//��ѯ��Ʒ
	bool OnRequestUserQueryGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//������Ʒ	
	bool OnRequestUserUpdateGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��¼��Ʒ	
	bool OnRequestUserRecordGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��������
protected:
	//���Ʋ�ѯ
	bool OnRequestControlQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//���ƿ�ʼ
	bool OnRequestControlStart(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//���ƽ���
	bool OnRequestControlFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//���Ƹ���
	bool OnRequestControlUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	

	//��������
private:	
	//��������
	bool UnlockRobotUser();
	//���ر���
	bool LoadUserPack(DWORD dwUserID);
	//��ѯ�ͱ�
	bool LoadUserSubsidy(DWORD dwUserID);
	//���ؿ���
	bool LoadControlInfo(DWORD dwUserID,tagUserControlItem& UserControlItem);
	//��������
	bool UpdateUserLoveLiness(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID);
	//��¼���
	VOID OnLogonDisposeResult(DWORD dwContextID, DWORD dwTokenID, LONG lResultCode, bool bMobileClient);
	//��¼ʧ��
	VOID OnLogonDisposeDBError(DWORD dwContextID, DWORD dwTokenID, IDataBaseException * pIException);		

	//���غ���
private:	
	//���дʻ�
	bool LoadFilterWords(DWORD dwContextID, DWORD dwTokenID);	
	//Լս����
	bool LoadBattleOption(DWORD dwContextID, DWORD dwTokenID);
	//��������
	bool LoadRobotParameter(DWORD dwContextID, DWORD dwTokenID);
	//�ͱ�����
	bool LoadSubsidyParameter(DWORD dwContextID, DWORD dwTokenID);
	//���Ʋ���
	bool LoadControlParameter(DWORD dwContextID, DWORD dwTokenID);
	//��Ϸ�ȼ�
	bool LoadGameLevelOption(DWORD dwContextID, DWORD dwTokenID);
	//��Ϸ��Ʒ
	bool LoadGameGoodsOption(DWORD dwContextID, DWORD dwTokenID);
	//�汾��Ϣ
	bool LoadGameVersionInfo(DWORD dwContextID, DWORD dwTokenID);	
	//ȫ������
	bool LoadGlobalOption(DWORD dwContextID, DWORD dwTokenID, DWORD dwOptionMask);
};

//////////////////////////////////////////////////////////////////////////

#endif