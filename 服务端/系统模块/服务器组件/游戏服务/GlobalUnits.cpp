#include "StdAfx.h"
#include "GlobalUnits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//��̬����
CGlobalUnits *	CGlobalUnits::m_pGlobalUnits=NULL;						//����ָ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalUnits::CGlobalUnits()
{
	//�������
	m_bAllowWisper=true;
	m_bAllowRoomChat=true;
	m_bAllowGameChat=true;
	m_bForbidPlayChat = false;
	m_bMemberCanChat = false;

	//�������
	m_bAllowEnterRoom=true;
	m_bAllowEnterGame=true;
	m_bForbidQueryUser = false;	
	m_bCloseWhilePlayer = false;
	m_bForbidLookon = false;
	m_cbAllowLookOnChat=0;	
	m_bAllowRobotAttend=false;
	m_bAllowRobotSimulate=false;

	//���ñ���
	ASSERT(m_pGlobalUnits==NULL);
	if (m_pGlobalUnits==NULL) m_pGlobalUnits=this;

	return;
}

//��������
CGlobalUnits::~CGlobalUnits()
{
}

//////////////////////////////////////////////////////////////////////////

//ȫ�ֶ���
CGlobalUnits						g_GlobalUnits;						//ȫ�ֵ�Ԫ

//////////////////////////////////////////////////////////////////////////
