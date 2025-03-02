#include "StdAfx.h"
#include "Resource.h"
#include "SkinResource.h"
#include "PlatformResource.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinResource::CSkinResource()
{
}

//析构函数
CSkinResource::~CSkinResource()
{
}

//接口查询
VOID * __cdecl CSkinResource::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ISkinResource,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ISkinResource,Guid,dwQueryVer);
	return NULL;
}

//实例句柄
HINSTANCE __cdecl CSkinResource::GetResInstance()
{
	return g_PlatformResource.GetResInstance();
}

//创建字体
bool __cdecl CSkinResource::GetFontResource(CFont & Font)
{
	//释放对象
	ASSERT(Font.GetSafeHandle()==NULL);
	if (Font.GetSafeHandle()!=NULL) Font.DeleteObject();

	//创建字体
	Font.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	ASSERT(Font.GetSafeHandle()!=NULL);
    
	return (Font.GetSafeHandle()!=NULL);
}

//获取信息
bool __cdecl CSkinResource::GetEncircleResource(tagSkinEncircleResource & ResourceInfo)
{
	//设置变量
	ResourceInfo.uImageML=IDB_ENCIRCLE_ML;
	ResourceInfo.uImageMR=IDB_ENCIRCLE_MR;
	ResourceInfo.uImageTL=IDB_ENCIRCLE_TL;
	ResourceInfo.uImageTM=IDB_ENCIRCLE_TM;
	ResourceInfo.uImageTR=IDB_ENCIRCLE_TR;
	ResourceInfo.uImageBL=IDB_ENCIRCLE_BL;
	ResourceInfo.uImageBM=IDB_ENCIRCLE_BM;
	ResourceInfo.uImageBR=IDB_ENCIRCLE_BR;

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetMenuResource(tagSkinMenuResource & ResourceInfo)
{
	//变量定义
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//字体颜色
	ResourceInfo.crNormalText=RGB(1,43,75);
	ResourceInfo.crSelectText=RGB(0,84,190);

	//边框颜色
	ResourceInfo.crMenuBar=RGB(155,222,238);		
	ResourceInfo.crSeparator=RGB(11,202,231);
	ResourceInfo.crNormalBack=RGB(255,255,255);
	ResourceInfo.crSelectBack=RGB(217,244,249);
	ResourceInfo.crSelectBorder=RGB(76,147,223);

	//位置信息
	ResourceInfo.uItemHeight=22;
	ResourceInfo.uMenuBarWidth=22;

	//资源信息
	ResourceInfo.uFlags=IDB_SKIN_MENU_FLAGS;

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetEditResource(tagSkinEditResource & ResourceInfo)
{
	//变量定义
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//颜色变量
	ResourceInfo.crFocusTX=RGB(0,0,0);
	ResourceInfo.crFocusBK=RGB(255,255,255);
	ResourceInfo.crNoFocusTX=RGB(10,10,10);
	ResourceInfo.crNoFocusBK=RGB(255,255,255);
	ResourceInfo.crDisFocusTX=RGB(50,50,50);
	ResourceInfo.crDisFocusBK=RGB(125,125,125);
	ResourceInfo.crBoradNormal=RGB(132,130,132);

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetDialogResource(tagSkinDialogResource & ResourceInfo)
{
	//变量定义
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//控制信息
	ResourceInfo.bShowIcon=false;
	ResourceInfo.bShowTitle=true;
	ResourceInfo.hDefaultIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_SKIN_ICON));

	//偏移信息
	ResourceInfo.nXIconExcursion=13;
	ResourceInfo.nYIconExcursion=6;
	ResourceInfo.nXButtonExcursion=5;
	ResourceInfo.nYButtonExcursion=4;
	ResourceInfo.nXTitleTextExcursion=10;
	ResourceInfo.nYTitleTextExcursion=8;

	//边框信息
	ResourceInfo.crTitleText=RGB(250,250,250);
	ResourceInfo.crControlText=RGB(10,10,10);
	ResourceInfo.crBackGround=RGB(212,235,240);

	//标题信息
	ResourceInfo.uImageTL=IDB_SKIN_WND_TL;
	ResourceInfo.uImageTM=IDB_SKIN_WND_TM;
	ResourceInfo.uImageTR=IDB_SKIN_WND_TR;
	ResourceInfo.uImageMR=IDB_SKIN_WND_MR;
	ResourceInfo.uImageML=IDB_SKIN_WND_ML;
	ResourceInfo.uImageBL=IDB_SKIN_WND_BL;
	ResourceInfo.uImageBM=IDB_SKIN_WND_BM;
	ResourceInfo.uImageBR=IDB_SKIN_WND_BR;

	//按钮信息
	ResourceInfo.uButtonMin=IDB_SKIN_WND_MIN;
	ResourceInfo.uButtonMax=IDB_SKIN_WND_MAX;
	ResourceInfo.uButtonRes=IDB_SKIN_WND_RESORE;
	ResourceInfo.uButtonClose=IDB_SKIN_WND_CLOSE;
	
	return true;
}

//获取信息
bool __cdecl CSkinResource::GetButtonResource(tagSkinButtonResource & ResourceInfo)
{
	//变量定义
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//设置变量
	ResourceInfo.crButtonText=RGB(10,10,10);

	//资源变量
	ResourceInfo.uButton=IDB_SKIN_BUTTON_BACK;

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetComboBoxResource(tagSkinComboBoxResource & ResourceInfo)
{
	//变量定义
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//设置变量
	ResourceInfo.crBoradHover=RGB(132,130,132);
	ResourceInfo.crBoradNormal=RGB(132,130,132);

	//资源变量
	ResourceInfo.uThumb=IDB_SKIN_COMBOBOX_THUMB;

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetSplitterResource(tagSkinSplitterResource & ResourceInfo)
{
	//变量定义
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//标题信息
	ResourceInfo.crBorad=RGB(36,80,113);
	ResourceInfo.crBackGround=RGB(36,80,113);

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetListCtrlResource(tagSkinListCtrlResource & ResourceInfo)
{
	//变量定义
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//颜色变量
	ResourceInfo.crTitle=RGB(10,10,10);
	ResourceInfo.crListTX=RGB(69,69,69);
	ResourceInfo.crListBK=RGB(255,252,212);

	//资源信息
	ResourceInfo.uImageL=IDB_SKIN_HEADER_L;
	ResourceInfo.uImageM=IDB_SKIN_HEADER_M;
	ResourceInfo.uImageR=IDB_SKIN_HEADER_R;

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetHyperLinkResource(tagSkinHyperLinkResource & SkinHyperLinkResource)
{
	//设置变量
	SkinHyperLinkResource.crHoverText=RGB(250,0,0);
	SkinHyperLinkResource.crNormalText=RGB(0,0,240);
	SkinHyperLinkResource.crVisitedText=RGB(0,0,100);
	SkinHyperLinkResource.crBackColor=RGB(212,235,240);

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetTabControlResource(tagSkinTabControlResource & ResourceInfo)
{
	//颜色变量
	ResourceInfo.crNormalTX=RGB(10,10,10);
	ResourceInfo.crSelectTX=RGB(128,64,0);
	ResourceInfo.crControlBack=RGB(212,235,240);
	ResourceInfo.crControlBorad=RGB(125,125,125);

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetScrollResource(tagSkinScrollResource &ResourceInfo)
{
	ResourceInfo.uScrollImage=IDB_SKIN_SCROLL;

	return true;
}
//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(SkinResource);

//////////////////////////////////////////////////////////////////////////
