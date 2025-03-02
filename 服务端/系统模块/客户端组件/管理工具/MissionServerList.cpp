#include "StdAfx.h"
#include "MissionServerList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//激活任务
#define IDI_AVTIVE_MISSION				100								//激活任务
#define TIME_AVTIVE_MISSION				600000L							//激活任务

//////////////////////////////////////////////////////////////////////////
//静态变量

CMissionList * CMissionList::m_pMissionServerList=NULL;		//对象指针

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMissionList, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CMissionList::CMissionList()
{
	//设置变量
	m_bOnLineMission=false;

	//设置对象
	ASSERT(m_pMissionServerList==NULL);
	if (m_pMissionServerList==NULL) m_pMissionServerList=this;

	return;
}

//析构函数
CMissionList::~CMissionList()
{
	//释放对象
	ASSERT(m_pMissionServerList==this);
	if (m_pMissionServerList==this) m_pMissionServerList=NULL;

	return;
}

//连接事件
bool CMissionList::OnEventMissionLink(INT nErrorCode)
{
	////错误判断
	//if (nErrorCode!=0L)
	//{
	//	//状态效验
	//	if ((m_GameKindIDMission.GetCount()==0)&&(m_bOnLineMission==false))
	//	{
	//		ASSERT(FALSE);
	//		return false;
	//	}

	//	//创建窗口
	//	if (m_hWnd==NULL)
	//	{
	//		CRect rcCreate(0,0,0,0);
	//		Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
	//	}

	//	//设置时间
	//	SetTimer(IDI_AVTIVE_MISSION,TIME_AVTIVE_MISSION,NULL);

	//	return true;
	//}

	//////更新人数
	////if (m_bOnLineMission==true)
	////{
	////	//设置变量
	////	m_bOnLineMission=false;

	////	//变量定义
	////	CMD_GP_GetOnline GetOnline;
	////	ZeroMemory(&GetOnline,sizeof(GetOnline));

	////	//变量定义
	////	POSITION Position=NULL;
	////	CMissionManager * pMissionManager=GetMissionManager();
	////	CServerListManager * pServerListManager=CServerListManager::GetInstance();

	////	do
	////	{
	////		//获取房间
	////		CGameServerItem * pGameServerItem=pServerListManager->EmunGameServerItem(Position);

	////		//溢出判断
	////		if (GetOnline.wServerCount>=CountArray(GetOnline.wOnLineServerID))
	////		{
	////			ASSERT(FALSE);
	////			break;
	////		}

	////		//设置房间
	////		if (pGameServerItem!=NULL)
	////		{
	////			tagPCGameServer * pGameServer=&pGameServerItem->m_GameServer;
	////			GetOnline.wOnLineServerID[GetOnline.wServerCount++]=pGameServer->wServerID;
	////		}

	////	} while (Position!=NULL);

	////	//发送数据
	////	WORD wHeadSize=sizeof(GetOnline)-sizeof(GetOnline.wOnLineServerID);
	////	WORD wSendSize=wHeadSize+sizeof(GetOnline.wOnLineServerID[0])*GetOnline.wServerCount;
	////	pMissionManager->SendData(MAKECMMD(MDM_GP_SERVER_LIST, SERVICE_MODULE_LOGON),SUB_GP_GET_ONLINE,&GetOnline,wSendSize);
	////}

	//////列表任务
	////if (m_GameKindIDMission.GetCount()>0L)
	////{
	////	//变量定义
	////	WORD wKindIDItem[MAX_KIND];
	////	WORD wKindIDCount=__min(CountArray(wKindIDItem),m_GameKindIDMission.GetCount());

	////	//变量定义
	////	WORD wKindIDKey=0;
	////	POSITION KindIDPos=m_GameKindIDMission.GetStartPosition();

	////	//构造数据
	////	for (WORD i=0;i<wKindIDCount;i++) 
	////	{
	////		m_GameKindIDMission.GetNextAssoc(KindIDPos,wKindIDKey,wKindIDItem[i]);
	////		m_GameKindIDPending[wKindIDItem[i]]=wKindIDItem[i];
	////	}

	////	//删除任务
	////	m_GameKindIDMission.RemoveAll();

	////	//发送数据
	////	CMissionManager * pMissionManager=GetMissionManager();
	////	pMissionManager->SendData(MAKECMMD(MDM_GP_SERVER_LIST, SERVICE_MODULE_LOGON),SUB_GP_GET_SERVER,wKindIDItem,wKindIDCount*sizeof(WORD));
	////}

	//CMD_GA_C_LinkService linkService;
	//ZeroMemory(&linkService, sizeof(linkService));

	//linkService.dwStationID = 1000;
	//linkService.wServiceID = INVALID_WORD;
	//linkService.wServiceModule = SERVICE_MODULE_LOGON;

	////发送数据
	//CMissionManager* pMissionManager = GetMissionManager();
	//pMissionManager->SendData(MAKECMMD(MDM_GA_ACCESS_SERVICE, SERVICE_MODULE_ACCESS), SUB_GA_C_LINK_SERVICE, &linkService, sizeof(linkService));

	return true;
}

//关闭事件
bool CMissionList::OnEventMissionShut(BYTE cbShutReason)
{
	////列表任务
	//if (m_GameKindIDPending.GetCount()>0)
	//{
	//	//变量定义
	//	WORD wKindIDKey=0,wKindID=0;
	//	POSITION Position=m_GameKindIDPending.GetStartPosition();

	//	//储备任务
	//	while (Position!=NULL)
	//	{
	//		//枚举任务
	//		m_GameKindIDPending.GetNextAssoc(Position,wKindIDKey,wKindID);

	//		//加入任务
	//		m_GameKindIDMission[wKindIDKey]=wKindID;
	//	}

	//	//删除任务
	//	m_GameKindIDPending.RemoveAll();
	//}

	////继续任务
	//if ((m_GameKindIDMission.GetCount()>0)||(m_bOnLineMission==true))
	//{
	//	//创建窗口
	//	if (m_hWnd==NULL)
	//	{
	//		CRect rcCreate(0,0,0,0);
	//		Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
	//	}

	//	//设置时间
	//	SetTimer(IDI_AVTIVE_MISSION,TIME_AVTIVE_MISSION,NULL);
	//}

	return true;
}

//读取事件
bool CMissionList::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//WORD wCmd = LOCMMD(Command.wMainCmdID);
	//if (wCmd==MDM_GP_SERVER_LIST)
	//{
	//	switch (Command.wSubCmdID)
	//	{
	//	case SUB_GP_LIST_TYPE:		//种类信息
	//		{
	//			//效验参数
	//			ASSERT(wDataSize%sizeof(tagGameType)==0);
	//			if (wDataSize%sizeof(tagGameType)!=0) return false;

	//			//变量定义
	//			WORD wItemCount=wDataSize/sizeof(tagGameType);
	//			tagGameType * pGameType=(tagGameType *)(pData);

	//			//获取对象
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//更新数据
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameType(pGameType++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_KIND:		//类型信息
	//		{
	//			//效验参数
	//			ASSERT(wDataSize%sizeof(tagGameKind)==0);
	//			if (wDataSize%sizeof(tagGameKind)!=0) return false;

	//			//变量定义
	//			WORD wItemCount=wDataSize/sizeof(tagGameKind);
	//			tagGameKind * pGameKind=(tagGameKind *)(pData);

	//			//获取对象
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//更新数据
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameKind(pGameKind++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_NODE:		//节点信息
	//		{
	//			//效验参数
	//			ASSERT(wDataSize%sizeof(tagGameNode)==0);
	//			if (wDataSize%sizeof(tagGameNode)!=0) return false;

	//			//变量定义
	//			WORD wItemCount=wDataSize/sizeof(tagGameNode);
	//			tagGameNode * pGameNode=(tagGameNode *)(pData);

	//			//获取对象
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//更新数据
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameNode(pGameNode++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_SERVER:	//房间信息
	//		{
	//			//效验参数
	//			ASSERT(wDataSize%sizeof(tagPCGameServer)==0);
	//			if (wDataSize%sizeof(tagPCGameServer)!=0) return false;

	//			//变量定义
	//			WORD wItemCount=wDataSize/sizeof(tagPCGameServer);
	//			tagPCGameServer * pGameServer=(tagPCGameServer *)(pData);

	//			//获取对象
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//更新数据
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameServer(pGameServer++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_CUSTOM:	//定制信息
	//		{
	//			//效验参数
	//			ASSERT(wDataSize%sizeof(tagGameCustom)==0);
	//			if (wDataSize%sizeof(tagGameCustom)!=0) return false;

	//			//变量定义
	//			WORD wItemCount=wDataSize/sizeof(tagGameCustom);
	//			tagGameCustom * pGameCustom=(tagGameCustom *)(pData);

	//			//获取对象
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//更新数据
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameCustom(pGameCustom++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_FINISH:	//列表信息
	//		{
	//			//列表完成
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			if (CServerListManager::GetInstance()!=NULL) CServerListManager::GetInstance()->OnEventListFinish();

	//			//更新人数
	//			CServerListManager::GetInstance()->UpdateOnlineCount();

	//			return true;
	//		}
	//	case SUB_GP_SERVER_FINISH:	//房间完成
	//		{
	//			//效验数据
	//			ASSERT(wDataSize%sizeof(WORD)==0);
	//			if (wDataSize%sizeof(WORD)!=0) return false;

	//			//获取对象
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//任务处理
	//			for (UINT i=0;i<(wDataSize/sizeof(WORD));i++)
	//			{
	//				m_GameKindIDPending.RemoveKey(((WORD *)pData)[i]);
	//				pServerListManager->OnEventKindFinish(((WORD *)pData)[i]);
	//			}

	//			//终止任务
	//			if ((m_GameKindIDPending.GetCount()==0L)&&(m_bOnLineMission==false))
	//			{
	//				CMissionManager * pMissionManager=GetMissionManager();
	//				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this);
	//			}

	//			return true;
	//		}
	//	case SUB_GR_KINE_ONLINE:	//类型在线
	//		{
	//			//变量定义
	//			CMD_GP_KindOnline * pKindOnline=(CMD_GP_KindOnline *)pData;
	//			WORD wHeadSize=(sizeof(CMD_GP_KindOnline)-sizeof(pKindOnline->OnLineInfoKind));

	//			//效验数据
	//			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pKindOnline->wKindCount*sizeof(tagOnLineInfoKind))));
	//			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pKindOnline->wKindCount*sizeof(tagOnLineInfoKind)))) return false;

	//			//获取对象
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//设置人数
	//			for (WORD i=0;i<pKindOnline->wKindCount;i++)
	//			{
	//				tagOnLineInfoKind * pOnLineInfoKind=&pKindOnline->OnLineInfoKind[i];
	//				pServerListManager->SetKindOnLineCount(pOnLineInfoKind->wKindID,pOnLineInfoKind->dwOnLineCount);
	//			}

	//			//更新人数
	//			pServerListManager->UpdateOnlineCount();

	//			return true;
	//		}
	//	case SUB_GR_SERVER_ONLINE:	//房间在线
	//		{
	//			//变量定义
	//			CMD_GP_ServerOnline * pServerOnline=(CMD_GP_ServerOnline *)pData;
	//			WORD wHeadSize=(sizeof(CMD_GP_ServerOnline)-sizeof(pServerOnline->OnLineInfoServer));

	//			//效验数据
	//			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pServerOnline->wServerCount*sizeof(tagOnLineInfoServer))));
	//			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pServerOnline->wServerCount*sizeof(tagOnLineInfoServer)))) return false;

	//			//获取对象
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//设置人数
	//			for (WORD i=0;i<pServerOnline->wServerCount;i++)
	//			{
	//				tagOnLineInfoServer * pOnLineInfoServer=&pServerOnline->OnLineInfoServer[i];
	//				pServerListManager->SetServerOnLineCount(pOnLineInfoServer->wServerID,pOnLineInfoServer->dwOnLineCount);
	//			}

	//			//终止任务
	//			if ((m_GameKindIDPending.GetCount()==0L)&&(m_bOnLineMission==false))
	//			{
	//				CMissionManager * pMissionManager=GetMissionManager();
	//				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this);
	//			}

	//			return true;
	//		}
	//	}
	//}
	//else if (wCmd == MDM_GA_ACCESS_SERVICE)
	//{
	//	if (Command.wSubCmdID == SUB_GA_S_LINK_SUCCESS)
	//	{
	//		//更新人数
	//		if (m_bOnLineMission == true)
	//		{
	//			//设置变量
	//			m_bOnLineMission = false;

	//			//变量定义
	//			CMD_GP_GetOnline GetOnline;
	//			ZeroMemory(&GetOnline, sizeof(GetOnline));

	//			//变量定义
	//			POSITION Position = NULL;
	//			CMissionManager* pMissionManager = GetMissionManager();
	//			CServerListManager* pServerListManager = CServerListManager::GetInstance();

	//			do
	//			{
	//				//获取房间
	//				CGameServerItem* pGameServerItem = pServerListManager->EmunGameServerItem(Position);

	//				//溢出判断
	//				if (GetOnline.wServerCount >= CountArray(GetOnline.wOnLineServerID))
	//				{
	//					ASSERT(FALSE);
	//					break;
	//				}

	//				//设置房间
	//				if (pGameServerItem != NULL)
	//				{
	//					tagPCGameServer* pGameServer = &pGameServerItem->m_GameServer;
	//					GetOnline.wOnLineServerID[GetOnline.wServerCount++] = pGameServer->wServerID;
	//				}

	//			} while (Position != NULL);

	//			//发送数据
	//			WORD wHeadSize = sizeof(GetOnline) - sizeof(GetOnline.wOnLineServerID);
	//			WORD wSendSize = wHeadSize + sizeof(GetOnline.wOnLineServerID[0]) * GetOnline.wServerCount;
	//			pMissionManager->SendData(MAKECMMD(MDM_GP_SERVER_LIST, SERVICE_MODULE_LOGON), SUB_GP_GET_ONLINE, &GetOnline, wSendSize);
	//		}

	//		//列表任务
	//		if (m_GameKindIDMission.GetCount() > 0L)
	//		{
	//			//变量定义
	//			WORD wKindIDItem[MAX_KIND];
	//			WORD wKindIDCount = __min(CountArray(wKindIDItem), m_GameKindIDMission.GetCount());

	//			//变量定义
	//			WORD wKindIDKey = 0;
	//			POSITION KindIDPos = m_GameKindIDMission.GetStartPosition();

	//			//构造数据
	//			for (WORD i = 0; i < wKindIDCount; i++)
	//			{
	//				m_GameKindIDMission.GetNextAssoc(KindIDPos, wKindIDKey, wKindIDItem[i]);
	//				m_GameKindIDPending[wKindIDItem[i]] = wKindIDItem[i];
	//			}

	//			//删除任务
	//			m_GameKindIDMission.RemoveAll();

	//			//发送数据
	//			CMissionManager* pMissionManager = GetMissionManager();
	//			pMissionManager->SendData(MAKECMMD(MDM_GP_SERVER_LIST, SERVICE_MODULE_LOGON), SUB_GP_GET_SERVER, wKindIDItem, wKindIDCount * sizeof(WORD));
	//		}
	//	}
	//}

	return true;
}

//更新人数
VOID CMissionList::UpdateOnLineInfo()
{
	//效验状态
	if (m_bOnLineMission==true) return;

	//设置变量
	m_bOnLineMission=true;

	//关闭窗口
	if (m_hWnd!=NULL) DestroyWindow();

	//激活连接
	ASSERT(GetMissionManager());
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	return;
}

//更新类型
VOID CMissionList::UpdateServerInfo(WORD wKindID)
{
	//关闭窗口
	if (m_hWnd!=NULL) DestroyWindow();

	//设置变量
	m_GameKindIDMission[wKindID]=wKindID;

	//激活连接
	ASSERT(GetMissionManager());
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);
	
	return;
}

//加载服务
VOID CMissionList::LoadServerInfo()
{
	//激活连接
	ASSERT(GetMissionManager());
	CMissionManager* pMissionManager = GetMissionManager();
	if (pMissionManager != NULL) pMissionManager->AvtiveMissionItem(this, false);

	return;
}

//时间消息
VOID CMissionList::OnTimer(UINT_PTR nIDEvent)
{
	//激活任务
	if (nIDEvent==IDI_AVTIVE_MISSION)
	{
		//关闭窗口
		DestroyWindow();

		//激活任务
		if (m_GameKindIDMission.GetCount()>0L)
		{
			ASSERT(GetMissionManager());
			CMissionManager * pMissionManager=GetMissionManager();
			if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);
		}

		return;
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
