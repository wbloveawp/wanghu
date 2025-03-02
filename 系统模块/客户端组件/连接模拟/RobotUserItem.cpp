#include "StdAfx.h"
#include "RobotUserItem.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CTimerItemArray CRobotUserItem::m_TimerItemStorage;					//库存数组

//////////////////////////////////////////////////////////////////////////

//构造函数
CRobotUserItem::CRobotUserItem()
{
	//状态变量
	m_bStartClient=false;
	m_cbGameStatus=GAME_STATUS_FREE;

	//接口变量
	m_pGlobalUserData =NULL;
	m_pVirtualLinkItem = NULL;
	m_pIRobotUserItemSink=NULL;

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
	//效验状态
	ASSERT(m_pGlobalUserData != NULL);
	if (m_pGlobalUserData == NULL) return 0;

	return m_pGlobalUserData->dwUserID;
}

//桌子号码
WORD CRobotUserItem::GetTableID()
{
	//效验状态
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return INVALID_TABLE;

	return m_pGlobalUserData->wTableID;
}

//椅子号码
WORD CRobotUserItem::GetChairID()
{
	//效验状态
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return INVALID_CHAIR;

	return m_pGlobalUserData->wChairID;
}

//用户状态
BYTE CRobotUserItem::GetUserStatus()
{
	//效验状态
	ASSERT (m_pGlobalUserData != NULL);
	if (m_pGlobalUserData == NULL) return US_NULL;

	return m_pGlobalUserData->cbUserStatus;
}

//用户名字
LPCTSTR CRobotUserItem::GetAccounts()
{
	//效验状态
	ASSERT(m_pGlobalUserData != NULL);
	if (m_pGlobalUserData == NULL) return NULL;

	return m_pGlobalUserData->szAccounts;
}

//用户积分
SCORE CRobotUserItem::GetUserScore()
{
	//效验状态
	ASSERT(m_pGlobalUserData != NULL);
	if (m_pGlobalUserData == NULL) return 0;

	return m_pGlobalUserData->lGameScore;
}

//发送函数
bool CRobotUserItem::SendSocketData(WORD wSubCmdID)
{
	//状态效验
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return false;

	//校验链接
	ASSERT(m_pVirtualLinkItem != NULL);
	if (m_pVirtualLinkItem == NULL) return false;

	//发送数据
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//发送函数
bool CRobotUserItem::SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return false;

	//校验链接
	ASSERT (m_pVirtualLinkItem != NULL);
	if (m_pVirtualLinkItem == NULL) return false;

	//发送数据
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_GAME,wSubCmdID,pData,wDataSize);

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
	//校验链接
	ASSERT(m_pVirtualLinkItem != NULL);
	if (m_pVirtualLinkItem == NULL) return false;

	//状态效验
	ASSERT((m_pGlobalUserData !=NULL)&&(m_pGlobalUserData->cbUserStatus==US_SIT));
	if ((m_pGlobalUserData ==NULL)||(m_pGlobalUserData->cbUserStatus!=US_SIT)) return false;

	//发送准备
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_FRAME,SUB_GF_USER_READY,pData,wDataSize);

	return true;
}

//发送聊天
bool CRobotUserItem::SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)
{
	//效验用户
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return false;

	//校验链接
	ASSERT (m_pVirtualLinkItem != NULL);
	if (m_pVirtualLinkItem == NULL) return false;

	//构造信息
	CMD_GF_UserChat UserChat;
	StringCchCopy(UserChat.szChatString,CountArray(UserChat.szChatString), pszChatMessage);

	//构造数据
	UserChat.crStringColor=crFontColor;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID= m_pGlobalUserData->dwUserID;
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//发送数据
	WORD wSendSize=sizeof(UserChat)-sizeof(UserChat.szChatString)+UserChat.wChatLength*sizeof(TCHAR);
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);

	return true;
}

//用户数据
VOID CRobotUserItem::SetGlobalUserData(tagGlobalUserData* pGlobalUserData)
{
	m_pGlobalUserData = pGlobalUserData;
}

//虚拟链接
VOID CRobotUserItem::SetVirtualLinkItem(CVirtualLinkItem* pVirtualLinkItem)
{
	m_pVirtualLinkItem = pVirtualLinkItem;
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

//游戏消息
bool CRobotUserItem::OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//游戏处理
	if (wMainCmdID==MDM_GF_GAME)
	{
		ASSERT(m_pIRobotUserItemSink!=NULL);
		if (m_pIRobotUserItemSink != NULL)
		{
			return m_pIRobotUserItemSink->OnEventGameMessage(wSubCmdID, pData, wDataSize);
		}		
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
			bool bLookonUser=(m_pGlobalUserData->cbUserStatus==US_LOOKON);
			if (m_pIRobotUserItemSink != NULL)
			{
				return m_pIRobotUserItemSink->OnEventSendGameScene(m_cbGameStatus, bLookonUser, pData, wDataSize);
			}			

			return true;
		}
		case SUB_GF_USER_READY:			//用户准备
		{
			if (m_pGlobalUserData->cbUserStatus>=US_READY) return true;

			//发送开始
			SendUserReady(NULL,0);			

			return true;
		}
		default:
		{
			if (m_pIRobotUserItemSink != NULL)
			{
				return m_pIRobotUserItemSink->OnEventFrameMessage(wSubCmdID, pData, wDataSize);
			}
			break;
		}
		}
	}

	return true;
}

//启动游戏
VOID CRobotUserItem::StartGameClient()
{
	//状态判断
	ASSERT((m_bStartClient==false)&&(m_pGlobalUserData !=NULL));
	ASSERT((m_pGlobalUserData->wTableID!=INVALID_TABLE)&&(m_pGlobalUserData->wChairID!=INVALID_CHAIR));

	//设置变量
	m_bStartClient=true;

	//变量定义
	CMD_GF_GameOption GameOption;
	ZeroMemory(&GameOption,sizeof(GameOption));

	//构造数据
	GameOption.cbAllowLookon=FALSE;
	GameOption.dwFrameVersion=VERSION_FRAME;
	GameOption.dwClientVersion=m_dwClientVersion;

	//发送数据
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_FRAME,SUB_GF_GAME_OPTION,&GameOption,sizeof(GameOption));

	return;
}

//关闭游戏
VOID CRobotUserItem::CloseGameClient()
{
	//状态判断
	ASSERT((m_bStartClient==true)&&(m_pGlobalUserData !=NULL));
	ASSERT((m_pGlobalUserData->wTableID==INVALID_TABLE)&&(m_pGlobalUserData->wChairID==INVALID_CHAIR));

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
	m_bStartClient=false;
	m_cbGameStatus=GAME_STATUS_FREE;

	//用户状态
	if (m_pGlobalUserData != NULL)
	{
		m_pGlobalUserData->cbUserStatus = US_NULL;
		m_pGlobalUserData->wChairID = INVALID_CHAIR;
		m_pGlobalUserData->wTableID = INVALID_TABLE;
	}

	//绑定变量
	m_pGlobalUserData = NULL;
	m_pVirtualLinkItem = NULL;

	//删除时间
	m_TimerItemStorage.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//复位游戏
	if (m_pIRobotUserItemSink!=NULL) m_pIRobotUserItemSink->ResetUserItemSink();

	return;
}

//////////////////////////////////////////////////////////////////////////
