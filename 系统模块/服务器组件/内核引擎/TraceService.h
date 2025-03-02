#ifndef TRACE_SERVICE_HEAD_FILE
#define TRACE_SERVICE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//׷�ٷ���
class KERNEL_ENGINE_CLASS CTraceService
{
	//��������
protected:
	//���캯��
	CTraceService();

	//��������
public:
	//���÷���
	static bool SetTraceService(IUnknownEx * pIUnknownEx);
	//��ȡ����
	static VOID * GetTraceService(const IID & Guid, DWORD dwQueryVer);
	//��ȡ����
	static VOID * GetTraceServiceManager(const IID & Guid, DWORD dwQueryVer);

	//״̬����
public:
	//׷��״̬
	static bool IsEnableTrace(enTraceLevel TraceLevel);
	//׷�ٿ���
	static bool EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace);

	//���ܺ���
public:
	//׷����Ϣ
	static bool TraceString(LPCTSTR pszFormat, enTraceLevel TraceLevel, ...);
};

//////////////////////////////////////////////////////////////////////////

#endif