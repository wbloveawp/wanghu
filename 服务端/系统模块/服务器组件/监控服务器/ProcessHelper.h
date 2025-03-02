#ifndef PROCESS_HELPER_HEAD_FILE
#define PROCESS_HELPER_HEAD_FILE
#pragma once

//�����ļ�
#include "Stdafx.h"

/////////////////////////////////////////////////////////////////////////////////////////
//��������
class CProcessHelper
{
	//��������
public:
	//���캯��
	CProcessHelper();
	//��������
	virtual ~CProcessHelper();

	//���̹���
public:
	//���ҽ���
	static bool IsProcessExists(DWORD dwProcessID);
	//��ֹ����
	static void TerminateProcess(DWORD dwProcessID);
	//��������
	static DWORD LaunchProcess(LPCTSTR pszExecuteName, LPCTSTR pszLaunchParam,int nShow);

	//�������
public:
	//��ʼ����
	static bool StartService(DWORD dwProcessID);
	//�رշ���
	static bool ConcludeService(DWORD dwProcessID);	

	//��������
public:
	//���ھ��
	static HWND GetWindowHwndByPID(DWORD dwProcessID);
	//��ȡ·��
	static bool GetProcessPathByPId(DWORD dwProcessId, TCHAR szProcessPath[MAX_PATH]);

	//������Ϣ
public:
	//CPUʹ��
	static int  GetCPUUsage(DWORD dwProcessID, int64_t& LastTime, int64_t& LastSystemTime);
	//�ڴ�ʹ��
	static bool GetMemoryUsage(DWORD dwProcessID, UINT64* pMemory, UINT64* pVMemory);
	//IO�ֽ���
	static bool GetIOBytes(DWORD dwProcessID, UINT64* pReadBytes, UINT64* pWriteBytes);		
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif
