#include "StdAfx.h"
#include "ThreadLock.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CThreadLock::CThreadLock(CCriticalSection & CriticalSection, bool bLockAtOnce) 
	: m_CriticalSection(CriticalSection)
{
	//���ñ���
	m_nLockCount=0;

	//��������
	if (bLockAtOnce==true)
	{
		Lock();
	}

	return;
}

//��������
CThreadLock::~CThreadLock()
{
	//�������
	while (m_nLockCount>0)
	{
		UnLock();
	}

	return;
}

//��������
VOID CThreadLock::Lock()
{	
	//��������
	m_CriticalSection.Lock();
	m_nLockCount++;

	return;
}

//��������
VOID CThreadLock::UnLock()
{	
	//Ч��״̬
	ASSERT(m_nLockCount>0);
	if (m_nLockCount==0) return;

	//�������
	m_nLockCount--;

	m_CriticalSection.Unlock();

	return;
}

//////////////////////////////////////////////////////////////////////////
