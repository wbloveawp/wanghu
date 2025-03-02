#include "StdAfx.h"
#include "Resource.h"
#include "DlgServerWizard.h"

//////////////////////////////////////////////////////////////////////////
//��ǩ����
#define TAB_SERVER_OPTION1					0
#define TAB_SERVER_OPTION2					1
#define TAB_SERVER_OPTION3					2
#define TAB_SERVER_OPTION4					3
#define TAB_SERVER_OPTION5					4

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardStep1, CDlgServerWizardItem)
	ON_NOTIFY(NM_DBLCLK, IDC_MODULE_LIST, OnNMDblclkModuleList)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionCustom, CDlgServerOptionItem)
	ON_WM_SETFOCUS()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizard, CDialog)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardStep2, CDlgServerWizardItem)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SERVER_OPTION, OnTcnSelchangeServerOption)
	ON_CBN_SELCHANGE(IDC_SERVER_TYPE, OnCbnSelchangeServerType)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizardItem::CDlgServerWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_bSaveData=true;

	//���ñ���
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//��������
CDlgServerWizardItem::~CDlgServerWizardItem()
{
}

//��������
bool CDlgServerWizardItem::SaveItemData()
{
	//��������
	if ((m_hWnd!=NULL)&&(m_bSaveData==false))
	{
		//��������
		m_bSaveData=SaveInputInfo();

		//����ж�
		if (m_bSaveData==false) return false;
	}

	return true;
}

//������
bool CDlgServerWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) 
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//��������
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//������Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���ñ���
	m_bSaveData=false;

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//ȷ������
VOID CDlgServerWizardItem::OnOK() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgServerWizardItem::OnCancel() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//λ����Ϣ
VOID CDlgServerWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////
//����ѡ����
	
//���캯��
CDlgServerOptionItem::CDlgServerOptionItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
}

//��������
CDlgServerOptionItem::~CDlgServerOptionItem()
{
}


//ȷ������
VOID CDlgServerOptionItem::OnOK()
{
	//Ͷ����Ϣ
	//GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgServerOptionItem::OnCancel()
{
	//Ͷ����Ϣ
	//GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//��������
void CDlgServerOptionItem::CreateOptionItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//��������
	Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));
}

//λ����Ϣ
VOID CDlgServerOptionItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);
}

//////////////////////////////////////////////////////////////////////////
//����ѡ��1

//���캯��
CDlgServerOption1::CDlgServerOption1() : CDlgServerOptionItem(IDD_SERVER_OPTION_1)
{
}

//��������
CDlgServerOption1::~CDlgServerOption1()
{
}

//��ʼ������
BOOL CDlgServerOption1::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_KIND_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_NODE_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SORT_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_MAX_PLAYER))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EXPGAIN_RATE))->LimitText(3);

	//��������
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_SERVER_LEVEL);
	LPCTSTR pszServerLevel[] = { TEXT("������"),TEXT("����"),TEXT("�߼�"),TEXT("רҵ"),TEXT("����") };

	//���켶��
	for (INT i = 0; i < CountArray(pszServerLevel); i++)
	{
		WORD wServerLevel = i + 1;
		pComboBox->SetItemData(pComboBox->InsertString(i,pszServerLevel[i]), wServerLevel);		
	}

	//ѡ�񼶱�
	for (INT i = 0; i < CountArray(pszServerLevel); i++)
	{
		WORD wServerLevel = (WORD)pComboBox->GetItemData(i);
		if (wServerLevel == m_pGameServiceOption->wServerLevel)
		{
			pComboBox->SetCurSel(i);
			break;
		}
	}

	//��������
	pComboBox = (CComboBox*)GetDlgItem(IDC_MATCH_TYPE);
	LPCTSTR pszMatchType[] = { TEXT("��ʱ��"),TEXT("��ʱ��"),TEXT("������")};
	BYTE cbMatchType[] = { MATCH_TYPE_IMMEDIATE, MATCH_TYPE_LOCKTIME, MATCH_TYPE_TOURNAMENT };

	//���켶��
	for (INT i = 0; i < CountArray(pszMatchType); i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i, pszMatchType[i]), cbMatchType[i]);
	}

	//��������
	if (m_pGameServiceOption->wServerType & GAME_GENRE_MATCH)
	{
		for (INT i = 0; i < CountArray(pszServerLevel); i++)
		{
			BYTE cbMatchType = (BYTE)pComboBox->GetItemData(i);
			if (cbMatchType == m_pGameServiceOption->cbMatchType)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}	

	//��������
	if (m_pGameServiceOption->lCellScore!=0) SetDlgItemInt(IDC_CELL_SCORE,(LONG)SCORE_ZOOMOUT(m_pGameServiceOption->lCellScore));
	if (m_pGameServiceOption->lLessScore!=0) SetDlgItemInt(IDC_LESS_SCORE,(LONG)SCORE_ZOOMOUT(m_pGameServiceOption->lLessScore));
	if (m_pGameServiceOption->wMaxPlayer!=0) SetDlgItemInt(IDC_MAX_PLAYER,(LONG)m_pGameServiceOption->wMaxPlayer);
	if (m_pGameServiceOption->lRestrictScore!=0) SetDlgItemInt(IDC_RESTRICT_SCORE,(LONG)SCORE_ZOOMOUT(m_pGameServiceOption->lRestrictScore));
	if (m_pGameServiceOption->szServiceAddr[0]!=0) SetDlgItemText(IDC_SERVICE_ADDR,m_pGameServiceOption->szServiceAddr);
	if (m_pGameServiceOption->szServiceDomain[0]!=0) SetDlgItemText(IDC_SERVICE_DOMAIN,m_pGameServiceOption->szServiceDomain);
	if (m_pGameServiceOption->lRevenue!=0) SetDlgItemInt(IDC_GAME_REVENUE,m_pGameServiceOption->lRevenue);
	if (m_pGameServiceOption->lExpGainRate!= 0) SetDlgItemInt(IDC_EXPGAIN_RATE, m_pGameServiceOption->lExpGainRate);

	//�շ�ģʽ
	BOOL bCheck = CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	if( bCheck ) ((CButton *)GetDlgItem(IDC_FEE))->SetCheck(TRUE);
	else ((CButton *)GetDlgItem(IDC_PERCENT))->SetCheck(TRUE);

	//�ҽ�����
	WORD wGameKindID=m_pGameServiceOption->wKindID;
	WORD wAttribKindID=m_pGameServiceAttrib->wKindID;
	if (m_pGameServiceOption->wNodeID!=0) SetDlgItemInt(IDC_NODE_ID,m_pGameServiceOption->wNodeID);
	if (m_pGameServiceOption->wSortID!=0) SetDlgItemInt(IDC_SORT_ID,m_pGameServiceOption->wSortID);
	if ((wGameKindID!=0)&&(wGameKindID!=wAttribKindID))	SetDlgItemInt(IDC_KIND_ID,m_pGameServiceOption->wKindID);

	//���Կ���
	bCheck=(m_pGameServiceOption->wVisibleMask&MASK_VISIBLE_COMPUTER)!=0; 
	((CButton *)GetDlgItem(IDC_VISIBLE_COMPUTER))->SetCheck(bCheck);

	//�ֻ�����
	bCheck=(m_pGameServiceOption->wVisibleMask&MASK_VISIBLE_MOBILE)!=0; 
	((CButton *)GetDlgItem(IDC_VISIBLE_MOBILE))->SetCheck(bCheck);

	//H5����
	bCheck=(m_pGameServiceOption->wVisibleMask&MASK_VISIBLE_FLASH)!=0; 
	((CButton *)GetDlgItem(IDC_VISIBLE_FLASH))->SetCheck(bCheck);

	return TRUE;
}

//��������
bool CDlgServerOption1::SaveInputInfo()
{
	//�ҽ�����
	m_pGameServiceOption->wKindID=GetDlgItemInt(IDC_KIND_ID);
	m_pGameServiceOption->wNodeID=GetDlgItemInt(IDC_NODE_ID);
	m_pGameServiceOption->wSortID=GetDlgItemInt(IDC_SORT_ID);

	//��������
	m_pGameServiceOption->wMaxPlayer=GetDlgItemInt(IDC_MAX_PLAYER);
	m_pGameServiceOption->lCellScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_CELL_SCORE));
	m_pGameServiceOption->lLessScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_LESS_SCORE));
	m_pGameServiceOption->lRestrictScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_RESTRICT_SCORE));
	m_pGameServiceOption->lRevenue=GetDlgItemInt(IDC_GAME_REVENUE);
	m_pGameServiceOption->lExpGainRate =GetDlgItemInt(IDC_EXPGAIN_RATE);
	GetDlgItemText(IDC_SERVICE_ADDR,m_pGameServiceOption->szServiceAddr,CountArray(m_pGameServiceOption->szServiceAddr));
	GetDlgItemText(IDC_SERVICE_DOMAIN,m_pGameServiceOption->szServiceDomain,CountArray(m_pGameServiceOption->szServiceDomain));

	//��������
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_SERVER_LEVEL);
	if (pComboBox->GetCurSel() == -1)
	{
		m_pGameServiceOption->wServerLevel = 0;		
	}
	else
	{
		m_pGameServiceOption->wServerLevel = (WORD)pComboBox->GetItemData(pComboBox->GetCurSel());
	}

	//��������
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
	{
		pComboBox = (CComboBox*)GetDlgItem(IDC_MATCH_TYPE);
		if (pComboBox->GetCurSel() == -1)
		{
			m_pGameServiceOption->cbMatchType = 0;
		}
		else
		{
			m_pGameServiceOption->cbMatchType = (BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());
		}
	}

	//���Կ���	
	if (((CButton *)GetDlgItem(IDC_VISIBLE_COMPUTER))->GetCheck()) 
	{
		m_pGameServiceOption->wVisibleMask|=MASK_VISIBLE_COMPUTER;
	}
	else
	{
		m_pGameServiceOption->wVisibleMask&=~MASK_VISIBLE_COMPUTER;
	}

	//�ֻ�����
	if (((CButton *)GetDlgItem(IDC_VISIBLE_MOBILE))->GetCheck()) 
	{
		m_pGameServiceOption->wVisibleMask|=MASK_VISIBLE_MOBILE;
	}
	else
	{
		m_pGameServiceOption->wVisibleMask&=~MASK_VISIBLE_MOBILE;
	}
	
	//H5����
	if (((CButton *)GetDlgItem(IDC_VISIBLE_FLASH))->GetCheck()) 
	{
		m_pGameServiceOption->wVisibleMask|=MASK_VISIBLE_FLASH;
	}
	else
	{
		m_pGameServiceOption->wVisibleMask&=~MASK_VISIBLE_FLASH;
	}

	//�շ�ģʽ
	bool bEnable = ((CButton *)GetDlgItem(IDC_FEE))->GetCheck()?true:false;
	CServerRule::SetChargeModeFee( m_pGameServiceOption->dwServerRule,bEnable );

	//��������
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH && m_pGameServiceOption->cbMatchType==MATCH_TYPE_NONE)
	{
		AfxMessageBox(TEXT("��ѡ��������ͣ�"), MB_ICONERROR);
		GetDlgItem(IDC_MATCH_TYPE)->SetFocus();
		return false;
	}

	//˰�ձ���
	if (CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule)==false)
	{
		if (m_pGameServiceOption->lRevenue>=1000||m_pGameServiceOption->lRevenue<0)
		{
			AfxMessageBox(TEXT("��Ϸ˰��ֵ��Ч������д��Ϸ˰��ֵ����Ч��Χ�� 0 �� 999"),MB_ICONERROR);
			GetDlgItem(IDC_GAME_REVENUE)->SetFocus();
			return false;
		}
	}
	else if (m_pGameServiceOption->lRevenue<0)
	{
		AfxMessageBox(TEXT("��Ϸ˰��ֵ��Ч������д��Ϸ˰��ֵ����Чֵ������ڵ���0"),MB_ICONERROR);
		GetDlgItem(IDC_GAME_REVENUE)->SetFocus();
		return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOption1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//���ͱ��
VOID CDlgServerOption1::OnEventServerTypeChange(WORD wServerType)
{
	//���ÿؼ�
	GetDlgItem(IDC_MATCH_TYPE)->ShowWindow((wServerType&GAME_GENRE_MATCH)?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC_MATCH_TYPE)->ShowWindow((wServerType&GAME_GENRE_MATCH)?SW_SHOW:SW_HIDE);
}

//////////////////////////////////////////////////////////////////////////
//����ѡ��2

//���캯��
CDlgServerOption2::CDlgServerOption2() : CDlgServerOptionItem(IDD_SERVER_OPTION_2)
{
}

//��������
CDlgServerOption2::~CDlgServerOption2()
{
}

//��ʼ������
BOOL CDlgServerOption2::OnInitDialog()
{
	__super::OnInitDialog();

	//��ͽ����Ա
	if (m_pGameServiceOption->lMinEnterMember != 0)
		SetDlgItemInt(IDC_EDT_MIN_MEMBER, (UINT)m_pGameServiceOption->lMinEnterMember);
	//��߽����Ա
	if (m_pGameServiceOption->lMaxEnterMember != 0)
		SetDlgItemInt(IDC_EDT_MAX_MEMEBR, (UINT)m_pGameServiceOption->lMaxEnterMember);
	//��ͽ������
	if (m_pGameServiceOption->lMinEnterScore!=0) 
		SetDlgItemInt(IDC_EDT_MIN_ENTER,(UINT)SCORE_ZOOMOUT(m_pGameServiceOption->lMinEnterScore));
	//��߽������
	if (m_pGameServiceOption->lMaxEnterScore!=0) 
		SetDlgItemInt(IDC_EDT_MAX_ENTER,(UINT)SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
	

	//��ʱд��
	BOOL bCheck = CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->SetCheck(bCheck);

	//ÿ�ּ�¼����
	bCheck = CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE))->SetCheck(bCheck);

	//��¼��Ϸ¼��
	bCheck = CServerRule::IsRecordGameVideo(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_RECORD_GAME_VIDEO))->SetCheck(bCheck);

	//�����ֹ����
	bCheck = CServerRule::IsForbidRoomChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_ROOM_CHAT))->SetCheck(bCheck);

	//�����ֹ˽��
	bCheck = CServerRule::IsForbidRoomWisper(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_ROOM_WISPER))->SetCheck(bCheck);

	//��Ϸ��ֹ����
	bCheck = CServerRule::IsForbidGameChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBIT_GAME_CHAT))->SetCheck(bCheck);

	//������Ϸ�������
	bCheck = CServerRule::IsAllowRobotAttend(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_ROBOT_ATTEND))->SetCheck(bCheck);

	//������Ϸ����ռ��
	bCheck = CServerRule::IsAllowRobotSimulate(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_ROBOT_SIMULATE))->SetCheck(bCheck);

	//��������Ϸ״̬�ı�
	bCheck = CServerRule::IsTableEndImmediate(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_TABLE_END_IMMEDIATE))->SetCheck(bCheck);

	//��̬������Ϸ
	bCheck = CServerRule::IsDynamicJoinGame(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_DYNAMIC_JOIN_GAME))->SetCheck(bCheck);

	//ͬ����ֹ˽��
	bCheck = CServerRule::IsForbidPlayChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBIT_PLAY_CHAT))->SetCheck(bCheck);

	//��Ա������
	bCheck = CServerRule::IsMemberCanChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_MEMBER_CAN_CHAT))->SetCheck(bCheck);

	//��ֹ�Թ�
	bCheck = CServerRule::IsForbidLookon(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_LOOKON))->SetCheck(bCheck);

	//��ֹ������
	bCheck = CServerRule::IsForbidBugle(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BUGLE))->SetCheck(bCheck);

	//ǿ�˽�ֹ����
	bCheck = CServerRule::IsForbidForceStandup(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_FORCE_STANDUP))->SetCheck(bCheck);

	//���߳�ʱ��ֹ����
	bCheck = CServerRule::IsForbidOfflineStandup(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_OFFLINE_STANDUP))->SetCheck(bCheck);	

	//�����û���Ϣ
	bCheck = CServerRule::IsHideUserInfo(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_HIDE_USER_INFO))->SetCheck(bCheck);

	//������������
	bCheck = CServerRule::IsAllowVoiceChat(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_VOICE_CHAT))->SetCheck(bCheck);

	//�������͵ͱ�
	bCheck = CServerRule::IsAllowLargessSubsidy(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_LARGESS_Subsidy))->SetCheck(bCheck);

	//����Э���ɢ
	bCheck = CServerRule::IsAllowAgreementDismiss(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_ALLOW_AGREEMENT_DISMISS))->SetCheck(bCheck);	

	//���ݷ���ƥ���û�
	bCheck = CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_MATCH_USER_BY_SCORE))->SetCheck(bCheck);	

	return TRUE;
}

//��������
bool CDlgServerOption2::SaveInputInfo()
{
	//��������
	bool bEnable=false;

	//��������
	m_pGameServiceOption->lMinEnterMember = GetDlgItemInt(IDC_EDT_MIN_MEMBER);
	m_pGameServiceOption->lMaxEnterMember = GetDlgItemInt(IDC_EDT_MAX_MEMEBR);
	m_pGameServiceOption->lMinEnterScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_EDT_MIN_ENTER));
	m_pGameServiceOption->lMaxEnterScore= SCORE_ZOOMIN(GetDlgItemInt(IDC_EDT_MAX_ENTER));	

	//��ʱд��
	bEnable = ((CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->GetCheck()?true:false;
	CServerRule::SetImmediateWriteScore( m_pGameServiceOption->dwServerRule,bEnable );

	//ÿ�ּ�¼����
	bEnable = ((CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE))->GetCheck()?true:false;
	CServerRule::SetRecordGameScore( m_pGameServiceOption->dwServerRule,bEnable );

	//��¼��Ϸ¼��
	bEnable = ((CButton *)GetDlgItem(IDC_RECORD_GAME_VIDEO))->GetCheck()?true:false;
	CServerRule::SetRecordGameVideo( m_pGameServiceOption->dwServerRule,bEnable );

	//�����ֹ����
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_ROOM_CHAT))->GetCheck()?true:false;
	CServerRule::SetForbidRoomChat( m_pGameServiceOption->dwServerRule,bEnable );

	//�����ֹ˽��
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_ROOM_WISPER))->GetCheck()?true:false;
	CServerRule::SetForbidRoomWisper( m_pGameServiceOption->dwServerRule,bEnable );

	//��Ϸ��ֹ����
	bEnable = ((CButton *)GetDlgItem(IDC_FORBIT_GAME_CHAT))->GetCheck()?true:false;
	CServerRule::SetForbidGameChat( m_pGameServiceOption->dwServerRule,bEnable );

	//������Ϸ���������
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_ROBOT_ATTEND))->GetCheck()?true:false;
	CServerRule::SetAllowRobotAttend( m_pGameServiceOption->dwServerRule,bEnable );

	//������Ϸ������ռ��
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_ROBOT_SIMULATE))->GetCheck()?true:false;
	CServerRule::SetAllowRobotSimulate( m_pGameServiceOption->dwServerRule,bEnable );

	//��������Ϸ״̬�ı�
	bEnable = ((CButton *)GetDlgItem(IDC_TABLE_END_IMMEDIATE))->GetCheck()?true:false;
	CServerRule::SetTableEndImmediate( m_pGameServiceOption->dwServerRule,bEnable );

	//��̬������Ϸ
	bEnable = ((CButton *)GetDlgItem(IDC_DYNAMIC_JOIN_GAME))->GetCheck()?true:false;
	CServerRule::SetDynamicJoinGame( m_pGameServiceOption->dwServerRule,bEnable );

	//ͬ����ֹ˽��
	bEnable = ((CButton *)GetDlgItem(IDC_FORBIT_PLAY_CHAT))->GetCheck()?true:false;
	CServerRule::SetForbidPlayChat( m_pGameServiceOption->dwServerRule,bEnable );

	//��Ա������
	bEnable = ((CButton *)GetDlgItem(IDC_MEMBER_CAN_CHAT))->GetCheck()?true:false;
	CServerRule::SetMemberCanChat( m_pGameServiceOption->dwServerRule,bEnable );

	//��ֹ�Թ�
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_LOOKON))->GetCheck()?true:false;
	CServerRule::SetForbidLookon( m_pGameServiceOption->dwServerRule,bEnable );

	//��ֹ������
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BUGLE))->GetCheck()?true:false;
	CServerRule::SetForbidBugle(m_pGameServiceOption->dwServerRule,bEnable);

	//ǿ�˽�ֹ����
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_FORCE_STANDUP))->GetCheck()?true:false;
	CServerRule::SetForbidForceStandup(m_pGameServiceOption->dwServerRule,bEnable);

	//���߳�ʱ��ֹ����
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_OFFLINE_STANDUP))->GetCheck()?true:false;
	CServerRule::SetForbidOfflineStandup(m_pGameServiceOption->dwServerRule,bEnable);

	//�����û���Ϣ
	bEnable = ((CButton *)GetDlgItem(IDC_HIDE_USER_INFO))->GetCheck()?true:false;
	CServerRule::SetHideUserInfo(m_pGameServiceOption->dwServerRule,bEnable);	

	//������������
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_VOICE_CHAT))->GetCheck()?true:false;
	CServerRule::SetAllowVoiceChat(m_pGameServiceOption->dwServerRule,bEnable);	

	//�������͵ͱ�
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_LARGESS_Subsidy))->GetCheck()?true:false;
	CServerRule::SetAllowLargessSubsidy(m_pGameServiceOption->dwServerRule,bEnable);	

	//����Э���ɢ
	bEnable = ((CButton *)GetDlgItem(IDC_ALLOW_AGREEMENT_DISMISS))->GetCheck()?true:false;
	CServerRule::SetAllowAgreementDismiss(m_pGameServiceOption->dwServerRule,bEnable);	

	//���ݷ���ƥ���û�
	bEnable = ((CButton *)GetDlgItem(IDC_MATCH_USER_BY_SCORE))->GetCheck()?true:false;
	CServerRule::SetMatchUserByScore(m_pGameServiceOption->dwServerRule,bEnable);	

	//�������͵ͱ�
	if (CServerRule::IsAllowLargessSubsidy(m_pGameServiceOption->dwServerRule)==true &&
	  (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_BATTLE))==0)
	{
		AfxMessageBox(TEXT("��Ǹ��ֻ�вƸ����ͷ����֧�ֵͱ����ͣ�"),MB_ICONERROR);
		return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOption2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
//����ѡ��3

//���캯��
CDlgServerOption3::CDlgServerOption3() : CDlgServerOptionItem(IDD_SERVER_OPTION_3)
{
}

//��������
CDlgServerOption3::~CDlgServerOption3()
{
}

//��ʼ������
BOOL CDlgServerOption3::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	bool bCheck=false;

	//��ֹʤ������
	bCheck = CServerRule::IsForbidWinRateSet(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_WINRATE))->SetCheck(bCheck);

	//��ֹ��������
	bCheck = CServerRule::IsForbidFleeRateSet(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_FLEERATE))->SetCheck(bCheck);

	//��ֹ�ɼ�����
	bCheck = CServerRule::IsForbidScoreLimit(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_SCORELIMIT))->SetCheck(bCheck);

	//��ֹ�������ܻ�ӭ�����Ϸ������
	bCheck = CServerRule::IsForbidBlackListSet(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BLACK))->SetCheck(bCheck);

	//��ֹIP����
	bCheck = CServerRule::IsForbidIPLimit(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_IPSET))->SetCheck(bCheck);

	//��ֹ��������
	bCheck = CServerRule::IsForbidPasswordSet(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_PASSWORD))->SetCheck(bCheck);

	//��ֹ��ʾ�׷�
	bCheck = CServerRule::IsForbidShowBaseScore(m_pGameServiceOption->dwServerRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_TBLE_SHOW_BASE_SCORE))->SetCheck(bCheck);

	//��ֹ���û�
	bCheck=CBattleRule::IsForbidBindUser(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BIND_USER))->SetCheck(bCheck);

	//��ֹ���ͳɼ�
	bCheck=CBattleRule::IsForbidSendGrade(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_SEND_GRADE))->SetCheck(bCheck);

	//��ֹд�뽱��
	bCheck=CBattleRule::IsForbidBattleReward(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BATTLE_REWARD))->SetCheck(bCheck);

	//��ֹԼս��¼
	bCheck=CBattleRule::IsForbidBattleRecord(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_BATTLE_RECORD))->SetCheck(bCheck);

	//��ֹͬIP����
	bCheck=CBattleRule::IsForbidSameIPRemind(m_pGameServiceOption->dwBattleRule)?TRUE:FALSE;
	((CButton *)GetDlgItem(IDC_FORBID_SAMEIP_REMIND))->SetCheck(bCheck);
	

	return TRUE;
}

//��������
bool CDlgServerOption3::SaveInputInfo()
{
	//��������
	bool bEnable=false;

	//��ֹʤ������
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_WINRATE))->GetCheck()?true:false;
	CServerRule::SetForbidWinRateSet(m_pGameServiceOption->dwServerRule,bEnable);

	//��ֹ��������
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_FLEERATE))->GetCheck()?true:false;
	CServerRule::SetForbidFleeRateSet(m_pGameServiceOption->dwServerRule,bEnable);

	//��ֹ�ɼ�����
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_SCORELIMIT))->GetCheck()?true:false;
	CServerRule::SetForbidScoreLimit(m_pGameServiceOption->dwServerRule,bEnable);

	//��ֹ�������ܻ�ӭ�����Ϸ������
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BLACK))->GetCheck()?true:false;
	CServerRule::SetForbidBlackListSet(m_pGameServiceOption->dwServerRule,bEnable);

	//��ֹIP����
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_IPSET))->GetCheck()?true:false;
	CServerRule::SetForbidIPLimit(m_pGameServiceOption->dwServerRule,bEnable);

	//��ֹ��������
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_PASSWORD))->GetCheck()?true:false;
	CServerRule::SetForbidPasswordSet(m_pGameServiceOption->dwServerRule,bEnable);

	//��ֹ��ʾ�׷�
	bEnable = ((CButton *)GetDlgItem(IDC_TBLE_SHOW_BASE_SCORE))->GetCheck()?true:false;
	CServerRule::SetForbidShowBaseScore( m_pGameServiceOption->dwServerRule,bEnable );

	//��ֹ���û�
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BIND_USER))->GetCheck()?true:false;
	CBattleRule::SetForbidBindUser(m_pGameServiceOption->dwBattleRule,bEnable );

	//��ֹ���ͳɼ�
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_SEND_GRADE))->GetCheck()?true:false;
	CBattleRule::SetForbidSendGrade(m_pGameServiceOption->dwBattleRule,bEnable );

	//��ֹд�뽱��
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BATTLE_REWARD))->GetCheck()?true:false;
	CBattleRule::SetForbidWriteReward(m_pGameServiceOption->dwBattleRule,bEnable );

	//��ֹԼս��¼
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_BATTLE_RECORD))->GetCheck()?true:false;
	CBattleRule::SetForbidBattleRecord(m_pGameServiceOption->dwBattleRule,bEnable );

	//��ֹͬIP����
	bEnable = ((CButton *)GetDlgItem(IDC_FORBID_SAMEIP_REMIND))->GetCheck()?true:false;
	CBattleRule::SetForbidSameIPRemind(m_pGameServiceOption->dwBattleRule,bEnable );

	return true;
}

//�����ؼ�
VOID CDlgServerOption3::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
//����ѡ��4

//���캯��
CDlgServerOption4::CDlgServerOption4() : CDlgServerOptionItem(IDD_SERVER_OPTION_4)
{
}

//��������
CDlgServerOption4::~CDlgServerOption4()
{
}

//��ʼ������
BOOL CDlgServerOption4::OnInitDialog()
{
	__super::OnInitDialog();

	//����ģʽ
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_IMMEDIATELY)
	{
		//���ÿؼ�
		CheckRadioButton(IDC_RADIO_IMMEDIATELY_MODE,IDC_RADIO_DELAYWAIT_MODE,IDC_RADIO_IMMEDIATELY_MODE);
	}

	//��ʱģʽ
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
	{
		//���ÿؼ�
		CheckRadioButton(IDC_RADIO_IMMEDIATELY_MODE,IDC_RADIO_DELAYWAIT_MODE,IDC_RADIO_DELAYWAIT_MODE);
	}

	//�ؼ�����
	SetDlgItemInt(IDC_MIN_PLAYER_COUNT,m_pGameServiceOption->wMinPlayerCount);
	SetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT,m_pGameServiceOption->wMinDistributeUser);
	SetDlgItemInt(IDC_DISTRIBUTE_INTERVAL,m_pGameServiceOption->wDistributeTimeSpace);
	SetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER,m_pGameServiceOption->wMinPartakeGameUser);
	SetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER,m_pGameServiceOption->wMaxPartakeGameUser);

	//ͬ������
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_SAME_TABLE)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_TABLE))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_TABLE))->SetCheck(BST_UNCHECKED);
	}

	//��ַ����
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(BST_UNCHECKED);
	}

	//��������
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_SAME_MACHINE)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_MACHINE))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_MACHINE))->SetCheck(BST_UNCHECKED);
	}

	//��������
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_PLAYER_FIRST)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_PLAYER_FIRST))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_PLAYER_FIRST))->SetCheck(BST_UNCHECKED);
	}


	return TRUE;	
}

//��������
bool CDlgServerOption4::SaveInputInfo()
{
	//��ȡ����
	m_pGameServiceOption->wMinPlayerCount=GetDlgItemInt(IDC_MIN_PLAYER_COUNT);
	m_pGameServiceOption->wMinDistributeUser=GetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT);
	m_pGameServiceOption->wDistributeTimeSpace=GetDlgItemInt(IDC_DISTRIBUTE_INTERVAL);
	m_pGameServiceOption->wMinPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER);
	m_pGameServiceOption->wMaxPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER);

	//����ģʽ
	if (((CButton *)GetDlgItem(IDC_RADIO_IMMEDIATELY_MODE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeMode=DISTRIBUTE_MODE_IMMEDIATELY;
	}

	//��ʱģʽ
	if (((CButton *)GetDlgItem(IDC_RADIO_DELAYWAIT_MODE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeMode=DISTRIBUTE_MODE_DELAYWAIT;
	}

	//ͬ������
	if (((CButton *)GetDlgItem(IDC_ALLOW_SAME_TABLE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_SAME_TABLE;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_SAME_TABLE;
	}

	//��ַ����
	if (((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_SAME_ADDRESS;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_SAME_ADDRESS;
	}

	//��������
	if (((CButton *)GetDlgItem(IDC_ALLOW_SAME_MACHINE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_SAME_MACHINE;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_SAME_MACHINE;
	}

	//��������
	if (((CButton *)GetDlgItem(IDC_ALLOW_PLAYER_FIRST))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_PLAYER_FIRST;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_PLAYER_FIRST;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOption4::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizardStep1::CDlgServerWizardStep1() : CDlgServerWizardItem(IDD_SERVER_WIZARD_1)
{
	//���ñ���
	m_pGameModuleInfo=NULL;

	return;
}

//��������
CDlgServerWizardStep1::~CDlgServerWizardStep1()
{
}

//�ؼ���
VOID CDlgServerWizardStep1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROMPT, m_StaticPrompt);
	DDX_Control(pDX, IDC_MODULE_LIST, m_ModuleListControl);
}

//��ʼ������
BOOL CDlgServerWizardStep1::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetDlgItemText(IDC_PROMPT,TEXT("�������������Ϸ�б���ѡ����Ϸ�����"));

	//�����б�
	LoadDBModuleItem();

	return TRUE;
}

//��������
bool CDlgServerWizardStep1::SaveInputInfo()
{
	//��������
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_ModuleListControl.GetFirstSelectedItemPosition();
	
	//��ȡѡ��
	if (Position!=NULL)
	{
		INT nListItem=m_ModuleListControl.GetNextSelectedItem(Position);
		pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);
	}

	//ѡ���ж�
	if (pGameModuleInfo==NULL)
	{
		AfxMessageBox(TEXT("�����ȴ���Ϸ�б���ѡ����Ϸ���"),MB_ICONERROR);
		return false;
	}

	//���ñ���
	if (m_pGameModuleInfo!=pGameModuleInfo)
	{
		//��Ϸģ��
		m_GameServiceManager.CloseInstance();
		m_GameServiceManager.SetModuleCreateInfo(pGameModuleInfo->szServerDLLName,GAME_SERVICE_CREATE_NAME);

		//�����ж�
		if (pGameModuleInfo->dwNativeVersion==0)
		{
			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] ���������û�а�װ�����Ȱ�װ�������"),pGameModuleInfo->szModuleName);

			//��ʾ��Ϣ
			AfxMessageBox(szString,MB_ICONERROR);

			return false;
		}

		//�����ж�
		if (pGameModuleInfo->dwNativeVersion!=pGameModuleInfo->dwModuleVersion)
		{
			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] ������������ˣ����ذ汾���ͣ��޷�������Ϸ���䣡"),pGameModuleInfo->szModuleName);

			//��ʾ��Ϣ
			AfxMessageBox(szString,MB_ICONERROR);
			return false;
		}

		//����ģ��
		if (m_GameServiceManager.CreateInstance()==false)
		{
			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] �����������ʧ�ܣ�������Ϸ����ʧ��(%s)"),pGameModuleInfo->szModuleName,m_GameServiceManager.GetErrorDescribe());

			//��ʾ��Ϣ
			AfxMessageBox(szString,MB_ICONERROR);

			return false;
		}

		//����ģ��
		m_pGameModuleInfo=pGameModuleInfo;
		ZeroMemory(m_pGameServiceOption,sizeof(tagGameServiceOption));
		ZeroMemory(m_pGameServiceAttrib,sizeof(m_pGameServiceAttrib));

		//Ĭ������
		CServerRule::SetForbidShowBaseScore(m_pGameServiceOption->dwServerRule,true);
		m_pDlgServerWizard->SetGameServiceManager(m_GameServiceManager.GetInterface());

		return true;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerWizardStep1::RectifyControl(INT nWidth, INT nHeight)
{
	//������ʾ
	if (m_StaticPrompt.m_hWnd!=NULL)
	{
		m_StaticPrompt.SetWindowPos(NULL,5,8,nWidth-10,12,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//�����б�
	if (m_ModuleListControl.m_hWnd!=NULL)
	{
		m_ModuleListControl.SetWindowPos(NULL,5,28,nWidth-10,nHeight-28,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	return;
}

//�����б�
bool CDlgServerWizardStep1::LoadDBModuleItem()
{
	//������Ϣ
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer)==true)
	{
		//�����б�
		m_ModuleListControl.DeleteAllItems();

		//��������
		POSITION Position=m_ModuleInfoBuffer.m_GameModuleInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position!=NULL)
		{
			//��ȡ����
			WORD wModuleID=0L;
			tagGameModuleInfo * pGameModuleInfo=NULL;
			m_ModuleInfoBuffer.m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);

			//�����б�
			ASSERT(pGameModuleInfo!=NULL);
			if (pGameModuleInfo!=NULL) m_ModuleListControl.InsertModuleInfo(pGameModuleInfo);
		}
	}

	return false;
}

//˫���б�
VOID CDlgServerWizardStep1::OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//��������
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		tagGameModuleInfo * pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);

		//���°汾
		if( pGameModuleInfo->dwModuleVersion < pGameModuleInfo->dwNativeVersion )
		{
			CModuleInfoManager ModuleInfoManager;
			if( !ModuleInfoManager.UpdateGameModuleInfo(*pGameModuleInfo) ) return;
		}

		//Ͷ����Ϣ
		if (pGameModuleInfo->dwNativeVersion!=0L)
		{
			GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDC_NEXT,0),0);
		}

		return;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizardStep2::CDlgServerWizardStep2() : CDlgServerWizardItem(IDD_SERVER_WIZARD_2)
{
}

//��������
CDlgServerWizardStep2::~CDlgServerWizardStep2()
{
}

//��ʼ������
BOOL CDlgServerWizardStep2::OnInitDialog()
{
	__super::OnInitDialog();
	
	//��ʼ������
	InitBaseOption();

	//��ȡ�ؼ�
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_SERVER_OPTION);
	pTabCtrl->SetItemSize(CSize(140,25));

	//��������
	pTabCtrl->InsertItem(TAB_SERVER_OPTION1,TEXT("��������"));
	pTabCtrl->InsertItem(TAB_SERVER_OPTION2,TEXT("����ѡ��"));
	pTabCtrl->InsertItem(TAB_SERVER_OPTION3,TEXT("����ѡ��"));
	pTabCtrl->InsertItem(TAB_SERVER_OPTION4,TEXT("��������"));	
	pTabCtrl->InsertItem(TAB_SERVER_OPTION5,TEXT("������"));		

	//��������
	m_pDlgServerOptionItem[0]=&m_DlgServerOption1;
	m_pDlgServerOptionItem[1]=&m_DlgServerOption2;
	m_pDlgServerOptionItem[2]=&m_DlgServerOption3;
	m_pDlgServerOptionItem[3]=&m_DlgServerOption4;	
	m_pDlgServerOptionItem[4]=&m_DlgServerOption5;	

	//��������
	CRect rcCreate(0,0,0,0);

	//��������
	for (int i=0; i<CountArray(m_pDlgServerOptionItem); i++)
	{
		//ָ���ж�
		if (m_pDlgServerOptionItem[i]==NULL) continue;

		//���ÿؼ�		
		m_pDlgServerOptionItem[i]->m_pDlgServerWizard=m_pDlgServerWizard;
		m_pDlgServerOptionItem[i]->m_pGameServiceAttrib=m_pGameServiceAttrib;
		m_pDlgServerOptionItem[i]->m_pGameServiceOption=m_pGameServiceOption;

		//��������		
		m_pDlgServerOptionItem[i]->CreateOptionItem(rcCreate,pTabCtrl);
	}

	//��������
	ActiveOptionItem(TAB_SERVER_OPTION1);

	//�ֶ�����
	OnCbnSelchangeServerType();

	return TRUE;
}

//��������
bool CDlgServerWizardStep2::SaveInputInfo()
{
	//��������
	if (SaveBaseOption()==false) return false;

	//��������
	for (int i=0; i<CountArray(m_pDlgServerOptionItem); i++)
	{
		if (m_pDlgServerOptionItem[i]->SaveInputInfo() == false) return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerWizardStep2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//��������
VOID CDlgServerWizardStep2::InitBaseOption()
{
	//��������
	((CEdit *)GetDlgItem(IDC_TABLE_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_SERVER_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_NAME))->LimitText(LEN_SERVER-1);

	//��������
	WORD wSupportType = CServerRule::GetSupportType();
	m_pGameServiceAttrib->wSupporType &= wSupportType;

	//��������
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	WORD wServerType[]={GAME_GENRE_GOLD,GAME_GENRE_SCORE,GAME_GENRE_MATCH,GAME_GENRE_BATTLE,GAME_GENRE_RANKING};
	LPCTSTR pszServerType[]={TEXT("�Ƹ�����"),TEXT("��������"),TEXT("��������"),TEXT("Լս����"),TEXT("��λ����") };

	//��������
	for (INT i=0;i<CountArray(wServerType);i++)
	{
		if ((m_pGameServiceAttrib->wSupporType&wServerType[i])==0) continue;
		pComboBox->SetItemData(pComboBox->AddString(pszServerType[i]),wServerType[i]);
	}

	//��Ϸ����
	for (INT i=0;i<CountArray(wServerType);i++)
	{
		WORD wItemServerType=(WORD)pComboBox->GetItemData(i);
		if ((wItemServerType&m_pGameServiceOption->wServerType)!=0) pComboBox->SetCurSel(i);
	}

	//��������
	CComboBox * pTableComboBox=(CComboBox *)GetDlgItem(IDC_TABLE_GENRE);
	WORD wTableGenre[]={TABLE_GENRE_NORMAL,TABLE_GENRE_DISTRIBUTE};
	LPCTSTR pszTableGenre[]={TEXT("��ͨ����"),TEXT("ϵͳ����")};

	//����ģʽ
	for (INT i=0;i<CountArray(wTableGenre);i++)
	{		
		if ((m_pGameServiceAttrib->wSupporType&wTableGenre[i])==0) continue;
		pTableComboBox->SetItemData(pTableComboBox->AddString(pszTableGenre[i]),wTableGenre[i]);		
	}

	//Ĭ������
	if (pTableComboBox->GetCount()==0)
	{		
		pTableComboBox->SetItemData(pTableComboBox->AddString(pszTableGenre[0]),wTableGenre[0]);						
	}

	//����ģʽ
	for (INT i=0;i<CountArray(wTableGenre);i++)
	{
		WORD wItemServerType=(WORD)pTableComboBox->GetItemData(i);
		if ((wItemServerType&m_pGameServiceOption->wServerType)!=0) 
		{
			pTableComboBox->SetCurSel(i);
			break;
		}
	}

	//Ĭ��ѡ��
	if (pTableComboBox->GetCurSel()==-1)
	{	
		for (INT i=0;i<CountArray(wTableGenre);i++)
		{
			if ((pTableComboBox->GetItemData(i)&wTableGenre[0])!=0) 
			{
				pTableComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//��������
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceAttrib->szGameName);
	}
	else SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName);

	//��ȡ���ݿ�
	CDatabaseListManager *pDBListMgr = CDatabaseListManager::GetInstance();
	if( pDBListMgr )
	{
		CComboBox *pcomboDB = ((CComboBox *)GetDlgItem(IDC_COMBO_DB));
		pcomboDB->ResetContent();

		POSITION pos = pDBListMgr->GetStartPosition();
		while( pos )
		{
			tagDataBaseList *pDBList = pDBListMgr->GetNext(pos);
			ASSERT( pDBList );
			if( !pDBList ) break;

			//��������
			TCHAR szDBName[128] = {0};
			_sntprintf_s( szDBName,CountArray(szDBName),TEXT("%s(%s)"),pDBList->szDBAliasName,pDBList->szDBName );
			int nIndex = pcomboDB->AddString(szDBName);
			if( CB_ERR == nIndex ) break;

			//��������
			pcomboDB->SetItemData( nIndex,pDBList->wDBListID );

			//ѡ����Ŀ
			if( pDBList->wDBListID == m_pGameServiceOption->wDBListID )
			{
				pcomboDB->SetCurSel(nIndex);
			}
		}
	}

	//��������
	if (m_pGameServiceOption->wServerPort!=0) 
	{
		SetDlgItemInt(IDC_SERVER_PORT,(LONG)m_pGameServiceOption->wServerPort);
	}
	else
	{
		SetDlgItemText(IDC_SERVER_PORT,TEXT("�Զ�����"));
	}

	if (m_pGameServiceOption->wTableCount!=0) SetDlgItemInt(IDC_TABLE_COUNT,(LONG)m_pGameServiceOption->wTableCount);
}

//��������
bool CDlgServerWizardStep2::SaveBaseOption()
{
	//��������
	m_pGameServiceOption->wServerPort=GetDlgItemInt(IDC_SERVER_PORT);
	m_pGameServiceOption->wTableCount=GetDlgItemInt(IDC_TABLE_COUNT);

	//��Ϸ����
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	m_pGameServiceOption->wServerType=(WORD)pServerType->GetItemData(pServerType->GetCurSel());

	//����ģʽ
	CComboBox * pTableComboBox=(CComboBox *)GetDlgItem(IDC_TABLE_GENRE);
	m_pGameServiceOption->wServerType|=(WORD)pTableComboBox->GetItemData(pTableComboBox->GetCurSel());

	//�ַ�����
	GetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName,CountArray(m_pGameServiceOption->szServerName));

	//��������
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		AfxMessageBox(TEXT("�������ֲ���Ϊ�գ���������Ϸ��������"),MB_ICONERROR);
		GetDlgItem(IDC_SERVER_NAME)->SetFocus();
		return false;
	}

	//��Ϸ����
	if ((m_pGameServiceOption->wServerType==0)||(m_pGameServiceOption->wServerType==INVALID_WORD))
	{
		AfxMessageBox(TEXT("��������ģʽ��Ч����ѡ�񷿼������ģʽ"),MB_ICONERROR);
		GetDlgItem(IDC_SERVER_TYPE)->SetFocus();
		
		return false;
	}

	//������Ŀ
	if ((m_pGameServiceOption->wTableCount==0)||(m_pGameServiceOption->wTableCount>MAX_TABLE))
	{
		AfxMessageBox(TEXT("������Ŀ��Ч������д��Ϸ������Ŀ����Ч��Χ�� 1 �� 512"),MB_ICONERROR);
		GetDlgItem(IDC_TABLE_COUNT)->SetFocus();
		return false;
	}

	//������Ŀ
	if ((m_pGameServiceOption->wTableCount>1)&&(m_pGameServiceAttrib->wChairCount>=MAX_CHAIR))
	{
		AfxMessageBox(TEXT("������Ŀ��Ч��������Ϸֻ������һ�����ӣ�"),MB_ICONERROR);
		GetDlgItem(IDC_TABLE_COUNT)->SetFocus();
		return false;
	}

	//���ݿ�����
	int nCurSel=((CComboBox *)GetDlgItem(IDC_COMBO_DB))->GetCurSel();
	if (nCurSel!=CB_ERR)
	{
		m_pGameServiceOption->wDBListID=(WORD)((CComboBox *)GetDlgItem(IDC_COMBO_DB))->GetItemData(nCurSel);
	}
	else
	{
		AfxMessageBox(TEXT("��ѡ��һ����Ч�����ݿ�"),MB_ICONERROR);
		GetDlgItem(IDC_COMBO_DB)->SetFocus();
		return false;
	}
	
	return true;
}

//��������
VOID CDlgServerWizardStep2::ActiveOptionItem(BYTE cbItemIndex)
{
	//�ж�����
	if (m_cbItemIndex==cbItemIndex) return;

	//��������
	m_cbItemIndex=cbItemIndex;

	//�ؼ�����
	CRect rcTabCtrl;
	GetDlgItem(IDC_SERVER_OPTION)->GetClientRect(&rcTabCtrl);	

	//���ÿؼ�
	if (m_pDlgServerOptionItem[m_cbItemIndex]->m_hWnd!=NULL)
	{
		m_pDlgServerOptionItem[m_cbItemIndex]->SetWindowPos(NULL,4,30,rcTabCtrl.Width()-10,rcTabCtrl.Height()-30-4,SWP_NOZORDER|SWP_SHOWWINDOW);		
	}

	//���ÿؼ�
	for (int i=0; i<CountArray(m_pDlgServerOptionItem); i++)
	{
		if (i==m_cbItemIndex) continue;
		if (m_pDlgServerOptionItem[i]->m_hWnd!=NULL) m_pDlgServerOptionItem[i]->ShowWindow(SW_HIDE);
	}
}

//ѡ����:
void CDlgServerWizardStep2::OnCbnSelchangeServerType()
{
	//��Ϸ����
	CComboBox* pServerType = (CComboBox*)GetDlgItem(IDC_SERVER_TYPE);
	auto wServerType = (WORD)pServerType->GetItemData(pServerType->GetCurSel());

	//֪ͨ���
	for (int i = 0; i < CountArray(m_pDlgServerOptionItem); i++)
	{
		m_pDlgServerOptionItem[i]->OnEventServerTypeChange(wServerType);
	}
}

//ѡ����
void CDlgServerWizardStep2::OnTcnSelchangeServerOption(NMHDR *pNMHDR, LRESULT *pResult)
{
	//��ȡ�ؼ�
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_SERVER_OPTION);
	ActiveOptionItem(pTabCtrl->GetCurSel());	
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionCustom::CDlgServerOptionCustom() : CDlgServerOptionItem(IDD_SERVER_OPTION_CUSTOM)
{
	//���ñ���
	m_hCustomRule=NULL;

	return;
}

//��������
CDlgServerOptionCustom::~CDlgServerOptionCustom()
{
}

//��ʼ������
BOOL CDlgServerOptionCustom::OnInitDialog()
{
	__super::OnInitDialog();

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	IGameServiceCustomRule * pIGameServiceCustomRule=m_pDlgServerWizard->m_pIGameServiceCustomRule;
	if( pIGameServiceCustomRule == NULL ) return TRUE;

	//��������
	m_hCustomRule=pIGameServiceCustomRule->CreateCustomRule(this,rcClient,m_pGameServiceOption->cbCustomRule,sizeof(m_pGameServiceOption->cbCustomRule));

	//����Ĭ��ֵ
	if (m_pGameServiceOption->wServerID == 0)
	{
		pIGameServiceCustomRule->DefaultCustomRule(m_pGameServiceOption->cbCustomRule, sizeof(m_pGameServiceOption->cbCustomRule));
	}

	return TRUE;
}

//��������
bool CDlgServerOptionCustom::SaveInputInfo()
{
	IGameServiceCustomRule * pIGameServiceCustomRule=m_pDlgServerWizard->m_pIGameServiceCustomRule;
	if (pIGameServiceCustomRule==NULL) return true;

	//��������
	if ((m_hCustomRule!=NULL)&&(m_pDlgServerWizard->m_pIGameServiceCustomRule->SaveCustomRule(m_pGameServiceOption->cbCustomRule, sizeof(m_pGameServiceOption->cbCustomRule))==false))
	{
		return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOptionCustom::RectifyControl(INT nWidth, INT nHeight)
{
	//����λ��
	if (m_hCustomRule!=NULL) ::SetWindowPos(m_hCustomRule,NULL,0,0,nWidth,nHeight,SWP_NOMOVE|SWP_NOZORDER);

	return;
}

//������Ϣ
VOID CDlgServerOptionCustom::OnNcDestroy()
{
	//���ñ���
	m_hCustomRule=NULL;

	__super::OnNcDestroy();
}

//������Ϣ
VOID CDlgServerOptionCustom::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	//���ý���
	if (m_hCustomRule!=NULL) ::SetFocus(m_hCustomRule);

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizard::CDlgServerWizard() : CDialog(IDD_SERVER_WIZARD)
{
	//���ñ���
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_ServerWizardStep1;
	m_pWizardItem[1]=&m_ServerWizardStep2;

	//�ӿڱ���
	m_pIGameServiceManager=NULL;
	m_pIGameServiceCustomRule=NULL;

	//������Ϣ
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//��������
CDlgServerWizard::~CDlgServerWizard()
{
}

//��ʼ������
BOOL CDlgServerWizard::OnInitDialog()
{
	__super::OnInitDialog();

	m_DBListMgr.Load();

	//������
	ActiveWizardItem((m_pIGameServiceManager==NULL)?0:1);

	return FALSE;
}

//ȷ������
VOID CDlgServerWizard::OnOK()
{
	OnBnClickedFinish();

	return;
}

//��ȡ����
VOID CDlgServerWizard::GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//���ñ���
	GameServiceAttrib=m_ModuleInitParameter.GameServiceAttrib;

	return;
}

//��ȡ����
VOID CDlgServerWizard::GetGameServiceOption(tagGameServiceOption & GameServiceOption)
{
	//���ñ���
	GameServiceOption=m_ModuleInitParameter.GameServiceOption;

	return;
}

//��������
VOID CDlgServerWizard::SetGameServiceOption(tagGameServiceOption & GameServiceOption)
{
	//���ñ���
	m_ModuleInitParameter.GameServiceOption=GameServiceOption;

	return;
}

//���ýӿ�
VOID CDlgServerWizard::SetGameServiceManager(IGameServiceManager * pIGameServiceManager)
{
	//��������
	if (m_wActiveIndex==0)
	{
		for (WORD i=1;i<CountArray(m_pWizardItem);i++)
		{
			if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->m_hWnd!=NULL))
			{
				m_pWizardItem[i]->DestroyWindow();
			}
		}
	}

	//����ӿ�
	m_pIGameServiceManager=pIGameServiceManager;

	//��������
	pIGameServiceManager->GetGameServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//�Զ��ӿ�
	m_pIGameServiceCustomRule=QUERY_OBJECT_PTR_INTERFACE(m_pIGameServiceManager,IGameServiceCustomRule);

	return;
}

//������
bool CDlgServerWizard::ActiveWizardItem(WORD wIndex)
{
	//�ж�״̬
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//�������
	CDlgServerWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//���ñ���
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//��������
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	} 

	//��ȡλ��
	CRect rcItemRect;
	GetDlgItem(IDC_ITEM_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//��������
	m_pWizardItem[wIndex]->m_pDlgServerWizard=this;
	m_pWizardItem[wIndex]->m_pGameServiceAttrib=&m_ModuleInitParameter.GameServiceAttrib;
	m_pWizardItem[wIndex]->m_pGameServiceOption=&m_ModuleInitParameter.GameServiceOption;

	//��������
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//��������
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//�������	
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);
	pButtonFinish->EnableWindow((m_wActiveIndex>0)?TRUE:FALSE);
	
	//�������
	TCHAR szTitle[128]=TEXT("");
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("���������� --- [ ���� %d ]"),m_wActiveIndex+1);
	
	//���ñ���
	SetWindowText(szTitle);

	return true;
}

//��һ��
VOID CDlgServerWizard::OnBnClickedNext()
{
	//Ч�����
	ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//����ҳ��
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//��ɰ�ť
VOID CDlgServerWizard::OnBnClickedFinish()
{
	//��������
	ASSERT(m_wActiveIndex>=1);
	if (m_pWizardItem[m_wActiveIndex]->SaveItemData()==false) return;

	//��������
	for (WORD i=0;i<CountArray(m_pWizardItem);i++)
	{
		if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->SaveItemData()==false))
		{
			ActiveWizardItem(i);
			return;
		}
	}

	//������Ϣ
	tagCreateServerInfo CreateServerInfo;
	ZeroMemory(&CreateServerInfo,sizeof(CreateServerInfo));

	//��������
	ASSERT(m_pIGameServiceManager!=NULL);
	m_pIGameServiceManager->RectifyOptionParameter(m_ModuleInitParameter.GameServiceOption);

	//ģ������
	CreateServerInfo.wModuleID=m_ModuleInitParameter.GameServiceAttrib.wKindID;
	CreateServerInfo.wServerID=m_ModuleInitParameter.GameServiceOption.wServerID;
	CreateServerInfo.wDBListID = m_ModuleInitParameter.GameServiceOption.wDBListID;
	CreateServerInfo.cbMatchType = m_ModuleInitParameter.GameServiceOption.cbMatchType;

	//�ҽ�����
	CreateServerInfo.wKindID=m_ModuleInitParameter.GameServiceOption.wKindID;
	CreateServerInfo.wNodeID=m_ModuleInitParameter.GameServiceOption.wNodeID;
	CreateServerInfo.wSortID=m_ModuleInitParameter.GameServiceOption.wSortID;
	CreateServerInfo.wVisibleMask=m_ModuleInitParameter.GameServiceOption.wVisibleMask;

	//��������
	CreateServerInfo.lRevenue=m_ModuleInitParameter.GameServiceOption.lRevenue;
	CreateServerInfo.lCellScore = m_ModuleInitParameter.GameServiceOption.lCellScore;
	CreateServerInfo.lLessScore = m_ModuleInitParameter.GameServiceOption.lLessScore;
	CreateServerInfo.lExpGainRate = m_ModuleInitParameter.GameServiceOption.lExpGainRate;
	CreateServerInfo.lRestrictScore=m_ModuleInitParameter.GameServiceOption.lRestrictScore;
	CreateServerInfo.lMinEnterScore = m_ModuleInitParameter.GameServiceOption.lMinEnterScore;
	CreateServerInfo.lMaxEnterScore = m_ModuleInitParameter.GameServiceOption.lMaxEnterScore;
	CreateServerInfo.lMinEnterMember = m_ModuleInitParameter.GameServiceOption.lMinEnterMember;
	CreateServerInfo.lMaxEnterMember = m_ModuleInitParameter.GameServiceOption.lMaxEnterMember;
	
	//��������
	StringCchCopy(CreateServerInfo.szServiceAddr, CountArray(CreateServerInfo.szServiceAddr),m_ModuleInitParameter.GameServiceOption.szServiceAddr);
	StringCchCopy(CreateServerInfo.szServiceDomain, CountArray(CreateServerInfo.szServiceDomain),m_ModuleInitParameter.GameServiceOption.szServiceDomain);

	//Լս����
	CreateServerInfo.dwBattleRule = m_ModuleInitParameter.GameServiceOption.dwBattleRule;

	//������Ϣ
	CreateServerInfo.wMaxPlayer=m_ModuleInitParameter.GameServiceOption.wMaxPlayer;
	CreateServerInfo.wTableCount=m_ModuleInitParameter.GameServiceOption.wTableCount;
	CreateServerInfo.wServerType=m_ModuleInitParameter.GameServiceOption.wServerType;
	CreateServerInfo.wServerPort=m_ModuleInitParameter.GameServiceOption.wServerPort;
	CreateServerInfo.wServerLevel=m_ModuleInitParameter.GameServiceOption.wServerLevel;
	CreateServerInfo.dwServerRule=m_ModuleInitParameter.GameServiceOption.dwServerRule;
	CreateServerInfo.dwAttachUserRight = m_ModuleInitParameter.GameServiceOption.dwAttachUserRight;	
	StringCchCopy(CreateServerInfo.szServerName, CountArray(CreateServerInfo.szServerName),m_ModuleInitParameter.GameServiceOption.szServerName);

	//�������
	CreateServerInfo.cbDistributeMode=m_ModuleInitParameter.GameServiceOption.cbDistributeMode;
	CreateServerInfo.cbDistributeRule=m_ModuleInitParameter.GameServiceOption.cbDistributeRule;
	CreateServerInfo.wMinPlayerCount=m_ModuleInitParameter.GameServiceOption.wMinPlayerCount;
	CreateServerInfo.wMinDistributeUser=m_ModuleInitParameter.GameServiceOption.wMinDistributeUser;	
	CreateServerInfo.wMinPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser;
	CreateServerInfo.wMaxPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser;
	CreateServerInfo.wDistributeTimeSpace=m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace;

	//��չ����
	CWHService::GetMachineID(CreateServerInfo.szServiceMachine);
	CopyMemory(CreateServerInfo.cbCustomRule,m_ModuleInitParameter.GameServiceOption.cbCustomRule,sizeof(CreateServerInfo.cbCustomRule));

	//��������
	tagGameServerInfo GameServerResult;
	CServerInfoManager ServerInfoManager;
	
	//���뷿��
	if (CreateServerInfo.wServerID==0)
	{
		if (ServerInfoManager.InsertGameServer(&CreateServerInfo,GameServerResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ���䴴���ɹ�"),TraceLevel_Normal);
	}

	//�޸ķ���
	if (CreateServerInfo.wServerID!=0)
	{
		//�޸ķ���
		if (ServerInfoManager.ModifyGameServer(&CreateServerInfo,GameServerResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ�������óɹ�"),TraceLevel_Normal);
	}

	//��������
	UINT uCustomRuleSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);

	//��������
	m_ModuleInitParameter.GameServiceOption.wDBListID = GameServerResult.wDBListID;
	
	//�ҽ�����
	m_ModuleInitParameter.GameServiceOption.wKindID=GameServerResult.wKindID;
	m_ModuleInitParameter.GameServiceOption.wNodeID=GameServerResult.wNodeID;
	m_ModuleInitParameter.GameServiceOption.wSortID=GameServerResult.wSortID;
	m_ModuleInitParameter.GameServiceOption.wVisibleMask=GameServerResult.wVisibleMask;

	//��������
	m_ModuleInitParameter.GameServiceOption.wServerID=GameServerResult.wServerID;
	m_ModuleInitParameter.GameServiceOption.wTableCount=GameServerResult.wTableCount;
	m_ModuleInitParameter.GameServiceOption.wServerType=GameServerResult.wServerType;
	m_ModuleInitParameter.GameServiceOption.wServerPort=GameServerResult.wServerPort;
	m_ModuleInitParameter.GameServiceOption.wServerLevel=GameServerResult.wServerLevel;
	m_ModuleInitParameter.GameServiceOption.cbMatchType = GameServerResult.cbMatchType;
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServerName, LEN_SERVER,GameServerResult.szServerName);

	//��������
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceAddr,CountArray(m_ModuleInitParameter.GameServiceOption.szServiceAddr), GameServerResult.szServiceAddr);
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceDomain,CountArray(m_ModuleInitParameter.GameServiceOption.szServiceDomain), GameServerResult.szServiceDomain);

	//��������
	m_ModuleInitParameter.GameServiceOption.lRevenue=GameServerResult.lRevenue;
	m_ModuleInitParameter.GameServiceOption.lCellScore=GameServerResult.lCellScore;
	m_ModuleInitParameter.GameServiceOption.lLessScore=GameServerResult.lLessScore;
	m_ModuleInitParameter.GameServiceOption.lExpGainRate = GameServerResult.lExpGainRate;
	m_ModuleInitParameter.GameServiceOption.lRestrictScore=GameServerResult.lRestrictScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterScore=GameServerResult.lMinEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterScore=GameServerResult.lMaxEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterMember=GameServerResult.lMinEnterMember;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterMember=GameServerResult.lMaxEnterMember;

	//Լս����
	m_ModuleInitParameter.GameServiceOption.dwBattleRule=GameServerResult.dwBattleRule;

	//�������
	m_ModuleInitParameter.GameServiceOption.cbDistributeMode=GameServerResult.cbDistributeMode;
	m_ModuleInitParameter.GameServiceOption.cbDistributeRule=GameServerResult.cbDistributeRule;
	m_ModuleInitParameter.GameServiceOption.wMinPlayerCount=GameServerResult.wMinPlayerCount;
	m_ModuleInitParameter.GameServiceOption.wMinDistributeUser=GameServerResult.wMinDistributeUser;	
	m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser=GameServerResult.wMinPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser=GameServerResult.wMaxPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace=GameServerResult.wDistributeTimeSpace;

	//��������
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceAddr, LEN_ADDRESS, GameServerResult.szServiceAddr);
	StringCchCopy(m_ModuleInitParameter.GameServiceOption.szServiceDomain, LEN_DOMAIN, GameServerResult.szServiceDomain);

	//��������
	m_ModuleInitParameter.GameServiceOption.wMaxPlayer=GameServerResult.wMaxPlayer;
	m_ModuleInitParameter.GameServiceOption.dwServerRule=GameServerResult.dwServerRule;
	m_ModuleInitParameter.GameServiceOption.dwAttachUserRight = GameServerResult.dwAttachUserRight;

	//��չ����
	CopyMemory(m_ModuleInitParameter.GameServiceOption.cbCustomRule,GameServerResult.cbCustomRule,uCustomRuleSize);

	//�رմ���
	EndDialog(IDOK);

	return;
}

//////////////////////////////////////////////////////////////////////////