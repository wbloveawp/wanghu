#ifndef HTTP_SERVICE_HEAD_FILE
#define HTTP_SERVICE_HEAD_FILE

#pragma once

#include "Wininet.h"
#include "DownLoadSocket.h"
#include "HttpHeadTranslate.h"
#include "DownLoadMission.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//HTTP����
class CHttpService : public IHttpService, public IDownLoadSocketSink
{
	//�������
protected:
	LPVOID							m_lpvBuffer;						//�����ַ
	DWORD							m_dwBufSize;						//�����С

	//������Ϣ
protected:
	enDownLoadStatus				m_DownLoadStatus;					//����״̬
	tagRequestInfo					m_RequestInfo;						//������Ϣ

	//�ļ���Ϣ
protected:
	DWORD							m_dwDownLoadSize;					//���ش�С
	DWORD							m_dwTotalFileSize;					//�ļ���С

	//�������
protected:
	CDownLoadSocket					m_DownLoadSocket;					//��������
	CHttpHeadTranslate				m_HttpHeadTranslate;				//Э�鷭��
	IHttpServiceSink				*m_pIHttpServiceSink;				//����ص�

	//��������
public:
	//���캯��
	CHttpService();
	//��������
	virtual ~CHttpService();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//���ýӿ�
	virtual bool __cdecl SetHttpServiceSink(IHttpServiceSink * pSink);

	//���ƽӿ�
public:
	//��������
	virtual bool __cdecl ConnectServer(LPCTSTR lpURL);
	//ֹͣ����
	virtual VOID __cdecl StopService();

	//״̬�ӿ�
public:
	//������Ϣ
	virtual bool __cdecl OnSocketConnect(INT nErrorCode);
	//�ر���Ϣ
	virtual bool __cdecl OnSocketClose(bool bCloseByServer);

	//���ݽӿ�
public:
	//���ݰ���
	virtual VOID __cdecl OnHttpDataStream(const VOID * pStreamData, WORD wStreamSize);
	//���ݰ�ͷ
	virtual VOID __cdecl OnHttpDataHead(const VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode);

	//��������
protected:
	//��������
	bool SendHttpRequest(const tagRequestInfo & RequestInfo);
	//��ȡ��Ϣ
	INTERNET_SCHEME GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif