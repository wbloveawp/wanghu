#include "Stdafx.h"
#include "RobotUserManager.h"

/////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CRobotUserManager::CRobotUserManager()
{
	//设置变量
	m_szModuleDLLName[0] = 0;
	m_szCreateProcName[0] = 0;
	m_hDllInstance = NULL;

	//默认配置
	StringCchCopyA(m_szCreateProcName, CountArray(m_szCreateProcName), "CreateRobotUserItemSink");
}

//析构函数
CRobotUserManager::~CRobotUserManager()
{
	ResetUserManager();

	//释放 DLL
	if (m_hDllInstance != NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance = NULL;
	}
}

//模块名称
VOID CRobotUserManager::SetModuleDLLName(LPCTSTR pszModuleDLLName)
{
	StringCchCopy(m_szModuleDLLName,CountArray(m_szModuleDLLName), pszModuleDLLName);
}

//创建过程名
VOID  CRobotUserManager::SetCreateProcName(LPCSTR pszCreateProcName)
{
	StringCchCopyA(m_szCreateProcName, CountArray(m_szCreateProcName), pszCreateProcName);
}

//释放对象
VOID CRobotUserManager::FreeRobotUserItem(CRobotUserItem* pRobotUserItem)
{
	//重置数据
	if (pRobotUserItem != NULL)
	{
		pRobotUserItem->RepositUserItem();

		//移除对象
		m_StockRobotUserItem.RemoveStockItem(pRobotUserItem);
	}	
}

//激活对象
CRobotUserItem* CRobotUserManager::ActiveRobotUserItem()
{
	//创建机器
	auto pRobotUserItem = m_StockRobotUserItem.CreateStockItem();
	
	//获取对象
	if (pRobotUserItem != NULL && pRobotUserItem->m_pIRobotUserItemSink==NULL)
	{
		try
		{
			//创建陪玩
			auto pIRobotUserItemSink = (IRobotUserItemSink*)CreateRobotUserItemSink(IID_IRobotUserItemSink, VER_IRobotUserItemSink);			

			//设置用户
			if ((pIRobotUserItemSink != NULL) && (pIRobotUserItemSink->InitUserItemSink(pRobotUserItem) == false))
			{
				//错误断言
				ASSERT(FALSE);

				//删除对象
				SafeRelease(pIRobotUserItemSink);

				return NULL;
			}

			//设置接口
			pRobotUserItem->m_pIRobotUserItemSink = pIRobotUserItemSink;			
		}
		catch (...)
		{
			ASSERT(FALSE);
			return NULL;
		}
	}

	return pRobotUserItem;
}

//重置数据
VOID CRobotUserManager::ResetUserManager()
{
	//设置变量
	m_szModuleDLLName[0] = 0;

	//移除对象
	m_StockRobotUserItem.DeleteAllItem();

	//关闭模块
	if (m_AndroidServiceHelper.GetInterface()!=NULL)
	{
		m_AndroidServiceHelper.CloseInstance();
	}	
}

//创建机器
VOID* CRobotUserManager::CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//加载模块
		if (m_hDllInstance == NULL)
		{
			//构造路径
			TCHAR szDirectory[MAX_PATH];
			CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

			//文件路径
			TCHAR szModulePath[MAX_PATH];
			_sntprintf_s(szModulePath,CountArray(szModulePath),TEXT("%s\\RobotModule\\%s"), szDirectory, m_szModuleDLLName);

			m_hDllInstance = AfxLoadLibrary(szModulePath);
			if (m_hDllInstance == NULL) throw TEXT("TEST服务模块不存在");
		}

		//寻找函数
		ModuleCreateProc* CreateProc = (ModuleCreateProc*)GetProcAddress(m_hDllInstance, m_szCreateProcName);
		if (CreateProc == NULL) throw TEXT("TEST服务模块组件不合法");

		//创建组件
		return CreateProc(Guid, dwQueryVer);
	}
	catch (...) {}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
