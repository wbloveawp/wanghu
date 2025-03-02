#pragma once

#include"KernelEngineHead.h"
#include "Cipher.h"

class CWebSocketServiceThread : public CWHThread
{
	//�ں˱���
protected:
	HWND							m_hWnd;								//���ھ��
	SOCKET							m_hSocket;							//���Ӿ��
	BYTE				            m_TCPSocketStatus;					//����״̬

																		//�������
protected:
	CWHDataQueue					m_DataQueue;						//�������
	CCriticalSection				m_CriticalSection;					//��������

																		//���ձ���
protected:
	WORD							m_wRecvSize;						//���ճ���
	BYTE							m_cbRecvBuf[1024];					//���ջ���

	bool							m_bFlag;

																		//�������
protected:
	bool							m_bNeedBuffer;						//����״̬
	DWORD							m_dwBufferData;						//��������
	DWORD							m_dwBufferSize;						//�����С
	LPBYTE							m_pcbDataBuffer;					//��������

																		//��������
protected:
	DWORD							m_dwSendTickCount;					//����ʱ��
	DWORD							m_dwRecvTickCount;					//����ʱ��

																		//��������
public:
	//���캯��
	CWebSocketServiceThread();
	//��������
	virtual ~CWebSocketServiceThread();

	//���غ���
public:
	//���к���
	virtual bool OnEventThreadRun();
	//��ʼ�¼�
	virtual bool OnEventThreadStrat();
	//ֹͣ�¼�
	virtual bool OnEventThreadConclude();
	//ֹͣ�߳�
	virtual bool ConcludeThread(DWORD dwWaitSeconds);

	//������
public:
	//Ͷ������
	bool PostThreadRequest(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize);

	//���ƺ���
private:
	//�ر�����
	VOID PerformCloseSocket(BYTE cbShutReason);
	//���ӷ�����
	DWORD PerformConnect(DWORD dwServerIP, WORD wPort);
	//���ͺ���
	DWORD PerformSendData(WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	DWORD PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��������
private:
	//��������
	DWORD SendBuffer(VOID * pBuffer, WORD wSendSize);
	//��������
	VOID AmortizeBuffer(VOID * pData, WORD wDataSize);


	//������
private:
	//������Ϣ
	LRESULT OnSocketNotify(WPARAM wParam, LPARAM lParam);
	//������Ϣ
	LRESULT OnServiceRequest(WPARAM wParam, LPARAM lParam);

	//��������
private:
	//�����ȡ
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//���緢��
	LRESULT OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam);
	//����ر�
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);
	//��������
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);
};

class CWebSocketService : public IWEBSocketService
{
public:
	CWebSocketService();
	~CWebSocketService();

public:
	WORD		m_wServiceID;
	bool		m_bService;

	IWEBSocketEvent					*m_pIWebSocketEvent;
	CWebSocketServiceThread			m_WebSocketServiceThread;
	//�ӿ�
public:
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�����ӿ�
public:
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();
	//IWebSocketService �ӿ�
public:
	//���ú���
	virtual bool SetServiceID(WORD wServiceID);
	//���ýӿ�
	virtual bool SetWEBSocketEvent(IUnknownEx * pIUnknownEx);

	//���ܽӿ�
public:
	//�ر�����
	virtual bool CloseSocket(BYTE cbShutReason);
	//���Ӳ���
	virtual bool ConnectServer(LPCTSTR pszURL, WORD wPort);
	//��������
	virtual bool SendRequestData(VOID * pData, WORD wDataSize);

	//������Ϣ
	bool OnEventWEBSocketLink(WORD wServiceID, WORD wRequestID, INT nErrorCode);
	//�ر���Ϣ
	bool OnEventWEBSocketShut(WORD wServiceID, WORD wRequestID, BYTE cbShutReason);
	//���ݰ���
	bool OnEventWEBSocketMain(WORD wServiceID, WORD wRequestID, VOID * pcbMailData, WORD wStreamSize);
	//���ݰ�ͷ
	bool OnEventWEBSocketHead(WORD wServiceID, WORD wRequestID, VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode);
};

