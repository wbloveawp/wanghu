#include "stdafx.h"
#include "ServiceAttribManager.h"

//////////////////////////////////////////////////////////////////////////
// 

//��������
tagServiceAttrib CServiceAttribManager::m_ServiceAttribArray[] =
{
	{SERVICE_MODULE_GATE,TEXT("���ط�����"),true,false,0,100},
	{SERVICE_MODULE_GAME,TEXT("��Ϸ������"),true,true,0,INVALID_WORD},
	{SERVICE_MODULE_LOGON,TEXT("��¼������"),true,true,MAP_KIND_HASH,100},

#ifdef CHAT_SERVICE_MODULE
	{SERVICE_MODULE_CHAT,TEXT("���������"),false,true,MAP_KIND_HASH,100},
	{SERVICE_MODULE_SYNC,TEXT("ͬ��������"),false,true,MAP_KIND_HASH,100},
	{SERVICE_MODULE_QUERY,TEXT("��ѯ������"),false,true,MAP_KIND_HASH,100},
#endif

#ifdef VIDEO_SERVICE_MODULE
	{SERVICE_MODULE_STORE,TEXT("�洢������"),true,false,0,INVALID_WORD},
#endif

#ifdef MATCH_SERVICE_MODULE
	{SERVICE_MODULE_MATCH,TEXT("����������"),true,true,0,1},
#endif

#ifdef BATTLE_SERVICE_MODULE
	{SERVICE_MODULE_CLUB,TEXT("���ֲ�����"),true,true,MAP_KIND_ROUTE,100},
	{SERVICE_MODULE_BATTLE,TEXT("Լս������"),true,true,MAP_KIND_HASH,1},
	{SERVICE_MODULE_CLUBPLAZA,TEXT("���ֲ��㳡"),true,true,MAP_KIND_HASH,100},
#endif

#ifdef RANKING_SERVICE_MODULE
	{SERVICE_MODULE_RANKING,TEXT("��λ������"),true,true,MAP_KIND_HASH,1},
#endif

	{SERVICE_MODULE_LOGGER,TEXT("��־������"),true,false,0,1},
	{SERVICE_MODULE_MONITOR,TEXT("��ط�����"),true,false,0,100},
	{SERVICE_MODULE_MESSAGE,TEXT("��Ϣ������"),true,true,MAP_KIND_HASH,100},
	{SERVICE_MODULE_MASTER,TEXT("��̨������"),true,false,0,100}
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CServiceAttribManager::CServiceAttribManager()
{
}

//��������
CServiceAttribManager::~CServiceAttribManager()
{
}

//ö�ٷ���
tagServiceAttrib* CServiceAttribManager::EnumServiceAttrib(WORD wIndex)
{
	if (wIndex >= CountArray(m_ServiceAttribArray))
	{
		return NULL;
	}

	return &m_ServiceAttribArray[wIndex];
}

//��ȡ����
tagServiceAttrib* CServiceAttribManager::SearchServiceAttrib(WORD wServiceModule)
{
	for (int i = 0; i < CountArray(m_ServiceAttribArray); i++)
	{
		if (m_ServiceAttribArray[i].wServiceModule == wServiceModule)
		{
			return &m_ServiceAttribArray[i];
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////