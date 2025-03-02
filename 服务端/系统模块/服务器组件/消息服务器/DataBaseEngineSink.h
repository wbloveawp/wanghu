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

	//�������
protected:
	CWininetHttp					m_WininetHttp;						//��ҳ���

	//�������
protected:
	CDataBaseAide					m_AccountsDBAide;					//�˺����ݿ�	
	CDataBaseHelper					m_AccountsDBModule;					//�˺����ݿ�

	//�������
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�	
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

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

	//ϵͳ�¼�
public:
	//������ҳ
	bool OnRequestRequestWebPage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//�ʼ�����
protected:
	//��ѯ�ʼ�
	bool OnRequestUserQueryMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸��ʼ�
	bool OnRequestUserModifyMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//�����ʼ�
	bool OnRequestUserInsertMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//ɾ������
	bool OnRequestUserDeleteMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ȡ����
	bool OnRequestClaimMailAttach(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	

	//��������
protected:
	//���²Ƹ�
	bool UpdateUserWealth(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID);	
	//������Ʒ
	bool UpdateUserGoods(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID, tagGoodsItem GoodsItemList[], WORD wGoodsCount);

	//��������
protected:		
	//����쳣
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif