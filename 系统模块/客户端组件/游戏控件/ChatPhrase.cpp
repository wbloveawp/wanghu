#include "Stdafx.h"
#include "Resource.h"
#include "ChatPhrase.h"

//////////////////////////////////////////////////////////////////////////

//�б���
#define IMGAE_WIDTH					20									//�б���

//������Ϣ
struct tagSortParameter
{
	bool							bAscendSort;						//�����־
	UINT							uChatListIndex;						//�б�����
	CChatPhraseList *				pChatPhraseList;					//�б�ؼ�
};

//////////////////////////////////////////////////////////////////////////
//��̬����

CImageList	CChatPhraseList::m_PhraseFlagImage;							//״̬λͼ

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChatPhrase, CSkinDialog)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CChatPhraseList, CSkinListCtrl)
	ON_WM_NCDESTROY()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CChatPhraseList::CChatPhraseList()
{
	//��Դ����
	m_bAscendSort=false;

	return;
}

//��������
CChatPhraseList::~CChatPhraseList()
{
}

//�����б�
bool CChatPhraseList::InitListControl()
{
	//���ر���
	if (m_PhraseFlagImage.m_hImageList==NULL)
	{
		//����ͼƬ
		CBitmap Image;
		BITMAP ImageInfo;
		Image.LoadBitmap(IDB_PHRASE_FLAG_IMAGE);

		//����ͼƬ
		Image.GetBitmap(&ImageInfo);
		m_PhraseFlagImage.Create(IMGAE_WIDTH,ImageInfo.bmHeight,ILC_COLOR16|ILC_MASK,0,0);

		//����ͼƬ
		m_PhraseFlagImage.Add(&Image,RGB(255,0,255));
	}

	//���ÿؼ�
	SetImageList(&m_PhraseFlagImage,LVSIL_SMALL);
	SetExtendedStyle(GetExtendedStyle()|LVS_EX_SUBITEMIMAGES);

	//��������
	if (GetHeaderCtrl()!=NULL)
	{
		CFont TitleFont;
		TitleFont.CreatePointFont(130,TEXT("����"));
		m_SkinHeadCtrl.SetFont(&TitleFont);
	}

	return true;
}

//���к���
INT CALLBACK CChatPhraseList::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	return 0;
}

//������Ϣ
VOID CChatPhraseList::OnNcDestroy()
{
	//��Դ����
	m_bAscendSort=false;

	__super::OnNcDestroy();
}

//�����Ϣ
VOID CChatPhraseList::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult) 
{
	//��������
	NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

	//��������
	tagSortParameter SortParameter;
	ZeroMemory(&SortParameter,sizeof(SortParameter));

	//���ñ���
	SortParameter.pChatPhraseList=this;
	SortParameter.bAscendSort=m_bAscendSort;
	SortParameter.uChatListIndex=pNMListView->iSubItem;

	//���ñ���
	m_bAscendSort=!m_bAscendSort;

	//�����б�
	SortItems(SortFunction,(LPARAM)&SortParameter);

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CChatPhrase::CChatPhrase() : CSkinDialog(IDD_CHAT_PHRASE)
{
}

//��������
CChatPhrase::~CChatPhrase()
{
}

//�ؼ���
VOID CChatPhrase::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_SELECT, m_btSelect);
	DDX_Control(pDX, IDC_CHAT_SHORT_LIST, m_ChatPhraseList);
}

//��ʼ������
BOOL CChatPhrase::OnInitDialog()
{
	__super::OnInitDialog();

	//�����б�
	m_ChatPhraseList.InitListControl();
	m_ChatPhraseList.InsertColumn(0,TEXT("����"),LVCFMT_LEFT,60);
	m_ChatPhraseList.InsertColumn(1,TEXT(""),LVCFMT_LEFT,IMGAE_WIDTH);
	m_ChatPhraseList.InsertColumn(2,TEXT("����"),LVCFMT_LEFT,400);

	//���ñ���
	SetWindowText(TEXT("������"));

	//��������
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return FALSE;
}

//�����ؼ�
VOID CChatPhrase::RectifyControl(INT nWidth, INT nHeight)
{
	//�ƶ��ؼ�
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

	//��ȡλ��
	CRect rcButton;
	m_btSelect.GetWindowRect(&rcButton);

	//�����ؼ�
	DeferWindowPos(hDwp,m_btCancel,NULL,nWidth-GetXBorder()-5-rcButton.Width(),nHeight-GetYBorder()-rcButton.Height()-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btSelect,NULL,nWidth-GetXBorder()-10-rcButton.Width()*2,nHeight-GetYBorder()-rcButton.Height()-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_ChatPhraseList,NULL,GetXBorder()+5,GetCaptionHeight()+6,nWidth-GetXBorder()*2-10,
		nHeight-GetCaptionHeight()-GetYBorder()-rcButton.Height()-16,uFlags);

	//�ƶ�����
	EndDeferWindowPos(hDwp);

	return;
}

//////////////////////////////////////////////////////////////////////////
