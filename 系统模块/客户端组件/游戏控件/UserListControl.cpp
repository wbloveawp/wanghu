#include "StdAfx.h"
#include "Resource.h"
#include "UserListControl.h"

//////////////////////////////////////////////////////////////////////////

//��ֹ����
#define ENJOIN_COUNT				3									//��ֹ����

//Ĭ������
#define LIST_STYTE					LVS_EX_SUBITEMIMAGES|LVS_EX_FULLROWSELECT		\
									|LVS_EX_INFOTIP/*|LVS_EX_FLATSB*/					\
									|LVS_EX_ONECLICKACTIVATE

//////////////////////////////////////////////////////////////////////////

//����λ��
#define SORT_POSITION_AFTER			1									//����λ��
#define SORT_POSITION_FRONT			-1									//ǰ��λ��

//////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagSortParameter
{
	bool							bAscendSort;						//�����־
	BYTE							cbDataDescribe;						//�ֶ�����
	CUserListControl *				pUserListControl;					//�б�ؼ�
};

//������Ϣ
struct tagSearchSortParameter
{
	bool							bAscendSort;						//�����־
	BYTE							cbDataDescribe;						//�ֶ�����
	CSearchUserListControl *		pListControl;						//�б�ؼ�
};

//////////////////////////////////////////////////////////////////////////
//��̬����

CImageList			CUserListControl::m_ExperienceImage;				//����λͼ
CImageList			CUserListControl::m_UserStatusImage;				//״̬λͼ

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

//���캯��
CUserListHeaderCtrl::CUserListHeaderCtrl()
{
}

//��������
CUserListHeaderCtrl::~CUserListHeaderCtrl()
{
}

//�ػ�����
VOID CUserListHeaderCtrl::OnPaint()
{
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcRect;
	GetClientRect(&rcRect);

	//��������
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc, rcRect.Width(), rcRect.Height());

	//����
	CFont TitleFont;
	TitleFont.CreatePointFont(100, TEXT("����"));

	//���� DC
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(&BufferBmp);
	BufferDC.SetTextColor(RGB(0, 0, 0));
	BufferDC.SelectObject(TitleFont);

	//����λͼ
	//CImageHandle ImageHandleL(&CSkinListCtrl::m_SkinAttribute.m_SkinImageL);
	//CImageHandle ImageHandleM(&CSkinListCtrl::m_SkinAttribute.m_SkinImageM);
	//CImageHandle ImageHandleR(&CSkinListCtrl::m_SkinAttribute.m_SkinImageR);

	//�滭����
	//ImageHandleM->StretchBlt(BufferDC, 0, 0, rcRect.Width(), rcRect.Height());
	BufferDC.FillSolidRect(0, 0, rcRect.Width(), rcRect.Height(), RGB(191, 144, 78));

	//�滭����
	CRect rcItem;
	HDITEM HDItem;
	TCHAR szBuffer[64];
	for (INT i = 0; i < GetItemCount(); i++)
	{
		//�������
		HDItem.mask = HDI_TEXT;
		HDItem.pszText = szBuffer;
		HDItem.cchTextMax = CountArray(szBuffer);

		//��ȡ��Ϣ
		GetItem(i, &HDItem);
		GetItemRect(i, &rcItem);

		//�滭����
		//INT nLeftWidth = ImageHandleL->GetWidth();
		//INT nRightWidth = ImageHandleR->GetWidth();
		//ImageHandleL->StretchBlt(BufferDC, rcItem.left, rcItem.top, nLeftWidth, rcItem.Height());
		//ImageHandleM->StretchBlt(BufferDC, rcItem.left + nLeftWidth, rcItem.top, rcItem.Width() - nLeftWidth - nRightWidth, rcItem.Height());
		//ImageHandleR->StretchBlt(BufferDC, rcItem.right - nRightWidth, rcItem.top, nRightWidth, rcItem.Height());
		BufferDC.FillSolidRect(rcItem.left, rcItem.top+1, 1, rcItem.Height()-2, RGB(50, 30, 17));

		//�滭����
		rcItem.DeflateRect(3, 1, 3, 1);
		const UINT uFormat = DT_WORDBREAK | DT_EDITCONTROL | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_CENTER;
		BufferDC.DrawText(szBuffer, lstrlen(szBuffer), &rcItem, uFormat);
	}

	//�滭����
	dc.BitBlt(0, 0, rcRect.Width(), rcRect.Height(), &BufferDC, 0, 0, SRCCOPY);

	//������Դ
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();

	return;
}

//��������
BOOL CUserListHeaderCtrl::OnEraseBkgnd(CDC* pDC)
{
	//ˢ�½���
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//���ñ�־
VOID CUserListHeaderCtrl::SetSortArrow(INT nSortColumn, enSortType SortType)
{
	//���ñ���
	m_SortType = SortType;
	m_nColumn = nSortColumn;

	//�ػ�����
	Invalidate(FALSE);

	return;
}

//��ȡ��־
VOID CUserListHeaderCtrl::GetSortArrow(INT& nSortColumn, enSortType& SortType)
{
	//���ñ���
	SortType = m_SortType;
	nSortColumn = m_nColumn;

	return;
}

//�ؼ���Ϣ
BOOL CUserListHeaderCtrl::OnChildNotify(UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT * pLResult)
{
	//��������
	NMHEADER * pNMHearder=(NMHEADER*)lParam;

	//�϶���Ϣ
	if ((pNMHearder->hdr.code==HDN_BEGINTRACKA)||(pNMHearder->hdr.code==HDN_BEGINTRACKW))
	{
		//��ֹ�϶�
		if (pNMHearder->iItem<ENJOIN_COUNT)
		{
			*pLResult=TRUE;
			return TRUE;
		}
	}

	return __super::OnChildNotify(uMessage,wParam,lParam,pLResult);
}

//�����Ϣ
BOOL CUserListHeaderCtrl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CUserListControl::CUserListControl()
{
	//��Դ����
	m_dwMySelfUserID=0L;
	m_bAscendSort=false;
	m_bCreateColumn=false;

	//�ӿڱ���
	m_pIGameLevelParser=NULL;
	m_pIGameUserManager=NULL;

	//�б���Ϣ
	m_wColumnCount=0;
	ZeroMemory(m_cbDataDescribe,sizeof(m_cbDataDescribe));

	//��Դ����
	ZeroMemory(&m_ColorUserList,sizeof(m_ColorUserList));

	//Ĭ�϶���
	m_ColorUserList.crSelectTX = COLOR_SELECT_TX;
	m_ColorUserList.crSelectBK = COLOR_SELECT_BK;
	m_ColorUserList.crNormalTX = COLOR_NORMAL_TX;
	m_ColorUserList.crNormalBK = COLOR_NORMAL_BK;

	//��Ҷ���
	m_ColorUserList.crMyselfTX = COLOR_MYSELF_TX;
	m_ColorUserList.crMyselfBK = COLOR_MYSELF_BK;
	m_ColorUserList.crMasterTX = COLOR_MASTER_TX;
	m_ColorUserList.crMasterBK = COLOR_MASTER_BK;
	m_ColorUserList.crMemberTX = COLOR_MEMBER_TX;
	m_ColorUserList.crMemberBK = COLOR_MEMBER_BK;

	return;
}

//��������
CUserListControl::~CUserListControl()
{
}

//�ؼ���
VOID CUserListControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//���ù���
	m_SkinScrollBar.InitScroolBar(this);

	//��������
	SetExtendedStyle(LIST_STYTE);
	m_UserListHeaderCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	//���ؾ���
	if (m_ExperienceImage.m_hImageList==NULL)
	{
		//����ͼƬ
		CBitmap Image;
		BITMAP ImageInfo;
		Image.LoadBitmap(IDB_USER_EXPERIENCE1);

		//����ͼƬ
		Image.GetBitmap(&ImageInfo);
		m_ExperienceImage.Create(ImageInfo.bmWidth/3,ImageInfo.bmHeight,ILC_COLOR16|ILC_MASK,0,0);

		//����ͼƬ
		m_ExperienceImage.Add(&Image, RGB(255, 0, 255));
	}

	//����״̬
	if (m_UserStatusImage.m_hImageList==NULL)
	{
		//����ͼƬ
		CBitmap Image;
		BITMAP ImageInfo;
		Image.LoadBitmap(IDB_USER_STATUS_IMAGE);

		//����ͼƬ
		Image.GetBitmap(&ImageInfo);
		m_UserStatusImage.Create(WIDTH_STATUS,ImageInfo.bmHeight,ILC_COLOR16|ILC_MASK,0,0);

		//����ͼƬ
		m_UserStatusImage.Add(&Image, RGB(255, 0, 255));
	}

	//���ÿؼ�
	SetInsertMarkColor(m_ColorUserList.crNormalBK);
	SetBkColor(m_ColorUserList.crNormalBK);
	SetImageList(&m_UserStatusImage, LVSIL_SMALL);

	//��������
	if (GetHeaderCtrl()!=NULL)
	{
		CFont TitleFont;
		TitleFont.CreatePointFont(130,TEXT("����"));
		m_UserListHeaderCtrl.SetFont(&TitleFont);
	}

	//�û���־
	m_cbDataDescribe[m_wColumnCount]=UD_NULL;
	InsertColumn(m_wColumnCount++,TEXT(""),LVCFMT_CENTER,0,0);

	//�û���־
	m_cbDataDescribe[m_wColumnCount]=UD_FLAG;
	InsertColumn(m_wColumnCount++,TEXT("��ʶ"),LVCFMT_CENTER,40,0);

	//�û�״̬
	m_cbDataDescribe[m_wColumnCount]=UD_STATUS;
	InsertColumn(m_wColumnCount++,TEXT("״̬"),LVCFMT_CENTER, 40,0);

	//�û�����
	m_cbDataDescribe[m_wColumnCount] = UD_ACCOUNTS;
	InsertColumn(m_wColumnCount++, TEXT("�û���"), LVCFMT_LEFT, 100);

	//��ϷID
	m_cbDataDescribe[m_wColumnCount] = UD_GAME_ID;
	InsertColumn(m_wColumnCount++, TEXT("��ϷID"), LVCFMT_LEFT, 62);

	//���Ӻ���
	m_cbDataDescribe[m_wColumnCount] = UD_TABLE;
	InsertColumn(m_wColumnCount++, TEXT("���Ӻ���"), LVCFMT_LEFT, 62);

	//���Ӻ���
	m_cbDataDescribe[m_wColumnCount] = UD_CHAIR;
	InsertColumn(m_wColumnCount++, TEXT("���Ӻ���"), LVCFMT_LEFT, 62);

	//�û�����
	m_cbDataDescribe[m_wColumnCount] = UD_SCORE;
	InsertColumn(m_wColumnCount++, TEXT("�û�����"), LVCFMT_LEFT, 90);

	//��Ϸ����
	m_cbDataDescribe[m_wColumnCount] = UD_PLAY_COUNT;
	InsertColumn(m_wColumnCount++, TEXT("��Ϸ����"), LVCFMT_LEFT, 62);

	//�û�ʤ��
	m_cbDataDescribe[m_wColumnCount] = UD_WIN_RATE;
	InsertColumn(m_wColumnCount++, TEXT("�û�ʤ��"), LVCFMT_LEFT, 62);

	//�û�����
	m_cbDataDescribe[m_wColumnCount] = UD_FLEE_RATE;
	InsertColumn(m_wColumnCount++, TEXT("�û�����"), LVCFMT_LEFT, 62);

	return;
}

//�滭����
VOID CUserListControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//��������
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	//��ȡ����
	INT nItemID=lpDrawItemStruct->itemID;
	INT nColumnCount=m_UserListHeaderCtrl.GetItemCount();

	//���Ʊ���
	COLORREF rcTextColor=m_ColorUserList.crNormalTX;
	COLORREF rcBackColor=m_ColorUserList.crNormalBK;
	IClientUserItem * pIClientUserItem=(IClientUserItem *)lpDrawItemStruct->itemData;

	//��ɫ����
	if (lpDrawItemStruct->itemState&ODS_SELECTED)
	{
		//ѡ����ɫ
		rcTextColor=m_ColorUserList.crSelectTX;
		rcBackColor=m_ColorUserList.crSelectBK;
	}
	else if (pIClientUserItem->GetUserID()==m_dwMySelfUserID)
	{
		//�Լ���ɫ
		rcTextColor=m_ColorUserList.crMyselfTX;
		rcBackColor=m_ColorUserList.crMasterBK;
	}
	else if (pIClientUserItem->GetMasterOrder()!=0)
	{
		//������ɫ
		rcTextColor=m_ColorUserList.crMasterTX;
		rcBackColor=m_ColorUserList.crMasterBK;
	}
	else if (pIClientUserItem->GetMemberOrder()!=0)
	{
		//��Ա��ɫ
		rcTextColor=m_ColorUserList.crMemberTX;
		rcBackColor=m_ColorUserList.crMemberBK;
	}

	//�����ж�
	if (pIClientUserItem->IsRobot())
	{
		rcTextColor = COLOR_ANDROID_TX;
		rcBackColor = COLOR_ANDROID_BK;
	}

	//�滭����
	CRect rcClipBox;
	pDC->GetClipBox(&rcClipBox);

	//���� DC
	pDC->SetBkColor(rcBackColor);
	pDC->SetTextColor(rcTextColor);

	//�滭ѡ��
	if (lpDrawItemStruct->itemState&ODS_FOCUS)
	{
		COLORREF rcFillColor=GetBkColor();
		pDC->Draw3dRect(&lpDrawItemStruct->rcItem,rcFillColor,rcFillColor);
	}

	//�滭����
	for (INT i=1;i<nColumnCount;i++)
	{
		//��ȡλ��
		CRect rcSubItem;
		GetSubItemRect(nItemID,i,LVIR_LABEL,rcSubItem);

		//���ñ߽�
		//if (i<=ENJOIN_COUNT) rcItem.left=rcSubItem.left;

		//�滭�ж�
		if (rcSubItem.left>rcClipBox.right) break;
		if (rcSubItem.right<rcClipBox.left) continue;
		if (rcSubItem.left==rcSubItem.right) continue;

		//�滭����
		//if (i>=ENJOIN_COUNT) 
			pDC->FillSolidRect(&rcSubItem,rcBackColor);

		//�滭��Ϣ
		if (i<ENJOIN_COUNT)
		{
			//�滭��־
			UINT uImageIndex=GetImageIndex(pIClientUserItem,i);
			if (uImageIndex!=INDEX_INVALID) m_UserStatusImage.Draw(pDC,uImageIndex,CPoint(rcSubItem.left,rcSubItem.top),ILD_TRANSPARENT);
		}
		else if (m_cbDataDescribe[i]==UD_EXPERIENCE)
		{
			//��ȡ����
			BYTE cbOrderIndex[MAX_EXPERIENCE];
			WORD wIndexCount=pIClientUserItem->GetExperienceIndex(cbOrderIndex,CountArray(cbOrderIndex));

			//����λ��
			INT nStartPos=rcSubItem.left;
			nStartPos+=__max(0,((rcSubItem.Width()-wIndexCount*(WIDTH_EXPERIENCE+2))-2)/2);

			//�滭����
			for (WORD j=0;j<wIndexCount;j++)
			{
				CPoint DrawPoint(nStartPos+j*(WIDTH_EXPERIENCE+2),rcSubItem.top);
				m_ExperienceImage.Draw(pDC,cbOrderIndex[j],DrawPoint,ILD_TRANSPARENT);
			}
		}
		else
		{
			//��ȡ����
			TCHAR szString[256]=TEXT("");
			GetItemText(nItemID,i,szString,CountArray(szString));

			//�滭����
			rcSubItem.left+=5;
			pDC->DrawText(szString,lstrlen(szString),&rcSubItem,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		}
	}

	//�滭ѡ��
	if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->DrawFocusRect(&rcItem);

	return;
}

//���ýӿ�
bool CUserListControl::SetMySelfUserID(DWORD dwMySelfUserID)
{
	//���ñ���
	m_dwMySelfUserID=dwMySelfUserID;

	return true;
}

//���ýӿ�
bool CUserListControl::SetGameLevelParser(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameLevelParser)!=NULL);
		m_pIGameLevelParser=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameLevelParser);

		//�ɹ��ж�
		if (m_pIGameLevelParser==NULL) return false;
	}
	else m_pIGameLevelParser=NULL;

	return true;
}

//���ýӿ�
bool CUserListControl::SetGameUserManager(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameUserManager)!=NULL);
		m_pIGameUserManager=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameUserManager);

		//�ɹ��ж�
		if (m_pIGameUserManager==NULL) return false;
	}
	else m_pIGameUserManager=NULL;

	return true;
}

//������ɫ
bool CUserListControl::SetColorUserList(const tagColorUserList & ColorUserList)
{
	//���ñ���
	m_ColorUserList=ColorUserList;

	//���½���
	if (m_hWnd!=NULL)
	{
		//���ñ���
		SetBkColor(ColorUserList.crNormalBK);

		//�غϴ���
		RedrawWindow();
	}

	return true;
}

//�����б�
bool CUserListControl::SetColumnDescribe(const tagColumnItem ColumnItem[], WORD wColumnCount)
{
	//�����ж�
	if (m_bCreateColumn==true) return true;

	//��������
	ASSERT(wColumnCount<(CountArray(m_cbDataDescribe)-m_wColumnCount));
	wColumnCount=__min(CountArray(m_cbDataDescribe)-m_wColumnCount,wColumnCount);

	//�����б�
	SetRedraw(FALSE);
	for (WORD i=0;i<wColumnCount;i++)
	{
		CW2CT strColumnName(ColumnItem[i].szColumnName);
		m_cbDataDescribe[m_wColumnCount]=ColumnItem[i].cbDataDescribe;
		InsertColumn(m_wColumnCount++,strColumnName,LVCFMT_LEFT,ColumnItem[i].cbColumnWidth);
	}
	SetRedraw(TRUE);

	//���ñ���
	m_bCreateColumn=true;

	return true;
}

//�����û�
bool CUserListControl::InsertUserItem(IClientUserItem * pIClientUserItem)
{
	//Ч��״̬
	ASSERT(pIClientUserItem!=NULL);
	if (m_hWnd==NULL) return false;
	
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIClientUserItem;
	INT nItem=FindItem(&FindInfo);
	if (nItem!=-1)
	{
		UpdateUserItem(pIClientUserItem);
		return true;
	}

	//�����û�
	for (WORD i=0;i<m_wColumnCount;i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pIClientUserItem);
			INT nImageIndex=GetImageIndex(pIClientUserItem,i);
			nItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,TEXT(""),0,0,0,(LPARAM)pIClientUserItem);
		}
		else
		{
			//ͼ������
			if (m_cbDataDescribe[i]==UD_NULL) continue;
			if (m_cbDataDescribe[i]>=UD_IMAGE) continue;

			//�ַ�����
			SetItem(nItem,i,LVIF_TEXT,GetDescribeString(pIClientUserItem,i),0,0,0,0);
		}
	}

	return true;
}

//�����û�
bool CUserListControl::UpdateUserItem(IClientUserItem * pIClientUserItem)
{
	//Ч��״̬
	ASSERT(pIClientUserItem!=NULL);
	if (GetSafeHwnd()==NULL) return false;
	
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIClientUserItem;
	INT nItem=FindItem(&FindInfo);
	if (nItem==-1) return false;

	//�����û�
	for (WORD i=0;i<m_wColumnCount;i++)
	{
		//ͼ������
		if (m_cbDataDescribe[i]==UD_NULL) continue;
		if (m_cbDataDescribe[i]>=UD_IMAGE) continue;

		//�ַ�����
		SetItem(nItem,i,LVIF_TEXT,GetDescribeString(pIClientUserItem,i),0,0,0,0);
	}

	//��������
	RedrawItems(nItem,nItem);

	return true;
}

//ɾ���û�
bool CUserListControl::DeleteUserItem(IClientUserItem * pIClientUserItem)
{
	//Ч��״̬
	ASSERT(pIClientUserItem!=NULL);
	if (m_hWnd==NULL) return false;
	
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//�������
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIClientUserItem;

	//ɾ������
	INT nItem=FindItem(&FindInfo);
	if (nItem!=-1) 
	{
		DeleteItem(nItem);
		return true;
	}

	return false;
}

//��ȡλ��
INT CUserListControl::GetInsertIndex(IClientUserItem * pIClientUserItem)
{
	//��������
	INT nItemCount=GetItemCount();
	tagClientUserInfo * pUserInfo=pIClientUserItem->GetUserInfo();

	//�Լ��ж�
	if (pUserInfo->dwUserID==m_dwMySelfUserID) return 0;

	//��ͨ�û�
	BYTE cbCompanion=pIClientUserItem->GetUserCompanion();
	if ((pUserInfo->cbMasterOrder==0)&&(cbCompanion!=CP_FRIEND)&&(pUserInfo->dwMemberPoint==0)) return nItemCount;

	//��������
	IClientUserItem * pIUserItemTemp=NULL;
	LONG nImportOrder=pIClientUserItem->GetImportOrder();

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		pIUserItemTemp=(IClientUserItem *)GetItemData(i);
		if (pIUserItemTemp->GetUserID()==m_dwMySelfUserID) continue;
		if (nImportOrder>=pIUserItemTemp->GetImportOrder()) return i;
	}

	return nItemCount;
}

//��ȡλͼ
UINT CUserListControl::GetImageIndex(IClientUserItem * pIClientUserItem, WORD wColumnIndex)
{
	//Ч�����
	ASSERT(pIClientUserItem!=NULL);
	ASSERT(wColumnIndex<m_wColumnCount);

	//��ȡ����
	switch (m_cbDataDescribe[wColumnIndex])
	{
	case UD_FLAG:		//�û���־
		{
			//��������
			BYTE cbCompanion=pIClientUserItem->GetUserCompanion();
			tagClientUserInfo * pUserInfo=pIClientUserItem->GetUserInfo();

			//��������
			if (cbCompanion==CP_FRIEND) return INDEX_FRIEND;
			if( cbCompanion == CP_DETEST ) return INDEX_DETEST;
			if (pUserInfo->cbMasterOrder>0) return INDEX_MASTER+(pUserInfo->cbMasterOrder-1);
			if (pUserInfo->dwMemberPoint>0) return INDEX_MEMBER+(pUserInfo->dwMemberPoint-1);

			return INDEX_NORMAL;
		}
	case UD_STATUS:		//�û�״̬
		{
			//��������
			BYTE cbGender=pIClientUserItem->GetGender();
			BYTE cbUserStatus=pIClientUserItem->GetUserStatus();

			//��������
			return INDEX_STATUS+cbGender*COUNT_STATUS+cbUserStatus-US_FREE;
		}
	}

	return INDEX_INVALID;
}

//�����ַ�
LPCTSTR CUserListControl::GetDescribeString(IClientUserItem * pIClientUserItem, WORD wColumnIndex)
{
	//Ч�����
	ASSERT(pIClientUserItem!=NULL);
	ASSERT(wColumnIndex<m_wColumnCount);
	ASSERT(m_cbDataDescribe[wColumnIndex]<UD_IMAGE);

	//��������
	static TCHAR szDescribe[512]=TEXT("");
	tagClientUserInfo * pUserInfo=pIClientUserItem->GetUserInfo();

	//�����ַ�
	switch (m_cbDataDescribe[wColumnIndex])
	{
	case UD_GAME_ID:		//��Ϸ��ʶ
		{
			if (pUserInfo->dwGameID==0L) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->dwGameID);
			break;
		}
	case UD_USER_ID:		//�û���ʶ
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->dwUserID);
			break;
		}
	case UD_ACCOUNTS:		//�û��ʺ�
		{
			CW2CT strAccounts(pUserInfo->szAccounts);
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),(LPCTSTR)strAccounts);
			break;
		}
	case UD_GENDER:			//�û��Ա�
		{
			if (pUserInfo->cbGender==GENDER_FEMALE) lstrcpyn(szDescribe,TEXT("��ü"),CountArray(szDescribe));
			else if (pUserInfo->cbGender==GENDER_MANKIND) lstrcpyn(szDescribe,TEXT("˧��"),CountArray(szDescribe));
			break;
		}
	case UD_UNDER_WRITE:	//����ǩ��
		{
			CW2CT strUnderWrite(pUserInfo->szUnderWrite);
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),(LPCTSTR)strUnderWrite);
			break;
		}
	case UD_TABLE:			//���Ӻ���
		{
			if (pUserInfo->wTableID==INVALID_TABLE) 
				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("��Ч"));
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->wTableID+1);
			break;
		}
	case UD_CHAIR:			//���Ӻ���
		{
			if (pUserInfo->wChairID==INVALID_TABLE) 
				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("��Ч"));
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->wChairID+1);
			break;
		}
	//case UD_GOLD:			//�û����
	//	{
	//		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%I64d"),pUserInfo->lGold);
	//		break;
	//	}
	case UD_SCORE:			//�û�����
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%I64d"),pUserInfo->lScore);
			break;
		}
	case UD_SEGMENT:		//��Ϸ��λ
		{
			//if (pUserInfo->cbSegmentMode==0)
			//{
			//	if (pUserInfo->cbGameSegment<18)
			//	{
			//		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%2ld ��"),18-pUserInfo->cbGameSegment);
			//	}
			//	else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%2ld ��"),pUserInfo->cbGameSegment-17);
			//}
			//else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%2ld P"),pUserInfo->cbMasterSegment+1);
			break;
		}
	case UD_EXPERIENCE:		//�û�����
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lExperience);
			break;
		}
	case UD_WIN_COUNT:		//ʤ������
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lWinCount);
			break;
		}
	case UD_LOST_COUNT:		//�������
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lLostCount);
			break;
		}
	case UD_DRAW_COUNT:		//�;�����
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lDrawCount);
			break;
		}
	case UD_FLEE_COUNT:		//�Ӿ�����
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pUserInfo->lFleeCount);
			break;
		}
	case UD_PLAY_COUNT:		//��Ϸ����
		{
			LONG lPlayCount=pIClientUserItem->GetUserPlayCount();
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),lPlayCount);
			break;
		}
	case UD_WIN_RATE:		//�û�ʤ��
		{
			float fWinRate=pIClientUserItem->GetUserWinRate();
			if (fWinRate==0.0f) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%4.2f%%"),fWinRate);
			break;
		}
	case UD_LOST_RATE:		//�û�����
		{
			float fLostRate=pIClientUserItem->GetUserLostRate();
			if (fLostRate==0.0f) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%4.2f%%"),fLostRate);
			break;
		}
	case UD_DRAW_RATE:		//�û�����
		{
			float fDrawRate=pIClientUserItem->GetUserDrawRate();
			if (fDrawRate==0.0f) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%4.2f%%"),fDrawRate);
			break;
		}
	case UD_FLEE_RATE:		//�û�����
		{
			float fFleeRate=pIClientUserItem->GetUserFleeRate();
			if (fFleeRate==0.0f) szDescribe[0]=0;
			else _sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%4.2f%%"),fFleeRate);
			break;
		}
	case UD_GAME_LEVEL:		//��Ϸ�ȼ�
		{
			//��Ч����
			if (m_pIGameLevelParser==NULL)
			{
				ASSERT(FALSE);
				szDescribe[0]=0;
				break;
			}
				
			//��ȡ�ȼ�
			return m_pIGameLevelParser->GetLevelDescribe(pIClientUserItem);
		}
	case UD_AREA:			//�û�����
		{
			CW2CT strArea(pUserInfo->szArea);
			lstrcpyn(szDescribe,strArea,CountArray(szDescribe));
			break;
		}
	case UD_CITY:			//�û�����
		{
			CW2CT strCity(pUserInfo->szCity);
			lstrcpyn(szDescribe,strCity,CountArray(szDescribe));
			break;
		}
	case UD_PROVINCE:		//�û�ʡ��
		{
			CW2CT strProvince(pUserInfo->szProvince);
			lstrcpyn(szDescribe,strProvince,CountArray(szDescribe));
			break;
		}
	case UD_NOTE_INFO:		//�û���ע
		{
			CW2CT strUserNote(pIClientUserItem->GetUserNoteInfo());
			lstrcpyn(szDescribe,(LPCTSTR)strUserNote,CountArray(szDescribe));
			break;
		}
	case UD_LOOKON_USER:	//�Թ��û�
		{
			//��Ч����
			if (m_pIGameUserManager==NULL)
			{
				ASSERT(FALSE);
				szDescribe[0]=0;
				break;
			}

			//��ȡ�û�
			if (pIClientUserItem->GetUserStatus()==US_LOOKON)
			{
				//��ȡ��Ϣ
				WORD wChairID=pIClientUserItem->GetChairID();
				IClientUserItem * pITableUserItem=m_pIGameUserManager->GetTableUserItem(wChairID);

				//�������
				if ((pITableUserItem==NULL)||(pITableUserItem==pIClientUserItem))
				{
					szDescribe[0]=0;
					break;
				}

				//�����ֶ�
				CW2CT strAccounts(pITableUserItem->GetAccounts());
				lstrcpyn(szDescribe,(LPCTSTR)strAccounts,CountArray(szDescribe));
			}
			else szDescribe[0]=0;

			break;
		}
	default:				//δ֪�ֶ�
		{
			szDescribe[0]=0;
			break;
		}
	}

	return szDescribe;
}

//���к���
INT CALLBACK CUserListControl::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	//�������
	IClientUserItem * pIClientUserItem1=(IClientUserItem *)lParam1;
	IClientUserItem * pIClientUserItem2=(IClientUserItem *)lParam2;
    tagSortParameter * pSortParameter=(tagSortParameter *)lParamList;
	CUserListControl * pUserListControl=pSortParameter->pUserListControl;

	//�Լ��ö�
	if (pIClientUserItem1->GetUserID()==pUserListControl->m_dwMySelfUserID) return SORT_POSITION_FRONT;
	if (pIClientUserItem2->GetUserID()==pUserListControl->m_dwMySelfUserID) return SORT_POSITION_AFTER;

	//�Ա�����
	switch (pSortParameter->cbDataDescribe)
	{
	case UD_GAME_ID:		//��Ϸ��ʶ
		{
			//��ȡ����
			DWORD dwGameID1=pIClientUserItem1->GetGameID();
			DWORD dwGameID2=pIClientUserItem2->GetGameID();

			//�Ա�����
			INT nResult=dwGameID1>dwGameID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_USER_ID:		//�û���ʶ
		{
			//��ȡ����
			DWORD dwUserID1=pIClientUserItem1->GetGameID();
			DWORD dwUserID2=pIClientUserItem2->GetGameID();

			//�Ա�����
			INT nResult=dwUserID1>dwUserID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_ACCOUNTS:		//�û��ʺ�
		{
			//��ȡ����
			BYTE cbCompanion1=pIClientUserItem1->GetUserCompanion();
			BYTE cbCompanion2=pIClientUserItem2->GetUserCompanion();

			//�����ö�
			if (((cbCompanion1==CP_FRIEND)&&(cbCompanion2!=CP_FRIEND))
				||((cbCompanion1!=CP_FRIEND)&&(cbCompanion2==CP_FRIEND)))
			{
				return (cbCompanion1==CP_FRIEND)?SORT_POSITION_FRONT:SORT_POSITION_AFTER;
			}

			//��ȡ����
			BYTE cbMasterRight1=pIClientUserItem1->GetMasterOrder();
			BYTE cbMasterRight2=pIClientUserItem2->GetMasterOrder();

			//�����ö�
			if (cbMasterRight1!=cbMasterRight2)
			{
				return (cbMasterRight1==cbMasterRight2)?SORT_POSITION_FRONT:SORT_POSITION_AFTER;
			}
			
			//��ȡ����
			BYTE cbMemberRight1=pIClientUserItem1->GetMemberOrder();
			BYTE cbMemberRight2=pIClientUserItem2->GetMemberOrder();

			//��Ա�ö�
			if (cbMemberRight1!=cbMemberRight2)
			{
				return (cbMemberRight1==cbMemberRight2)?SORT_POSITION_FRONT:SORT_POSITION_AFTER;
			}

			//��ȡ����
			LPCWSTR pszAccounts1=pIClientUserItem1->GetAccounts();
			LPCWSTR pszAccounts2=pIClientUserItem2->GetAccounts();
			
			//�Ա�����
			INT nResult=wcscmp(pszAccounts1,pszAccounts2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_GENDER:			//�û��Ա�
		{
			//��ȡ����
			BYTE cbGender1=pIClientUserItem1->GetGender();
			BYTE cbGender2=pIClientUserItem2->GetGender();

			//�Ա�����
			INT nResult=cbGender1>cbGender2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_UNDER_WRITE:	//����ǩ��
		{
			//��ȡ����
			LPCWSTR pszUnderWrite1=pIClientUserItem1->GetUnderWrite();
			LPCWSTR pszUnderWrite2=pIClientUserItem2->GetUnderWrite();

			//�Ա�����
			INT nResult=wcscmp(pszUnderWrite1,pszUnderWrite2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_TABLE:			//���Ӻ���
		{
			//��ȡ����
			WORD wTableID1=pIClientUserItem1->GetTableID();
			WORD wTableID2=pIClientUserItem2->GetTableID();

			//�Ա�����
			INT nResult=wTableID1>wTableID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_CHAIR:			//���Ӻ���
		{
			//��ȡ����
			WORD wChairID1=pIClientUserItem1->GetChairID();
			WORD wChairID2=pIClientUserItem2->GetChairID();

			//�Ա�����
			INT nResult=wChairID1>wChairID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	//case UD_GOLD:			//�û����
	//	{
	//		//��ȡ����
	//		LONGLONG lGold1=pIClientUserItem1->GetUserGold();
	//		LONGLONG lGold2=pIClientUserItem2->GetUserGold();

	//		//�Ա�����
	//		INT nResult=lGold1>lGold2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	//		return (pSortParameter->bAscendSort)?nResult:-nResult;
	//	}
	case UD_SCORE:			//�û�����
		{
			//��ȡ����
			LONGLONG lScore1=pIClientUserItem1->GetUserScore();
			LONGLONG lScore2=pIClientUserItem2->GetUserScore();

			//�Ա�����
			INT nResult=lScore1>lScore2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_SEGMENT:		//��Ϸ��λ
		{
			////��ȡ����
			//WORD wGameSegment1=pIClientUserItem1->GetGameSegment();
			//WORD wGameSegment2=pIClientUserItem2->GetGameSegment();

			////�Ա�����
			//INT nResult=wGameSegment1>wGameSegment2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return 0; // (pSortParameter->bAscendSort) ? nResult : -nResult;
		}
	case UD_EXPERIENCE:		//�û�����
		{
			//��ȡ����
			LONG lExperience1=pIClientUserItem1->GetUserExperience();
			LONG lExperience2=pIClientUserItem2->GetUserExperience();

			//�Ա�����
			INT nResult=lExperience1>lExperience2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_WIN_COUNT:		//ʤ������
		{
			//��ȡ����
			LONG lWinCount1=pIClientUserItem1->GetUserWinCount();
			LONG lWinCount2=pIClientUserItem2->GetUserWinCount();

			//�Ա�����
			INT nResult=lWinCount1>lWinCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_LOST_COUNT:		//�������
		{
			//��ȡ����
			LONG lLostCount1=pIClientUserItem1->GetUserLostCount();
			LONG lLostCount2=pIClientUserItem2->GetUserLostCount();

			//�Ա�����
			INT nResult=lLostCount1>lLostCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_DRAW_COUNT:		//�;�����
		{
			//��ȡ����
			LONG lDrawCount1=pIClientUserItem1->GetUserDrawCount();
			LONG lDrawCount2=pIClientUserItem2->GetUserDrawCount();

			//�Ա�����
			INT nResult=lDrawCount1>lDrawCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_FLEE_COUNT:		//�Ӿ�����
		{
			//��ȡ����
			LONG lFleeCount1=pIClientUserItem1->GetUserFleeCount();
			LONG lFleeCount2=pIClientUserItem2->GetUserFleeCount();

			//�Ա�����
			INT nResult=lFleeCount1>lFleeCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_PLAY_COUNT:		//��Ϸ����
		{
			//��ȡ����
			LONG lPlayCount1=pIClientUserItem1->GetUserPlayCount();
			LONG lPlayCount2=pIClientUserItem2->GetUserPlayCount();

			//�Ա�����
			INT nResult=lPlayCount1>lPlayCount2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_WIN_RATE:		//�û�ʤ��
		{
			//��ȡ����
			float fWinRate1=pIClientUserItem1->GetUserWinRate();
			float fWinRate2=pIClientUserItem2->GetUserWinRate();

			//�Ա�����
			INT nResult=fWinRate1>fWinRate2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_LOST_RATE:		//�û�����
		{
			//��ȡ����
			float fLostRate1=pIClientUserItem1->GetUserLostRate();
			float fLostRate2=pIClientUserItem2->GetUserLostRate();

			//�Ա�����
			INT nResult=fLostRate1>fLostRate2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_DRAW_RATE:		//�û�����
		{
			//��ȡ����
			float fDrawRate1=pIClientUserItem1->GetUserDrawRate();
			float fDrawRate2=pIClientUserItem2->GetUserDrawRate();

			//�Ա�����
			INT nResult=fDrawRate1>fDrawRate2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_FLEE_RATE:		//�û�����
		{
			//��ȡ����
			float fFleeRate1=pIClientUserItem1->GetUserFleeRate();
			float fFleeRate2=pIClientUserItem2->GetUserFleeRate();

			//�Ա�����
			INT nResult=fFleeRate1>fFleeRate2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_GAME_LEVEL:		//��Ϸ�ȼ�
		{
			//��ȡ����
			LONGLONG lScore1=pIClientUserItem1->GetUserScore();
			LONGLONG lScore2=pIClientUserItem2->GetUserScore();

			//�Ա�����
			INT nResult=lScore1>lScore2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_AREA:			//�û�����
		{
			//��ȡ����
			LPCWSTR pszUserArea1=pIClientUserItem1->GetUserArea();
			LPCWSTR pszUserArea2=pIClientUserItem2->GetUserArea();

			//�Ա�����
			INT nResult=wcscmp(pszUserArea1,pszUserArea2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_CITY:			//�û�����
		{
			//��ȡ����
			LPCWSTR pszUserCity1=pIClientUserItem1->GetUserCity();
			LPCWSTR pszUserCity2=pIClientUserItem2->GetUserCity();

			//�Ա�����
			INT nResult=wcscmp(pszUserCity1,pszUserCity2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_PROVINCE:		//�û�ʡ��
		{
			//��ȡ����
			LPCWSTR pszUserProvince1=pIClientUserItem1->GetUserProvince();
			LPCWSTR pszUserProvince2=pIClientUserItem2->GetUserProvince();

			//�Ա�����
			INT nResult=wcscmp(pszUserProvince1,pszUserProvince2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_NOTE_INFO:		//�û���ע
		{
			//��ȡ����
			LPCWSTR pszNoteInfo1=pIClientUserItem1->GetUserNoteInfo();
			LPCWSTR pszNoteInfo2=pIClientUserItem2->GetUserNoteInfo();

			//�Ա�����
			INT nResult=wcscmp(pszNoteInfo1,pszNoteInfo2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_STATUS:			//�û�״̬
		{
			//��ȡ����
			BYTE cbUserStatus1=pIClientUserItem1->GetUserStatus();
			BYTE cbUserStatus2=pIClientUserItem2->GetUserStatus();

			//�Ա�����
			INT nResult=cbUserStatus1>cbUserStatus2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	}

	return 0;
}

//������Ϣ
VOID CUserListControl::OnNcDestroy()
{
	//��Դ����
	m_dwMySelfUserID=0L;
	m_bAscendSort=false;
	m_bCreateColumn=false;

	//�ӿڱ���
	m_pIGameLevelParser=NULL;
	m_pIGameUserManager=NULL;

	//�б���Ϣ
	m_wColumnCount=0;
	ZeroMemory(m_cbDataDescribe,sizeof(m_cbDataDescribe));

	__super::OnNcDestroy();
}

//�滭����
BOOL CUserListControl::OnEraseBkgnd(CDC * pDC)
{
	//�滭����
	if (m_UserListHeaderCtrl.m_hWnd!=NULL)
	{
		//��ȡλ��
		CRect rcClient;
		CRect rcHeaderCtrl;
		GetClientRect(&rcClient);
		m_UserListHeaderCtrl.GetWindowRect(&rcHeaderCtrl);

		//�滭����
		rcClient.top=rcHeaderCtrl.Height();
		pDC->FillSolidRect(&rcClient,GetBkColor());

		return TRUE;
	}

	return __super::OnEraseBkgnd(pDC);
}

//�����Ϣ
VOID CUserListControl::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult) 
{
	//��������
	NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

	//��������
	if (m_wColumnCount!=0)
	{
		//��������
		tagSortParameter SortParameter;
		ZeroMemory(&SortParameter,sizeof(SortParameter));

		//���ñ���
		SortParameter.pUserListControl=this;
		SortParameter.bAscendSort=m_bAscendSort;
		SortParameter.cbDataDescribe=m_cbDataDescribe[pNMListView->iSubItem];

		//���ñ���
		m_bAscendSort=!m_bAscendSort;
		m_UserListHeaderCtrl.SetSortArrow(pNMListView->iSubItem,(m_bAscendSort==true)?SortType_Ascend:SortType_Descale);

		//�����б�
		SortItems(SortFunction,(LPARAM)&SortParameter);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSearchUserListControl::CSearchUserListControl()
{
	//��Դ����
	m_bAscendSort=false;
	m_bCreateColumn=false;
	m_dwMySelfUserID=0L;

	//�б���Ϣ
	m_wColumnCount=0;
	ZeroMemory(m_cbDataDescribe,sizeof(m_cbDataDescribe));

	//��Դ����
	ZeroMemory(&m_ColorUserList,sizeof(m_ColorUserList));

	return;
}

//��������
CSearchUserListControl::~CSearchUserListControl()
{
}

//�ؼ���
VOID CSearchUserListControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//���ù���
	m_SkinScrollBar.InitScroolBar(this);

	//��������
	SetExtendedStyle(LIST_STYTE);
	m_UserListHeaderCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	//��������
	if (GetHeaderCtrl()!=NULL)
	{
		CFont TitleFont;
		TitleFont.CreatePointFont(130,TEXT("����"));
		m_UserListHeaderCtrl.SetFont(&TitleFont);
	}

	//�û���־
	m_cbDataDescribe[m_wColumnCount]=UD_NULL;
	InsertColumn(m_wColumnCount++,TEXT(""),LVCFMT_CENTER,0,0);

	//�û���־
	m_cbDataDescribe[m_wColumnCount]=UD_FLAG;
	InsertColumn(m_wColumnCount++,TEXT(""),LVCFMT_CENTER,WIDTH_STATUS,0);

	//�û�״̬
	m_cbDataDescribe[m_wColumnCount]=UD_STATUS;
	InsertColumn(m_wColumnCount++,TEXT(""),LVCFMT_CENTER,WIDTH_STATUS,0);

	//�û�����
	m_cbDataDescribe[m_wColumnCount] = UD_ACCOUNTS;
	InsertColumn(m_wColumnCount++,TEXT("�û���"),LVCFMT_LEFT,104);

	//��ϷID
	m_cbDataDescribe[m_wColumnCount] = UD_GAME_ID;
	InsertColumn(m_wColumnCount++,TEXT("��Ϸ ID"),LVCFMT_LEFT,62);

	//��ǰλ��
	m_cbDataDescribe[m_wColumnCount] = UD_LOCATION;
	InsertColumn(m_wColumnCount++,TEXT("��ǰλ��"),LVCFMT_LEFT,150);

	return;
}

//�滭����
VOID CSearchUserListControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//��������
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	//��ȡ����
	INT nItemID=lpDrawItemStruct->itemID;
	INT nColumnCount=m_UserListHeaderCtrl.GetItemCount();

	//���Ʊ���
	COLORREF rcTextColor=m_ColorUserList.crNormalTX;
	COLORREF rcBackColor=m_ColorUserList.crNormalBK;
	IGlobalUserItem * pIGlobalUserItem=(IGlobalUserItem *)lpDrawItemStruct->itemData;

	//��ɫ����
	if (lpDrawItemStruct->itemState&ODS_SELECTED)
	{
		//ѡ����ɫ
		rcTextColor=m_ColorUserList.crSelectTX;
		rcBackColor=m_ColorUserList.crSelectBK;
	}
	else if (pIGlobalUserItem->GetUserID()==m_dwMySelfUserID)
	{
		//�Լ���ɫ
		rcTextColor=m_ColorUserList.crMyselfTX;
		rcBackColor=m_ColorUserList.crMasterBK;
	}

	//�滭����
	CRect rcClipBox;
	pDC->GetClipBox(&rcClipBox);

	//���� DC
	pDC->SetBkColor(rcBackColor);
	pDC->SetTextColor(rcTextColor);

	//�滭ѡ��
	if (lpDrawItemStruct->itemState&ODS_FOCUS)
	{
		COLORREF rcFillColor=GetBkColor();
		pDC->Draw3dRect(&lpDrawItemStruct->rcItem,rcFillColor,rcFillColor);
	}

	//�滭����
	for (INT i=1;i<nColumnCount;i++)
	{
		//��ȡλ��
		CRect rcSubItem;
		GetSubItemRect(nItemID,i,LVIR_LABEL,rcSubItem);

		//���ñ߽�
		if (i<=ENJOIN_COUNT) rcItem.left=rcSubItem.left;

		//�滭�ж�
		if (rcSubItem.left>rcClipBox.right) break;
		if (rcSubItem.right<rcClipBox.left) continue;
		if (rcSubItem.left==rcSubItem.right) continue;

		//�滭����
		pDC->FillSolidRect(&rcSubItem,rcBackColor);

		//��ȡ����
		TCHAR szString[256]=TEXT("");
		GetItemText(nItemID,i,szString,CountArray(szString));

		//�滭����
		rcSubItem.left+=5;
		pDC->DrawText(szString,lstrlen(szString),&rcSubItem,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	//�滭ѡ��
	if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->DrawFocusRect(&rcItem);

	return;
}

//���ýӿ�
bool CSearchUserListControl::SetMySelfUserID(DWORD dwMySelfUserID)
{
	//���ñ���
	m_dwMySelfUserID=dwMySelfUserID;

	return true;
}

//������ɫ
bool CSearchUserListControl::SetColorUserList(const tagColorUserList & ColorUserList)
{
	//���ñ���
	m_ColorUserList=ColorUserList;

	//���½���
	if (m_hWnd!=NULL)
	{
		//���ñ���
		SetBkColor(ColorUserList.crNormalBK);

		//�غϴ���
		RedrawWindow();
	}

	return true;
}

//�����б�
bool CSearchUserListControl::SetColumnDescribe(const tagColumnItem ColumnItem[], WORD wColumnCount)
{
	//�����ж�
	if (m_bCreateColumn==true) return true;

	//��������
	ASSERT(wColumnCount<(CountArray(m_cbDataDescribe)-m_wColumnCount));
	wColumnCount=__min(CountArray(m_cbDataDescribe)-m_wColumnCount,wColumnCount);

	//�����б�
	SetRedraw(FALSE);
	for (WORD i=0;i<wColumnCount;i++)
	{
		CW2CT strColumnName(ColumnItem[i].szColumnName);
		m_cbDataDescribe[m_wColumnCount]=ColumnItem[i].cbDataDescribe;
		InsertColumn(m_wColumnCount++,strColumnName,LVCFMT_LEFT,ColumnItem[i].cbColumnWidth);
	}
	SetRedraw(TRUE);

	//���ñ���
	m_bCreateColumn=true;

	return true;
}

//�����û�
bool CSearchUserListControl::InsertUserItem(IGlobalUserItem * pIGlobalUserItem)
{
	//Ч��״̬
	ASSERT(pIGlobalUserItem!=NULL);
	if (m_hWnd==NULL) return false;

	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIGlobalUserItem;
	INT nItem=FindItem(&FindInfo);
	if (nItem!=-1)
	{
		UpdateUserItem(pIGlobalUserItem);
		return true;
	}

	//�����û�
	for (WORD i=0;i<m_wColumnCount;i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pIGlobalUserItem);
			INT nImageIndex=GetImageIndex(pIGlobalUserItem,i);
			nItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,TEXT(""),0,0,0,(LPARAM)pIGlobalUserItem);
		}
		else
		{
			//ͼ������
			if (m_cbDataDescribe[i]==UD_NULL) continue;
			if (m_cbDataDescribe[i]>=UD_IMAGE) continue;

			//�ַ�����
			SetItem(nItem,i,LVIF_TEXT,GetDescribeString(pIGlobalUserItem,i),0,0,0,0);
		}
	}

	return true;
}

//�����û�
bool CSearchUserListControl::UpdateUserItem(IGlobalUserItem * pIGlobalUserItem)
{
	//Ч��״̬
	ASSERT(pIGlobalUserItem!=NULL);
	if (GetSafeHwnd()==NULL) return false;

	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIGlobalUserItem;
	INT nItem=FindItem(&FindInfo);
	if (nItem==-1) return false;

	//�����û�
	for (WORD i=0;i<m_wColumnCount;i++)
	{
		//ͼ������
		if (m_cbDataDescribe[i]==UD_NULL) continue;
		if (m_cbDataDescribe[i]>=UD_IMAGE) continue;

		//�ַ�����
		SetItem(nItem,i,LVIF_TEXT,GetDescribeString(pIGlobalUserItem,i),0,0,0,0);
	}

	//��������
	RedrawItems(nItem,nItem);

	return true;
}

//ɾ���û�
bool CSearchUserListControl::DeleteUserItem(IGlobalUserItem * pIGlobalUserItem)
{
	//Ч��״̬
	ASSERT(pIGlobalUserItem!=NULL);
	if (m_hWnd==NULL) return false;

	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//�������
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pIGlobalUserItem;

	//ɾ������
	INT nItem=FindItem(&FindInfo);
	if (nItem!=-1) 
	{
		DeleteItem(nItem);
		return true;
	}

	return false;
}

//��ȡλ��
INT CSearchUserListControl::GetInsertIndex(IGlobalUserItem * pIGlobalUserItem)
{
	//��������
	INT nItemCount=GetItemCount();

	//�Լ��ж�
	if (pIGlobalUserItem->GetUserID()==m_dwMySelfUserID) return 0;

	//��ͨ�û�
	return nItemCount;
}

//��ȡλͼ
UINT CSearchUserListControl::GetImageIndex(IGlobalUserItem * pIGlobalUserItem, WORD wColumnIndex)
{
	//Ч�����
	ASSERT(pIGlobalUserItem!=NULL);
	ASSERT(wColumnIndex<m_wColumnCount);

	return INDEX_INVALID;
}

//�����ַ�
LPCTSTR CSearchUserListControl::GetDescribeString(IGlobalUserItem * pIGlobalUserItem, WORD wColumnIndex)
{
	//Ч�����
	ASSERT(pIGlobalUserItem!=NULL);
	ASSERT(wColumnIndex<m_wColumnCount);
	ASSERT(m_cbDataDescribe[wColumnIndex]<UD_IMAGE);

	//��������
	static TCHAR szDescribe[512]=TEXT("");

	//�����ַ�
	switch (m_cbDataDescribe[wColumnIndex])
	{
	case UD_GAME_ID:		//��Ϸ��ʶ
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pIGlobalUserItem->GetGameID());
			break;
		}
	case UD_USER_ID:		//�û���ʶ
		{
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%ld"),pIGlobalUserItem->GetUserID());
			break;
		}
	case UD_ACCOUNTS:		//�û��ʺ�
		{
			CW2CT strAccounts(pIGlobalUserItem->GetAccounts());
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("%s"),(LPCTSTR)strAccounts);
			break;
		}
	case UD_LOCATION:		//�û�λ��
		{
			szDescribe[0] = 0;

			//��ȡ������Ϣ
			WORD wDataSize = 0;
			LPBYTE lpBuffer = (LPBYTE)pIGlobalUserItem->GetExtraInfo(wDataSize,DTP_GR_SERVER_INFO);
			if( wDataSize > 0 && lpBuffer != NULL )
			{
				//��ȡ������Ϣ
				WORD wProcessSize = 0;
				do 
				{
					tagGameServerItem *pGameServerItem = (tagGameServerItem *)(lpBuffer+wProcessSize);

					if( szDescribe[0] != 0 )
						StringCbCat( szDescribe,CountArray(szDescribe),TEXT("��") );

					CW2CT strServerName(pGameServerItem->szServerName);
					StringCbCat( szDescribe,CountArray(szDescribe),(LPCTSTR)strServerName );

					wProcessSize += sizeof(tagGameServerItem)-sizeof(pGameServerItem->szServerName)+
						CountStringBufferW(pGameServerItem->szServerName);
				} while (wProcessSize < wDataSize );
			}

			break;
		}
	default:				//δ֪�ֶ�
		{
			szDescribe[0]=0;
			break;
		}
	}

	return szDescribe;
}

//���к���
INT CALLBACK CSearchUserListControl::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	//�������
	IGlobalUserItem * pIUserItem1=(IGlobalUserItem *)lParam1;
	IGlobalUserItem * pIUserItem2=(IGlobalUserItem *)lParam2;
	tagSearchSortParameter * pSortParameter=(tagSearchSortParameter *)lParamList;
	CSearchUserListControl * pListControl=pSortParameter->pListControl;

	//�Լ��ö�
	if (pIUserItem1->GetUserID()==pListControl->m_dwMySelfUserID) return SORT_POSITION_FRONT;
	if (pIUserItem2->GetUserID()==pListControl->m_dwMySelfUserID) return SORT_POSITION_AFTER;

	//�Ա�����
	switch (pSortParameter->cbDataDescribe)
	{
	case UD_GAME_ID:		//��Ϸ��ʶ
		{
			//��ȡ����
			DWORD dwGameID1=pIUserItem1->GetGameID();
			DWORD dwGameID2=pIUserItem2->GetGameID();

			//�Ա�����
			INT nResult=dwGameID1>dwGameID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_USER_ID:		//�û���ʶ
		{
			//��ȡ����
			DWORD dwUserID1=pIUserItem1->GetGameID();
			DWORD dwUserID2=pIUserItem2->GetGameID();

			//�Ա�����
			INT nResult=dwUserID1>dwUserID2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_ACCOUNTS:		//�û��ʺ�
		{
			//��ȡ����
			LPCWSTR pszAccounts1=pIUserItem1->GetAccounts();
			LPCWSTR pszAccounts2=pIUserItem2->GetAccounts();

			//�Ա�����
			INT nResult=wcscmp(pszAccounts1,pszAccounts2);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	case UD_NOTE_INFO:		//�û���ע
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

			//�Ա�����
			INT nResult=wcscmp(pServerInfo1->szServerName,pServerInfo2->szServerName);
			return (pSortParameter->bAscendSort)?nResult:-nResult;
		}
	}

	return 0;
}

//������Ϣ
VOID CSearchUserListControl::OnNcDestroy()
{
	//��Դ����
	m_dwMySelfUserID=0L;
	m_bAscendSort=false;
	m_bCreateColumn=false;

	//�б���Ϣ
	m_wColumnCount=0;
	ZeroMemory(m_cbDataDescribe,sizeof(m_cbDataDescribe));

	__super::OnNcDestroy();
}

//�滭����
BOOL CSearchUserListControl::OnEraseBkgnd(CDC * pDC)
{
	//�滭����
	if (m_UserListHeaderCtrl.m_hWnd!=NULL)
	{
		//��ȡλ��
		CRect rcClient;
		CRect rcHeaderCtrl;
		GetClientRect(&rcClient);
		m_UserListHeaderCtrl.GetWindowRect(&rcHeaderCtrl);

		//�滭����
		rcClient.top=rcHeaderCtrl.Height();
		pDC->FillSolidRect(&rcClient,GetBkColor());

		return TRUE;
	}

	return __super::OnEraseBkgnd(pDC);
}

//�����Ϣ
VOID CSearchUserListControl::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult) 
{
	//��������
	NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

	//��������
	if (m_wColumnCount!=0)
	{
		//��������
		tagSearchSortParameter SortParameter;
		ZeroMemory(&SortParameter,sizeof(SortParameter));

		//���ñ���
		SortParameter.pListControl=this;
		SortParameter.bAscendSort=m_bAscendSort;
		SortParameter.cbDataDescribe=m_cbDataDescribe[pNMListView->iSubItem];

		//���ñ���
		m_bAscendSort=!m_bAscendSort;
		m_UserListHeaderCtrl.SetSortArrow(pNMListView->iSubItem,(m_bAscendSort==true)?SortType_Ascend:SortType_Descale);

		//�����б�
		SortItems(SortFunction,(LPARAM)&SortParameter);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////