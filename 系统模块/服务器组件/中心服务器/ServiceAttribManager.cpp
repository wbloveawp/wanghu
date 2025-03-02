#include "stdafx.h"
#include "ServiceAttribManager.h"

//////////////////////////////////////////////////////////////////////////
// 

//服务属性
tagServiceAttrib CServiceAttribManager::m_ServiceAttribArray[] =
{
	{SERVICE_MODULE_GATE,TEXT("网关服务器"),true,false,0,100},
	{SERVICE_MODULE_GAME,TEXT("游戏服务器"),true,true,0,INVALID_WORD},
	{SERVICE_MODULE_LOGON,TEXT("登录服务器"),true,true,MAP_KIND_HASH,100},

#ifdef CHAT_SERVICE_MODULE
	{SERVICE_MODULE_CHAT,TEXT("聊天服务器"),false,true,MAP_KIND_HASH,100},
	{SERVICE_MODULE_SYNC,TEXT("同步服务器"),false,true,MAP_KIND_HASH,100},
	{SERVICE_MODULE_QUERY,TEXT("查询服务器"),false,true,MAP_KIND_HASH,100},
#endif

#ifdef VIDEO_SERVICE_MODULE
	{SERVICE_MODULE_STORE,TEXT("存储服务器"),true,false,0,INVALID_WORD},
#endif

#ifdef MATCH_SERVICE_MODULE
	{SERVICE_MODULE_MATCH,TEXT("比赛服务器"),true,true,0,1},
#endif

#ifdef BATTLE_SERVICE_MODULE
	{SERVICE_MODULE_CLUB,TEXT("俱乐部服务"),true,true,MAP_KIND_ROUTE,100},
	{SERVICE_MODULE_BATTLE,TEXT("约战服务器"),true,true,MAP_KIND_HASH,1},
	{SERVICE_MODULE_CLUBPLAZA,TEXT("俱乐部广场"),true,true,MAP_KIND_HASH,100},
#endif

#ifdef RANKING_SERVICE_MODULE
	{SERVICE_MODULE_RANKING,TEXT("排位服务器"),true,true,MAP_KIND_HASH,1},
#endif

	{SERVICE_MODULE_LOGGER,TEXT("日志服务器"),true,false,0,1},
	{SERVICE_MODULE_MONITOR,TEXT("监控服务器"),true,false,0,100},
	{SERVICE_MODULE_MESSAGE,TEXT("消息服务器"),true,true,MAP_KIND_HASH,100},
	{SERVICE_MODULE_MASTER,TEXT("后台服务器"),true,false,0,100}
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CServiceAttribManager::CServiceAttribManager()
{
}

//析构函数
CServiceAttribManager::~CServiceAttribManager()
{
}

//枚举服务
tagServiceAttrib* CServiceAttribManager::EnumServiceAttrib(WORD wIndex)
{
	if (wIndex >= CountArray(m_ServiceAttribArray))
	{
		return NULL;
	}

	return &m_ServiceAttribArray[wIndex];
}

//获取服务
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