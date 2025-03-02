#include "StdAfx.h"
#include "Mmsystem.h"
#include "Resource.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//״̬��Ϣ
struct tagAstatInfo
{
	ADAPTER_STATUS					AdapterStatus;						//����״̬
	NAME_BUFFER						NameBuff[16];						//���ֻ���
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalUnits::CGlobalUnits()
{
	//�汾��Ϣ
	m_dwPlazaVersion=VERSION_PLAZA;
	m_dwFrameVersion=VERSION_FRAME;

	return;
}

//��������
CGlobalUnits::~CGlobalUnits()
{
	//��������
	m_ParameterGlobal.SaveParameter();

	return;
}

//�ӿڲ�ѯ
VOID * __cdecl CGlobalUnits::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGlobalUnits,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGlobalUnits,Guid,dwQueryVer);
	return NULL;
}

//�������
bool CGlobalUnits::InitGlobalUnits()
{
	//�������
	srand((UINT)time(NULL));

	//��������
	m_ParameterGlobal.LoadParameter();

	//������Դ
	if (m_PlatformResourceModule.GetInterface()==NULL)
	{
		//������Դ
		CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
		m_PlatformResourceModule.SetModuleCreateInfo(pParameterGlobal->m_szResourceModule,MODULE_CREATE_FUNCTION);

		//������Դ
		if (m_PlatformResourceModule.CreateInstance()==false) return false;
	}

	//������Դ
	IPlatformResource * pIPlatformResource=m_PlatformResourceModule.GetInterface();
	ISkinResource * pISkinResource=(ISkinResource *)pIPlatformResource->GetSkinResource(IID_ISkinResource,VER_ISkinResource);
	IPlazaResource * pIPlazaResource=(IPlazaResource *)pIPlatformResource->GetPlazaResource(IID_IPlazaResource,VER_IPlazaResource);

	//��ʼ����
	tagSkinScrollResource ScrollResource;
	pISkinResource->GetScrollResource(ScrollResource);	
	CSkinScrollBar::m_SkinAttribute.Initialization(m_PlatformResourceModule->GetResInstance(),ScrollResource.uScrollImage);   

	//������Դ
	if (CSkinResourceManager::SetSkinResource(pISkinResource)==false) return false;

	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	SetCurrentDirectory(szDirectory);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//ȫ�ֶ���
CGlobalUnits						g_GlobalUnits;						//ȫ�ֵ�Ԫ

//////////////////////////////////////////////////////////////////////////
