#ifndef THREAD_LOCK_HEAD_FILE
#define THREAD_LOCK_HEAD_FILE

#pragma once

#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//��Դ����
class COMMON_SERVICE_CLASS CThreadLock
{
	//��������
private:
	INT								m_nLockCount;					//��������
	CCriticalSection &				m_CriticalSection;				//��������

	//��������
public:
	//���캯��
	CThreadLock(CCriticalSection & CriticalSection, bool bLockAtOnce=true);
	//��������
	virtual ~CThreadLock();

	//��������
public:
	//��������
	VOID Lock();
	//�������� 
	VOID UnLock();

	//״̬����
public:
	//��������
	inline INT GetLockCount() { return m_nLockCount; }
};

//////////////////////////////////////////////////////////////////////////

#endif