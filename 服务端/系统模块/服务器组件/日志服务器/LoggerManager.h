#ifndef LOGGER_MANAGER_HEAD_FILE
#define LOGGER_MANAGER_HEAD_FILE
#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CMap<CString, LPCTSTR, CString, CString> CMapLogFormat;
typedef CMap<CString, LPCTSTR, CDayLogger*, CDayLogger*> CMapDayLogger;

/////////////////////////////////////////////////////////////////////////////////////
//��־����
class CLoggerManager
{
	//״̬����
protected:
	bool								m_bService;							//����״̬

	//���ñ���
protected:
	TCHAR								m_szLogPath[MAX_PATH];				//��־·��

	//�������
protected:
	CLoggerThread						m_LoggerThread;						//��־�߳�

	//�������
protected:
	CMapDayLogger						m_MapDayLogger;						//��־�ֵ�
	CMapLogFormat						m_MapLogFormat;						//��ʽ�ֵ�

	//��������
public:
	//���캯��
	CLoggerManager();
	//��������
	virtual~CLoggerManager();

	//������
public:
	//��������
	bool StartService();
	//ֹͣ����
	bool ConcludeService();

	//���ú���
public:
	//����·��
	void SetLogPath(LPCTSTR pszLogPath);

	//������
public:
	//��ȡ��־
	CDayLogger& GetDayLogger(const tagTimeLogInfo& LogInfo);

	//��������
protected:
	//��־����
	CString GetLogName(const tagTimeLogInfo& LogInfo);
	//��־·��
	CString GetLogPath(const tagTimeLogInfo& LogInfo);
	//��־����
	CLogTypePtr GetLogType(const CString& strFormat, const CString& strLogType);
	//������־
	CDayLogger& MakeDayLogger(const tagTimeLogInfo& LogInfo, const CString& strLogName);

	//��������
private:
	//������־
	void CleanLoggers();
};

/////////////////////////////////////////////////////////////////////////////////////
#endif

