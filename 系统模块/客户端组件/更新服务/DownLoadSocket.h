#ifndef DOWN_LOAD_SOCKET_HEAD_FILE
#define DOWN_LOAD_SOCKET_HEAD_FILE

#pragma once

#include "WinSock2.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//��������
class CDownLoadSocket : public CWnd
{
	//״̬����
protected:
	WORD							m_wRequestID;						//�����ʶ
	bool							m_bHeadValid;						//��Ч��־
	bool							m_bCloseByServer;					//�رշ�ʽ

	//���ݰ�ͷ
protected:
	WORD							m_wHeadSize;						//��ͷ����
	CHAR							m_szHeadData[1024];					//���ݰ�ͷ

	//�ں˱���
protected:
	SOCKET							m_hSocket;							//���Ӿ��
	BYTE							m_cbSocketStatus;					//����״̬
	IDownLoadSocketSink *			m_pIDownLoadSocketSink;				//�ص��ӿ�

	//������Ϣ
protected:
	enProxyServerType				m_ProxyServerType;					//��������
	tagProxyServerInfo				m_ProxyServerInfo;					//������Ϣ

	//��������
public:
	//���캯��
	CDownLoadSocket();
	//��������
	virtual ~CDownLoadSocket();

	//��Ϣ�ӿ�
public:
	//��ȡ״̬
	BYTE GetSocketStatus() { return m_cbSocketStatus; }

	//���ú���
public:
	//���ýӿ�
	bool SetDownLoadSocketSink(IUnknownEx * pIUnknownEx);

	//����ӿ�
public:
	//��������
	enProxyServerType GetProxyServerType() { return m_ProxyServerType; }
	//���ô���
	bool SetProxyServerInfo(enProxyServerType ProxyServerType, const tagProxyServerInfo & ProxyServerInfo);

	//�����ӿ�
public:
	//�ر�����
	bool CloseSocket(bool bNotify);
	//���Ӳ���
	bool Connect(LPCTSTR pszServerAddr, WORD wPort);
	//��������
	bool SendRequestData(VOID * const pData, WORD wDataSize);

	//��������
protected:
	//���Ӵ���
	bool ConnectProxyServer();
	//��ַ����
	DWORD TranslateAddr(LPCTSTR pszServerAddr);

	//������
protected:
	//�����ȡ
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//����ر�
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);
	//��������
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);

	//��Ϣӳ��
protected:
	//������Ϣ
	LRESULT	OnSocketNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif