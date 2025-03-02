#include "StdAfx.h"
#include "RobotUserItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//静态变量
CTimerItemArray CRobotUserItem::m_TimerItemStorage;					//库存数组

//////////////////////////////////////////////////////////////////////////

//构造函数
CRobotUserItem::CRobotUserItem()
{
	//状态变量
	m_bWaitLeave=false;
	m_bStartClient=false;
	m_wRobotAction=0;
	m_nCurrTimerID=0;
	m_dwPlayInnings=0;
	m_cbGameStatus=GAME_STATUS_FREE;

	//用户状态
	m_CurrentUserStatus.cbUserStatus=US_NULL;
	m_CurrentUserStatus.wChairID=INVALID_CHAIR;
	m_CurrentUserStatus.wTableID=INVALID_TABLE;

	//绑定变量
	m_dwClientVersion=0L;
	m_pIServerUserItem=NULL;
	m_pITableFrameItem=NULL;

	//时间变量
	m_dwMinSitInterval=0;
	m_dwMaxSitInterval=0;
	m_dwStandupTickCount=0;
	m_dwReposeTickCount=0;

	//索引变量
	m_wRoundID=1;
	m_wRobotIndex=INVALID_WORD_INDEX;

	//接口变量
	m_pIServerUserManager=NULL;
	m_pIRobotUserManager=NULL;
	m_pIRobotUserItemSink=NULL;

	//机器信息
	ZeroMemory(&m_RobotService,sizeof(m_RobotService));
	ZeroMemory(&m_RobotItemConfig,sizeof(m_RobotItemConfig));

	return;
}

//析构函数
CRobotUserItem::~CRobotUserItem()
{
	//删除时间
	KillGameTimer(0);

	//释放对象
	SafeRelease(m_pIRobotUserItemSink);

	return;
}

//接口查询
VOID * CRobotUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRobotUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRobotUserItem,Guid,dwQueryVer);
	return NULL;
}

//获取 I D
DWORD CRobotUserItem::GetUserID()
{
	return m_RobotItemConfig.RobotAccountsInfo.dwUserID;
}

//桌子号码
WORD CRobotUserItem::GetTableID()
{
	//效验状态
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return INVALID_TABLE;

	return m_pIServerUserItem->GetTableID();
}

//椅子号码
WORD CRobotUserItem::GetChairID()
{
	//效验状态
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return INVALID_CHAIR;

	return m_pIServerUserItem->GetChairID();
}

//用户状态
BYTE CRobotUserItem::GetUserStatus()
{
	//效验状态
	ASSERT (m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return US_NULL;

	return m_pIServerUserItem->GetUserStatus();
}

//用户名字
LPCTSTR CRobotUserItem::GetAccounts()
{
	//效验状态
	ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return NULL;

	return m_pIServerUserItem->GetAccounts();
}

//用户积分
SCORE CRobotUserItem::GetUserScore()
{
	//效验状态
	ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return 0;

	return m_pIServerUserItem->GetUserFreeScore();
}

//发送函数
bool CRobotUserItem::SendSocketData(WORD wSubCmdID)
{
	//状态效验
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//发送数据
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//发送函数
bool CRobotUserItem::SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//发送数据
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//删除时间
bool CRobotUserItem::KillGameTimer(UINT nTimerID)
{
	//删除时间
	if (nTimerID!=0)
{
		//寻找子项
		for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
	{
			//获取时间
			tagTimerItem * pTimerItem=m_TimerItemActive[i];

			//删除判断
			if (pTimerItem->nTimerID==nTimerID)
		{
				m_TimerItemActive.RemoveAt(i);
				m_TimerItemStorage.Add(pTimerItem);

				return true;
			}
		}
	}
	else
{
		m_TimerItemStorage.Append(m_TimerItemActive);
		m_TimerItemActive.RemoveAll();
	}

	return false;
}

//设置时间
bool CRobotUserItem::SetGameTimer(UINT nTimerID, UINT nElapse)
{
	//寻找子项
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
{
		//获取时间
		tagTimerItem * pTimerItem=m_TimerItemActive[i];

		//设置判断
		if (pTimerItem->nTimerID==nTimerID)
	{
			pTimerItem->nTimeLeave=nElapse;
			return true;
		}
	}

	//变量定义
	tagTimerItem * pTimerItem=NULL;
	WORD wStorageCount=(WORD)m_TimerItemStorage.GetCount();

	//库存子项
	if (wStorageCount>0)
{
		//获取对象
		pTimerItem=m_TimerItemStorage[wStorageCount-1];

		//加入数组
		m_TimerItemActive.Add(pTimerItem);
		m_TimerItemStorage.RemoveAt(wStorageCount-1);
	}

	//创建子项
	if (pTimerItem==NULL)
{
		try
	{
			//创建对象
			pTimerItem=new tagTimerItem;
			if (pTimerItem==NULL) return false;

			//加入数组
			m_TimerItemActive.Add(pTimerItem);
		}
		catch (...)
	{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置变量
	pTimerItem->nTimerID=nTimerID;
	pTimerItem->nTimeLeave=nElapse;

	return true;
}

//发送准备
bool CRobotUserItem::SendUserReady(VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT((m_pIServerUserItem!=NULL)&&(m_pIServerUserItem->GetUserStatus()==US_SIT));
	if ((m_pIServerUserItem==NULL)||(m_pIServerUserItem->GetUserStatus()!=US_SIT)) return false;

	//校验动作
	if (m_wRobotAction&(ROBOT_WAITSTANDUP|ROBOT_WAITLEAVE)) return true;

	//发送准备
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_FRAME,SUB_GF_USER_READY,pData,wDataSize);

	return true;
}

//发送聊天
bool CRobotUserItem::SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)
{
	//效验用户
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//构造信息
	CMD_GF_UserChat UserChat;
	StringCchCopy(UserChat.szChatString,CountArray(UserChat.szChatString), pszChatMessage);

	//构造数据
	UserChat.crStringColor=crFontColor;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=m_pIServerUserItem->GetUserID();
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//发送数据
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	WORD wSendSize=sizeof(UserChat)-sizeof(UserChat.szChatString)+UserChat.wChatLength*sizeof(TCHAR);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);

	return true;
}

//设置动作
void CRobotUserItem::SetRobotAction(WORD wRobotAction, bool bEnable)
{
	if (bEnable == true)
	{
		m_wRobotAction |= wRobotAction;
	}
	else
	{
		m_wRobotAction &= ~wRobotAction;
	}
}

//机器动作
bool CRobotUserItem::JudgeRobotAction(WORD wAction)
{
	return (m_wRobotAction&wAction) != 0;
}

//桌子接口
VOID CRobotUserItem::SetTableFrameItem(ITableFrameItem* pITableFrameItem)
{
	m_pITableFrameItem = pITableFrameItem;
}

//真人数目
WORD CRobotUserItem::GetTableRealUserCount()
{
	if (m_pITableFrameItem == NULL)
	{
		return 0;
	}

	return m_pITableFrameItem->GetRealUserCount();
}

//同桌用户
IServerUserItem* CRobotUserItem::GetTableUserItem(WORD wChairID)
{
	if (m_pITableFrameItem == NULL)
	{
		return NULL;
	}

	return m_pITableFrameItem->GetTableUserItem(wChairID);
}

//时间事件
bool CRobotUserItem::OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//寻找子项
	for (INT_PTR i=m_TimerItemActive.GetCount()-1;i>=0;i--)
{
		//变量定义
		tagTimerItem * pTimerItem=m_TimerItemActive[i];

		//时间处理
		if (pTimerItem->nTimeLeave<=1L)
	{
			//设置数组
			m_TimerItemActive.RemoveAt(i);
			m_TimerItemStorage.Add(pTimerItem);

			//设置变量
			m_nCurrTimerID=pTimerItem->nTimerID;

			//时间通知
			ASSERT(m_pIRobotUserItemSink!=NULL);
			if (m_pIRobotUserItemSink!=NULL) m_pIRobotUserItemSink->OnEventTimer(pTimerItem->nTimerID);
		}
		else
	{
			//设置变量
			pTimerItem->nTimeLeave--;
		}
	}

	return true;
}

//网络消息
bool CRobotUserItem::OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//登录失败
	if ((wMainCmdID==MDM_GR_LOGON)&&(wSubCmdID==SUB_GR_LOGON_FAILURE))
	{
		return OnSocketSubLogonFailure(pData,wDataSize);
	}

	//登录完成
	if ((wMainCmdID==MDM_GR_LOGON)&&(wSubCmdID==SUB_GR_LOGON_FINISH))
	{
		return OnSocketSubLogonFinish(pData,wDataSize);
	}

	//用户状态
	if ((wMainCmdID==MDM_GR_USER)&&(wSubCmdID==SUB_GR_USER_STATUS))
	{
		return OnSocketSubUserStatus(pData,wDataSize);
	}

	//请求失败
	if ((wMainCmdID==MDM_GR_USER)&&(wSubCmdID==SUB_GR_REQUEST_FAILURE))
	{
		return OnSocketSubRequestFailure(pData,wDataSize);
	}	

	//游戏消息
	if ((wMainCmdID==MDM_GF_GAME)||(wMainCmdID==MDM_GF_FRAME))
	{
		return OnSocketGameMessage(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//系统消息
	if (wMainCmdID==MDM_CM_SYSTEM && wSubCmdID==SUB_CM_SYSTEM_MESSAGE)
		return OnSocketSubSystemMessage(pData,wDataSize);

	return true;
}

//游戏消息
bool CRobotUserItem::OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//游戏处理
	if (wMainCmdID==MDM_GF_GAME)
	{
		ASSERT(m_pIRobotUserItemSink!=NULL);
		return m_pIRobotUserItemSink->OnEventGameMessage(wSubCmdID,pData,wDataSize);
	}

	//框架处理
	if (wMainCmdID==MDM_GF_FRAME)
	{
		//默认处理
		switch (wSubCmdID)
		{
		case SUB_GF_GAME_STATUS:		//游戏状态
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GF_GameStatus));
			if (wDataSize!=sizeof(CMD_GF_GameStatus)) return false;

			//消息处理
			CMD_GF_GameStatus * pGameStatus=(CMD_GF_GameStatus *)pData;

			//设置变量
			m_cbGameStatus=pGameStatus->cbGameStatus;

			return true;
		}
		case SUB_GF_GAME_SCENE:			//游戏场景
		{
			//游戏处理
			bool bLookonUser=(m_pIServerUserItem->GetUserStatus()==US_LOOKON);
			return m_pIRobotUserItemSink->OnEventSendGameScene(m_cbGameStatus,bLookonUser,pData,wDataSize);
		}
		case SUB_GF_USER_READY:			//用户准备
		{
			if (m_pIServerUserItem->GetUserStatus()>=US_READY) return true;				

			//发送开始
			SendUserReady(NULL,0);			

			return true;
		}
		default:
		{
			if (m_pIRobotUserItemSink!=NULL)
				return m_pIRobotUserItemSink->OnEventFrameMessage(wSubCmdID,pData,wDataSize);
			break;
		}
		}
	}

	return true;
}

//登录失败
bool CRobotUserItem::OnSocketSubLogonFailure(VOID * pData, WORD wDataSize)
{
	//提取数据
	CMD_GR_LogonFailure * pLogonFailure=(CMD_GR_LogonFailure *)pData;

	CTraceService::TraceString(TEXT("机器人登录失败, UserID=%d, ErrorMsg=%s, ErrorCode=%d"),TraceLevel_Info, GetUserID(), pLogonFailure->szDescribeString, pLogonFailure->lErrorCode);

	return true;
}

//登录完成
bool CRobotUserItem::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	//设置变量
	m_cbGameStatus=GAME_STATUS_FREE;
	m_pIServerUserItem=m_pIServerUserManager->SearchUserItem(GetUserID());

	//状态效验
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//用户状态
	m_CurrentUserStatus.wChairID=m_pIServerUserItem->GetChairID();
	m_CurrentUserStatus.wTableID=m_pIServerUserItem->GetTableID();
	m_CurrentUserStatus.cbUserStatus=m_pIServerUserItem->GetUserStatus();

	//规则变量
	CMD_GR_UserRule UserRule;
	ZeroMemory(&UserRule,sizeof(UserRule));

	//发送规则
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GR_USER,SUB_GR_USER_RULE,&UserRule,sizeof(UserRule));

	//设置时间
	m_dwStandupTickCount=(DWORD)time(NULL);
	m_dwReposeTickCount=m_dwMinSitInterval+(DWORD)(Random_Double*(__max(m_dwMaxSitInterval-m_dwMinSitInterval,1)));

	//重入判断
	if (m_pIServerUserItem->GetTableID()!=INVALID_TABLE)
{
		StartGameClient();
	}

	return true;
}

//用户状态
bool CRobotUserItem::OnSocketSubUserStatus(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserStatus));
	if (wDataSize<sizeof(CMD_GR_UserStatus)) return false;

	//变量定义
	CMD_GR_UserStatus * pUserStatus=(CMD_GR_UserStatus *)pData;

	//玩家判断
	if (pUserStatus->dwUserID!=m_RobotItemConfig.RobotAccountsInfo.dwUserID)
{
		return true;
	}

	//用户状态
	tagUserStatus LastUserStatus;
	CopyMemory(&LastUserStatus,&m_CurrentUserStatus,sizeof(LastUserStatus));
	CopyMemory(&m_CurrentUserStatus,&pUserStatus->UserStatus,sizeof(m_CurrentUserStatus));

	//开始切换
	if ((LastUserStatus.cbUserStatus!=US_READY)&&(m_CurrentUserStatus.cbUserStatus==US_READY))
{
	}

	//机器起立
	if(m_CurrentUserStatus.cbUserStatus==US_FREE)
{
		//设置局数
		m_dwPlayInnings=0;
		m_wRobotAction=0;

		//设置时间
		m_dwStandupTickCount=(DWORD)time(NULL);
		m_dwReposeTickCount=m_dwMinSitInterval+(DWORD)(Random_Double*(__max(m_dwMaxSitInterval-m_dwMinSitInterval,1)));
	}

	//游戏开始
	if(m_CurrentUserStatus.cbUserStatus==US_PLAYING)
	{
		m_dwPlayInnings++;
	}

	//坐下状态
	if(m_CurrentUserStatus.cbUserStatus==US_SIT)
	{
		if(m_RobotService.dwSwitchTableInnings > 0 &&
		m_RobotService.dwSwitchTableInnings<=m_dwPlayInnings)
	{
			//设置动作
			m_dwPlayInnings=0;
			m_wRobotAction|=ROBOT_WAITSTANDUP;
		}
	}

	//空闲状态
	if (m_CurrentUserStatus.cbUserStatus==US_FREE || m_CurrentUserStatus.cbUserStatus == US_NULL)
	{
		m_wRobotAction=0;
		m_pITableFrameItem = NULL;
	}

	//关闭判断
	if ((m_bStartClient==true)&&(m_CurrentUserStatus.wTableID==INVALID_TABLE))
	{
		//关闭游戏
		CloseGameClient();

		return true;
	}

	//启动判断
	if ((m_bStartClient==false)&&(m_CurrentUserStatus.wTableID!=INVALID_TABLE))
	{
		//启动游戏
		StartGameClient();

		return true;
	}

	return true;
}

//请求失败
bool CRobotUserItem::OnSocketSubRequestFailure(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GR_RequestFailure));
	if (wDataSize>sizeof(CMD_GR_RequestFailure)) return false;

	//变量定义
	CMD_GR_RequestFailure * pRequestFailure=(CMD_GR_RequestFailure *)pData;

	//游戏币不足
	if (pRequestFailure->cbFailureCode==RFC_SCORE_UNENOUGH)
{
		m_pIRobotUserManager->PostponeRobotAction(m_pIServerUserItem->GetUserID(),ROBOT_ACTION_INSURE);
	}

	return true;
}

//系统消息
bool CRobotUserItem::OnSocketSubSystemMessage(VOID * pData, WORD wDataSize)
{
	//变量定义
	CMD_CM_SystemMessage * pSystemMessage=(CMD_CM_SystemMessage *)pData;
	WORD wHeadSize=sizeof(CMD_CM_SystemMessage)-sizeof(pSystemMessage->szString);

	//效验参数
	ASSERT((wDataSize>wHeadSize)&&(wDataSize==(wHeadSize+pSystemMessage->wLength*sizeof(TCHAR))));
	if ((wDataSize<=wHeadSize)||(wDataSize!=(wHeadSize+pSystemMessage->wLength*sizeof(TCHAR)))) return false;

	//关闭处理
	if ((pSystemMessage->wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0)
{
		//关闭处理
		DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
		m_pIRobotUserManager->DeleteRobotUserItem(dwRobotID,false);
	}

	return true;
}

//启动游戏
VOID CRobotUserItem::StartGameClient()
{
	//状态判断
	ASSERT((m_bStartClient==false)&&(m_pIServerUserItem!=NULL));
	ASSERT((m_pIServerUserItem->GetTableID()!=INVALID_TABLE)&&(m_pIServerUserItem->GetChairID()!=INVALID_CHAIR));

	//设置变量
	m_bStartClient=true;

	//变量定义
	CMD_GF_GameOption GameOption;
	ZeroMemory(&GameOption,sizeof(GameOption));

	//构造数据
	//ASSERT(m_dwClientVersion!=0);
	GameOption.cbAllowLookon=FALSE;
	GameOption.dwFrameVersion=VERSION_FRAME;
	GameOption.dwClientVersion=m_dwClientVersion;

	//发送数据
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_FRAME,SUB_GF_GAME_OPTION,&GameOption,sizeof(GameOption));

	return;
}

//关闭游戏
VOID CRobotUserItem::CloseGameClient()
{
	//状态判断
	ASSERT((m_bStartClient==true)&&(m_pIServerUserItem!=NULL));
	ASSERT((m_pIServerUserItem->GetTableID()==INVALID_TABLE)&&(m_pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//删除时间
	KillGameTimer(0);

	//设置变量
	m_bStartClient=false;
	m_cbGameStatus=GAME_STATUS_FREE;

	//游戏复位
	if (m_pIRobotUserItemSink!=NULL) m_pIRobotUserItemSink->ResetUserItemSink();

	return;
}

//复位数据
VOID CRobotUserItem::RepositUserItem()
{
	//状态变量
	//状态变量
	m_bWaitLeave=false;
	m_bStartClient=false;
	m_wRobotAction=0;
	m_dwPlayInnings=0;
	m_cbGameStatus=GAME_STATUS_FREE;

	//用户状态
	m_CurrentUserStatus.cbUserStatus=US_NULL;
	m_CurrentUserStatus.wChairID=INVALID_CHAIR;
	m_CurrentUserStatus.wTableID=INVALID_TABLE;

	//时间变量
	m_dwMinSitInterval=0;
	m_dwMaxSitInterval=0;
	m_dwStandupTickCount=0;
	m_dwReposeTickCount=0;

	//状态信息
	ZeroMemory(&m_RobotService,sizeof(m_RobotService));
	ZeroMemory(&m_RobotItemConfig,sizeof(m_RobotItemConfig));

	//绑定变量
	m_pIServerUserItem=NULL;
	m_pITableFrameItem=NULL;

	//删除时间
	m_TimerItemStorage.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//索引变量
	m_wRoundID=__max(1,m_wRoundID+1);

	//复位游戏
	if (m_pIRobotUserItemSink!=NULL) m_pIRobotUserItemSink->ResetUserItemSink();

	return;
}

//////////////////////////////////////////////////////////////////////////
