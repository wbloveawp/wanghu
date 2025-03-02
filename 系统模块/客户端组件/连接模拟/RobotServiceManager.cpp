#include "Stdafx.h"
#include "RobotServiceManager.h"

/////////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CRobotUserManager::CRobotUserManager()
{
	//���ñ���
	m_szModuleDLLName[0] = 0;
	m_szCreateProcName[0] = 0;

	//Ĭ������
	StringCchCopyA(m_szCreateProcName, CountArray(m_szCreateProcName), "CreateAndroidService");
}

//��������
CRobotUserManager::~CRobotUserManager()
{
	m_StockRobotUserItem.DeleteAllItem();
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
	//�Ƴ�����
	m_StockRobotUserItem.RemoveStockItem(pRobotUserItem);
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

	//�ر�ģ��
	if (m_AndroidServiceHelper.GetInterface()!=NULL)
	{
		m_AndroidServiceHelper.CloseInstance();
	}

	//�Ƴ�����
	m_StockRobotUserItem.RemoveAllItem();
}

//��������
VOID* CRobotUserManager::CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//�������
		if (m_AndroidServiceHelper.GetInterface() == NULL)
		{
			//������Ϣ
			m_AndroidServiceHelper.SetModuleCreateInfo(m_szModuleDLLName, m_szCreateProcName);

			//����ģ��
			if (!m_AndroidServiceHelper.CreateInstance())
			{
				throw 0;
			}
		}

		//����������
		VOID* pAndroidObject = m_AndroidServiceHelper->CreateRobotUserItemSink(Guid, dwQueryVer);
		if (pAndroidObject == NULL)
		{
			throw TEXT("����������ʧ��");
		}

		return pAndroidObject;
	}
	catch (...) {}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
