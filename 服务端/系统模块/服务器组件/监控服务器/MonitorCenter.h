#ifndef MONITOR_CENTER_HEAD_FILE
#define MONITOR_CENTER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ForwardPacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////
//���Ͷ���

//����ͳ����Ϣ
struct tagProcessStatInfo
{
	//���̲���
	int							nCPUUsage;						//CPUʹ��
	UINT64						uReadBytes;						//���ֽ���	 
	UINT64						uWriteBytes;					//д�ֽ���
	UINT64						uMemoryUsage;					//�ڴ�ʹ��
	UINT64						uVMemoryUsage;					//�ڴ�ʹ��
	DWORD						dwLastTimeStamp;				//ʱ���

	//Ԥ������
	DWORD						dwCpuTestTime;					//����ʱ��	
	DWORD						dwCpuAlarmTime;					//Ԥ��ʱ��		
	UINT64						uInitMemoryUsage;				//�ڴ�ʹ��
	UINT64						uInitVMemoryUsage;				//�ڴ�ʹ��
	UINT64						uAlarmMemoryUsage;				//�ڴ�ʹ��
	UINT64						uAlarmVMemoryUsage;				//�ڴ�ʹ��

	//��������
	int64_t						CpuLastTime;					//����ʱ��
	int64_t						CpuLastSystemTime;				//ϵͳʱ��
};

//�������
struct tagServiceProcessItem
{	
	//������Ϣ
	HWND						hWndRecv;						//���ھ��
	DWORD						dwProcessID;					//���̱�ʶ

	//������Ϣ
	WORD						wServiceID;						//�����ʶ
	WORD						wServiceModule;					//����ģ��
	WORD						wServiceStatus;					//����״̬
	TCHAR						szServiceName[32];				//��������
	TCHAR						szProcessPath[MAX_PATH];		//����·��

	//ͳ����Ϣ
	tagProcessStatInfo			ProcessStatInfo;				//ͳ����Ϣ	
};

//���������Ϣ
struct tagServiceCrashInfo
{
	WORD						wServiceID;						//�����ʶ
	WORD						wServiceModule;					//����ģ��	
	DWORD						dwCrashTime;					//����ʱ��
};


//////////////////////////////////////////////////////////////////////////
//���Ͷ���	
typedef CWHStockData<DWORD, tagServiceCrashInfo>		CStockCrashInfo;
typedef CWHStockData<DWORD, tagServiceProcessItem>		CStockProcessItem;

//////////////////////////////////////////////////////////////////////////

//�������
class CMonitorCenter : public CWnd, public IProcessTradeSink
{
	//��������
protected:
	bool							m_bServiceStatus;					//����״̬
	IAttemperEngine*				m_pIAttemperEngine;					//��������	

	//��������
protected:
	SYSTEM_INFO						m_SystemInfo;						//ϵͳ��Ϣ

	//������Ϣ
protected:
	TCHAR							m_szServerName[32];					//��������
	tagServiceOption*				m_pServiceOption;					//����ѡ��

	//�������
protected:
	CUIControlSink					m_UIControlSink;					//�������	

	//���̹���
protected:
	CStockCrashInfo					m_StockCrashInfo;					//������Ϣ
	CStockProcessItem				m_StockProcessItem;					//���ֿ̲�

	//����ͨѶ
protected:
	CProcessTradeHelper				m_ProcessTrade;						//ͨѶ���

	//��̬����
public:
	static CMonitorCenter*			g_pMonitorCenter;					//����ָ��

	//��������
public:
	//���캯��
	CMonitorCenter();
	//��������
	virtual ~CMonitorCenter();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);	

	//�������
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();	

	//����ͨѶ
public:
	//��������
	virtual bool OnProcessTradeData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend);

	//���ú���
public:
	//��������
	VOID SetServerName(LPCTSTR pszServerName);
	//������Ϣ
	VOID SetServiceOption(tagServiceOption* pServiceOption);
	//���ýӿ�
	VOID SetAttemperEngine(IAttemperEngine* pIAttemperEngine);	

	//��������
public:
	//���̼��
	bool OnEventProcessMonitor(WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend);

	//�������
private:
	//��������
	bool StartService(tagServiceProcessItem* pServiceProcessItem);
	//ֹͣ����
	bool ConcludeService(tagServiceProcessItem* pServiceProcessItem);

	//�ڲ�����
private:
	//�رս���
	VOID NotifyCloseProcess(HWND hWndClient);
	//���Ϳ���
	bool SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize);

	//��⺯��
private:
	//����CPUռ��
	VOID AnalyseCpuUsage(tagServiceProcessItem* pServiceProcessItem);
	//�����ڴ�ռ��
	VOID AnalyseMemoryUsage(tagServiceProcessItem* pServiceProcessItem);	

	//��Ϣӳ��
protected:
	//������Ϣ
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	//��������
protected:
	//����Ԥ��
	VOID SendExceptionAlarmNotice(DWORD dwProcessID,CMD_CS_SendExceptionAlarm* pSendExceptionAlarm);

	//��Ϣ�¼�
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif