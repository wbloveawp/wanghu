#include "StdAfx.h"
#include "Resource.h"
#include "SkinResource.h"
#include "PlatformResource.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinResource::CSkinResource()
{
}

//��������
CSkinResource::~CSkinResource()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CSkinResource::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ISkinResource,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ISkinResource,Guid,dwQueryVer);
	return NULL;
}

//ʵ�����
HINSTANCE __cdecl CSkinResource::GetResInstance()
{
	return g_PlatformResource.GetResInstance();
}

//��������
bool __cdecl CSkinResource::GetFontResource(CFont & Font)
{
	//�ͷŶ���
	ASSERT(Font.GetSafeHandle()==NULL);
	if (Font.GetSafeHandle()!=NULL) Font.DeleteObject();

	//��������
	Font.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("����"));
	ASSERT(Font.GetSafeHandle()!=NULL);
    
	return (Font.GetSafeHandle()!=NULL);
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetEncircleResource(tagSkinEncircleResource & ResourceInfo)
{
	//���ñ���
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

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetMenuResource(tagSkinMenuResource & ResourceInfo)
{
	//��������
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//������ɫ
	ResourceInfo.crNormalText=RGB(1,43,75);
	ResourceInfo.crSelectText=RGB(0,84,190);

	//�߿���ɫ
	ResourceInfo.crMenuBar=RGB(155,222,238);		
	ResourceInfo.crSeparator=RGB(11,202,231);
	ResourceInfo.crNormalBack=RGB(255,255,255);
	ResourceInfo.crSelectBack=RGB(217,244,249);
	ResourceInfo.crSelectBorder=RGB(76,147,223);

	//λ����Ϣ
	ResourceInfo.uItemHeight=22;
	ResourceInfo.uMenuBarWidth=22;

	//��Դ��Ϣ
	ResourceInfo.uFlags=IDB_SKIN_MENU_FLAGS;

	return true;
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetEditResource(tagSkinEditResource & ResourceInfo)
{
	//��������
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//��ɫ����
	ResourceInfo.crFocusTX=RGB(0,0,0);
	ResourceInfo.crFocusBK=RGB(255,255,255);
	ResourceInfo.crNoFocusTX=RGB(10,10,10);
	ResourceInfo.crNoFocusBK=RGB(255,255,255);
	ResourceInfo.crDisFocusTX=RGB(50,50,50);
	ResourceInfo.crDisFocusBK=RGB(125,125,125);
	ResourceInfo.crBoradNormal=RGB(132,130,132);

	return true;
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetDialogResource(tagSkinDialogResource & ResourceInfo)
{
	//��������
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//������Ϣ
	ResourceInfo.bShowIcon=false;
	ResourceInfo.bShowTitle=true;
	ResourceInfo.hDefaultIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_SKIN_ICON));

	//ƫ����Ϣ
	ResourceInfo.nXIconExcursion=13;
	ResourceInfo.nYIconExcursion=6;
	ResourceInfo.nXButtonExcursion=5;
	ResourceInfo.nYButtonExcursion=4;
	ResourceInfo.nXTitleTextExcursion=10;
	ResourceInfo.nYTitleTextExcursion=8;

	//�߿���Ϣ
	ResourceInfo.crTitleText=RGB(250,250,250);
	ResourceInfo.crControlText=RGB(10,10,10);
	ResourceInfo.crBackGround=RGB(212,235,240);

	//������Ϣ
	ResourceInfo.uImageTL=IDB_SKIN_WND_TL;
	ResourceInfo.uImageTM=IDB_SKIN_WND_TM;
	ResourceInfo.uImageTR=IDB_SKIN_WND_TR;
	ResourceInfo.uImageMR=IDB_SKIN_WND_MR;
	ResourceInfo.uImageML=IDB_SKIN_WND_ML;
	ResourceInfo.uImageBL=IDB_SKIN_WND_BL;
	ResourceInfo.uImageBM=IDB_SKIN_WND_BM;
	ResourceInfo.uImageBR=IDB_SKIN_WND_BR;

	//��ť��Ϣ
	ResourceInfo.uButtonMin=IDB_SKIN_WND_MIN;
	ResourceInfo.uButtonMax=IDB_SKIN_WND_MAX;
	ResourceInfo.uButtonRes=IDB_SKIN_WND_RESORE;
	ResourceInfo.uButtonClose=IDB_SKIN_WND_CLOSE;
	
	return true;
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetButtonResource(tagSkinButtonResource & ResourceInfo)
{
	//��������
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//���ñ���
	ResourceInfo.crButtonText=RGB(10,10,10);

	//��Դ����
	ResourceInfo.uButton=IDB_SKIN_BUTTON_BACK;

	return true;
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetComboBoxResource(tagSkinComboBoxResource & ResourceInfo)
{
	//��������
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//���ñ���
	ResourceInfo.crBoradHover=RGB(132,130,132);
	ResourceInfo.crBoradNormal=RGB(132,130,132);

	//��Դ����
	ResourceInfo.uThumb=IDB_SKIN_COMBOBOX_THUMB;

	return true;
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetSplitterResource(tagSkinSplitterResource & ResourceInfo)
{
	//��������
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//������Ϣ
	ResourceInfo.crBorad=RGB(36,80,113);
	ResourceInfo.crBackGround=RGB(36,80,113);

	return true;
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetListCtrlResource(tagSkinListCtrlResource & ResourceInfo)
{
	//��������
	HINSTANCE hInstance=g_PlatformResource.GetResInstance();

	//��ɫ����
	ResourceInfo.crTitle=RGB(10,10,10);
	ResourceInfo.crListTX=RGB(69,69,69);
	ResourceInfo.crListBK=RGB(255,252,212);

	//��Դ��Ϣ
	ResourceInfo.uImageL=IDB_SKIN_HEADER_L;
	ResourceInfo.uImageM=IDB_SKIN_HEADER_M;
	ResourceInfo.uImageR=IDB_SKIN_HEADER_R;

	return true;
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetHyperLinkResource(tagSkinHyperLinkResource & SkinHyperLinkResource)
{
	//���ñ���
	SkinHyperLinkResource.crHoverText=RGB(250,0,0);
	SkinHyperLinkResource.crNormalText=RGB(0,0,240);
	SkinHyperLinkResource.crVisitedText=RGB(0,0,100);
	SkinHyperLinkResource.crBackColor=RGB(212,235,240);

	return true;
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetTabControlResource(tagSkinTabControlResource & ResourceInfo)
{
	//��ɫ����
	ResourceInfo.crNormalTX=RGB(10,10,10);
	ResourceInfo.crSelectTX=RGB(128,64,0);
	ResourceInfo.crControlBack=RGB(212,235,240);
	ResourceInfo.crControlBorad=RGB(125,125,125);

	return true;
}

//��ȡ��Ϣ
bool __cdecl CSkinResource::GetScrollResource(tagSkinScrollResource &ResourceInfo)
{
	ResourceInfo.uScrollImage=IDB_SKIN_SCROLL;

	return true;
}
//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(SkinResource);

//////////////////////////////////////////////////////////////////////////
