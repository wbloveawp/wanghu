#include "StdAfx.h"
#include "DataBaseListManager.h"
#include "ModuleDBParameter.h"

//////////////////////////////////////////////////////////////////////////

bool CDatabaseListManager::m_bLoad = false;
CDatabaseListManager * CDatabaseListManager::m_spDBListMgr = NULL;

//////////////////////////////////////////////////////////////////////////

//构造函数
CDatabaseListManager::CDatabaseListManager()
{
	if( m_spDBListMgr == NULL )
	{
		m_spDBListMgr = this;
	}
}

//析构函数
CDatabaseListManager::~CDatabaseListManager()
{
	if( m_spDBListMgr == this )
	{
		m_spDBListMgr = NULL;
	}

	//变量定义
	WORD wDBListID=0;
	tagDataBaseList * pItem=NULL;
	POSITION Position=m_ListMap.GetStartPosition();

	//删除索引
	while (Position!=NULL)
	{
		m_ListMap.GetNextAssoc(Position,wDBListID,pItem);
		SafeDelete(pItem);
	}

	//删除数组
	for (INT_PTR i=0;i<m_ListArray.GetCount();i++)
	{
		pItem=m_ListArray[i];
		SafeDelete(pItem);
	}

	//删除引用
	m_ListMap.RemoveAll();
	m_ListArray.RemoveAll();

	return;
}

//加载数据库
bool CDatabaseListManager::Load()
{
	//加载判断
	if( !m_bLoad )
	{
		//设置标志
		m_bLoad = true;

		//变量定义
		CDataBaseAide PlatformDBAide;
		CDataBaseHelper PlatformDBModule;

		//创建对象
		if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
		{
			AfxMessageBox(TEXT("LoadGameModuleInfo 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
			return false;
		}

		//连接数据库
		try
		{
			//变量定义
			CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
			tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

			//设置连接
			PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
			CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

			//发起连接
			PlatformDBModule->OpenConnection();
			PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

			//读取列表
			PlatformDBAide.ResetParameter();
			if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadDatabaseList"),true)==DB_SUCCESS)
			{
				//清空列表
				Reset();

				//读取列表
				while (PlatformDBModule->IsRecordsetEnd()==false)
				{
					//变量定义
					tagDataBaseList item = {0};

					//读取信息
					item.wDBListID = PlatformDBAide.GetValue_WORD(TEXT("DBListID"));
					item.wDBInfoID = PlatformDBAide.GetValue_WORD(TEXT("DBInfoID"));
					item.dwDBGenre = PlatformDBAide.GetValue_DWORD(TEXT("DBGenre"));
					PlatformDBAide.GetValue_String(TEXT("DBName"),item.szDBName,CountArray(item.szDBName));
					PlatformDBAide.GetValue_String(TEXT("DBAliasName"),item.szDBAliasName,CountArray(item.szDBAliasName));

					//列表处理
					Insert(&item);

					//移动记录
					PlatformDBModule->MoveToNext();
				}
			}

			return true;
		}
		catch (IDataBaseException * pIException)
		{
			//错误信息
			LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
			CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(pszDescribe,MB_ICONERROR);
		}
	}

	return true;
}

//重置数据
bool CDatabaseListManager::Reset()
{
	//变量定义
	WORD wDBListID=0;
	tagDataBaseList * pItem=NULL;
	POSITION Position=m_ListMap.GetStartPosition();

	//删除对象
	while (Position!=NULL)
	{
		m_ListMap.GetNextAssoc(Position,wDBListID,pItem);
		m_ListArray.Add(pItem);
	}

	//删除索引
	m_ListMap.RemoveAll();

	return true;
}

//删除数据
bool CDatabaseListManager::Delete(WORD wDBListID)
{
	//查找类型
	tagDataBaseList * pItem=NULL;
	if (m_ListMap.Lookup(wDBListID,pItem)==FALSE) return false;

	//删除数据
	m_ListMap.RemoveKey(wDBListID);
	m_ListArray.Add(pItem);

	//设置变量
	ZeroMemory(pItem,sizeof(tagDataBaseList));

	return true;
}

//插入数据
bool CDatabaseListManager::Insert(tagDataBaseList * pItem)
{
	//效验参数
	ASSERT(pItem!=NULL);
	if (pItem==NULL) return false;

	//查找现存
	WORD wDBListID=pItem->wDBListID;
	tagDataBaseList * pItemInsert=SearchItem(wDBListID);

	//创建判断
	if (pItemInsert==NULL)
	{
		//创建对象
		pItemInsert=CreateItem();

		//结果判断
		if (pItemInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_ListMap[wDBListID] = pItemInsert;
	CopyMemory(pItemInsert,pItem,sizeof(tagDataBaseList));

	return true;
}

//获取数目
INT_PTR CDatabaseListManager::GetCount()
{
	return m_ListMap.GetCount();
}

//查找数据
tagDataBaseList * CDatabaseListManager::SearchItem(WORD wDBListID)
{
	tagDataBaseList * pItem=NULL;
	m_ListMap.Lookup(wDBListID,pItem);
	return pItem;
}

//遍历数据
POSITION CDatabaseListManager::GetStartPosition()
{
	return m_ListMap.GetStartPosition();
}

//遍历数据
tagDataBaseList * CDatabaseListManager::GetNext( POSITION &pos )
{
	//变量定义
	WORD wDBListID = 0;
	tagDataBaseList *pItem = NULL;

	m_ListMap.GetNextAssoc(pos,wDBListID,pItem);

	return pItem;
}

//创建对象
tagDataBaseList * CDatabaseListManager::CreateItem()
{
	//变量定义
	tagDataBaseList * pItem=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_ListArray.GetCount();
		if (nArrayCount>0)
		{
			pItem=m_ListArray[nArrayCount-1];
			m_ListArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pItem=new tagDataBaseList;
			if (pItem==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//设置变量
	ZeroMemory(pItem,sizeof(tagDataBaseList));

	return pItem;
}

//////////////////////////////////////////////////////////////////////////
