#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////

//���ݿ⹳��
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//�洢����
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//��������

	//�������
protected:
	CDataBaseAide					m_GameBattleDBAide;					//ƽ̨���ݿ�	
	CDataBaseHelper					m_GameBattleDBModule;				//ƽ̨���ݿ�

	//�������
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�

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

	//ϵͳ����
protected:
	//�û���¼
	bool OnRequestLogonService(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����Ⱥ��
	bool OnRequestCreateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����Ⱥ��
	bool OnRequestUpdateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//ɾ��Ⱥ��
	bool OnRequestDeleteClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����Ⱥ��
	bool OnRequestSearchClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//ת��Ⱥ��
	bool OnRequestTransferClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//���³�Ա
	bool OnRequestUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//ɾ����Ա
	bool OnRequestDeleteMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//��������
	bool OnRequestApplyRequest(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//������Ӧ
	bool OnRequestApplyRespond(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����ɾ��
	bool OnRequestApplyDelete(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//���볷��
	bool OnRequestApplyRepeal(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//Լս����
	bool OnRequestSettleBattle(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�޸Ļ���
	bool OnRequestModifyScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//���ͻ���
	bool OnRequestPresentScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��Ǽ�¼
	bool OnRequestDeleteRecord(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�������
	bool OnRequestDetectClubName(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�������
	bool OnRequestCleanupScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����˰��
	bool OnRequestCleanupRevenue(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestCleanupSetup(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�����
	bool OnRequestCleanupRankList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����ϲ�
	bool OnRequestRequestMerge(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��Ӧ�ϲ�
	bool OnRequestRespondMerge(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//ȡ������
	bool OnRequestCancelPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�������
	bool OnRequestRequestPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��Ӧ����
	bool OnRequestRespondPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);		
	//��������
	bool OnRequestLoadClubOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestLoadForbidWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	
	//��������
protected:		
	//��ȡȺ��
	WORD ExtractClubID(LPCTSTR pszClubIDArray, CDWordArray & ClubIDArray);
	//����쳣
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif