#include "Stdafx.h"
#include "RobotUserManager.h"

/////////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CRobotUserManager::CRobotUserManager()
{
	//���ñ���
	m_szModuleDLLName[0] = 0;
	m_szCreateProcName[0] = 0;
	m_hDllInstance = NULL;

	//Ĭ������
	StringCchCopyA(m_szCreateProcName, CountArray(m_szCreateProcName), "CreateRobotUserItemSink");
}

//��������
CRobotUserManager::~CRobotUserManager()
{
	ResetUserManager();

	//�ͷ� DLL
	if (m_hDllInstance != NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance = NULL;
	}
}

//ģ������
VOID CRobotUserManager::SetModuleDLLName(LPCTSTR pszModuleDLLName)
{
	StringCchCopy(m_szModuleDLLName,CountArray(m_szModuleDLLName), pszModuleDLLName);
}

//����������
VOID  CRobotUserManager::SetCreateProcName(LPCSTR pszCreateProcName)
{
	StringCchCopyA(m_szCreateProcName, CountArray(m_szCreateProcName), pszCreateProcName);
}

//�ͷŶ���
VOID CRobotUserManager::FreeRobotUserItem(CRobotUserItem* pRobotUserItem)
{
	//��������
	if (pRobotUserItem != NULL)
	{
		pRobotUserItem->RepositUserItem();

		//�Ƴ�����
		m_StockRobotUserItem.RemoveStockItem(pRobotUserItem);
	}	
}

//�������
CRobotUserItem* CRobotUserManager::ActiveRobotUserItem()
{
	//��������
	auto pRobotUserItem = m_StockRobotUserItem.CreateStockItem();
	
	//��ȡ����
	if (pRobotUserItem != NULL && pRobotUserItem->m_pIRobotUserItemSink==NULL)
	{
		try
		{
			//��������
			auto pIRobotUserItemSink = (IRobotUserItemSink*)CreateRobotUserItemSink(IID_IRobotUserItemSink, VER_IRobotUserItemSink);			

			//�����û�
			if ((pIRobotUserItemSink != NULL) && (pIRobotUserItemSink->InitUserItemSink(pRobotUserItem) == false))
			{
				//�������
				ASSERT(FALSE);

				//ɾ������
				SafeRelease(pIRobotUserItemSink);

				return NULL;
			}

			//���ýӿ�
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

//��������
VOID CRobotUserManager::ResetUserManager()
{
	//���ñ���
	m_szModuleDLLName[0] = 0;

	//�Ƴ�����
	m_StockRobotUserItem.DeleteAllItem();

	//�ر�ģ��
	if (m_AndroidServiceHelper.GetInterface()!=NULL)
	{
		m_AndroidServiceHelper.CloseInstance();
	}	
}

//��������
VOID* CRobotUserManager::CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//����ģ��
		if (m_hDllInstance == NULL)
		{
			//����·��
			TCHAR szDirectory[MAX_PATH];
			CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

			//�ļ�·��
			TCHAR szModulePath[MAX_PATH];
			_sntprintf_s(szModulePath,CountArray(szModulePath),TEXT("%s\\RobotModule\\%s"), szDirectory, m_szModuleDLLName);

			m_hDllInstance = AfxLoadLibrary(szModulePath);
			if (m_hDllInstance == NULL) throw TEXT("TEST����ģ�鲻����");
		}

		//Ѱ�Һ���
		ModuleCreateProc* CreateProc = (ModuleCreateProc*)GetProcAddress(m_hDllInstance, m_szCreateProcName);
		if (CreateProc == NULL) throw TEXT("TEST����ģ��������Ϸ�");

		//�������
		return CreateProc(Guid, dwQueryVer);
	}
	catch (...) {}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
