#ifndef REDIRECT_SERVICE_HEAD_FILE
#define REDIRECT_SERVICE_HEAD_FILE

#pragma once

#include "Wininet.h"
#include "DownLoadSocket.h"
#include "HttpHeadTranslate.h"
#include "DownLoadMission.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//�ض������
class CRedirectService : public IRedirectService, public IDownLoadSocketSink
{
	//��ַ����
protected:
	TCHAR							m_szRedirectServer[LEN_SERVER];		//�����ַ

	//������Ϣ
protected:
	enDownLoadStatus				m_DownLoadStatus;					//����״̬
	tagRequestInfo					m_RequestInfo;						//������Ϣ

	//�ļ���Ϣ
protected:
	DWORD							m_dwDownLoadSize;					//���ش�С
	DWORD							m_dwTotalFileSize;					//�ļ���С
	CHAR							m_szDownLoadBuffer[1024];			//�ļ�����

	//�������
protected:
	CDownLoadSocket					m_DownLoadSocket;					//��������
	CHttpHeadTranslate				m_HttpHeadTranslate;				//Э�鷭��
	IRedirectServiceSink			*m_pIRedirectServiceSink;			//����ص�

	//��������
public:
	//���캯��
	CRedirectService();
	//��������
	virtual ~CRedirectService();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//���ýӿ�
	virtual bool __cdecl SetRedirectServiceSink(IUnknownEx * pIUnknownEx);

	//���ƽӿ�
public:
	//������
	virtual bool __cdecl RequestRedirect(LPCTSTR lpURL);
	//ֹͣ����
	virtual VOID __cdecl StopRedirect();

	//��ѯ�ӿ�
public:
	//��ȡ�����ַ
	virtual LPCTSTR __cdecl GetRedirectServer() { return m_szRedirectServer; }

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
	bool ConnectServer(LPCTSTR pszDownLoadUrl);
	//��������
	bool SendHttpRequest(const tagRequestInfo & RequestInfo);
	//��ȡ��Ϣ
	INTERNET_SCHEME GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif