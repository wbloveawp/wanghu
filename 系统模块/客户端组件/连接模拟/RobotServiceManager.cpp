#include "Stdafx.h"
#include "RobotServiceManager.h"

/////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CRobotUserManager::CRobotUserManager()
{
	//设置变量
	m_szModuleDLLName[0] = 0;
	m_szCreateProcName[0] = 0;

	//默认配置
	StringCchCopyA(m_szCreateProcName, CountArray(m_szCreateProcName), "CreateAndroidService");
}

//析构函数
CRobotUserManager::~CRobotUserManager()
{
	m_StockRobotUserItem.DeleteAllItem();
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
	//移除对象
	m_StockRobotUserItem.RemoveStockItem(pRobotUserItem);
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

	//关闭模块
	if (m_AndroidServiceHelper.GetInterface()!=NULL)
	{
		m_AndroidServiceHelper.CloseInstance();
	}

	//移除对象
	m_StockRobotUserItem.RemoveAllItem();
}

//创建机器
VOID* CRobotUserManager::CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//创建组件
		if (m_AndroidServiceHelper.GetInterface() == NULL)
		{
			//创建信息
			m_AndroidServiceHelper.SetModuleCreateInfo(m_szModuleDLLName, m_szCreateProcName);

			//创建模块
			if (!m_AndroidServiceHelper.CreateInstance())
			{
				throw 0;
			}
		}

		//创建机器人
		VOID* pAndroidObject = m_AndroidServiceHelper->CreateRobotUserItemSink(Guid, dwQueryVer);
		if (pAndroidObject == NULL)
		{
			throw TEXT("创建机器人失败");
		}

		return pAndroidObject;
	}
	catch (...) {}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
