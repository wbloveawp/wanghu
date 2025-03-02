#ifndef TRACE_LOG_HEAD_FILE
#define TRACE_LOG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <fstream>

//////////////////////////////////////////////////////////////////////////

//�����־
class CTraceLog: public ITraceService
{
	//�������
public:
	bool							m_bLoging;							//������־
	std::ofstream					m_LogFile;							//�����־

	//�������
protected:
	ITraceService*					m_pITraceService;					//�ڲ����

	//��������
public:
	//���캯��
	CTraceLog();
	//��������
	virtual ~CTraceLog();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//��Ϣ�ӿ�
public:
	//׷����Ϣ
	virtual bool TraceString(LPCTSTR pszString, enTraceLevel TraceLevel);

	//���ܺ���
public:
	//��ʼ��
	void InitLog(IUnknownEx *pITraceService);
	//������־
	bool OpenLog(LPCTSTR lpszFile);
	//�ر���־
	bool CloseLog();
	//��ѯ״̬
	bool IsLoging() { return m_bLoging; }
};

//////////////////////////////////////////////////////////////////////////

#endif