#include "StdAfx.h"
#include "Resource.h"
#include "UserListControl.h"

//////////////////////////////////////////////////////////////////////////

//禁止列数
#define ENJOIN_COUNT				3									//禁止列数

//默认属性
#define LIST_STYTE					LVS_EX_SUBITEMIMAGES|LVS_EX_FULLROWSELECT		\
									|LVS_EX_INFOTIP/*|LVS_EX_FLATSB*/					\
									|LVS_EX_ONECLICKACTIVATE

//////////////////////////////////////////////////////////////////////////

//排序位置
#define SORT_POSITION_AFTER			1									//后面位置
#define SORT_POSITION_FRONT			-1									//前面位置

//////////////////////////////////////////////////////////////////////////

//排序信息
struct tagSortParameter
{
	bool							bAscendSort;						//升序标志
	BYTE							cbDataDescribe;						//字段类型
	CUserListControl *				pUserListControl;					//列表控件
};

//排序信息
struct tagSearchSortParameter
{
	bool							bAscendSort;						//升序标志
	BYTE							cbDataDescribe;						//字段类型
	CSearchUserListControl *		pListControl;						//列表控件
};

//////////////////////////////////////////////////////////////////////////
//静态变量

CImageList			CUserListControl::m_ExperienceImage;				//经验位图
CImageList			CUserListControl::m_UserStatusImage;				//状态位图

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUserListHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CUserListControl, CListCtrl)
	ON_WM_NCDESTROY()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSearchUserListControl, CListCtrl)
	ON_WM_NCDESTROY()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserListHeaderCtrl::CUserListHeaderCtrl()
{
}

//析构函数
CUserListHeaderCtrl::~CUserListHeaderCtrl()
{
}

//重画函数
VOID CUserListHeaderCtrl::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcRect;
	GetClientRect(&rcRect);

	//建立缓冲
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc, rcRect.Width(), rcRect.Height());

	//字体
	CFont TitleFont;
	TitleFont.CreatePointFont(100, TEXT("宋体"));

	//设置 DC
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(&BufferBmp);
	BufferDC.SetTextColor(RGB(0, 0, 0));
	BufferDC.SelectObject(TitleFont);

	//加载位图
	//CImageHandle ImageHandleL(&CSkinListCtrl::m_SkinAttribute.m_SkinImageL);
	//CImageHandle ImageHandleM(&CSkinListCtrl::m_SkinAttribute.m_SkinImageM);
	//CImageHandle ImageHandleR(&CSkinListCtrl::m_SkinAttribute.m_SkinImageR);

	//绘画背景
	//ImageHandleM->StretchBlt(BufferDC, 0, 0, rcRect.Width(), rcRect.Height());
	BufferDC.FillSolidRect(0, 0, rcRect.Width(), rcRect.Height(), RGB(191, 144, 78));

	//绘画子项
	CRect rcItem;
	HDITEM HDItem;
	TCHAR szBuffer[64];
	for (INT i = 0; i < GetItemCount(); i++)
	{
		//构造变量
		HDItem.mask = HDI_TEXT;
		HDItem.pszText = szBuffer;
		HDItem.cchTextMax = CountArray(szBuffer);

		//获取信息
		GetItem(i, &HDItem);
		GetItemRect(i, &rcItem);

		//绘画背景
		//INT nLeftWidth = ImageHandleL->GetWidth();
		//INT nRightWidth = ImageHandleR->GetWidth();
		//ImageHandleL->StretchBlt(BufferDC, rcItem.left, rcItem.top, nLeftWidth, rcItem.Height());
		//ImageHandleM->StretchBlt(BufferDC, rcItem.left + nLeftWidth, rcItem.top, rcItem.Width() - nLeftWidth - nRightWidth, rcItem.Height());
		//ImageHandleR->StretchBlt(BufferDC, rcItem.right - nRightWidth, rcItem.top, nRightWidth, rcItem.Height());
		BufferDC.FillSolidRect(rcItem.left, rcItem.top+1, 1, rcItem.Height()-2, RGB(50, 30, 17));

		//绘画标题
		rcItem.DeflateRect(3, 1, 3, 1);
		const UINT uFormat = DT_WORDBREAK | DT_EDITCONTROL | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_CENTER;
		BufferDC.DrawText(szBuffer, lstrlen(szBuffer), &rcItem, uFormat);
	}

	//绘画界面
	dc.BitBlt(0, 0, rcRect.Width(), rcRect.Height(), &BufferDC, 0, 0, SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();

	return;
}

//背景函数
BOOL CUserListHeaderCtrl::OnEraseBkgnd(CDC* pDC)
{
	//刷新界面
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//设置标志
VOID CUserListHeaderCtrl::SetSortArrow(INT nSortColumn, enSortType SortType)
{
	//设置变量
	m_SortType = SortType;
	m_nColumn = nSortColumn;

	//重画界面
	Invalidate(FALSE);

	return;
}

//获取标志
VOID CUserListHeaderCtrl::GetSortArrow(INT& nSortColumn, enSortType& SortType)
{
	//设置变量
	SortType = m_SortType;
	nSortColumn = m_nColumn;

	return;
}

//控件消息
BOOL CUserListHeaderCtrl::OnChildNotify(UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT * pLResult)
{
	//变量定义
	NMHEADER * pNMHearder=(NMHEADER*)lParam;

	//拖动消息
	if ((pNMHearder->hdr.code==HDN_BEGINTRACKA)||(pNMHearder->hdr.code==HDN_BEGINTRACKW))
	{
		//禁止拖动
		if (pNMHearder->iItem<ENJOIN_COUNT)
		{
			*pLResult=TRUE;
			return TRUE;
		}
	}

	return __super::OnChildNotify(uMessage,wParam,lParam,pLResult);
}

//光标消息
BOOL CUserListHeaderCtrl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserListControl::CUserListControl()
{
	//资源变量
	m_dwMySelfUserID=0L;
	m_bAscendSort=false;
	m_bCreateColumn=false;

	//接口变量
	m_pIGameLevelParser=NULL;
	m_pIGameUserManager=NULL;

	//列表信息
	m_wColumnCount=0;
	ZeroMemory(m_cbDataDescribe,sizeof(m_cbDataDescribe));

	//资源变量
	ZeroMemory(&m_ColorUserList,sizeof(m_ColorUserList));

	//默认定义
	m_ColorUserList.crSelectTX = COLOR_SELECT_TX;
	m_ColorUserList.crSelectBK = COLOR_SELECT_BK;
	m_ColorUserList.crNormalTX = COLOR_NORMAL_TX;
	m_ColorUserList.crNormalBK = COLOR_NORMAL_BK;

	//玩家定义
	m_ColorUserList.crMyselfTX = COLOR_MYSELF_TX;
	m_ColorUserList.crMyselfBK = COLOR_MYSELF_BK;
	m_ColorUserList.crMasterTX = COLOR_MASTER_TX;
	m_ColorUserList.crMasterBK = COLOR_MASTER_BK;
	m_ColorUserList.crMemberTX = COLOR_MEMBER_TX;
	m_ColorUserList.crMemberBK = COLOR_MEMBER_BK;

	return;
}

//析构函数
CUserListControl::~CUserListControl()
{
}

//控件绑定
VOID CUserListControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//设置滚动
	m_SkinScrollBar.InitScroolBar(this);

	//设置属性
	SetExtendedStyle(LIST_STYTE);
	m_UserListHeaderCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	//加载经验
	if (m_ExperienceImage.m_hImageList==NULL)
	{
		//加载图片
		CBitmap Image;
		BITMAP ImageInfo;
		Image.LoadBitmap(IDB_USER_EXPERIENCE1);

		//创建图片
		Image.GetBitmap(&ImageInfo);
		m_ExperienceImage.Create(ImageInfo.bmWidth/3,ImageInfo.bmHeight,ILC_COLOR16|ILC_MASK,0,0);

		//插入图片
		m_ExperienceImage.Add(&Image, RGB(255, 0, 255));
	}

	//加载状态
	if (m_UserStatusImage.m_hImageList==NULL)
	{
		//加载图片
		CBitmap Image;
		BITMAP ImageInfo;
		Image.LoadBitmap(IDB_USER_STATUS_IMAGE);

		//创建图片
		Image.GetBitmap(&ImageInfo);
		m_UserStatusImage.Create(WIDTH_STATUS,ImageInfo.bmHeight,ILC_COLOR16|ILC_MASK,0,0);

		//插入图片
		m_UserStatusImage.Add(&Image, RGB(255, 0, 255));
	}

	//设置控件
	SetInsertMarkColor(m_ColorUserList.crNormalBK);
	SetBkColor(m_ColorUserList.crNormalBK);
	SetImageList(&m_UserStatusImage, LVSIL_SMALL);

	//设置字体
	if (GetHeaderCtrl()!=NULL)
	{
		CFont TitleFont;
		TitleFont.CreatePointFont(130,TEXT("宋体"));
		m_UserListHeaderCtrl.SetFont(&TitleFont);
	}

	//用户标志
	m_cbDataDescribe[m_wColumnCount]=UD_NULL;
	InsertColumn(m_wColumnCount++,TEXT(""),LVCFMT_CENTER,0,0);

	//用户标志
	m_cbDataDescribe[m_wColumnCount]=UD_FLAG;
	InsertColumn(m_wColumnCount++,TEXT("标识"),LVCFMT_CENTER,40,0);

	//用户状态
	m_cbDataDescribe[m_wColumnCount]=UD_STATUS;
	InsertColumn(m_wColumnCount++,TEXT("状态"),LVCFMT_CENTER, 40,0);

	//用户名称
	m_cbDataDescribe[m_wColumnCount] = UD_ACCOUNTS;
	InsertColumn(m_wColumnCount++, TEXT("用户名"), LVCFMT_LEFT, 100);

	//游戏ID
	m_cbDataDescribe[m_wColumnCount] = UD_GAME_ID;
	InsertColumn(m_wColumnCount++, TEXT("游戏ID"), LVCFMT_LEFT, 62);

	//桌子号码
	m_cbDataDescribe[m_wColumnCount] = UD_TABLE;
	InsertColumn(m_wColumnCount++, TEXT("桌子号码"), LVCFMT_LEFT, 62);

	//椅子号码
	m_cbDataDescribe[m_wColumnCount] = UD_CHAIR;
	InsertColumn(m_wColumnCount++, TEXT("椅子号码"), LVCFMT_LEFT, 62);

	//用户积分
	m_cbDataDescribe[m_wColumnCount] = UD_SCORE;
	InsertColumn(m_wColumnCount++, TEXT("用户积分"), LVCFMT_LEFT, 90);

	//游戏局数
	m_cbDataDescribe[m_wColumnCount] = UD_PLAY_COUNT;
	InsertColumn(m_wColumnCount++, TEXT("游戏局数"), LVCFMT_LEFT, 62);

	//用户胜率
	m_cbDataDescribe[m_wColumnCount] = UD_WIN_RATE;
	InsertColumn(m_wColumnCount++, TEXT("用户胜率"), LVCFMT_LEFT, 62);

	//用户逃率
	m_cbDataDescribe[m_wColumnCount] = UD_FLEE_RATE;
	InsertColumn(m_wColumnCount++, TEXT("用户逃率"), LVCFMT_LEFT, 62);

	return;
}

//绘画函数
VOID CUserListControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//变量定义
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	//获取属性
	INT nItemID=lpDrawItemStruct->itemID;
	INT nColumnCount=m_UserListHeaderCtrl.GetItemCount();

	//定制变量
	COLORREF rcTextColor=m_ColorUserList.crNormalTX;
	COLORREF rcBackColor=m_ColorUserList.crNormalBK;
	IClientUserItem * pIClientUserItem=(IClientUserItem *)lpDrawItemStruct->itemData;

	//颜色定制
	if (lpDrawItemStruct->itemState&ODS_SELECTED)
	{
		//选择颜色
		rcTextColor=m_ColorUserList.crSelectTX;
		rcBackColor=m_ColorUserList.crSelectBK;
	}
	else if (pIClientUserItem->GetUserID()==m_dwMySelfUserID)
	{
		//自己颜色
		rcTextColor=m_ColorUserList.crMyselfTX;
		rcBackColor=m_ColorUserList.crMasterBK;
	}
	else if (pIClientUserItem->GetMasterOrder()!=0)
	{
		//管理颜色
		rcTextColor=m_ColorUserList.crMasterTX;
		rcBackColor=m_ColorUserList.crMasterBK;
	}
	else if (pIClientUserItem->GetMemberOrder()!=0)
	{
		//会员颜色
		rcTextColor=m_ColorUserList.crMemberTX;
		rcBackColor=m_ColorUserList.crMemberBK;
	}

	//机器判断
	if (pIClientUserItem->IsRobot())
	{
		rcTextColor = COLOR_ANDROID_TX;
		rcBackColor = COLOR_ANDROID_BK;
	}

	//绘画区域
	CRect rcClipBox;
	pDC->GetClipBox(&rcClipBox);

	//设置 DC
	pDC->SetBkColor(rcBackColor);
	pDC->SetTextColor(rcTextColor);

	//绘画选择
	if (lpDrawItemStruct->itemState&ODS_FOCUS)
	{
		COLORREF rcFillColor=GetBkColor();
		pDC->Draw3dRect(&lpDrawItemStruct->rcItem,rcFillColor,rcFillColor);
	}

	//绘画子项
	for (INT i=1;i<nColumnCount;i++)
	{
		//获取位置
		CRect rcSubItem;
		GetSubItemRect(nItemID,i,LVIR_LABEL,rcSubItem);

		//设置边界
		//if (i<=ENJOIN_COUNT) rcItem.left=rcSubItem.left;

		//绘画判断
		if (rcSubItem.left>rcClipBox.right) break;
		if (rcSubItem.right<rcClipBox.left) continue;
		if (rcSubItem.left==rcSubItem.right) continue;

		//绘画背景
		//if (i>=ENJOIN_COUNT) 
			pDC->FillSolidRect(&rcSubItem,rcBackColor);

		//绘画信息
		if (i<ENJOIN_COUNT)
		{
			//绘画标志
			UINT uImageIndex=GetImageIndex(pIClientUserItem,i);
			if (uImageIndex!=INDEX_INVALID) m_UserStatusImage.Draw(pDC,uImageIndex,CPoint(rcSubItem.left,rcSubItem.top),ILD_TRANSPARENT);
		}
		else if (m_cbDataDescribe[i]==UD_EXPERIENCE)
		{
			//获取经验
			BYTE cbOrderIndex[MAX_EXPERIENCE];
			WORD wIndexCount=pIClientUserItem->GetExperienceIndex(cbOrderIndex,CountArray(cbOrderIndex));

			//计算位置
			INT nStartPos=rcSubItem.left;
			nStartPos+=__max(0,((rcSubItem.Width()-wIndexCount*(WIDTH_EXPERIENCE+2))-2)/2);

			//绘画经验
			for (WORD j=0;j<wIndexCount;j++)
			{
				CPoint DrawPoint(nStartPos+j*(WIDTH_EXPERIENCE+2),rcSubItem.top);
				m_ExperienceImage.Draw(pDC,cbOrderIndex[j],DrawPoint,ILD_TRANSPARENT);
			}
		}
		else
		{
			//获取文字
			TCHAR szString[256]=TEXT("");
			GetItemText(nItemID,i,szString,CountArray(szString));

			//绘画文字
			rcSubItem.left+=5;
			pDC->DrawText(szString,lstrlen(szString),&rcSubItem,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		}
	}

	//绘画选择
	if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->DrawFocusRect(&rcItem);

	return;
}

//设置接口
bool CUserListControl::SetMySelfUserID(DWORD dwMySelfUserID)
{
	//设置变量
	m_dwMySelfUserID=dwMySelfUserID;

	return true;
}

//设置接口
bool CUserListControl::SetGameLevelParser(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameLevelParser)!=NULL);
		m_pIGameLevelParser=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameLevelParser);

		//成功判断
		if (m_pIGameLevelParser==NULL) return false;
	}
	else m_pIGameLevelParser=NULL;

	return true;
}

//设置接口
bool CUserListControl::SetGameUserManager(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameUserManager)!=NULL);
		m_pIGameUserManager=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameUserManager);

		//成功判断
		if (m_pIGameUserManager==NULL) return false;
	}
	else m_pIGameUserManager=NULL;

	return true;
}

//设置颜色
bool CUserListControl::SetColorUserList(const tagColorUserList & ColorUserList)
{
	//设置变量
	m_ColorUserList=ColorUserList;

	//更新界面
	if (m_hWnd!=NULL)
	{
		//设置背景
		SetBkColor(ColorUserList.crNormalBK);

		//重合窗口
		RedrawWindow();
	}

	return true;
}

//设置列表
bool CUserListControl::SetColumnDescribe(const tagColumnItem ColumnItem[], WORD wColumnCount)
{
	//构造判断
	if (m_bCreateColumn==true) return true;

	//调整参数
	ASSERT(wColumnCount<(CountArray(m_cbDataDescribe)-m_wColumnCount));
	wColumnCount=__min(CountArray(m_cbDataDescribe)-m_wColumnCount,wColumnCount);

	//设置列表
	SetRedraw(FALSE);
	for (WORD i=0;i<wColumnCount;i++)
	{
		CW2CT strColumnName(ColumnItem[i].szColumnName);
		m_cbDataDescribe[m_wColumnCount]=ColumnItem[i].cbDataDescribe;
		InsertColumn(m_wColumnCount++,strColumnName,LVCFMT_LEFT,ColumnItem[i].cbColumnWidth);
	}
	SetRedraw(TRUE);

	//设置变量
	m_bCreateColumn=true;

	return true;
}

//插入用户
bool CUserListControl::InsertUserItem(IClientUserItem * pIClientUserItem)
{
	//效验状态
	ASSERT(pIClientUserItem!=NULL);
	if (m_hWnd==NULL) return false;
	
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIClientUserItem;
	INT nItem=FindItem(&FindInfo);
	if (nItem!=-1)
	{
		UpdateUserItem(pIClientUserItem);
		return true;
	}

	//插入用户
	for (WORD i=0;i<m_wColumnCount;i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pIClientUserItem);
			INT nImageIndex=GetImageIndex(pIClientUserItem,i);
			nItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,TEXT(""),0,0,0,(LPARAM)pIClientUserItem);
		}
		else
		{
			//图形子项
			if (m_cbDataDescribe[i]==UD_NULL) continue;
			if (m_cbDataDescribe[i]>=UD_IMAGE) continue;

			//字符子项
			SetItem(nItem,i,LVIF_TEXT,GetDescribeString(pIClientUserItem,i),0,0,0,0);
		}
	}

	return true;
}

//更新用户
bool CUserListControl::UpdateUserItem(IClientUserItem * pIClientUserItem)
{
	//效验状态
	ASSERT(pIClientUserItem!=NULL);
	if (GetSafeHwnd()==NULL) return false;
	
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIClientUserItem;
	INT nItem=FindItem(&FindInfo);
	if (nItem==-1) return false;

	//更新用户
	for (WORD i=0;i<m_wColumnCount;i++)
	{
		//图形子项
		if (m_cbDataDescribe[i]==UD_NULL) continue;
		if (m_cbDataDescribe[i]>=UD_IMAGE) continue;

		//字符子项
		SetItem(nItem,i,LVIF_TEXT,GetDescribeString(pIClientUserItem,i),0,0,0,0);
	}

	//更新子项
	RedrawItems(nItem,nItem);

	return true;
}

//删除用户
bool CUserListControl::DeleteUserItem(IClientUserItem * pIClientUserItem)
{
	//效验状态
	ASSERT(pIClientUserItem!=NULL);
	if (m_hWnd==NULL) return false;
	
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//构造变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIClientUserItem;

	//删除子项
	INT nItem=FindItem(&FindInfo);
	if (nItem!=-1) 
	{
		DeleteItem(nItem);
		return true;
	}

	return false;
}

//获取位置
INT CUserListControl::GetInsertIndex(IClientUserItem * pIClientUserItem)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagClientUserInfo * pUserInfo=pIClientUserItem->GetUserInfo();

	//自己判断
	if (pUserInfo->dwUserID==m_dwMySelfUserID) return 0;

	//普通用户
	BYTE cbCompanion=pIClientUserItem->GetUserCompanion();
	if ((pUserInfo->cbMasterOrder==0)&&(cbCompanion!=CP_FRIEND)&&(pUserInfo->dwMemberPoint==0)) return nItemCount;

	//变量定义
	IClientUserItem * pIUserItemTemp=NULL;
	LONG nImportOrder=pIClientUserItem->GetImportOrder();

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		pIUserItemTemp=(IClientUserItem *)GetItemData(i);
		if (pIUserItemTemp->GetUserID()==m_dwMySelfUserID) continue;
		if (nImportOrder>=pIUserItemTemp->GetImportOrder()) return i;
	}

	return nItemCount;
}

//获取位图
UINT CUserListControl::GetImageIndex(IClientUserItem * pIClientUserItem, WORD wColumnIndex)
{
	//效验参数
	ASSERT(pIClientUserItem!=NULL);
	ASSERT(wColumnIndex<m_wColumnCount);

	//获取索引
	switch (m_cbDataDescribe[wColumnIndex])
	{
	case UD_FLAG:		//用户标志
		{
			//变量定义
			BYTE cbCompanion=pIClientUserItem->GetUserCompanion();
			tagClientUserInfo * pUserInfo=pIClientUserItem->GetUserInfo();

			//索引计算
			if (cbCompanion==CP_FRIEND) return INDEX_FRIEND;
			if( cbCompanion == CP_DETEST ) return INDEX_DETEST;
			if (pUserInfo->cbMasterOrder>0) return INDEX_MASTER+(pUserInfo->cbMasterOrder-1);
			if (pUserInfo->dwMemberPoint>0) return INDEX_MEMBER+(pUserInfo->dwMemberPoint-1);

			return INDEX_NORMAL;
		}
	case UD_STATUS:		//用户状态
		{
			//变量定义
			BYTE cbGender=pIClientUserItem->GetGender();
			BYTE cbUserStatus=pIClientUserItem->GetUserStatus();

			//索引计算
			return INDEX_STATUS+cbGender*COUNT_STATUS+cbUserStatus-US_FREE;
		}
	}

	return INDEX_INVALID;
}

//描述字符
LPCTSTR CUserListControl::GetDescribeString(IClientUserItem * pIClientUserItem, WORD wColumnIndex)
{
	//效验参数
	ASSERT(pIClientUserItem!=NULL);
	ASSERT(wColumnIndex<m_wColumnCount);
	ASSERT(m_cbDataDescribe[wColumnIndex]<UD_IMAGE);

	//变量定义
	static TCHAR szDescribe[512]=TEXT("");
	tagClientUserInfo * pUserInfo=pIClientUserItem->GetUserInfo();

	//构造字符
	switch (m_cbDataDescribe[wColumnIndex])
	{
	case UD_GAME_ID:		//游戏标识
		{
			if (pUserInfo->dwGameID==0L) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->dwGameID);
			break;
		}
	case UD_USER_ID:		//用户标识
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->dwUserID);
			break;
		}
	case UD_ACCOUNTS:		//用户帐号
		{
			CW2CT strAccounts(pUserInfo->szAccounts);
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),(LPCTSTR)strAccounts);
			break;
		}
	case UD_GENDER:			//用户性别
		{
			if (pUserInfo->cbGender==GENDER_FEMALE) lstrcpyn(szDescribe,TEXT("美眉"),CountArray(szDescribe));
			else if (pUserInfo->cbGender==GENDER_MANKIND) lstrcpyn(szDescribe,TEXT("帅哥"),CountArray(szDescribe));
			break;
		}
	case UD_UNDER_WRITE:	//个性签名
		{
			CW2CT strUnderWrite(pUserInfo->szUnderWrite);
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),(LPCTSTR)strUnderWrite);
			break;
		}
	case UD_TABLE:			//桌子号码
		{
			if (pUserInfo->wTableID==INVALID_TABLE) 
				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("无效"));
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->wTableID+1);
			break;
		}
	case UD_CHAIR:			//椅子号码
		{
			if (pUserInfo->wChairID==INVALID_TABLE) 
				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("无效"));
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->wChairID+1);
			break;
		}
	//case UD_GOLD:			//用户金币
	//	{
	//		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%I64d"),pUserInfo->lGold);
	//		break;
	//	}
	case UD_SCORE:			//用户积分
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%I64d"),pUserInfo->lScore);
			break;
		}
	case UD_SEGMENT:		//游戏段位
		{
			//if (pUserInfo->cbSegmentMode==0)
			//{
			//	if (pUserInfo->cbGameSegment<18)
			//	{
			//		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%2ld 级"),18-pUserInfo->cbGameSegment);
			//	}
			//	else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%2ld 段"),pUserInfo->cbGameSegment-17);
			//}
			//else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%2ld P"),pUserInfo->cbMasterSegment+1);
			break;
		}
	case UD_EXPERIENCE:		//用户经验
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lExperience);
			break;
		}
	case UD_WIN_COUNT:		//胜局盘数
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lWinCount);
			break;
		}
	case UD_LOST_COUNT:		//输局盘数
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lLostCount);
			break;
		}
	case UD_DRAW_COUNT:		//和局盘数
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lDrawCount);
			break;
		}
	case UD_FLEE_COUNT:		//逃局盘数
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lFleeCount);
			break;
		}
	case UD_PLAY_COUNT:		//游戏局数
		{
			LONG lPlayCount=pIClientUserItem->GetUserPlayCount();
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),lPlayCount);
			break;
		}
	case UD_WIN_RATE:		//用户胜率
		{
			float fWinRate=pIClientUserItem->GetUserWinRate();
			if (fWinRate==0.0f) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%4.2f%%"),fWinRate);
			break;
		}
	case UD_LOST_RATE:		//用户输率
		{
			float fLostRate=pIClientUserItem->GetUserLostRate();
			if (fLostRate==0.0f) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%4.2f%%"),fLostRate);
			break;
		}
	case UD_DRAW_RATE:		//用户和率
		{
			float fDrawRate=pIClientUserItem->GetUserDrawRate();
			if (fDrawRate==0.0f) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%4.2f%%"),fDrawRate);
			break;
		}
	case UD_FLEE_RATE:		//用户逃率
		{
			float fFleeRate=pIClientUserItem->GetUserFleeRate();
			if (fFleeRate==0.0f) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%4.2f%%"),fFleeRate);
			break;
		}
	case UD_GAME_LEVEL:		//游戏等级
		{
			//无效处理
			if (m_pIGameLevelParser==NULL)
			{
				ASSERT(FALSE);
				szDescribe[0]=0;
				break;
			}
				
			//获取等级
			return m_pIGameLevelParser->GetLevelDescribe(pIClientUserItem);
		}
	case UD_AREA:			//用户区域
		{
			CW2CT strArea(pUserInfo->szArea);
			lstrcpyn(szDescribe,strArea,CountArray(szDescribe));
			break;
		}
	case UD_CITY:			//用户城市
		{
			CW2CT strCity(pUserInfo->szCity);
			lstrcpyn(szDescribe,strCity,CountArray(szDescribe));
			break;
		}
	case UD_PROVINCE:		//用户省份
		{
			CW2CT strProvince(pUserInfo->szProvince);
			lstrcpyn(szDescribe,strProvince,CountArray(szDescribe));
			break;
		}
	case UD_NOTE_INFO:		//用户备注
		{
			CW2CT strUserNote(pIClientUserItem->GetUserNoteInfo());
			lstrcpyn(szDescribe,(LPCTSTR)strUserNote,CountArray(szDescribe));
			break;
		}
	case UD_LOOKON_USER:	//旁观用户
		{
			//无效处理
			if (m_pIGameUserManager==NULL)
			{
				ASSERT(FALSE);
				szDescribe[0]=0;
				break;
			}

			//获取用户
			if (pIClientUserItem->GetUserStatus()==US_LOOKON)
			{
				//获取信息
				WORD wChairID=pIClientUserItem->GetChairID();
				IClientUserItem * pITableUserItem=m_pIGameUserManager->GetTableUserItem(wChairID);

				//结果处理
				if ((pITableUserItem==NULL)||(pITableUserItem==pIClientUserItem))
				{
					szDescribe[0]=0;
					break;
				}

				//生成字段
				CW2CT strAccounts(pITableUserItem->GetAccounts());
				lstrcpyn(szDescribe,(LPCTSTR)strAccounts,CountArray(szDescribe));
			}
			else szDescribe[0]=0;

			break;
		}
	default:				//未知字段
		{
			szDescribe[0]=0;
			break;
		}
	}

	return szDescribe;
}

//排列函数
INT CALLBACK CUserListControl::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	//定义变量
	IClientUserItem * pIClientUserItem1=(IClientUserItem *)lParam1;
	IClientUserItem * pIClientUserItem2=(IClientUserItem *)lParam2;
    tagSortParameter * pSortParameter=(tagSortParameter *)lParamList;
	CUserListControl * pUserListControl=pSortParameter->pUserListControl;

	//自己置顶
	if (pIClientUserItem1->GetUserID()==pUserListControl->m_dwMySelfUserID) return SORT_POSITION_FRONT;
	if (pIClientUserItem2->GetUserID()==pUserListControl->m_dwMySelfUserID) return SORT_POSITION_AFTER;

	//对比数据
	switch (pSortParameter->cbDataDescribe)
	{
	case UD_GAME_ID:		//游戏标识
		{
			//获取变量
			DWORD dwGameID1=pIClientUserItem1->GetGameID();
			DWORD dwGameID2=pIClientUserItem2->GetGameID();

			//对比数据
			INT nResult=dwGameID1>dwGameID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_USER_ID:		//用户标识
		{
			//获取变量
			DWORD dwUserID1=pIClientUserItem1->GetGameID();
			DWORD dwUserID2=pIClientUserItem2->GetGameID();

			//对比数据
			INT nResult=dwUserID1>dwUserID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_ACCOUNTS:		//用户帐号
		{
			//获取变量
			BYTE cbCompanion1=pIClientUserItem1->GetUserCompanion();
			BYTE cbCompanion2=pIClientUserItem2->GetUserCompanion();

			//好友置顶
			if (((cbCompanion1==CP_FRIEND)&&(cbCompanion2!=CP_FRIEND))
				||((cbCompanion1!=CP_FRIEND)&&(cbCompanion2==CP_FRIEND)))
			{
				return (cbCompanion1==CP_FRIEND)?SORT_POSITION_FRONT:SORT_POSITION_AFTER;
			}

			//获取变量
			BYTE cbMasterRight1=pIClientUserItem1->GetMasterOrder();
			BYTE cbMasterRight2=pIClientUserItem2->GetMasterOrder();

			//管理置顶
			if (cbMasterRight1!=cbMasterRight2)
			{
				return (cbMasterRight1==cbMasterRight2)?SORT_POSITION_FRONT:SORT_POSITION_AFTER;
			}
			
			//获取变量
			BYTE cbMemberRight1=pIClientUserItem1->GetMemberOrder();
			BYTE cbMemberRight2=pIClientUserItem2->GetMemberOrder();

			//会员置顶
			if (cbMemberRight1!=cbMemberRight2)
			{
				return (cbMemberRight1==cbMemberRight2)?SORT_POSITION_FRONT:SORT_POSITION_AFTER;
			}

			//获取变量
			LPCWSTR pszAccounts1=pIClientUserItem1->GetAccounts();
			LPCWSTR pszAccounts2=pIClientUserItem2->GetAccounts();
			
			//对比数据
			INT nResult=wcscmp(pszAccounts1,pszAccounts2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_GENDER:			//用户性别
		{
			//获取变量
			BYTE cbGender1=pIClientUserItem1->GetGender();
			BYTE cbGender2=pIClientUserItem2->GetGender();

			//对比数据
			INT nResult=cbGender1>cbGender2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_UNDER_WRITE:	//个性签名
		{
			//获取变量
			LPCWSTR pszUnderWrite1=pIClientUserItem1->GetUnderWrite();
			LPCWSTR pszUnderWrite2=pIClientUserItem2->GetUnderWrite();

			//对比数据
			INT nResult=wcscmp(pszUnderWrite1,pszUnderWrite2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_TABLE:			//桌子号码
		{
			//获取变量
			WORD wTableID1=pIClientUserItem1->GetTableID();
			WORD wTableID2=pIClientUserItem2->GetTableID();

			//对比数据
			INT nResult=wTableID1>wTableID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_CHAIR:			//椅子号码
		{
			//获取变量
			WORD wChairID1=pIClientUserItem1->GetChairID();
			WORD wChairID2=pIClientUserItem2->GetChairID();

			//对比数据
			INT nResult=wChairID1>wChairID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	//case UD_GOLD:			//用户金币
	//	{
	//		//获取变量
	//		LONGLONG lGold1=pIClientUserItem1->GetUserGold();
	//		LONGLONG lGold2=pIClientUserItem2->GetUserGold();

	//		//对比数据
	//		INT nResult=lGold1>lGold2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	//		return (pSortParameter->bAscendSort)?nResult:-nResult;
	//	}
	case UD_SCORE:			//用户积分
		{
			//获取变量
			LONGLONG lScore1=pIClientUserItem1->GetUserScore();
			LONGLONG lScore2=pIClientUserItem2->GetUserScore();

			//对比数据
			INT nResult=lScore1>lScore2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_SEGMENT:		//游戏段位
		{
			////获取变量
			//WORD wGameSegment1=pIClientUserItem1->GetGameSegment();
			//WORD wGameSegment2=pIClientUserItem2->GetGameSegment();

			////对比数据
			//INT nResult=wGameSegment1>wGameSegment2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return 0; // (pSortParameter->bAscendSort) ? nResult : -nResult;
		}
	case UD_EXPERIENCE:		//用户经验
		{
			//获取变量
			LONG lExperience1=pIClientUserItem1->GetUserExperience();
			LONG lExperience2=pIClientUserItem2->GetUserExperience();

			//对比数据
			INT nResult=lExperience1>lExperience2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_WIN_COUNT:		//胜局盘数
		{
			//获取变量
			LONG lWinCount1=pIClientUserItem1->GetUserWinCount();
			LONG lWinCount2=pIClientUserItem2->GetUserWinCount();

			//对比数据
			INT nResult=lWinCount1>lWinCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_LOST_COUNT:		//输局盘数
		{
			//获取变量
			LONG lLostCount1=pIClientUserItem1->GetUserLostCount();
			LONG lLostCount2=pIClientUserItem2->GetUserLostCount();

			//对比数据
			INT nResult=lLostCount1>lLostCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_DRAW_COUNT:		//和局盘数
		{
			//获取变量
			LONG lDrawCount1=pIClientUserItem1->GetUserDrawCount();
			LONG lDrawCount2=pIClientUserItem2->GetUserDrawCount();

			//对比数据
			INT nResult=lDrawCount1>lDrawCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_FLEE_COUNT:		//逃局盘数
		{
			//获取变量
			LONG lFleeCount1=pIClientUserItem1->GetUserFleeCount();
			LONG lFleeCount2=pIClientUserItem2->GetUserFleeCount();

			//对比数据
			INT nResult=lFleeCount1>lFleeCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_PLAY_COUNT:		//游戏局数
		{
			//获取变量
			LONG lPlayCount1=pIClientUserItem1->GetUserPlayCount();
			LONG lPlayCount2=pIClientUserItem2->GetUserPlayCount();

			//对比数据
			INT nResult=lPlayCount1>lPlayCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_WIN_RATE:		//用户胜率
		{
			//获取变量
			float fWinRate1=pIClientUserItem1->GetUserWinRate();
			float fWinRate2=pIClientUserItem2->GetUserWinRate();

			//对比数据
			INT nResult=fWinRate1>fWinRate2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_LOST_RATE:		//用户输率
		{
			//获取变量
			float fLostRate1=pIClientUserItem1->GetUserLostRate();
			float fLostRate2=pIClientUserItem2->GetUserLostRate();

			//对比数据
			INT nResult=fLostRate1>fLostRate2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_DRAW_RATE:		//用户和率
		{
			//获取变量
			float fDrawRate1=pIClientUserItem1->GetUserDrawRate();
			float fDrawRate2=pIClientUserItem2->GetUserDrawRate();

			//对比数据
			INT nResult=fDrawRate1>fDrawRate2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_FLEE_RATE:		//用户逃率
		{
			//获取变量
			float fFleeRate1=pIClientUserItem1->GetUserFleeRate();
			float fFleeRate2=pIClientUserItem2->GetUserFleeRate();

			//对比数据
			INT nResult=fFleeRate1>fFleeRate2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_GAME_LEVEL:		//游戏等级
		{
			//获取变量
			LONGLONG lScore1=pIClientUserItem1->GetUserScore();
			LONGLONG lScore2=pIClientUserItem2->GetUserScore();

			//对比数据
			INT nResult=lScore1>lScore2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_AREA:			//用户区域
		{
			//获取变量
			LPCWSTR pszUserArea1=pIClientUserItem1->GetUserArea();
			LPCWSTR pszUserArea2=pIClientUserItem2->GetUserArea();

			//对比数据
			INT nResult=wcscmp(pszUserArea1,pszUserArea2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_CITY:			//用户城市
		{
			//获取变量
			LPCWSTR pszUserCity1=pIClientUserItem1->GetUserCity();
			LPCWSTR pszUserCity2=pIClientUserItem2->GetUserCity();

			//对比数据
			INT nResult=wcscmp(pszUserCity1,pszUserCity2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_PROVINCE:		//用户省份
		{
			//获取变量
			LPCWSTR pszUserProvince1=pIClientUserItem1->GetUserProvince();
			LPCWSTR pszUserProvince2=pIClientUserItem2->GetUserProvince();

			//对比数据
			INT nResult=wcscmp(pszUserProvince1,pszUserProvince2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_NOTE_INFO:		//用户备注
		{
			//获取变量
			LPCWSTR pszNoteInfo1=pIClientUserItem1->GetUserNoteInfo();
			LPCWSTR pszNoteInfo2=pIClientUserItem2->GetUserNoteInfo();

			//对比数据
			INT nResult=wcscmp(pszNoteInfo1,pszNoteInfo2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_STATUS:			//用户状态
		{
			//获取变量
			BYTE cbUserStatus1=pIClientUserItem1->GetUserStatus();
			BYTE cbUserStatus2=pIClientUserItem2->GetUserStatus();

			//对比数据
			INT nResult=cbUserStatus1>cbUserStatus2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	}

	return 0;
}

//销毁消息
VOID CUserListControl::OnNcDestroy()
{
	//资源变量
	m_dwMySelfUserID=0L;
	m_bAscendSort=false;
	m_bCreateColumn=false;

	//接口变量
	m_pIGameLevelParser=NULL;
	m_pIGameUserManager=NULL;

	//列表信息
	m_wColumnCount=0;
	ZeroMemory(m_cbDataDescribe,sizeof(m_cbDataDescribe));

	__super::OnNcDestroy();
}

//绘画背景
BOOL CUserListControl::OnEraseBkgnd(CDC * pDC)
{
	//绘画背景
	if (m_UserListHeaderCtrl.m_hWnd!=NULL)
	{
		//获取位置
		CRect rcClient;
		CRect rcHeaderCtrl;
		GetClientRect(&rcClient);
		m_UserListHeaderCtrl.GetWindowRect(&rcHeaderCtrl);

		//绘画背景
		rcClient.top=rcHeaderCtrl.Height();
		pDC->FillSolidRect(&rcClient,GetBkColor());

		return TRUE;
	}

	return __super::OnEraseBkgnd(pDC);
}

//点击消息
VOID CUserListControl::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult) 
{
	//变量定义
	NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

	//排列数据
	if (m_wColumnCount!=0)
	{
		//变量定义
		tagSortParameter SortParameter;
		ZeroMemory(&SortParameter,sizeof(SortParameter));

		//设置变量
		SortParameter.pUserListControl=this;
		SortParameter.bAscendSort=m_bAscendSort;
		SortParameter.cbDataDescribe=m_cbDataDescribe[pNMListView->iSubItem];

		//设置变量
		m_bAscendSort=!m_bAscendSort;
		m_UserListHeaderCtrl.SetSortArrow(pNMListView->iSubItem,(m_bAscendSort==true)?SortType_Ascend:SortType_Descale);

		//排列列表
		SortItems(SortFunction,(LPARAM)&SortParameter);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSearchUserListControl::CSearchUserListControl()
{
	//资源变量
	m_bAscendSort=false;
	m_bCreateColumn=false;
	m_dwMySelfUserID=0L;

	//列表信息
	m_wColumnCount=0;
	ZeroMemory(m_cbDataDescribe,sizeof(m_cbDataDescribe));

	//资源变量
	ZeroMemory(&m_ColorUserList,sizeof(m_ColorUserList));

	return;
}

//析构函数
CSearchUserListControl::~CSearchUserListControl()
{
}

//控件绑定
VOID CSearchUserListControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//设置滚动
	m_SkinScrollBar.InitScroolBar(this);

	//设置属性
	SetExtendedStyle(LIST_STYTE);
	m_UserListHeaderCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	//设置字体
	if (GetHeaderCtrl()!=NULL)
	{
		CFont TitleFont;
		TitleFont.CreatePointFont(130,TEXT("宋体"));
		m_UserListHeaderCtrl.SetFont(&TitleFont);
	}

	//用户标志
	m_cbDataDescribe[m_wColumnCount]=UD_NULL;
	InsertColumn(m_wColumnCount++,TEXT(""),LVCFMT_CENTER,0,0);

	//用户标志
	m_cbDataDescribe[m_wColumnCount]=UD_FLAG;
	InsertColumn(m_wColumnCount++,TEXT(""),LVCFMT_CENTER,WIDTH_STATUS,0);

	//用户状态
	m_cbDataDescribe[m_wColumnCount]=UD_STATUS;
	InsertColumn(m_wColumnCount++,TEXT(""),LVCFMT_CENTER,WIDTH_STATUS,0);

	//用户名称
	m_cbDataDescribe[m_wColumnCount] = UD_ACCOUNTS;
	InsertColumn(m_wColumnCount++,TEXT("用户名"),LVCFMT_LEFT,104);

	//游戏ID
	m_cbDataDescribe[m_wColumnCount] = UD_GAME_ID;
	InsertColumn(m_wColumnCount++,TEXT("游戏 ID"),LVCFMT_LEFT,62);

	//当前位置
	m_cbDataDescribe[m_wColumnCount] = UD_LOCATION;
	InsertColumn(m_wColumnCount++,TEXT("当前位置"),LVCFMT_LEFT,150);

	return;
}

//绘画函数
VOID CSearchUserListControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//变量定义
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	//获取属性
	INT nItemID=lpDrawItemStruct->itemID;
	INT nColumnCount=m_UserListHeaderCtrl.GetItemCount();

	//定制变量
	COLORREF rcTextColor=m_ColorUserList.crNormalTX;
	COLORREF rcBackColor=m_ColorUserList.crNormalBK;
	IGlobalUserItem * pIGlobalUserItem=(IGlobalUserItem *)lpDrawItemStruct->itemData;

	//颜色定制
	if (lpDrawItemStruct->itemState&ODS_SELECTED)
	{
		//选择颜色
		rcTextColor=m_ColorUserList.crSelectTX;
		rcBackColor=m_ColorUserList.crSelectBK;
	}
	else if (pIGlobalUserItem->GetUserID()==m_dwMySelfUserID)
	{
		//自己颜色
		rcTextColor=m_ColorUserList.crMyselfTX;
		rcBackColor=m_ColorUserList.crMasterBK;
	}

	//绘画区域
	CRect rcClipBox;
	pDC->GetClipBox(&rcClipBox);

	//设置 DC
	pDC->SetBkColor(rcBackColor);
	pDC->SetTextColor(rcTextColor);

	//绘画选择
	if (lpDrawItemStruct->itemState&ODS_FOCUS)
	{
		COLORREF rcFillColor=GetBkColor();
		pDC->Draw3dRect(&lpDrawItemStruct->rcItem,rcFillColor,rcFillColor);
	}

	//绘画子项
	for (INT i=1;i<nColumnCount;i++)
	{
		//获取位置
		CRect rcSubItem;
		GetSubItemRect(nItemID,i,LVIR_LABEL,rcSubItem);

		//设置边界
		if (i<=ENJOIN_COUNT) rcItem.left=rcSubItem.left;

		//绘画判断
		if (rcSubItem.left>rcClipBox.right) break;
		if (rcSubItem.right<rcClipBox.left) continue;
		if (rcSubItem.left==rcSubItem.right) continue;

		//绘画背景
		pDC->FillSolidRect(&rcSubItem,rcBackColor);

		//获取文字
		TCHAR szString[256]=TEXT("");
		GetItemText(nItemID,i,szString,CountArray(szString));

		//绘画文字
		rcSubItem.left+=5;
		pDC->DrawText(szString,lstrlen(szString),&rcSubItem,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	//绘画选择
	if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->DrawFocusRect(&rcItem);

	return;
}

//设置接口
bool CSearchUserListControl::SetMySelfUserID(DWORD dwMySelfUserID)
{
	//设置变量
	m_dwMySelfUserID=dwMySelfUserID;

	return true;
}

//设置颜色
bool CSearchUserListControl::SetColorUserList(const tagColorUserList & ColorUserList)
{
	//设置变量
	m_ColorUserList=ColorUserList;

	//更新界面
	if (m_hWnd!=NULL)
	{
		//设置背景
		SetBkColor(ColorUserList.crNormalBK);

		//重合窗口
		RedrawWindow();
	}

	return true;
}

//设置列表
bool CSearchUserListControl::SetColumnDescribe(const tagColumnItem ColumnItem[], WORD wColumnCount)
{
	//构造判断
	if (m_bCreateColumn==true) return true;

	//调整参数
	ASSERT(wColumnCount<(CountArray(m_cbDataDescribe)-m_wColumnCount));
	wColumnCount=__min(CountArray(m_cbDataDescribe)-m_wColumnCount,wColumnCount);

	//设置列表
	SetRedraw(FALSE);
	for (WORD i=0;i<wColumnCount;i++)
	{
		CW2CT strColumnName(ColumnItem[i].szColumnName);
		m_cbDataDescribe[m_wColumnCount]=ColumnItem[i].cbDataDescribe;
		InsertColumn(m_wColumnCount++,strColumnName,LVCFMT_LEFT,ColumnItem[i].cbColumnWidth);
	}
	SetRedraw(TRUE);

	//设置变量
	m_bCreateColumn=true;

	return true;
}

//插入用户
bool CSearchUserListControl::InsertUserItem(IGlobalUserItem * pIGlobalUserItem)
{
	//效验状态
	ASSERT(pIGlobalUserItem!=NULL);
	if (m_hWnd==NULL) return false;

	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIGlobalUserItem;
	INT nItem=FindItem(&FindInfo);
	if (nItem!=-1)
	{
		UpdateUserItem(pIGlobalUserItem);
		return true;
	}

	//插入用户
	for (WORD i=0;i<m_wColumnCount;i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pIGlobalUserItem);
			INT nImageIndex=GetImageIndex(pIGlobalUserItem,i);
			nItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,TEXT(""),0,0,0,(LPARAM)pIGlobalUserItem);
		}
		else
		{
			//图形子项
			if (m_cbDataDescribe[i]==UD_NULL) continue;
			if (m_cbDataDescribe[i]>=UD_IMAGE) continue;

			//字符子项
			SetItem(nItem,i,LVIF_TEXT,GetDescribeString(pIGlobalUserItem,i),0,0,0,0);
		}
	}

	return true;
}

//更新用户
bool CSearchUserListControl::UpdateUserItem(IGlobalUserItem * pIGlobalUserItem)
{
	//效验状态
	ASSERT(pIGlobalUserItem!=NULL);
	if (GetSafeHwnd()==NULL) return false;

	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIGlobalUserItem;
	INT nItem=FindItem(&FindInfo);
	if (nItem==-1) return false;

	//更新用户
	for (WORD i=0;i<m_wColumnCount;i++)
	{
		//图形子项
		if (m_cbDataDescribe[i]==UD_NULL) continue;
		if (m_cbDataDescribe[i]>=UD_IMAGE) continue;

		//字符子项
		SetItem(nItem,i,LVIF_TEXT,GetDescribeString(pIGlobalUserItem,i),0,0,0,0);
	}

	//更新子项
	RedrawItems(nItem,nItem);

	return true;
}

//删除用户
bool CSearchUserListControl::DeleteUserItem(IGlobalUserItem * pIGlobalUserItem)
{
	//效验状态
	ASSERT(pIGlobalUserItem!=NULL);
	if (m_hWnd==NULL) return false;

	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//构造变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIGlobalUserItem;

	//删除子项
	INT nItem=FindItem(&FindInfo);
	if (nItem!=-1) 
	{
		DeleteItem(nItem);
		return true;
	}

	return false;
}

//获取位置
INT CSearchUserListControl::GetInsertIndex(IGlobalUserItem * pIGlobalUserItem)
{
	//变量定义
	INT nItemCount=GetItemCount();

	//自己判断
	if (pIGlobalUserItem->GetUserID()==m_dwMySelfUserID) return 0;

	//普通用户
	return nItemCount;
}

//获取位图
UINT CSearchUserListControl::GetImageIndex(IGlobalUserItem * pIGlobalUserItem, WORD wColumnIndex)
{
	//效验参数
	ASSERT(pIGlobalUserItem!=NULL);
	ASSERT(wColumnIndex<m_wColumnCount);

	return INDEX_INVALID;
}

//描述字符
LPCTSTR CSearchUserListControl::GetDescribeString(IGlobalUserItem * pIGlobalUserItem, WORD wColumnIndex)
{
	//效验参数
	ASSERT(pIGlobalUserItem!=NULL);
	ASSERT(wColumnIndex<m_wColumnCount);
	ASSERT(m_cbDataDescribe[wColumnIndex]<UD_IMAGE);

	//变量定义
	static TCHAR szDescribe[512]=TEXT("");

	//构造字符
	switch (m_cbDataDescribe[wColumnIndex])
	{
	case UD_GAME_ID:		//游戏标识
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pIGlobalUserItem->GetGameID());
			break;
		}
	case UD_USER_ID:		//用户标识
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pIGlobalUserItem->GetUserID());
			break;
		}
	case UD_ACCOUNTS:		//用户帐号
		{
			CW2CT strAccounts(pIGlobalUserItem->GetAccounts());
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),(LPCTSTR)strAccounts);
			break;
		}
	case UD_LOCATION:		//用户位置
		{
			szDescribe[0] = 0;

			//获取房间信息
			WORD wDataSize = 0;
			LPBYTE lpBuffer = (LPBYTE)pIGlobalUserItem->GetExtraInfo(wDataSize,DTP_GR_SERVER_INFO);
			if( wDataSize > 0 && lpBuffer != NULL )
			{
				//提取房间信息
				WORD wProcessSize = 0;
				do 
				{
					tagGameServerItem *pGameServerItem = (tagGameServerItem *)(lpBuffer+wProcessSize);

					if( szDescribe[0] != 0 )
						StringCbCat( szDescribe,CountArray(szDescribe),TEXT("、") );

					CW2CT strServerName(pGameServerItem->szServerName);
					StringCbCat( szDescribe,CountArray(szDescribe),(LPCTSTR)strServerName );

					wProcessSize += sizeof(tagGameServerItem)-sizeof(pGameServerItem->szServerName)+
						CountStringBufferW(pGameServerItem->szServerName);
				} while (wProcessSize < wDataSize );
			}

			break;
		}
	default:				//未知字段
		{
			szDescribe[0]=0;
			break;
		}
	}

	return szDescribe;
}

//排列函数
INT CALLBACK CSearchUserListControl::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	//定义变量
	IGlobalUserItem * pIUserItem1=(IGlobalUserItem *)lParam1;
	IGlobalUserItem * pIUserItem2=(IGlobalUserItem *)lParam2;
	tagSearchSortParameter * pSortParameter=(tagSearchSortParameter *)lParamList;
	CSearchUserListControl * pListControl=pSortParameter->pListControl;

	//自己置顶
	if (pIUserItem1->GetUserID()==pListControl->m_dwMySelfUserID) return SORT_POSITION_FRONT;
	if (pIUserItem2->GetUserID()==pListControl->m_dwMySelfUserID) return SORT_POSITION_AFTER;

	//对比数据
	switch (pSortParameter->cbDataDescribe)
	{
	case UD_GAME_ID:		//游戏标识
		{
			//获取变量
			DWORD dwGameID1=pIUserItem1->GetGameID();
			DWORD dwGameID2=pIUserItem2->GetGameID();

			//对比数据
			INT nResult=dwGameID1>dwGameID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_USER_ID:		//用户标识
		{
			//获取变量
			DWORD dwUserID1=pIUserItem1->GetGameID();
			DWORD dwUserID2=pIUserItem2->GetGameID();

			//对比数据
			INT nResult=dwUserID1>dwUserID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_ACCOUNTS:		//用户帐号
		{
			//获取变量
			LPCWSTR pszAccounts1=pIUserItem1->GetAccounts();
			LPCWSTR pszAccounts2=pIUserItem2->GetAccounts();

			//对比数据
			INT nResult=wcscmp(pszAccounts1,pszAccounts2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_NOTE_INFO:		//用户备注
		{
			WORD wDataSize1 = 0, wDataSize2 = 0;
			tagGameServerItem *pServerInfo1 = (tagGameServerItem *)pIUserItem1->GetExtraInfo(wDataSize1,DTP_GR_SERVER_INFO);
			tagGameServerItem *pServerInfo2 = (tagGameServerItem *)pIUserItem2->GetExtraInfo(wDataSize2,DTP_GR_SERVER_INFO);

			if( pServerInfo1 == NULL || pServerInfo2 == NULL )
			{
				if( pServerInfo1 == NULL && pServerInfo2 != NULL )
					return (pSortParameter->bAscendSort)?SORT_POSITION_FRONT:SORT_POSITION_AFTER;
				else if( pServerInfo1 != NULL && pServerInfo2 == NULL )
					return (pSortParameter->bAscendSort)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
				else return SORT_POSITION_FRONT;
			}

			//对比数据
			INT nResult=wcscmp(pServerInfo1->szServerName,pServerInfo2->szServerName);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	}

	return 0;
}

//销毁消息
VOID CSearchUserListControl::OnNcDestroy()
{
	//资源变量
	m_dwMySelfUserID=0L;
	m_bAscendSort=false;
	m_bCreateColumn=false;

	//列表信息
	m_wColumnCount=0;
	ZeroMemory(m_cbDataDescribe,sizeof(m_cbDataDescribe));

	__super::OnNcDestroy();
}

//绘画背景
BOOL CSearchUserListControl::OnEraseBkgnd(CDC * pDC)
{
	//绘画背景
	if (m_UserListHeaderCtrl.m_hWnd!=NULL)
	{
		//获取位置
		CRect rcClient;
		CRect rcHeaderCtrl;
		GetClientRect(&rcClient);
		m_UserListHeaderCtrl.GetWindowRect(&rcHeaderCtrl);

		//绘画背景
		rcClient.top=rcHeaderCtrl.Height();
		pDC->FillSolidRect(&rcClient,GetBkColor());

		return TRUE;
	}

	return __super::OnEraseBkgnd(pDC);
}

//点击消息
VOID CSearchUserListControl::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult) 
{
	//变量定义
	NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

	//排列数据
	if (m_wColumnCount!=0)
	{
		//变量定义
		tagSearchSortParameter SortParameter;
		ZeroMemory(&SortParameter,sizeof(SortParameter));

		//设置变量
		SortParameter.pListControl=this;
		SortParameter.bAscendSort=m_bAscendSort;
		SortParameter.cbDataDescribe=m_cbDataDescribe[pNMListView->iSubItem];

		//设置变量
		m_bAscendSort=!m_bAscendSort;
		m_UserListHeaderCtrl.SetSortArrow(pNMListView->iSubItem,(m_bAscendSort==true)?SortType_Ascend:SortType_Descale);

		//排列列表
		SortItems(SortFunction,(LPARAM)&SortParameter);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////