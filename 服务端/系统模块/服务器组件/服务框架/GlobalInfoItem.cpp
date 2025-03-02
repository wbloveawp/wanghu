#include "StdAfx.h"
#include "GlobalInfoItem.h"

//////////////////////////////////////////////////////////////////////////


//构造函数
CGlobalUserItem::CGlobalUserItem()
{
	//用户属性
	m_dwUserID=0;
	m_wGateID=0;
	m_dwTokenID=0;
	m_dwGameID=0;
	m_cbGender=0;
	m_wFaceID=0;
	m_dwCustomID=0;
	m_szNickName[0]=0;

	//用户状态
	m_cbIMUserStatus = US_IM_OFFLINE;
	m_cbCBUserStatus = US_CB_OFFLINE;

	//状态信息
	m_cbShutUpStatus = 0;
	m_dwShutUpOverTime = 0;

	return;
}

//析构函数
CGlobalUserItem::~CGlobalUserItem()
{
}

//重置数据
VOID CGlobalUserItem::ResetData()
{
	//用户属性
	m_dwUserID = 0;
	m_wGateID = 0;
	m_dwTokenID = 0;
	m_dwGameID = 0;
	m_cbGender = 0;
	m_wFaceID = 0;
	m_dwCustomID = 0;
	m_szNickName[0] = 0;

	//用户状态
	m_cbIMUserStatus = US_IM_OFFLINE;
	m_cbCBUserStatus = US_CB_OFFLINE;

	//状态信息
	m_cbShutUpStatus = 0;
	m_dwShutUpOverTime = 0;
}

//////////////////////////////////////////////////////////////////////////
// 
//构造函数
CGlobalServiceItem::CGlobalServiceItem()
{
	//服务属性
	m_dwSocketID = 0;
	m_wOnlineCount = 0;	
	ZeroMemory(&m_ServiceItem, sizeof(m_ServiceItem));
}

//析构函数
CGlobalServiceItem::~CGlobalServiceItem()
{
}

//重置数据
VOID CGlobalServiceItem::ResetData()
{
	//服务属性
	m_dwSocketID = 0;
	m_wOnlineCount = 0;	
	ZeroMemory(&m_ServiceItem, sizeof(m_ServiceItem));
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CGlobalGateItem::CGlobalGateItem()
{
	//初始化
	m_StockTokenID.InitHashTable(1013);
}

//析构函数
CGlobalGateItem::~CGlobalGateItem()
{
	//重置数据
	ResetData();
}

//添加令牌
tagTokenParameter* CGlobalGateItem::ActiveTokenID(DWORD dwTokenID)
{
	//查找令牌
	auto pTokenParameter = m_StockTokenID.SearchStockItem(dwTokenID);
	if (pTokenParameter != NULL)
	{
		return pTokenParameter;
	}

	//创建令牌
	pTokenParameter = m_StockTokenID.CreateStockItem(dwTokenID);
	if (pTokenParameter != NULL)
	{
		ZeroMemory(pTokenParameter,sizeof(tagTokenParameter));

		pTokenParameter->pBindUserItem=NULL;
		pTokenParameter->dwTokenID = dwTokenID;
		pTokenParameter->dwActiveTime = (DWORD)time(NULL);
	}

	return pTokenParameter;
}

//移除令牌
VOID CGlobalGateItem::RemoveTokenID()
{
	m_TokenIDArray.RemoveAll();
	m_StockTokenID.RemoveAllItem();
}

//移除令牌
VOID CGlobalGateItem::RemoveTokenID(DWORD dwTokenID)
{
	auto pTokenParameter = m_StockTokenID.RemoveStockItem(dwTokenID);
	if (pTokenParameter != NULL)
	{
		ZeroMemory(pTokenParameter,sizeof(tagTokenParameter));
	}
}

//查询令牌
tagTokenParameter* CGlobalGateItem::SearchTokenID(DWORD dwTokenID)
{
	return m_StockTokenID.SearchStockItem(dwTokenID);
}

//查询令牌
tagTokenParameter* CGlobalGateItem::EnumTokenID(tagTokenParameter* pTokenParameter)
{
	if (pTokenParameter == NULL)
	{
		return m_StockTokenID.GetHeadStockItem();
	}

	return m_StockTokenID.GetNextStockItem(pTokenParameter);
}

//重置数据
VOID CGlobalGateItem::ResetData()
{
	__super::ResetData();

	//移除令牌
	m_TokenIDArray.RemoveAll();
	m_StockTokenID.RemoveAllItem();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalServerItem::CGlobalServerItem()
{
	//设置变量
	m_dwSocketID = 0;
	ZeroMemory(&m_GameServer, sizeof(m_GameServer));

	return;
}

//析构函数
CGlobalServerItem::~CGlobalServerItem()
{
}

//重置数据
VOID CGlobalServerItem::ResetData()
{
	//设置变量
	m_dwSocketID = 0;
	ZeroMemory(&m_GameServer, sizeof(m_GameServer));
}

//////////////////////////////////////////////////////////////////////////
