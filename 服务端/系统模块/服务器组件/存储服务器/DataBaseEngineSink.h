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
	
	//��̬����
protected:
	static CCriticalSection			m_DirectoryLocked;					//Ŀ¼����

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

	//�洢ʱ��
public:
	//�洢¼��
	bool OnRequestStoreVideo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//��̬����
protected:
	//�ļ�����
	static BOOL IsFileExist(LPCTSTR pszFileName);
	//Ŀ¼����
	static BOOL IsDirectoryExist(LPCTSTR pszDirectory);
	//����Ŀ¼
	static BOOL CreateDirectory(LPCTSTR pszFileDirectory);		
	//�����ļ�
	static BOOL CreateFile(LPCTSTR pszFileName,BYTE cbFileData[],DWORD dwFileSize);	
};

//////////////////////////////////////////////////////////////////////////

#endif