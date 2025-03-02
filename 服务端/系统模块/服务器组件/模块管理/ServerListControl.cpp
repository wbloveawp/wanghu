#include "StdAfx.h"
#include "ServerListControl.h"
#include "DataBaseListManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerListControl::CServerListControl()
{
}

//析构函数
CServerListControl::~CServerListControl()
{
}

//配置列表
VOID CServerListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("房间名字"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("游戏名字"),LVCFMT_CENTER,80);
	InsertColumn(nColIndex++,TEXT("房间类型"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("桌子数目"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("监听端口"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("房间标识"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("类型标识"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("数据库名"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("服务器名"),LVCFMT_LEFT,140);
	InsertColumn(nColIndex++,TEXT("客户端名"),LVCFMT_LEFT,120);

	return;
}

//子项排序
INT CServerListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagGameServerInfo * pGameServerInfo1=(tagGameServerInfo *)lParam1;
	tagGameServerInfo * pGameServerInfo2=(tagGameServerInfo *)lParam2;

	//安装排序
	bool bInstall1=(pGameServerInfo1->dwNativeVersion!=0L);
	bool bInstall2=(pGameServerInfo2->dwNativeVersion!=0L);
	if (bInstall1!=bInstall2) return (bInstall1==true)?1:-1;

	//子项排序
	switch (wColumnIndex)
	{
	case 0:		//房间名字
		{
			return lstrcmp(pGameServerInfo1->szServerName,pGameServerInfo2->szServerName);
		}
	case 1:		//游戏名字
		{
			return lstrcmp(pGameServerInfo1->szModuleName,pGameServerInfo2->szModuleName);
		}
	case 2:		//房间类型
		{
			return (pGameServerInfo1->wServerType>pGameServerInfo2->wServerType)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 3:		//桌子数目
		{
			return (pGameServerInfo1->wTableCount>pGameServerInfo2->wTableCount)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 4:		//监听端口
		{
			return (pGameServerInfo1->wServerPort>pGameServerInfo2->wServerPort)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 5:		//房间标识
		{
			return (pGameServerInfo1->wServerID>pGameServerInfo2->wServerID)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 6:		//类型标识
		{
			//房间标识
			WORD wKindID1=pGameServerInfo1->wKindID;
			if (wKindID1==0) wKindID1=pGameServerInfo1->wModuleID;

			//房间标识
			WORD wKindID2=pGameServerInfo2->wKindID;
			if (wKindID2==0) wKindID2=pGameServerInfo2->wModuleID;

			return (wKindID1>wKindID2)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 7:		//数据库名
		{
			return (pGameServerInfo1->wDBListID>pGameServerInfo2->wDBListID)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 8:		//服务器名
		{
			return lstrcmp(pGameServerInfo1->szServerDLLName,pGameServerInfo2->szServerDLLName);
		}
	case 9:		//客户端名
		{
			return lstrcmp(pGameServerInfo1->szClientEXEName,pGameServerInfo2->szClientEXEName);
		}
	}

	return 0;
}

//获取颜色
VOID CServerListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);
	tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)lItemParam;

	//没有安装
	if (pGameServerInfo->dwNativeVersion==0L)
	{
		//设置颜色
		ListItemColor.rcTextColor=RGB(125,125,125);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(50,50,50):CR_NORMAL_BK;

		return;
	}

	//存在更新
	if (pGameServerInfo->dwNativeVersion!=pGameServerInfo->dwModuleVersion)
	{
		//设置颜色
		ListItemColor.rcTextColor=RGB(128,0,0);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(230,230,0):CR_NORMAL_BK;

		return;
	}

	//版本一致
	if (pGameServerInfo->dwNativeVersion==pGameServerInfo->dwModuleVersion)
	{
		//设置颜色
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
		ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

		return;
	}

	return;
}

//插入列表
bool CServerListControl::InsertServerInfo(tagGameServerInfo * pGameServerInfo)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameServerInfo;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pGameServerInfo);
			LPCTSTR pszDescribe=GetDescribeString(pGameServerInfo,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameServerInfo);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameServerInfo,i),0,0,0,0);
		}
	}

	return true;
}

//插入索引
WORD CServerListControl::GetInsertIndex(tagGameServerInfo * pGameServerInfo)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagGameServerInfo * pGameServerTemp=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pGameServerTemp=(tagGameServerInfo *)GetItemData(i);

		//安装判断
		if ((pGameServerInfo->dwNativeVersion==0)&&(pGameServerTemp->dwNativeVersion!=0))
		{
			continue;
		}
		
		//游戏名字
		if (lstrcmp(pGameServerInfo->szModuleName,pGameServerTemp->szModuleName)<0)
		{
			return i;
		}

		//房间名字
		if (lstrcmp(pGameServerInfo->szServerName,pGameServerTemp->szServerName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//描述字符
LPCTSTR CServerListControl::GetDescribeString(tagGameServerInfo * pGameServerInfo, WORD wColumnIndex)
{
	//变量定义
	static TCHAR szDescribe[128]=TEXT("");

	//构造字符
	switch (wColumnIndex)
	{
	case 0:		//房间名字
		{
			return pGameServerInfo->szServerName;
		}
	case 1:		//游戏名字
		{
			return pGameServerInfo->szModuleName;
		}
	case 2:		//房间类型
		{
			//财富类型
			if (pGameServerInfo->wServerType&GAME_GENRE_GOLD)
			{
				StringCchCopy(szDescribe,CountArray(szDescribe), TEXT("财富类型"));
				return szDescribe;
			}

			//积分类型
			if (pGameServerInfo->wServerType&GAME_GENRE_SCORE)
			{
				StringCchCopy(szDescribe,CountArray(szDescribe), TEXT("积分类型"));
				return szDescribe;
			}

			//比赛类型
			if (pGameServerInfo->wServerType&GAME_GENRE_MATCH)
			{
				StringCchCopy(szDescribe,CountArray(szDescribe), TEXT("比赛类型"));
				return szDescribe;
			}

			//约战类型
			if (pGameServerInfo->wServerType&GAME_GENRE_BATTLE)
			{
				StringCchCopy(szDescribe,CountArray(szDescribe), TEXT("约战类型"));
				return szDescribe;
			}

			//排位类型
			if (pGameServerInfo->wServerType & GAME_GENRE_RANKING)
			{
				StringCchCopy(szDescribe, CountArray(szDescribe), TEXT("排位类型"));
				return szDescribe;
			}

			return TEXT("未知类型");
		}
	case 3:		//桌子数目
		{
			_itot_s(pGameServerInfo->wTableCount, szDescribe, 10);			
			return szDescribe;
		}
	case 4:		//监听端口
		{
			_itot_s(pGameServerInfo->wServerPort, szDescribe, 10);
			return szDescribe;
		}
	case 5:		//房间标识
		{
			_itot_s(pGameServerInfo->wServerID, szDescribe, 10);
			return szDescribe;
		}
	case 6:		//类型标识
		{
			_itot_s((pGameServerInfo->wKindID == 0) ? pGameServerInfo->wModuleID : pGameServerInfo->wKindID, szDescribe, 10);
			return szDescribe;
		}
	case 7:		//数据库名
		{
			CDatabaseListManager *pDBListMgr = CDatabaseListManager::GetInstance();
			if( pDBListMgr == NULL ) return TEXT("NULL");

			tagDataBaseList *pDBList = pDBListMgr->SearchItem(pGameServerInfo->wDBListID);
			if( pDBList == NULL ) return TEXT("NULL");

			static TCHAR szDBName[128] = {0};
			szDBName[0] = 0;
			_sntprintf_s( szDBName,CountArray(szDBName),TEXT("%s(%s)"),pDBList->szDBAliasName,pDBList->szDBName );
			return szDBName;
		}
	case 8:		//服务器名
		{
			return pGameServerInfo->szServerDLLName;
		}
	case 9:	//客户端名
		{
			return pGameServerInfo->szClientEXEName;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
