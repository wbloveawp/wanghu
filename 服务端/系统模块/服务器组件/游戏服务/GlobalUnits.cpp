#include "StdAfx.h"
#include "GlobalUnits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//静态变量
CGlobalUnits *	CGlobalUnits::m_pGlobalUnits=NULL;						//对象指针

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalUnits::CGlobalUnits()
{
	//聊天控制
	m_bAllowWisper=true;
	m_bAllowRoomChat=true;
	m_bAllowGameChat=true;
	m_bForbidPlayChat = false;
	m_bMemberCanChat = false;

	//房间控制
	m_bAllowEnterRoom=true;
	m_bAllowEnterGame=true;
	m_bForbidQueryUser = false;	
	m_bCloseWhilePlayer = false;
	m_bForbidLookon = false;
	m_cbAllowLookOnChat=0;	
	m_bAllowRobotAttend=false;
	m_bAllowRobotSimulate=false;

	//设置变量
	ASSERT(m_pGlobalUnits==NULL);
	if (m_pGlobalUnits==NULL) m_pGlobalUnits=this;

	return;
}

//析构函数
CGlobalUnits::~CGlobalUnits()
{
}

//////////////////////////////////////////////////////////////////////////

//全局对象
CGlobalUnits						g_GlobalUnits;						//全局单元

//////////////////////////////////////////////////////////////////////////
