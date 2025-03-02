#include "StdAfx.h"
#include "GameVideoManager.h"

//////////////////////////////////////////////////////////////////////////////////
//房间掩码
#define SERVER_MASK					0x270F										

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameVideoManager::CGameVideoManager()
{
	//组件接口
	m_pIGameVideoItemSink=NULL;

	//配置变量
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;
	ZeroMemory(&m_VideoServer,sizeof(m_VideoServer));

	return;
}

//析构函数
CGameVideoManager::~CGameVideoManager()
{	
	//释放对象
	for (INT_PTR i=0;i<m_GameVideoItemActive.GetCount();i++)
	{
		m_GameVideoItemActive[i]->Release();
	}

	//释放对象
	for (INT_PTR i=0;i<m_GameVideoItemBuffer.GetCount();i++)
	{
		m_GameVideoItemBuffer[i]->Release();
	}

	//删除数据
	m_GameVideoItemBuffer.RemoveAll();
	m_GameVideoItemActive.RemoveAll();

	return;
}

//接口查询
VOID * CGameVideoManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameVideoManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameVideoManager,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool CGameVideoManager::SetGameVideoItemSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameVideoItemSink)!=NULL);
		m_pIGameVideoItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameVideoItemSink);

		//成功判断
		if (m_pIGameVideoItemSink==NULL) return false;
	}
	else m_pIGameVideoItemSink=NULL;

	return true;
}

//配置组件
void CGameVideoManager::InitVideoManager(tagGameVideoParameter & GameVideoParameter)
{
	//配置变量
	m_pGameServiceOption=GameVideoParameter.pGameServiceOption;
	m_pGameServiceAttrib=GameVideoParameter.pGameServiceAttrib;

	//类型信息
	m_VideoServer.wKindID=m_pGameServiceAttrib->wKindID;
	m_VideoServer.wChairCount=m_pGameServiceAttrib->wChairCount;
	StringCchCopy(m_VideoServer.szProcessName,CountArray(m_VideoServer.szProcessName), m_pGameServiceAttrib->szClientEXEName);

	//房间信息
	m_VideoServer.wServerID=m_pGameServiceOption->wServerID;
	m_VideoServer.wServerType=m_pGameServiceOption->wServerType;
	m_VideoServer.dwServerRule=m_pGameServiceOption->dwServerRule;
	StringCchCopy(m_VideoServer.szServerName,CountArray(m_VideoServer.szServerName), m_pGameServiceOption->szServerName);
}

//删除用户
bool CGameVideoManager::DeleteVideoItem(IGameVideoItem * pIGameVideoItem)
{
	//参数校验
	//ASSERT (pIGameVideoItem!=NULL);
	if (pIGameVideoItem==NULL) return false;

	//寻找对象
	for (INT_PTR i=0;i<m_GameVideoItemActive.GetCount();i++)
	{
		//获取用户
		ASSERT(m_GameVideoItemActive[i]!=NULL);
		CGameVideoItem * pGameVideoItem=m_GameVideoItemActive[i];

		//用户判断
		if (pGameVideoItem==pIGameVideoItem)
		{
			//删除对象
			m_GameVideoItemActive.RemoveAt(i);
			m_GameVideoItemBuffer.Add(pGameVideoItem);

			//重置对象
			pGameVideoItem->ResetVideoItem();

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//激活录像
IGameVideoItem * CGameVideoManager::ActiveVideoItem(WORD wTableID)
{
	//变量定义
	CGameVideoItem * pGameVideoItem=NULL;

	//获取指针
	if (m_GameVideoItemBuffer.GetCount()==0L)
	{
		try
		{
			//创建对象
			pGameVideoItem=new CGameVideoItem;
			if (pGameVideoItem==NULL) throw 0;			
		}
		catch (...)
		{
			//错误判断
			ASSERT(FALSE);
			return NULL;
		}
	}
	else
	{
		//缓存对象
		INT_PTR nItemPostion=m_GameVideoItemBuffer.GetCount()-1L;
		pGameVideoItem=m_GameVideoItemBuffer[nItemPostion];
		m_GameVideoItemBuffer.RemoveAt(nItemPostion);
	}

	//设置对象	
	pGameVideoItem->m_wTableID=wTableID;
	pGameVideoItem->m_pVideoServer=&m_VideoServer;
	pGameVideoItem->m_pIGameVideoItemSink=m_pIGameVideoItemSink;

	//录像编号
	BuildVideoNumber(pGameVideoItem->m_szVideoNumber,CountArray(pGameVideoItem->m_szVideoNumber));

	//添加对象
	m_GameVideoItemActive.Add(pGameVideoItem);

	return pGameVideoItem;
}

//录像编号
VOID CGameVideoManager::BuildVideoNumber(WCHAR szBuffer[], WORD wBufferLen)
{
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//单位换算
	DWORD dwSecond=SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;

	//静态变量
	static WORD wVideoCount=0;
	static DWORD dwLastSecond=dwSecond;	

	//更新变量
	if (dwSecond!=dwLastSecond)
	{
		wVideoCount=0;
		dwLastSecond=dwSecond;
	}

	//房间标识
	WORD wServerID=m_pGameServiceOption?m_pGameServiceOption->wServerID:0;

	//格式化编号
	_snwprintf_s(szBuffer,wBufferLen,wBufferLen,TEXT("%05d%04d%02d%02d%05d%02d"),wServerID^SERVER_MASK,SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,dwSecond,++wVideoCount);
}

//////////////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(GameVideoManager);

//////////////////////////////////////////////////////////////////////////////////
