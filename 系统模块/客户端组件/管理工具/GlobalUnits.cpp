#include "StdAfx.h"
#include "Mmsystem.h"
#include "Resource.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//状态信息
struct tagAstatInfo
{
	ADAPTER_STATUS					AdapterStatus;						//网卡状态
	NAME_BUFFER						NameBuff[16];						//名字缓冲
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalUnits::CGlobalUnits()
{
	//版本信息
	m_dwPlazaVersion=VERSION_PLAZA;
	m_dwFrameVersion=VERSION_FRAME;

	return;
}

//析构函数
CGlobalUnits::~CGlobalUnits()
{
	//保存配置
	m_ParameterGlobal.SaveParameter();

	return;
}

//接口查询
VOID * __cdecl CGlobalUnits::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGlobalUnits,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGlobalUnits,Guid,dwQueryVer);
	return NULL;
}

//创建组件
bool CGlobalUnits::InitGlobalUnits()
{
	//随机种子
	srand((UINT)time(NULL));

	//加载配置
	m_ParameterGlobal.LoadParameter();

	//创建资源
	if (m_PlatformResourceModule.GetInterface()==NULL)
	{
		//设置资源
		CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
		m_PlatformResourceModule.SetModuleCreateInfo(pParameterGlobal->m_szResourceModule,MODULE_CREATE_FUNCTION);

		//创建资源
		if (m_PlatformResourceModule.CreateInstance()==false) return false;
	}

	//加载资源
	IPlatformResource * pIPlatformResource=m_PlatformResourceModule.GetInterface();
	ISkinResource * pISkinResource=(ISkinResource *)pIPlatformResource->GetSkinResource(IID_ISkinResource,VER_ISkinResource);
	IPlazaResource * pIPlazaResource=(IPlazaResource *)pIPlatformResource->GetPlazaResource(IID_IPlazaResource,VER_IPlazaResource);

	//初始滚动
	tagSkinScrollResource ScrollResource;
	pISkinResource->GetScrollResource(ScrollResource);	
	CSkinScrollBar::m_SkinAttribute.Initialization(m_PlatformResourceModule->GetResInstance(),ScrollResource.uScrollImage);   

	//设置资源
	if (CSkinResourceManager::SetSkinResource(pISkinResource)==false) return false;

	//工作目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	SetCurrentDirectory(szDirectory);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//全局对象
CGlobalUnits						g_GlobalUnits;						//全局单元

//////////////////////////////////////////////////////////////////////////
