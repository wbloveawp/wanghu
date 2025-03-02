#include "Stdafx.h"
#include "Resource.h"
#include "ChatPhrase.h"

//////////////////////////////////////////////////////////////////////////

//列表宽度
#define IMGAE_WIDTH					20									//列表宽度

//排序信息
struct tagSortParameter
{
	bool							bAscendSort;						//升序标志
	UINT							uChatListIndex;						//列表索引
	CChatPhraseList *				pChatPhraseList;					//列表控件
};

//////////////////////////////////////////////////////////////////////////
//静态变量

CImageList	CChatPhraseList::m_PhraseFlagImage;							//状态位图

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChatPhrase, CSkinDialog)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CChatPhraseList, CSkinListCtrl)
	ON_WM_NCDESTROY()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CChatPhraseList::CChatPhraseList()
{
	//资源变量
	m_bAscendSort=false;

	return;
}

//析构函数
CChatPhraseList::~CChatPhraseList()
{
}

//配置列表
bool CChatPhraseList::InitListControl()
{
	//加载背景
	if (m_PhraseFlagImage.m_hImageList==NULL)
	{
		//加载图片
		CBitmap Image;
		BITMAP ImageInfo;
		Image.LoadBitmap(IDB_PHRASE_FLAG_IMAGE);

		//创建图片
		Image.GetBitmap(&ImageInfo);
		m_PhraseFlagImage.Create(IMGAE_WIDTH,ImageInfo.bmHeight,ILC_COLOR16|ILC_MASK,0,0);

		//插入图片
		m_PhraseFlagImage.Add(&Image,RGB(255,0,255));
	}

	//设置控件
	SetImageList(&m_PhraseFlagImage,LVSIL_SMALL);
	SetExtendedStyle(GetExtendedStyle()|LVS_EX_SUBITEMIMAGES);

	//设置字体
	if (GetHeaderCtrl()!=NULL)
	{
		CFont TitleFont;
		TitleFont.CreatePointFont(130,TEXT("宋体"));
		m_SkinHeadCtrl.SetFont(&TitleFont);
	}

	return true;
}

//排列函数
INT CALLBACK CChatPhraseList::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	return 0;
}

//销毁消息
VOID CChatPhraseList::OnNcDestroy()
{
	//资源变量
	m_bAscendSort=false;

	__super::OnNcDestroy();
}

//点击消息
VOID CChatPhraseList::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult) 
{
	//变量定义
	NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

	//变量定义
	tagSortParameter SortParameter;
	ZeroMemory(&SortParameter,sizeof(SortParameter));

	//设置变量
	SortParameter.pChatPhraseList=this;
	SortParameter.bAscendSort=m_bAscendSort;
	SortParameter.uChatListIndex=pNMListView->iSubItem;

	//设置变量
	m_bAscendSort=!m_bAscendSort;

	//排列列表
	SortItems(SortFunction,(LPARAM)&SortParameter);

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CChatPhrase::CChatPhrase() : CSkinDialog(IDD_CHAT_PHRASE)
{
}

//析构函数
CChatPhrase::~CChatPhrase()
{
}

//控件绑定
VOID CChatPhrase::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_SELECT, m_btSelect);
	DDX_Control(pDX, IDC_CHAT_SHORT_LIST, m_ChatPhraseList);
}

//初始化函数
BOOL CChatPhrase::OnInitDialog()
{
	__super::OnInitDialog();

	//设置列表
	m_ChatPhraseList.InitListControl();
	m_ChatPhraseList.InsertColumn(0,TEXT("命令"),LVCFMT_LEFT,60);
	m_ChatPhraseList.InsertColumn(1,TEXT(""),LVCFMT_LEFT,IMGAE_WIDTH);
	m_ChatPhraseList.InsertColumn(2,TEXT("内容"),LVCFMT_LEFT,400);

	//设置标题
	SetWindowText(TEXT("聊天短语："));

	//调整界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return FALSE;
}

//调整控件
VOID CChatPhrase::RectifyControl(INT nWidth, INT nHeight)
{
	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

	//获取位置
	CRect rcButton;
	m_btSelect.GetWindowRect(&rcButton);

	//调整控件
	DeferWindowPos(hDwp,m_btCancel,NULL,nWidth-GetXBorder()-5-rcButton.Width(),nHeight-GetYBorder()-rcButton.Height()-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btSelect,NULL,nWidth-GetXBorder()-10-rcButton.Width()*2,nHeight-GetYBorder()-rcButton.Height()-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_ChatPhraseList,NULL,GetXBorder()+5,GetCaptionHeight()+6,nWidth-GetXBorder()*2-10,
		nHeight-GetCaptionHeight()-GetYBorder()-rcButton.Height()-16,uFlags);

	//移动结束
	EndDeferWindowPos(hDwp);

	return;
}

//////////////////////////////////////////////////////////////////////////
