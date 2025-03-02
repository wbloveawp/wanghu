#ifndef DOWN_LOAD_MISSION_HEAD_FILE
#define DOWN_LOAD_MISSION_HEAD_FILE

#pragma once

#include "Wininet.h"
#include "DownLoadSocket.h"
#include "HttpHeadTranslate.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//����˵��

//��˵��
class CDownLoadMission;

//����˵��
typedef CWHArray<CDownLoadMission *> CDownLoadMissionArray;		//��������

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//������Ϣ
struct tagRequestInfo
{
	WORD							wHostPort;							//���Ӷ˿�
	TCHAR							szHostName[128];					//��������
	TCHAR							szRequestPath[256];					//����·��
};

//////////////////////////////////////////////////////////////////////////

//��������
class CDownLoadMission : public IDownLoadMission, public IDownLoadSocketSink
{
	//��Ԫ����
	friend class CDownLoadService;

	//������Ϣ
protected:
	DWORD							m_dwMissionID;						//�����ʶ
	enDownLoadStatus				m_DownLoadStatus;					//����״̬

	//�ļ���Ϣ
protected:
	DWORD							m_dwDownLoadSize;					//���ش�С
	DWORD							m_dwTotalFileSize;					//�ļ���С
	LPCTSTR							m_pszFileEntityTag;					//�ļ���ʶ

	//�ں���Ϣ
protected:
	bool							m_bRelease;							//�ͷ��ͷ�
	tagRequestInfo					m_RequestInfo;						//������Ϣ
	tagMissionRequest				m_MissionRequest;					//��������
	IDownLoadMissionSink *			m_pIDownLoadMissionSink;			//�ص��ӿ�

	//�������
protected:
	CFile							m_BufferFile;						//�����ļ�
	CDownLoadSocket					m_DownLoadSocket;					//��������
	CHttpHeadTranslate				m_HttpHeadTranslate;				//Э�鷭��

	//��������
public:
	//���캯��
	CDownLoadMission();
	//��������
	virtual ~CDownLoadMission();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//���ýӿ�
	virtual bool __cdecl SetDownLoadMissionSink(IUnknownEx * pIUnknownEx);

	//������Ϣ
public:
	//�����ʶ
	virtual DWORD __cdecl GetMissionID() { return m_dwMissionID; }
	//����״̬
	virtual enDownLoadStatus __cdecl GetDownLoadStatus() { return m_DownLoadStatus; }

	//�ļ���Ϣ
public:
	//���ش�С
	virtual DWORD __cdecl GetDownLoadSize() { return m_dwDownLoadSize; }
	//�ļ���С
	virtual DWORD __cdecl GetTotalFileSize() { return m_dwTotalFileSize; }
	//�ļ���ʶ
	virtual LPCTSTR __cdecl GetFileEntityTag() { return m_pszFileEntityTag; }

	//���ƽӿ�
public:
	//��������
	virtual bool __cdecl StopDownLoad();
	//��ʼ����
	virtual bool __cdecl StartDownLoad(DWORD dwMissionID, const tagMissionRequest & MissionRequest);

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

	//���ܺ���
public:
	//�����ͷ�
	bool SetRelease(bool bRelease);
	//�Ա�����
	bool CompareRequest(const tagMissionRequest & MissionRequest);

	//��������
protected:
	//��������
	bool ConnectServer(LPCTSTR pszDownLoadUrl);
	//��������
	bool SendHttpRequest(const tagRequestInfo & RequestInfo);
	//�����ļ�
	bool CreateBufferFile(const tagMissionRequest & MissionRequest);
	//��ȡ��Ϣ
	INTERNET_SCHEME GetDownLoadInfo(LPCTSTR pszDownUrl, tagRequestInfo & RequestInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif