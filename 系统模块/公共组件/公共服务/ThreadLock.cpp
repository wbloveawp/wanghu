#include "StdAfx.h"
#include "ThreadLock.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CThreadLock::CThreadLock(CCriticalSection & CriticalSection, bool bLockAtOnce) 
	: m_CriticalSection(CriticalSection)
{
	//设置变量
	m_nLockCount=0;

	//锁定对象
	if (bLockAtOnce==true)
	{
		Lock();
	}

	return;
}

//析构函数
CThreadLock::~CThreadLock()
{
	//解除锁定
	while (m_nLockCount>0)
	{
		UnLock();
	}

	return;
}

//锁定函数
VOID CThreadLock::Lock()
{	
	//锁定对象
	m_CriticalSection.Lock();
	m_nLockCount++;

	return;
}

//解锁函数
VOID CThreadLock::UnLock()
{	
	//效验状态
	ASSERT(m_nLockCount>0);
	if (m_nLockCount==0) return;

	//解除锁定
	m_nLockCount--;

	m_CriticalSection.Unlock();

	return;
}

//////////////////////////////////////////////////////////////////////////
