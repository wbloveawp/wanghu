#include "StdAfx.h"
#include "Resource.h"
#include "DlgMatchWizard.h"

//////////////////////////////////////////////////////////////////////////
//��������

#define TAB_OPTION_SIGNUP1					0
#define TAB_OPTION_SIGNUP2					1
#define TAB_OPTION_RANKINGMODE				2
#define TAB_OPTION_DISTRIBUTE				3
#define TAB_OPTION_REWARD					4
//#define TAB_OPTION_ROUND					5

//////////////////////////////////////////////////////////////////////////
//��Ϣӳ��

BEGIN_MESSAGE_MAP(CDlgMatchItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchSignup1, CDlgMatchItem)
	ON_CBN_SELCHANGE(IDC_COMBO_FROM_MATCH, OnCbnSelchangeComboFromMatch)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchReward, CDlgMatchItem)
	ON_BN_CLICKED(IDC_ADD_REWARD, OnBnClickedAddReward)
	ON_BN_CLICKED(IDC_MODIFY_REWARD, OnBnClickedModifyReward)
	ON_BN_CLICKED(IDC_DELETE_REWARD, OnBnClickedDeleteReward)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REWARD, OnLvnItemchangedListReward)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizardStep1, CDlgMatchWizardItem)	
	ON_CBN_SELCHANGE(IDC_COMBO_MATCH_TYPE, OnCbnSelchangeComboMatchType)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MATCH_OPTION, OnTcnSelchangeMatchOption)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizardStep2, CDlgMatchWizardItem)	
	ON_BN_CLICKED(IDC_ADD_ROUND, OnBnClickedAddRound)	
	ON_BN_CLICKED(IDC_MODIFY_ROUND, OnBnClickedModifyRound)
	ON_BN_CLICKED(IDC_DELETE_ROUND, OnBnClickedDeleteRound)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROUND, OnLvnItemchangedListRound)
	ON_CBN_SELCHANGE(IDC_COMBO_PROMOTE_MODE, OnCbnSelchangeComboPromoteMode)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizard, CDialog)
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgPromoteModeItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchImmediate, CDlgMatchItem)	
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchTournament, CDlgMatchItem)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//���캯��
CDlgMatchItem::CDlgMatchItem(UINT nIDTemplate): CDialog(nIDTemplate)
{
	//���ñ���		
	m_pMatchOption=NULL;
	m_pMatchInfoBuffer=NULL;	
}

//��������
CDlgMatchItem::~CDlgMatchItem()
{
}

//���ñ���
VOID CDlgMatchItem::SetMatchInfoBuffer(CMatchInfoBuffer * pMatchInfoBuffer)
{
	m_pMatchInfoBuffer=pMatchInfoBuffer;
}

//��������
VOID CDlgMatchItem::SetMatchOption(tagMatchOption * pMatchOption)
{
	m_pMatchOption=pMatchOption;
}

//������
bool CDlgMatchItem::ShowMatchItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) CreateMatchItem(rcRect,pParentWnd);

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//��������
VOID CDlgMatchItem::CreateMatchItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//��������
	BOOL bSuccess=Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));
}

//���ͱ��
VOID CDlgMatchItem::OnEventMatchTypeChanged(BYTE cbMatchType)
{
}

//λ����Ϣ
VOID CDlgMatchItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchSignup1::CDlgMatchSignup1() : CDlgMatchItem(IDD_MATCH_SIGNUP1)
{
}

//��������
CDlgMatchSignup1::~CDlgMatchSignup1()
{
}

//��ʼ������
BOOL CDlgMatchSignup1::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	((CEdit *)GetDlgItem(IDC_EDIT_MATCH_FEE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SIGNUP_COUNT))->LimitText(3);

	//��������
	SetDlgItemInt(IDC_EDIT_SIGNUP_COUNT, (UINT)m_pMatchOption->wSignupCount);

	////����Ȧ����
	//if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_WECHAT_SHARE)
	//{
	//	//����ѡ��
	//	((CButton *)GetDlgItem(IDC_CHECK_WECHAT_SHARE))->SetCheck(TRUE);
	//}

	//ʹ�ñ���ȯ
	if (m_pMatchOption->cbSignupMode & SIGNUP_MODE_MATCH_TICKET)
	{
		//����ѡ��
		((CButton*)GetDlgItem(IDC_CHECK_MATCH_TICKET))->SetCheck(TRUE);

		//����ѡ��
		((CButton*)GetDlgItem(IDC_CHECK_RESET_SCORE))->SetCheck(m_pMatchOption->wBuyTicketOption&MATCH_BTOP_RESET_SCORE?TRUE:FALSE);

		//���ÿؼ�
		SetDlgItemInt(IDC_EDIT_USE_TICKET_COUNT, (int)m_pMatchOption->wUseTicketCount);
		SetDlgItemInt(IDC_EDIT_BUY_TICKET_COUNT, (int)m_pMatchOption->wBuyTicketCount);
		SetDlgItemInt(IDC_EDIT_BUY_TICKET_AMOUNT, (int)m_pMatchOption->lBuyTicketAmount);				

		//��������
		switch (m_pMatchOption->wBuyTicketMoneyID)
		{
		case GOODS_ID_GOLD:		//���
		{
			CheckRadioButton(IDC_RADIO_GOLD_1, IDC_RADIO_INGOT_1, IDC_RADIO_GOLD_1);
			break;
		}
		//case GOODS_ID_INGOT:	//��ʯ
		//{
		//	CheckRadioButton(IDC_RADIO_GOLD_1, IDC_RADIO_INGOT_1, IDC_RADIO_INGOT_1);
		//	break;
		//}
		}
	}

	//���ɷ���
	if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
	{
		//����ѡ��
		((CButton *)GetDlgItem(IDC_CHECK_SIGNUP_FEE))->SetCheck(TRUE);

		//��������
		SetDlgItemInt(IDC_EDIT_SIGNUP_FEE,(UINT)m_pMatchOption->lFeeAmount);

		//��������
		switch (m_pMatchOption->wFeeMoneyID)
		{
		case GOODS_ID_GOLD:		//���
		{
			CheckRadioButton(IDC_RADIO_GOLD, IDC_RADIO_INGOT, IDC_RADIO_GOLD);
			break;
		}
		//case GOODS_ID_INGOT:	//��ʯ
		//{
		//	CheckRadioButton(IDC_RADIO_GOLD, IDC_RADIO_INGOT, IDC_RADIO_INGOT);
		//	break;
		//}
		}
	}

	//��������
	//if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	//{
	//	//����ѡ��
	//	((CButton *)GetDlgItem(IDC_CHECK_MATCH_USER))->SetCheck(TRUE);		

	//	//��������
	//	CDateTimeCtrl * pEndtDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_END);
	//	pEndtDate->SetTime(&m_pMatchOption->MatchEndDate);

	//	//��ʼ����
	//	CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_START);
	//	pStartDate->SetTime(&m_pMatchOption->MatchStartDate);		

	//	//��������
	//	if (m_pMatchOption->cbFilterType==FILTER_TYPE_SINGLE_TURN)
	//	{			
	//		SetDlgItemInt(IDC_EDIT_SINGLE_RANK,m_pMatchOption->wMaxRankID);
	//		CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_SINGLE_TURN);			
	//	}

	//	//��������
	//	if (m_pMatchOption->cbFilterType==FILTER_TYPE_TOTAL_RANKING)
	//	{
	//		SetDlgItemInt(IDC_EDIT_TOTAL_RANK,m_pMatchOption->wMaxRankID);
	//		CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_TOTAL_RANKING);			
	//	}		
	//}
	//else
	//{
	//	//Ĭ������
	//	CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_SINGLE_TURN);
	//}

	////��ȡ�ؼ�
	//CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_FROM_MATCH);
	//if (pComboBox!=NULL)
	//{
	//	//���ر���
	//	if (m_pMatchInfoBuffer!=NULL)
	//	{
	//		//��������
	//		for (INT_PTR i=0;i<m_pMatchInfoBuffer->m_MatchOptionArray.GetCount();i++)
	//		{
	//			//��ȡ����
	//			tagMatchOption * pMatchOption=m_pMatchInfoBuffer->m_MatchOptionArray[i];
	//			if (pMatchOption->dwMatchID==m_pMatchOption->dwMatchID) continue;
	//		
	//			//�������
	//			int nIndex=pComboBox->AddString(CW2CT(pMatchOption->szMatchName));
	//			pComboBox->SetItemDataPtr(nIndex,pMatchOption);
	//			if (pMatchOption->dwMatchID==m_pMatchOption->dwFromMatchID)
	//			{
	//				if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	//				{
	//					pComboBox->SetCurSel(nIndex);
	//				}
	//			}
	//		}
	//	}

	//	//ģ��ѡ��
	//	OnCbnSelchangeComboFromMatch();
	//}
	
	return FALSE;
}

//��������
bool CDlgMatchSignup1::SaveInputInfo()
{
	//����Ȧ����
	//if (IsDlgButtonChecked(IDC_CHECK_WECHAT_SHARE))
	//{
	//	m_pMatchOption->cbSignupMode|=SIGNUP_MODE_WECHAT_SHARE;
	//}
	//else
	//{
	//	m_pMatchOption->cbSignupMode&=~SIGNUP_MODE_WECHAT_SHARE;
	//}

	//��������
	m_pMatchOption->wSignupCount = GetDlgItemInt(IDC_EDIT_SIGNUP_COUNT);

	//ʹ�ñ���ȯ
	if (IsDlgButtonChecked(IDC_CHECK_MATCH_TICKET))
	{
		//���ö��
		m_pMatchOption->cbSignupMode |= SIGNUP_MODE_MATCH_TICKET;
		m_pMatchOption->wUseTicketCount = GetDlgItemInt(IDC_EDIT_USE_TICKET_COUNT);

		//��Ʊѡ��
		if (IsDlgButtonChecked(IDC_CHECK_RESET_SCORE))
		{
			m_pMatchOption->wBuyTicketOption |= MATCH_BTOP_RESET_SCORE;
		}
		else
		{
			m_pMatchOption->wBuyTicketOption &= ~MATCH_BTOP_RESET_SCORE;
		}

		//��������
		m_pMatchOption->wBuyTicketCount = GetDlgItemInt(IDC_EDIT_BUY_TICKET_COUNT);
		m_pMatchOption->lBuyTicketAmount = GetDlgItemInt(IDC_EDIT_BUY_TICKET_AMOUNT);
		if (IsDlgButtonChecked(IDC_RADIO_GOLD_1)) m_pMatchOption->wBuyTicketMoneyID = GOODS_ID_GOLD;
		//if (IsDlgButtonChecked(IDC_RADIO_INGOT_1)) m_pMatchOption->wBuyTicketMoneyID = GOODS_ID_INGOT;
	}
	else
	{
		m_pMatchOption->cbSignupMode &= ~SIGNUP_MODE_MATCH_TICKET;
	}

	//���ɷ���
	if (IsDlgButtonChecked(IDC_CHECK_SIGNUP_FEE))
	{
		//���ö��
		m_pMatchOption->cbSignupMode|=SIGNUP_MODE_SIGNUP_FEE;
		m_pMatchOption->lFeeAmount=GetDlgItemInt(IDC_EDIT_SIGNUP_FEE);

		//��������
		if (IsDlgButtonChecked(IDC_RADIO_GOLD)) m_pMatchOption->wFeeMoneyID=GOODS_ID_GOLD;
		//if (IsDlgButtonChecked(IDC_RADIO_INGOT)) m_pMatchOption->wFeeMoneyID =GOODS_ID_INGOT;
	}
	else
	{
		m_pMatchOption->cbSignupMode&=~SIGNUP_MODE_SIGNUP_FEE;
	}

	////�����û�
	//if (IsDlgButtonChecked(IDC_CHECK_MATCH_USER))
	//{
	//	m_pMatchOption->cbSignupMode|=SIGNUP_MODE_MATCH_USER;

	//	//��ȡ�ؼ�
	//	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_FROM_MATCH);
	//	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	//	{
	//		tagMatchOption * pMatchOption=(tagMatchOption *)pComboBox->GetItemData(pComboBox->GetCurSel());
	//		if (pMatchOption!=NULL) m_pMatchOption->dwFromMatchID=pMatchOption->dwMatchID;
	//	}

	//	//��������
	//	CDateTimeCtrl * pEndtDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_END);
	//	pEndtDate->GetTime(&m_pMatchOption->MatchEndDate);

	//	//��ʼ���� 
	//	CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_START);
	//	pStartDate->GetTime(&m_pMatchOption->MatchStartDate);

	//	//��������
	//	if (IsDlgButtonChecked(IDC_RADIO_SINGLE_TURN))
	//	{			
	//		m_pMatchOption->cbFilterType=FILTER_TYPE_SINGLE_TURN;
	//		m_pMatchOption->wMaxRankID=GetDlgItemInt(IDC_EDIT_SINGLE_RANK);
	//	}

	//	//��������
	//	if (IsDlgButtonChecked(IDC_RADIO_TOTAL_RANKING))
	//	{
	//		m_pMatchOption->cbFilterType=FILTER_TYPE_TOTAL_RANKING;
	//		m_pMatchOption->wMaxRankID=GetDlgItemInt(IDC_EDIT_TOTAL_RANK);
	//	}
	//}
	//else
	//{
	//	m_pMatchOption->cbSignupMode&=~SIGNUP_MODE_MATCH_USER;
	//}

	//������ʽ
	if (m_pMatchOption->cbSignupMode==0)
	{
		AfxMessageBox(TEXT("������ʽ����Ϊ�գ�������ѡ��"));

		return false;
	}

	//�����û�
	//if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	//{
	//	if (m_pMatchOption->dwFromMatchID==0)
	//	{
	//		AfxMessageBox(TEXT("����ѡ�˲μ��ض�������Ϊ������ʽ������ѡ��һ��������ȷ�������û�����Դ��"));

	//		return false;
	//	}

	//	//���ұ���
	//	tagMatchOption * pMatchOption=m_pMatchInfoBuffer->SearchMatchOption(m_pMatchOption->dwFromMatchID);
	//	if (pMatchOption!=NULL)
	//	{
	//		if (pMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
	//		{
	//			if (m_pMatchOption->cbFilterType!=FILTER_TYPE_SINGLE_TURN)
	//			{
	//				AfxMessageBox(TEXT("��ѡ���˼�ʱ����Ϊ�����û�����Դ��ɸѡ��ʽ����Ϊ���ֱ���������"));

	//				return false;
	//			}
	//		}

	//		if (pMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
	//		{
	//			if (m_pMatchOption->cbFilterType!=FILTER_TYPE_TOTAL_RANKING)
	//			{
	//				AfxMessageBox(TEXT("��ѡ���˶�ʱ����Ϊ�����û�����Դ��ɸѡ��ʽ����Ϊ������������"));

	//				return false;
	//			}
	//		}
	//	}
	//}

	//�����ж�
	/*if (m_pMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE && m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	{
		AfxMessageBox(TEXT("��ʱ������ѡ������û���Ϊ������ʽ��������ѡ��"));

		return false;
	}*/

	return true;
}

//�����ؼ�
VOID CDlgMatchSignup1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//���ͱ��
VOID CDlgMatchSignup1::OnEventMatchTypeChanged(BYTE cbMatchType)
{
	//���ÿؼ�
	CButton * pCheckButton=(CButton *)GetDlgItem(IDC_CHECK_MATCH_USER);
	if (pCheckButton!=NULL)
	{
		pCheckButton->SetCheck(FALSE);
		pCheckButton->EnableWindow(cbMatchType==MATCH_TYPE_LOCKTIME);
	}	
}

//ѡ����
void CDlgMatchSignup1::OnCbnSelchangeComboFromMatch()
{
	//��ȡ����
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_FROM_MATCH);
	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	{
		tagMatchOption * pMatchOption=(tagMatchOption *)pComboBox->GetItemDataPtr(pComboBox->GetCurSel());

		//���ÿؼ�
		GetDlgItem(IDC_DATE_MATCH_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LOOP_PERIOD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SINGLE_RANK)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_SINGLE_TURN)->EnableWindow(TRUE);		
		GetDlgItem(IDC_EDIT_TOTAL_RANK)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TOTAL_RANKING)->EnableWindow(TRUE);				

		//��ʱ������
		if (pMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
		{
			//���ÿؼ�
			GetDlgItem(IDC_EDIT_TOTAL_RANK)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_TOTAL_RANKING)->EnableWindow(FALSE);			

			//����ѡ��
			CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_SINGLE_TURN);
		}

		//��ʱ������
		if (pMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
		{
			//���ÿؼ�
			GetDlgItem(IDC_DATE_MATCH_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_LOOP_PERIOD)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_SINGLE_TURN)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_SINGLE_RANK)->EnableWindow(FALSE);

			//����ѡ��
			CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_TOTAL_RANKING);
		}		
	}	
}


//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchSignup2::CDlgMatchSignup2() : CDlgMatchItem(IDD_MATCH_SIGNUP2)
{
}

//��������
CDlgMatchSignup2::~CDlgMatchSignup2()
{
}

//��ʼ������
BOOL CDlgMatchSignup2::OnInitDialog()
{
	__super::OnInitDialog();

	//�û�����
	if (m_pMatchOption->cbJoinCondition&MATCH_JOINCD_EXPERIENCE)
	{
		//����ѡ��
		((CButton *)GetDlgItem(IDC_CHECK_EXPERIENCE))->SetCheck(TRUE);

		//���þ���
		SetDlgItemInt(IDC_EDIT_EXPERIENCE,m_pMatchOption->lExperience);
	}

  //��Ա�ȼ�
	if (m_pMatchOption->cbJoinCondition&MATCH_JOINCD_MEMBER_ORDER)
	{
		//����ѡ��
		((CButton *)GetDlgItem(IDC_CHECK_MEMBER_ORDER))->SetCheck(TRUE);

		//��Ա�ȼ�
		BYTE cbMemberIndex=0;
		BYTE cbMemberOrder[]={0,6,9,12,15};
		for (BYTE i=0; i<CountArray(cbMemberOrder); i++)
		{
			if (m_pMatchOption->cbMemberOrder==cbMemberOrder[i])
			{
				cbMemberIndex=i;
				break;
			}
		}

		//����ѡ��
		CheckRadioButton(IDC_RADIO_NOAMAL,IDC_RADIO_MEMBER_D,IDC_RADIO_NOAMAL+cbMemberIndex);		
	}
	else
	{
		//Ĭ������
		CheckRadioButton(IDC_RADIO_NOAMAL,IDC_RADIO_MEMBER_D,IDC_RADIO_NOAMAL);		
	}	

	return FALSE;
}

//��������
bool CDlgMatchSignup2::SaveInputInfo()
{
	//�û�����
	if (IsDlgButtonChecked(IDC_CHECK_EXPERIENCE))
	{
		m_pMatchOption->cbJoinCondition|=MATCH_JOINCD_EXPERIENCE;

		//���þ���
		m_pMatchOption->lExperience=GetDlgItemInt(IDC_EDIT_EXPERIENCE);
	}
	else
	{
		m_pMatchOption->cbJoinCondition&=~MATCH_JOINCD_EXPERIENCE;
	}

	//��Ա�ȼ�
	if (IsDlgButtonChecked(IDC_CHECK_MEMBER_ORDER))
	{
		m_pMatchOption->cbJoinCondition|=MATCH_JOINCD_MEMBER_ORDER;		

		//���õȼ�
		BYTE cbMemberOrder[]={0,6,9,12,15};
		UINT nCheckedID=GetCheckedRadioButton(IDC_RADIO_NOAMAL,IDC_RADIO_MEMBER_D);
		m_pMatchOption->cbMemberOrder=cbMemberOrder[nCheckedID-IDC_RADIO_NOAMAL];
	}
	else
	{
		m_pMatchOption->cbJoinCondition&=~MATCH_JOINCD_MEMBER_ORDER;
	}	

	//������ʽ
	if (m_pMatchOption->cbSignupMode==0)
	{
		AfxMessageBox(TEXT("������ʽ����Ϊ�գ�������ѡ��"));

		return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgMatchSignup2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchDistribute::CDlgMatchDistribute() : CDlgMatchItem(IDD_MATCH_DISTRIBUTE)
{
}

//��������
CDlgMatchDistribute::~CDlgMatchDistribute()
{
}

//��ʼ������
BOOL CDlgMatchDistribute::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	((CEdit *)GetDlgItem(IDC_DISTRIBUTE_MINCOUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_DISTRIBUTE_INTERVAL))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_PARTAKE_MIN_GAMEUSER))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_PARTAKE_MAX_GAMEUSER))->LimitText(2);	

	//��ȡ����
	auto pMatchDistribute = (tagMatchDistribute*)m_pMatchOption->cbMatchDistribute;

	//ͬ��ѡ��
	if ((pMatchDistribute->wDistributeRule&DISTRIBUTE_SAME_TABLE)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_LAST_SAMETABLE))->SetCheck(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_LAST_SAMETABLE))->SetCheck(FALSE);
	}

	//��ַѡ��
	if ((pMatchDistribute->wDistributeRule&DISTRIBUTE_SAME_ADDRESS)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(FALSE);
	}

	//��������
	if (pMatchDistribute->wDistributeMinUser!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT, pMatchDistribute->wDistributeMinUser);
	}

	//������
	if (pMatchDistribute->wDistributeInterval!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_INTERVAL, pMatchDistribute->wDistributeInterval);
	}

	//��С����
	if (pMatchDistribute->wMinPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER, pMatchDistribute->wMinPartakeGameUser);
	}

	//�������
	if (pMatchDistribute->wMaxPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER, pMatchDistribute->wMaxPartakeGameUser);
	}

	return FALSE;
}

//��������
bool CDlgMatchDistribute::SaveInputInfo()
{
	//��ȡ����
	auto pMatchDistribute = (tagMatchDistribute*)m_pMatchOption->cbMatchDistribute;

	//��������
	pMatchDistribute->wDistributeMinUser=GetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT);
	pMatchDistribute->wDistributeInterval=GetDlgItemInt(IDC_DISTRIBUTE_INTERVAL);
	pMatchDistribute->wMinPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER);
	pMatchDistribute->wMaxPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER);

	//ͬ��ѡ��
	if (((CButton *)GetDlgItem(IDC_ALLOW_LAST_SAMETABLE))->GetCheck()==BST_CHECKED)
	{
		pMatchDistribute->wDistributeRule|=DISTRIBUTE_SAME_TABLE;
	}
	else
	{
		pMatchDistribute->wDistributeRule&=~DISTRIBUTE_SAME_TABLE;
	}

	//��ַѡ��
	if (((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->GetCheck()==BST_CHECKED)
	{
		pMatchDistribute->wDistributeRule|=DISTRIBUTE_SAME_ADDRESS;
	}
	else
	{
		pMatchDistribute->wDistributeRule&=~DISTRIBUTE_SAME_ADDRESS;
	}

	return true;
}

//�����ؼ�
VOID CDlgMatchDistribute::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}


////////////////////////////////////////////////////////////////////////////
//
////���캯��
//CDlgMatchRankingMode::CDlgMatchRankingMode() : CDlgMatchItem(IDD_MATCH_RANKING_MODE)
//{
//}
//
////��������
//CDlgMatchRankingMode::~CDlgMatchRankingMode()
//{
//}
//
////��ʼ������
//BOOL CDlgMatchRankingMode::OnInitDialog()
//{
//	__super::OnInitDialog();
//
//	//���ÿؼ�
//	((CEdit *)GetDlgItem(IDC_EDIT_COUNT_INNINGS))->LimitText(3);
//
//	//���ÿؼ�
//	CheckRadioButton(IDC_RADIO_TOTAL_GRADES,IDC_RADIO_SPECIALLY_GRADES,IDC_RADIO_TOTAL_GRADES+(m_pMatchOption->cbRankingMode-RANKING_MODE_TOTAL_GRADES));
//
//	//�ض���������
//	if (IsDlgButtonChecked(IDC_RADIO_SPECIALLY_GRADES))
//	{
//		//ͳ�ƾ���
//		SetDlgItemInt(IDC_EDIT_COUNT_INNINGS,m_pMatchOption->wCountInnings);
//
//		//���ÿؼ�
//		CheckRadioButton(IDC_RADIO_BEST_GRADES,IDC_RADIO_AVERAGEEX_GRADES,IDC_RADIO_BEST_GRADES+(m_pMatchOption->cbFilterGradesMode-FILTER_GRADES_MODE_BEST));
//	}
//	else
//	{
//		CheckRadioButton(IDC_RADIO_BEST_GRADES,IDC_RADIO_AVERAGEEX_GRADES,IDC_RADIO_BEST_GRADES);
//	}
//
//	return FALSE;
//}
//
////��������
//bool CDlgMatchRankingMode::SaveInputInfo()
//{
//	//������ʽ
//	if (IsDlgButtonChecked(IDC_RADIO_TOTAL_GRADES)) m_pMatchOption->cbRankingMode=RANKING_MODE_TOTAL_GRADES;
//	if (IsDlgButtonChecked(IDC_RADIO_SPECIALLY_GRADES)) m_pMatchOption->cbRankingMode=RANKING_MODE_SPECIAL_GRADES;
//
//	//�ض�����
//	if (m_pMatchOption->cbRankingMode==RANKING_MODE_SPECIAL_GRADES)
//	{
//		//ͳ�ƾ���
//		m_pMatchOption->wCountInnings=GetDlgItemInt(IDC_EDIT_COUNT_INNINGS);
//
//		//�Ϸ�У��
//		if (m_pMatchOption->wCountInnings==0)
//		{
//			AfxMessageBox(TEXT("ͳ�ƾ������ò��Ϸ������������ã�"));
//
//			return false;
//		}
//
//		//ɸѡģʽ
//		if (IsDlgButtonChecked(IDC_RADIO_BEST_GRADES)) m_pMatchOption->cbFilterGradesMode=FILTER_GRADES_MODE_BEST;
//		if (IsDlgButtonChecked(IDC_RADIO_AVERAGE_GRADES)) m_pMatchOption->cbFilterGradesMode=FILTER_GRADES_MODE_AVERAGE;
//		if (IsDlgButtonChecked(IDC_RADIO_AVERAGEEX_GRADES)) m_pMatchOption->cbFilterGradesMode=FILTER_GRADES_MODE_AVERAGEEX;
//	}	
//
//	return true;
//}
//
////�����ؼ�
//VOID CDlgMatchRankingMode::RectifyControl(INT nWidth, INT nHeight)
//{
//	return;
//}
//
////���ͱ��
//VOID CDlgMatchRankingMode::OnEventMatchTypeChanged(BYTE cbMatchType)
//{
//	//���ÿؼ�
//	GetDlgItem(IDC_RADIO_SPECIALLY_GRADES)->EnableWindow(cbMatchType==MATCH_TYPE_LOCKTIME);
//}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchReward::CDlgMatchReward() : CDlgMatchItem(IDD_MATCH_REWARD)
{
}

//��������
CDlgMatchReward::~CDlgMatchReward()
{
}

//��ʼ������
BOOL CDlgMatchReward::OnInitDialog()
{
	__super::OnInitDialog();	

	//���ÿؼ�
	((CEdit*)GetDlgItem(IDC_REWARD_GOODS))->LimitText(CountArray(tagMatchRewardItem::szRewardGoods));
	((CEdit*)GetDlgItem(IDC_REWARD_ENTITY))->LimitText(CountArray(tagMatchRewardItem::szRewardEntity));

	for (WORD i=0; i<m_pMatchOption->wRewardCount; i++)
	{
		m_RewardInfoBuffer.InsertMatchRewardItem(m_pMatchOption->MatchRewardItem[i]);
	}
	
	for(INT_PTR i=0;i<m_RewardInfoBuffer.m_MatchRewardItemArray.GetCount();i++) 
	{
		m_RewardListControl.InsertMatchReward(m_RewardInfoBuffer.m_MatchRewardItemArray[i]);
	}	

	return FALSE;
}

//�ؼ���
VOID CDlgMatchReward::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�����б�
	DDX_Control(pDX,IDC_LIST_REWARD,m_RewardListControl);
}

//��������
bool CDlgMatchReward::SaveInputInfo()
{
	//��������
	m_pMatchOption->wRewardCount=__min(CountArray(m_pMatchOption->MatchRewardItem),m_RewardInfoBuffer.GetMatchRewardCount());

	for (WORD i=0; i<m_pMatchOption->wRewardCount; i++)
	{
		CopyMemory(&m_pMatchOption->MatchRewardItem[i],m_RewardInfoBuffer.m_MatchRewardItemArray[i],sizeof(m_pMatchOption->MatchRewardItem[i]));
	}

	//�����ж�
	if (m_pMatchOption->wRewardCount==0)
	{
		AfxMessageBox(TEXT("����������һ�����"));

		return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgMatchReward::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//��������
bool CDlgMatchReward::GetMatchRewardItem(tagMatchRewardItem & MatchRewardItem)
{
	//��ȡ����
	MatchRewardItem.wEndRankID=GetDlgItemInt(IDC_RANKID_END);
	MatchRewardItem.wStartRankID=GetDlgItemInt(IDC_RANKID_START);	

	//��������
	if (MatchRewardItem.wEndRankID==0) 
	{
		MatchRewardItem.wEndRankID=MatchRewardItem.wStartRankID;
	}

	//����У��
	if (MatchRewardItem.wStartRankID == 0 || MatchRewardItem.wEndRankID < MatchRewardItem.wStartRankID)
	{
		AfxMessageBox(TEXT("������ı������β��Ϸ������������룡"));

		//���ý���
		GetDlgItem(IDC_RANKID_END)->SetFocus();

		return false;
	}

	//ʵ�ｱ��
	TCHAR szRewardEntity[CountArray(MatchRewardItem.szRewardEntity)];
	GetDlgItemText(IDC_REWARD_ENTITY,szRewardEntity,CountArray(szRewardEntity));

	//��Ʒ����
	TCHAR szRewardGoods[CountArray(MatchRewardItem.szRewardGoods)];
	GetDlgItemText(IDC_REWARD_GOODS, szRewardGoods, CountArray(szRewardGoods));

	//У�齱��
	if (szRewardEntity[0] == 0 && szRewardGoods[0] == 0)
	{
		AfxMessageBox(TEXT("����������һ�ֽ�����"));

		//���ý���
		GetDlgItem(IDC_REWARD_ENTITY)->SetFocus();

		return false;
	}

	//���鶨��
	CGoodsItemArray GoodsItemArray;

	//У��ʵ��
	GoodsItemArray.RemoveAll();
	if (szRewardEntity[0] != 0 && !g_GameGoodsManager->ParseFromJson(szRewardEntity, GoodsItemArray, GoodsItemArray))
	{
		AfxMessageBox(TEXT("�������\"ʵ�ｱ��\"��ʽ���Ϸ��򳤶ȹ���������������������룡"));

		//���ý���
		GetDlgItem(IDC_REWARD_ENTITY)->SetFocus();

		return false;		
	}

	//У����Ʒ
	GoodsItemArray.RemoveAll();
	if (szRewardGoods[0] != 0 && !g_GameGoodsManager->ParseFromJson(szRewardGoods, GoodsItemArray, GoodsItemArray))
	{
		AfxMessageBox(TEXT("�������\"��Ʒ����\"��ʽ���Ϸ��򳤶ȹ���������������������룡"));

		//���ý���
		GetDlgItem(IDC_REWARD_GOODS)->SetFocus();

		return false;
	}	

	//��������
	StringCchCopy(MatchRewardItem.szRewardGoods, CountArray(MatchRewardItem.szRewardGoods), szRewardGoods);

	//��������
	StringCchCopy(MatchRewardItem.szRewardEntity, CountArray(MatchRewardItem.szRewardEntity), szRewardEntity);	

	return true;
}

//��տؼ�
VOID CDlgMatchReward::CleanControl()
{
	//���ÿؼ�
	SetDlgItemText(IDC_RANKID_END,TEXT(""));
	SetDlgItemText(IDC_RANKID_START,TEXT(""));	
	SetDlgItemText(IDC_REWARD_GOLD,TEXT(""));
	SetDlgItemText(IDC_REWARD_MEDAL,TEXT(""));
	SetDlgItemText(IDC_REWARD_INGOT,TEXT(""));
	SetDlgItemText(IDC_REWARD_ENTITY,TEXT(""));
}

//��������
VOID CDlgMatchReward::SetMatchRewardItem(tagMatchRewardItem * pMatchRewardItem)
{
	//����У��
	ASSERT(pMatchRewardItem!=NULL);
	if (pMatchRewardItem==NULL) return;	

	//��������
	SetDlgItemInt(IDC_RANKID_END,(UINT)pMatchRewardItem->wEndRankID);
	SetDlgItemInt(IDC_RANKID_START,(UINT)pMatchRewardItem->wStartRankID);	

	//ʵ�ｱ��
	SetDlgItemText(IDC_REWARD_ENTITY,pMatchRewardItem->szRewardEntity);

	//��Ʒ����
	SetDlgItemText(IDC_REWARD_GOODS, pMatchRewardItem->szRewardGoods);
}


//��ӽ���
VOID CDlgMatchReward::OnBnClickedAddReward()
{
	//��������	
	tagMatchRewardItem MatchRewardItem;

	//��ȡ����
	if (GetMatchRewardItem(MatchRewardItem)==false) return;	

	//�����ж�
	for (WORD wRankID=MatchRewardItem.wStartRankID; wRankID<=MatchRewardItem.wEndRankID; wRankID++)
	{
		//�����ж�
		if (m_RewardInfoBuffer.SearchMatchReward(wRankID)!=NULL)
		{
			AfxMessageBox(TEXT("��Ǹ�������εĽ����Ѵ���,��Ҫ�޸������޸Ľ�����ť��"));

			return;
		}		
	}

	//���뽱��
	tagMatchRewardItem * pMatchRewardItem=m_RewardInfoBuffer.InsertMatchRewardItem(MatchRewardItem);
	if (pMatchRewardItem!=NULL)
	{
		m_RewardListControl.InsertMatchReward(pMatchRewardItem);
	}

	//����ؼ�
	CleanControl();
}

//�޸Ľ���
VOID CDlgMatchReward::OnBnClickedModifyReward()
{
	//��������	
	tagMatchRewardItem MatchRewardItem;

	//��ȡ����
	if (GetMatchRewardItem(MatchRewardItem) == false) return;

	//��ѯ����
	auto pMatchRewardItem = m_RewardInfoBuffer.SearchMatchReward(MatchRewardItem.wStartRankID);
	if (pMatchRewardItem == NULL)
	{
		AfxMessageBox(TEXT("��Ǹ��δ��ѯ������������Ľ������ã�"));
		return;
	}

	//У������
	if (pMatchRewardItem->wStartRankID != MatchRewardItem.wStartRankID || pMatchRewardItem->wEndRankID != MatchRewardItem.wEndRankID)
	{
		AfxMessageBox(TEXT("��Ǹ���������䲻���޸ģ�"));
		return;
	}

	//��������
	CopyMemory(pMatchRewardItem,&MatchRewardItem,sizeof(tagMatchRewardItem));

	//��������
	m_RewardListControl.UpdateMatchReward(pMatchRewardItem);
}

//�޸Ľ���
VOID CDlgMatchReward::OnBnClickedDeleteReward()
{
	//��������	
	tagMatchRewardItem MatchRewardItem;

	//��ȡ����
	MatchRewardItem.wEndRankID = GetDlgItemInt(IDC_RANKID_END);
	MatchRewardItem.wStartRankID = GetDlgItemInt(IDC_RANKID_START);

	//���ҽ���
	tagMatchRewardItem * pMatchRewardItem=m_RewardInfoBuffer.SearchMatchReward(MatchRewardItem.wStartRankID);
	if (pMatchRewardItem==NULL)
	{
		AfxMessageBox(TEXT("��Ǹ�������εĽ���������,��Ҫ���������ӽ�����ť��"));
		return;
	}

	//���¿ؼ�
	if (pMatchRewardItem->wStartRankID==MatchRewardItem.wStartRankID && pMatchRewardItem->wEndRankID==MatchRewardItem.wEndRankID)
	{
		m_RewardListControl.DeleteMatchReward(pMatchRewardItem);
		m_RewardInfoBuffer.DeleteMatchRewardItem(pMatchRewardItem->wStartRankID);	
	}

	//����ؼ� 
	CleanControl();
}

//������
VOID CDlgMatchReward::OnLvnItemchangedListReward(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//��������
	if (pNMLV->iItem!=LB_ERR)
	{
		//��������
		tagMatchRewardItem * pMatchRewardItem=(tagMatchRewardItem *)m_RewardListControl.GetItemData(pNMLV->iItem);
		if (pMatchRewardItem!=NULL) SetMatchRewardItem(pMatchRewardItem);
	}

	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchLockTime::CDlgMatchLockTime() : CDlgMatchItem(IDD_MATCH_LOCKTIME)
{
}

//��������
CDlgMatchLockTime::~CDlgMatchLockTime()
{
}

//��ʼ������
BOOL CDlgMatchLockTime::OnInitDialog()
{
	__super::OnInitDialog();

	//��������	
	((CEdit *)GetDlgItem(IDC_REVIVE_FEE))->LimitText(8);	
	((CEdit *)GetDlgItem(IDC_REVIVE_TIMES))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_REVIVE_MULTI))->LimitText(8);
	((CEdit *)GetDlgItem(IDC_MIN_PLAYER_COUNT))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_MAX_PLAYER_COUNT))->LimitText(2);

	//��������
	TCHAR szValue[8];

	//��
	for (int i = 1; i <= 7; i++)
	{
		_sntprintf_s(szValue, CountArray(szValue), TEXT(" %d"), i);
		auto pItemData = (tagItemData*)m_ComboWeekDay.GetItemDataPtr(m_ComboWeekDay.InsertString(i - 1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}
	}

	//��
	for (int i = 1; i <= 31; i++)
	{
		_sntprintf_s(szValue, CountArray(szValue), TEXT(" %d"), i);
		auto pItemData = (tagItemData*)m_ComboMonthDay.GetItemDataPtr(m_ComboMonthDay.InsertString(i - 1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}
	}

	//ʱ
	CComboBox * pComboBox1=(CComboBox *)GetDlgItem(IDC_COMBO_HOUR_1);
	CComboBox * pComboBox2=(CComboBox *)GetDlgItem(IDC_COMBO_HOUR_2);
	CComboBox * pComboBox3=(CComboBox *)GetDlgItem(IDC_COMBO_HOUR_3);
	for (int i=0; i<24; i++)
	{
		_itot_s(i,szValue,10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i,szValue),i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i,szValue),i);
	}

	for (int i=0; i<240; i++)
	{
		_itot_s(i,szValue,10);
		pComboBox3->SetItemData(pComboBox3->InsertString(i,szValue),i);
	}

	//��
	pComboBox1=(CComboBox *)GetDlgItem(IDC_COMBO_MINUTE_1);
	pComboBox2=(CComboBox *)GetDlgItem(IDC_COMBO_MINUTE_2);
	pComboBox3=(CComboBox *)GetDlgItem(IDC_COMBO_MINUTE_3);
	for (int i=0; i<60; i++)
	{
		_itot_s(i,szValue,10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i,szValue),i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i,szValue),i);
		pComboBox3->SetItemData(pComboBox3->InsertString(i,szValue),i);
	}

	//��
	pComboBox1=(CComboBox *)GetDlgItem(IDC_COMBO_SECOND_1);
	pComboBox2=(CComboBox *)GetDlgItem(IDC_COMBO_SECOND_2);
	pComboBox3=(CComboBox *)GetDlgItem(IDC_COMBO_SECOND_3);
	for (int i=0; i<60; i++)
	{
		_itot_s(i,szValue,10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i,szValue),i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i,szValue),i);
		pComboBox3->SetItemData(pComboBox3->InsertString(i,szValue),i);
	}

	//��������
	tagLockTimeMatch * pLockTimeMatch=(tagLockTimeMatch *)m_pMatchOption->cbMatchRule;

	//ѭ������
	if (pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
	{
		CheckRadioButton(IDC_RADIO_MATCH_LOCKTIME,IDC_RADIO_MATCH_ROUND,IDC_RADIO_MATCH_ROUND);
	}

	//��ʱ����
	if (pLockTimeMatch->cbMatchMode==MATCH_MODE_LOCKTIME)
	{
		CheckRadioButton(IDC_RADIO_MATCH_LOCKTIME,IDC_RADIO_MATCH_ROUND,IDC_RADIO_MATCH_LOCKTIME);
	}	

	//��Ϸ����
	if (pLockTimeMatch->wMinPlayerCount!=0)
	{
		SetDlgItemInt(IDC_MIN_PLAYER_COUNT,(UINT)pLockTimeMatch->wMinPlayerCount);
	}

	//��Ϸ����
	if (pLockTimeMatch->wMaxPlayerCount!=0)
	{
		SetDlgItemInt(IDC_MAX_PLAYER_COUNT,(UINT)pLockTimeMatch->wMaxPlayerCount);
	}

	//�������
	if (pLockTimeMatch->lReviveMulti!=0)
	{
		SetDlgItemInt(IDC_REVIVE_MULTI,(UINT)pLockTimeMatch->lReviveMulti);
	}

	//�������
	if (pLockTimeMatch->cbReviveTimes!=0)
	{
		SetDlgItemInt(IDC_REVIVE_TIMES,(UINT)pLockTimeMatch->cbReviveTimes);
	}

	//�������
	if (pLockTimeMatch->lFeeAmount!=0)
	{
		SetDlgItemInt(IDC_REVIVE_FEE,(UINT)pLockTimeMatch->lFeeAmount);
	}	

	//��������
	switch (pLockTimeMatch->wFeeMoneyID)
	{
	case GOODS_ID_GOLD:		//���
	{
		CheckRadioButton(IDC_RADIO_FEETYPE_GOLD, IDC_RADIO_FEETYPE_INGOT, IDC_RADIO_FEETYPE_GOLD);
		break;
	}
	//case GOODS_ID_INGOT:	//��ʯ
	//{
	//	CheckRadioButton(IDC_RADIO_FEETYPE_GOLD, IDC_RADIO_FEETYPE_INGOT, IDC_RADIO_FEETYPE_INGOT);
	//	break;
	//}
	}	

	//ʱ�䶨��	
	SYSTEMTIME MatchInterVal=pLockTimeMatch->MatchInterval;
	SYSTEMTIME MatchLockTime=pLockTimeMatch->MatchLockTime;		
	SYSTEMTIME MatchSignupTime=pLockTimeMatch->MatchSignupTime;
	SYSTEMTIME MatchStartTime=pLockTimeMatch->MatchStartTime;	
	SYSTEMTIME MatchCutOffTime=pLockTimeMatch->MatchCutOffTime;	

	//��ʼ����
	CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->SetTime(&MatchStartTime);

	//��������
	CDateTimeCtrl * pCutOffDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pCutOffDate->SetTime(&MatchCutOffTime);

	//��ʼʱ��
	CDateTimeCtrl * pStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->SetTime(&MatchStartTime);

	//����ʱ��
	CDateTimeCtrl * pCutOffTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pCutOffTime->SetTime(&MatchCutOffTime);

  //�������
	SetMatchTime(MatchInterVal,IDC_COMBO_HOUR_1,IDC_COMBO_MINUTE_1,IDC_COMBO_SECOND_1);
	
	//����ʱ��
	SetMatchTime(MatchLockTime,IDC_COMBO_HOUR_2,IDC_COMBO_MINUTE_2,IDC_COMBO_SECOND_2);

	//����ʱ��
	SetMatchTime(MatchSignupTime,IDC_COMBO_HOUR_3,IDC_COMBO_MINUTE_3,IDC_COMBO_SECOND_3);

	//ÿ�ܿ�
	if (pLockTimeMatch->cbMatchCycle==MATCH_CYCLE_PER_WEEK)
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY,IDC_RADIO_EVERY_MONTH,IDC_RADIO_EVERY_WEEK);		
		SetComboDataList(&m_ComboWeekDay, pLockTimeMatch->cbDayFlagList, CountArray(pLockTimeMatch->cbDayFlagList));
	}
	//ÿ�¿�
	else if (MatchLockTime.wDay!=0)
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY,IDC_RADIO_EVERY_MONTH,IDC_RADIO_EVERY_MONTH);
		SetComboDataList(&m_ComboMonthDay, pLockTimeMatch->cbDayFlagList,CountArray(pLockTimeMatch->cbDayFlagList));
	}
	//ÿ�쿪
	else
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY,IDC_RADIO_EVERY_MONTH,IDC_RADIO_EVERY_DAY);
	}

	return FALSE;
}

//��������
VOID CDlgMatchLockTime::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_WEEK_DAY, m_ComboWeekDay);
	DDX_Control(pDX, IDC_COMBO_MONTH_DAY, m_ComboMonthDay);

	__super::DoDataExchange(pDX);
}

//��������
bool CDlgMatchLockTime::SaveInputInfo()
{
	//��������
	tagLockTimeMatch * pLockTimeMatch=(tagLockTimeMatch *)m_pMatchOption->cbMatchRule;
	ZeroMemory(pLockTimeMatch,sizeof(tagLockTimeMatch));

	//��ȡ�ؼ�
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_MODE);

	//��������
	pLockTimeMatch->wMinPlayerCount=GetDlgItemInt(IDC_MIN_PLAYER_COUNT);
	pLockTimeMatch->wMaxPlayerCount=GetDlgItemInt(IDC_MAX_PLAYER_COUNT);

	//��������
	pLockTimeMatch->lFeeAmount=GetDlgItemInt(IDC_REVIVE_FEE);
	pLockTimeMatch->lReviveMulti=GetDlgItemInt(IDC_REVIVE_MULTI);
	pLockTimeMatch->cbReviveTimes=GetDlgItemInt(IDC_REVIVE_TIMES);

	//��������
	if (IsDlgButtonChecked(IDC_RADIO_FEETYPE_GOLD)) pLockTimeMatch->wFeeMoneyID=GOODS_ID_GOLD;
	//if (IsDlgButtonChecked(IDC_RADIO_FEETYPE_INGOT)) pLockTimeMatch->wFeeMoneyID=GOODS_ID_INGOT;

	//����ģʽ
	if (IsDlgButtonChecked(IDC_RADIO_MATCH_ROUND)) pLockTimeMatch->cbMatchMode=MATCH_MODE_ROUND;
	if (IsDlgButtonChecked(IDC_RADIO_MATCH_LOCKTIME)) pLockTimeMatch->cbMatchMode=MATCH_MODE_LOCKTIME;

	//��Ϸ����
	if (pLockTimeMatch->wMinPlayerCount==0)
	{
		AfxMessageBox(TEXT("������Ϸ�������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}
	
	//��Ϸ����
	if (pLockTimeMatch->wMaxPlayerCount!=0 && pLockTimeMatch->wMaxPlayerCount<pLockTimeMatch->wMinPlayerCount)
	{
		AfxMessageBox(TEXT("�����Ϸ�������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}
  
	//ʱ�䶨��
	CTime MatchTime,MatchDate;
	CDateTimeCtrl * pDatTimeCtrl;				
  
	//��ʼ����
	pDatTimeCtrl=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pDatTimeCtrl->GetTime(MatchDate);

	//��ʼʱ��
	pDatTimeCtrl=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pDatTimeCtrl->GetTime(MatchTime);

	//�ϲ�ʱ��
	CTime MatchStartTime(MatchDate.GetYear(),MatchDate.GetMonth(),MatchDate.GetDay(),MatchTime.GetHour(),MatchTime.GetMinute(),MatchTime.GetSecond());	

	//����ʱ��	
	MatchStartTime.GetAsSystemTime(pLockTimeMatch->MatchStartTime);

	//��ֹ����
	pDatTimeCtrl=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pDatTimeCtrl->GetTime(MatchDate);	

	//��ֹʱ��
	pDatTimeCtrl=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pDatTimeCtrl->GetTime(MatchTime);

	//�ϲ�ʱ��
	CTime MatchCutOffTime(MatchDate.GetYear(),MatchDate.GetMonth(),MatchDate.GetDay(),MatchTime.GetHour(),MatchTime.GetMinute(),MatchTime.GetSecond());

	//��ȡʱ��
	MatchCutOffTime.GetAsSystemTime(pLockTimeMatch->MatchCutOffTime);	

	//����ʱ��
	GetMatchTime(pLockTimeMatch->MatchSignupTime,IDC_COMBO_HOUR_3,IDC_COMBO_MINUTE_3,IDC_COMBO_SECOND_3);

	//ѭ������
	if (pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
	{
		//���ñ���
		pLockTimeMatch->MatchInterval.wDay=0;
		pLockTimeMatch->MatchInterval.wYear=0;		
		pLockTimeMatch->MatchInterval.wMonth=0;
		pLockTimeMatch->MatchInterval.wDayOfWeek=0;

		//��ȡʱ��
		GetMatchTime(pLockTimeMatch->MatchInterval,IDC_COMBO_HOUR_1,IDC_COMBO_MINUTE_1,IDC_COMBO_SECOND_1);				
	}

	//��ʱ����
	if (pLockTimeMatch->cbMatchMode==MATCH_MODE_LOCKTIME)
	{
		//���ñ���
		pLockTimeMatch->MatchLockTime.wDay=0;
		pLockTimeMatch->MatchLockTime.wYear=0;		
		pLockTimeMatch->MatchLockTime.wMonth=0;
		pLockTimeMatch->MatchLockTime.wDayOfWeek=0;

		//��ʼʱ��
		GetMatchTime(pLockTimeMatch->MatchLockTime,IDC_COMBO_HOUR_2,IDC_COMBO_MINUTE_2,IDC_COMBO_SECOND_2);					

		//ÿ�ܿ�
		if (IsDlgButtonChecked(IDC_RADIO_EVERY_WEEK))
		{
			//��ȡ����
			GetComboDataList(&m_ComboWeekDay, pLockTimeMatch->cbDayFlagList, CountArray(pLockTimeMatch->cbDayFlagList));

			//��������
			pLockTimeMatch->cbMatchCycle = MATCH_CYCLE_PER_WEEK;
		}

		//ÿ�¿�
		if (IsDlgButtonChecked(IDC_RADIO_EVERY_MONTH)) 
		{
			//��ȡ����
			GetComboDataList(&m_ComboMonthDay, pLockTimeMatch->cbDayFlagList, CountArray(pLockTimeMatch->cbDayFlagList));

			//��������
			pLockTimeMatch->cbMatchCycle = MATCH_CYCLE_PER_MONTH;
		}
	}	

	return true;
}

//�����ؼ�
VOID CDlgMatchLockTime::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//����ʱ��
VOID CDlgMatchLockTime::GetMatchTime(SYSTEMTIME &MatchTime,UINT uHourID,UINT uMinuteID,UINT uSecondID)
{
	//ʱ
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uHourID);
	if (pComboBox!=NULL)
	{
		MatchTime.wHour=0;
		if (pComboBox->GetCurSel()!=-1) MatchTime.wHour=pComboBox->GetItemData(pComboBox->GetCurSel()); 
	}

	//��
	pComboBox=(CComboBox *)GetDlgItem(uMinuteID);
	if (pComboBox!=NULL)
	{
		MatchTime.wMinute=0;
		if (pComboBox->GetCurSel()!=-1) MatchTime.wMinute=pComboBox->GetItemData(pComboBox->GetCurSel()); 
	}

	//��
	pComboBox=(CComboBox *)GetDlgItem(uSecondID);
	if (pComboBox!=NULL)
	{
		MatchTime.wSecond=0;
		if (pComboBox->GetCurSel()!=-1) MatchTime.wSecond=pComboBox->GetItemData(pComboBox->GetCurSel()); 
	}
}

//����ʱ��
VOID CDlgMatchLockTime::SetMatchTime(const SYSTEMTIME &MatchTime,UINT uHourID,UINT uMinuteID,UINT uSecondID)
{
	//ʱ
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uHourID);
	if (pComboBox!=NULL)
	{
		for (int i=0;i<240;i++)
		{
			if (pComboBox->GetItemData(i)==MatchTime.wHour)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//��
	pComboBox=(CComboBox *)GetDlgItem(uMinuteID);
	if (pComboBox!=NULL)
	{
		for (int i=0;i<60;i++)
		{
			if (pComboBox->GetItemData(i)==MatchTime.wMinute)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//��
	pComboBox=(CComboBox *)GetDlgItem(uSecondID);
	if (pComboBox!=NULL)
	{
		for (int i=0;i<60;i++)
		{
			if (pComboBox->GetItemData(i)==MatchTime.wSecond)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}
}

//��ȡ���б�
VOID CDlgMatchLockTime::GetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount)
{
	//��ȡ����	
	for (INT i = 0; i < pCheckComboBox->GetCount(); i++)
	{
		auto pItemData = (tagItemData*)pCheckComboBox->GetItemDataPtr(i);
		cboDataFlagList[i] = pItemData->bChecked;
		if (i >= cbDataCount) break;
	}
}

//�������б�
VOID CDlgMatchLockTime::SetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount)
{
	//��ȡ����	
	for (INT j = 0; j < pCheckComboBox->GetCount(); j++)
	{
		auto pItemData = (tagItemData*)pCheckComboBox->GetItemDataPtr(j);
		if (pItemData->dwDataPtr > cbDataCount) continue;
		if (cboDataFlagList[pItemData->dwDataPtr - 1] == TRUE)
		{
			pItemData->bChecked = TRUE;
		}
	}

	//��������
	pCheckComboBox->UpdateData();
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchImmediate::CDlgMatchImmediate(): CDlgMatchItem(IDD_MATCH_IMMEDIATE)
{	
}

//��������
CDlgMatchImmediate::~CDlgMatchImmediate()
{
}

//��ʼ������
BOOL CDlgMatchImmediate::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_PLAYER_COUNT_MIN))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_PLAYER_COUNT_MAX))->LimitText(2);

	//��������
	tagImmediateMatch * pImmediateMatch=(tagImmediateMatch *)m_pMatchOption->cbMatchRule;

	//��������
	SetDlgItemInt(IDC_PLAYER_COUNT_MIN,pImmediateMatch->wMinPlayerCount);
	SetDlgItemInt(IDC_PLAYER_COUNT_MAX,pImmediateMatch->wMaxPlayerCount);

	//����ʱ��	
	SYSTEMTIME MatchStartTime=pImmediateMatch->MatchStartTime;	
	SYSTEMTIME MatchCutOffTime=pImmediateMatch->MatchCutOffTime;	

	//��ʼ����
	CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->SetTime(&MatchStartTime);

	//��ʼʱ��
	CDateTimeCtrl * pStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->SetTime(&MatchStartTime);

	//��ֹ����
	CDateTimeCtrl * pCutOffDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pCutOffDate->SetTime(&MatchCutOffTime);

	//��ֹʱ��
	CDateTimeCtrl * pCutOffTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pCutOffTime->SetTime(&MatchCutOffTime);

	return TRUE;
}

//�ؼ���
VOID CDlgMatchImmediate::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);	
}

//��������
bool CDlgMatchImmediate::SaveInputInfo()
{
	//��������
	tagImmediateMatch * pImmediateMatch=(tagImmediateMatch *)m_pMatchOption->cbMatchRule;

	//��ȡ����
	pImmediateMatch->wMinPlayerCount=GetDlgItemInt(IDC_PLAYER_COUNT_MIN);
	pImmediateMatch->wMaxPlayerCount=GetDlgItemInt(IDC_PLAYER_COUNT_MAX);

	//У������
	if (pImmediateMatch->wMinPlayerCount==0)
	{
		AfxMessageBox(TEXT("��Ϸ���������������������ã�"));

		return false;
  }

	//У������
	if (pImmediateMatch->wMinPlayerCount>pImmediateMatch->wMaxPlayerCount)
	{
		AfxMessageBox(TEXT("��Ϸ���������������������ã�"));

		return false;
  }

	//ʱ�䶨��	
	CTime StartTime,StartDate;
	CTime CutOffTime,CutOffDate;		
	CDateTimeCtrl * pStartDate,* pStartTime;
	CDateTimeCtrl * pCutOffDate,* pCutOffTime;

	//��ʼ����
	pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->GetTime(StartDate);

	//��ʼʱ��
	pStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->GetTime(StartTime);
  
	//��ֹ����
	pCutOffDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pCutOffDate->GetTime(CutOffDate);

	//��ֹʱ��
	pCutOffTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pCutOffTime->GetTime(CutOffTime);

	//�ϲ�ʱ��	
	CTime StartDateTime(StartDate.GetYear(),StartDate.GetMonth(),StartDate.GetDay(),StartTime.GetHour(),StartTime.GetMinute(),StartTime.GetSecond());
	CTime CutOffDateTime(CutOffDate.GetYear(),CutOffDate.GetMonth(),CutOffDate.GetDay(),CutOffTime.GetHour(),CutOffTime.GetMinute(),CutOffTime.GetSecond());	

	//����ʱ��	
	StartDateTime.GetAsSystemTime(pImmediateMatch->MatchStartTime);
	CutOffDateTime.GetAsSystemTime(pImmediateMatch->MatchCutOffTime);

	return true;
}

//�����ؼ�
VOID CDlgMatchImmediate::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchTournament::CDlgMatchTournament() : CDlgMatchItem(IDD_MATCH_TOURNAMENT)
{
}

//��������
CDlgMatchTournament::~CDlgMatchTournament()
{
}

//��ʼ������
BOOL CDlgMatchTournament::OnInitDialog()
{
	__super::OnInitDialog();

	//��������		
	((CEdit*)GetDlgItem(IDC_EDIT_PLAY_COUNT))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_EDIT_BASE_SCORE))->LimitText(8);
	((CEdit*)GetDlgItem(IDC_EDIT_MATCH_SCORE))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_MAX_TABLE_COUNT))->LimitText(5);
	((CEdit*)GetDlgItem(IDC_MIN_PLAYER_COUNT))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_MAX_PLAYER_COUNT))->LimitText(2);	

	//��������
	TCHAR szValue[8];
	
	//����
	for (int i = 1; i <= 7; i++)
	{
		_sntprintf_s(szValue,CountArray(szValue),TEXT(" %d"),i);
		auto pItemData = (tagItemData *)m_ComboWeekDay.GetItemDataPtr(m_ComboWeekDay.InsertString(i-1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}		
	}

	//����
	for (int i = 1; i <= MAX_DAY; i++)
	{
		_sntprintf_s(szValue, CountArray(szValue), TEXT(" %d"), i);
		auto pItemData = (tagItemData*)m_ComboMonthDay.GetItemDataPtr(m_ComboMonthDay.InsertString(i-1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}	
	}

	//����
	for (int i = 1; i <= MAX_MONTH; i++)
	{
		_sntprintf_s(szValue, CountArray(szValue), TEXT(" %d"), i);
		auto pItemData = (tagItemData*)m_ComboYearMonth.GetItemDataPtr(m_ComboYearMonth.InsertString(i - 1, szValue));
		if (pItemData != NULL)
		{
			pItemData->dwDataPtr = i;
		}
	}

	
	//ʱ
	CComboBox* pComboBox1 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_1);
	CComboBox* pComboBox2 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_2);
	CComboBox* pComboBox3 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_3);
	CComboBox* pComboBox4 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_4);
	CComboBox* pComboBox5 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_5);
	CComboBox* pComboBox6 = (CComboBox*)GetDlgItem(IDC_COMBO_HOUR_6);
	for (int i = 0; i < 24; i++)
	{
		_itot_s(i, szValue, 10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i, szValue), i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i, szValue), i);
	}

	for (int i = 0; i < 1440; i++)
	{
		_itot_s(i, szValue, 10);
		pComboBox3->SetItemData(pComboBox3->InsertString(i, szValue), i);
		pComboBox4->SetItemData(pComboBox4->InsertString(i, szValue), i);
		pComboBox5->SetItemData(pComboBox5->InsertString(i, szValue), i);
		pComboBox6->SetItemData(pComboBox6->InsertString(i, szValue), i);
	}

	//��
	pComboBox1 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_1);
	pComboBox2 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_2);
	pComboBox3 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_3);
	pComboBox4 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_4);
	pComboBox5 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_5);
	pComboBox6 = (CComboBox*)GetDlgItem(IDC_COMBO_MINUTE_6);
	for (int i = 0; i < 60; i++)
	{
		_itot_s(i, szValue, 10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i, szValue), i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i, szValue), i);
		pComboBox3->SetItemData(pComboBox3->InsertString(i, szValue), i);
		pComboBox4->SetItemData(pComboBox4->InsertString(i, szValue), i);
		pComboBox5->SetItemData(pComboBox5->InsertString(i, szValue), i);
		pComboBox6->SetItemData(pComboBox6->InsertString(i, szValue), i);
	}

	//��
	pComboBox1 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_1);
	pComboBox2 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_2);
	pComboBox3 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_3);
	pComboBox4 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_4);
	pComboBox5 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_5);
	pComboBox6 = (CComboBox*)GetDlgItem(IDC_COMBO_SECOND_6);
	for (int i = 0; i < 60; i++)
	{
		_itot_s(i, szValue, 10);
		pComboBox1->SetItemData(pComboBox1->InsertString(i, szValue), i);
		pComboBox2->SetItemData(pComboBox2->InsertString(i, szValue), i);
		pComboBox3->SetItemData(pComboBox3->InsertString(i, szValue), i);
		pComboBox4->SetItemData(pComboBox4->InsertString(i, szValue), i);
		pComboBox5->SetItemData(pComboBox5->InsertString(i, szValue), i);
		pComboBox6->SetItemData(pComboBox6->InsertString(i, szValue), i);
	}

	//��������
	auto pTournamentMatch = (tagTournamentMatch*)m_pMatchOption->cbMatchRule;

	//ѭ������
	if (pTournamentMatch->cbMatchMode == MATCH_MODE_ROUND)
	{
		CheckRadioButton(IDC_RADIO_MATCH_LOCKTIME, IDC_RADIO_MATCH_ROUND, IDC_RADIO_MATCH_ROUND);
	}

	//��ʱ����
	if (pTournamentMatch->cbMatchMode == MATCH_MODE_LOCKTIME)
	{
		CheckRadioButton(IDC_RADIO_MATCH_LOCKTIME, IDC_RADIO_MATCH_ROUND, IDC_RADIO_MATCH_LOCKTIME);
	}

	//��Ϸ����
	if (pTournamentMatch->wPlayCount!= 0)
	{
		SetDlgItemInt(IDC_EDIT_PLAY_COUNT, (UINT)pTournamentMatch->wPlayCount);
	}

	//��Ϸ�׷�
	if (pTournamentMatch->lInitBaseScore != 0)
	{
		SetDlgItemInt(IDC_EDIT_BASE_SCORE, (UINT)pTournamentMatch->lInitBaseScore);
	}

	//��ʼ����
	SetDlgItemInt(IDC_EDIT_MATCH_SCORE, (UINT)pTournamentMatch->lInitMatchScore);

	//�������
	if (pTournamentMatch->wMaxTableCount!= 0)
	{
		SetDlgItemInt(IDC_MAX_TABLE_COUNT, (UINT)pTournamentMatch->wMaxTableCount);
	}

	//��Ϸ����
	if (pTournamentMatch->wMinPlayerCount != 0)
	{
		SetDlgItemInt(IDC_MIN_PLAYER_COUNT, (UINT)pTournamentMatch->wMinPlayerCount);
	}

	//��Ϸ����
	if (pTournamentMatch->wMaxPlayerCount != 0)
	{
		SetDlgItemInt(IDC_MAX_PLAYER_COUNT, (UINT)pTournamentMatch->wMaxPlayerCount);
	}

	//ʱ�䶨��	
	SYSTEMTIME MatchInterVal = pTournamentMatch->MatchInterval;
	SYSTEMTIME MatchLockTime = pTournamentMatch->MatchLockTime;
	SYSTEMTIME MatchStartTime = pTournamentMatch->MatchStartTime;
	SYSTEMTIME MatchCutOffTime = pTournamentMatch->MatchCutOffTime;
	SYSTEMTIME MatchSignupStart = pTournamentMatch->MatchSignupStart;
	SYSTEMTIME MatchSignupCutOff = pTournamentMatch->MatchSignupCutOff;	
	SYSTEMTIME MatchDurationTime = pTournamentMatch->MatchDurationTime;	
	SYSTEMTIME MatchSaleTicketTime = pTournamentMatch->MatchSaleTicketTime;

	//��ʼ����
	CDateTimeCtrl* pStartDate = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->SetTime(&MatchStartTime);

	//��������
	CDateTimeCtrl* pCutOffDate = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pCutOffDate->SetTime(&MatchCutOffTime);

	//��ʼʱ��
	CDateTimeCtrl* pStartTime = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->SetTime(&MatchStartTime);

	//����ʱ��
	CDateTimeCtrl* pCutOffTime = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pCutOffTime->SetTime(&MatchCutOffTime);

	//�������
	SetMatchTime(MatchInterVal, IDC_COMBO_HOUR_1, IDC_COMBO_MINUTE_1, IDC_COMBO_SECOND_1);

	//����ʱ��
	SetMatchTime(MatchLockTime, IDC_COMBO_HOUR_2, IDC_COMBO_MINUTE_2, IDC_COMBO_SECOND_2);

	//����ʱ��
	SetMatchTime(MatchSignupStart, IDC_COMBO_HOUR_3, IDC_COMBO_MINUTE_3, IDC_COMBO_SECOND_3);
	SetMatchTime(MatchSignupCutOff,IDC_COMBO_HOUR_5, IDC_COMBO_MINUTE_5, IDC_COMBO_SECOND_5);

	//����ʱ��
	SetMatchTime(MatchDurationTime, IDC_COMBO_HOUR_4, IDC_COMBO_MINUTE_4, IDC_COMBO_SECOND_4);

	//��Ʊʱ��
	SetMatchTime(MatchSaleTicketTime, IDC_COMBO_HOUR_6, IDC_COMBO_MINUTE_6, IDC_COMBO_SECOND_6);

	//ÿ�ܿ�
	if (pTournamentMatch->cbMatchCycle == MATCH_CYCLE_PER_WEEK)
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY, IDC_RADIO_EVERY_MONTH, IDC_RADIO_EVERY_WEEK);
		SetComboDataList(&m_ComboWeekDay, pTournamentMatch->cbDayFlagList,CountArray(pTournamentMatch->cbDayFlagList));
	}
	//ÿ�¿�
	else if (pTournamentMatch->cbMatchCycle == MATCH_CYCLE_PER_MONTH)
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY, IDC_RADIO_EVERY_MONTH, IDC_RADIO_EVERY_MONTH);
		SetComboDataList(&m_ComboMonthDay, pTournamentMatch->cbDayFlagList, CountArray(pTournamentMatch->cbDayFlagList));
		SetComboDataList(&m_ComboYearMonth, pTournamentMatch->cbMonthFlagList,CountArray(pTournamentMatch->cbMonthFlagList));
	}
	//ÿ�쿪
	else
	{
		CheckRadioButton(IDC_RADIO_EVERY_DAY, IDC_RADIO_EVERY_MONTH, IDC_RADIO_EVERY_DAY);
	}

	return FALSE;
}

//��������
VOID CDlgMatchTournament::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_WEEK_DAY, m_ComboWeekDay);
	DDX_Control(pDX, IDC_COMBO_MONTH_DAY, m_ComboMonthDay);
	DDX_Control(pDX, IDC_COMBO_YEAR_MONTH, m_ComboYearMonth);

	__super::DoDataExchange(pDX);
}

//��������
bool CDlgMatchTournament::SaveInputInfo()
{
	//��������
	tagTournamentMatch* pTournamentMatch = (tagTournamentMatch*)m_pMatchOption->cbMatchRule;
	ZeroMemory(pTournamentMatch, sizeof(tagTournamentMatch));

	//��ȡ�ؼ�
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_MATCH_MODE);

	//��������
	pTournamentMatch->wPlayCount = GetDlgItemInt(IDC_EDIT_PLAY_COUNT);
	pTournamentMatch->lInitBaseScore = GetDlgItemInt(IDC_EDIT_BASE_SCORE);
	pTournamentMatch->lInitMatchScore = GetDlgItemInt(IDC_EDIT_MATCH_SCORE);
	pTournamentMatch->wMaxTableCount = GetDlgItemInt(IDC_MAX_TABLE_COUNT);
	pTournamentMatch->wMinPlayerCount = GetDlgItemInt(IDC_MIN_PLAYER_COUNT);
	pTournamentMatch->wMaxPlayerCount = GetDlgItemInt(IDC_MAX_PLAYER_COUNT);

	//����ģʽ
	if (IsDlgButtonChecked(IDC_RADIO_MATCH_ROUND)) pTournamentMatch->cbMatchMode = MATCH_MODE_ROUND;
	if (IsDlgButtonChecked(IDC_RADIO_MATCH_LOCKTIME)) pTournamentMatch->cbMatchMode = MATCH_MODE_LOCKTIME;

	//��Ϸ����
	if (pTournamentMatch->wPlayCount == 0)
	{
		AfxMessageBox(TEXT("��Ϸ�������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}

	//��Ϸ�׷�
	if (pTournamentMatch->lInitBaseScore == 0)
	{
		AfxMessageBox(TEXT("��Ϸ�׷����ò��Ϸ������������룡"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}

	//�������
	if (pTournamentMatch->wMaxTableCount == 0)
	{
		AfxMessageBox(TEXT("����������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MAX_TABLE_COUNT)->SetFocus();

		return false;
	}

	//��Ϸ����
	if (pTournamentMatch->wMinPlayerCount == 0)
	{
		AfxMessageBox(TEXT("������Ϸ�������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}

	//��Ϸ����
	if (pTournamentMatch->wMaxPlayerCount != 0 && pTournamentMatch->wMaxPlayerCount < pTournamentMatch->wMinPlayerCount)
	{
		AfxMessageBox(TEXT("�����Ϸ�������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MIN_PLAYER_COUNT)->SetFocus();

		return false;
	}

	//ʱ�䶨��
	CTime MatchTime, MatchDate;
	CDateTimeCtrl* pDatTimeCtrl;

	//��ʼ����
	pDatTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_START_DATE);
	pDatTimeCtrl->GetTime(MatchDate);

	//��ʼʱ��
	pDatTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_START_TIME);
	pDatTimeCtrl->GetTime(MatchTime);

	//�ϲ�ʱ��
	CTime MatchStartTime(MatchDate.GetYear(), MatchDate.GetMonth(), MatchDate.GetDay(), MatchTime.GetHour(), MatchTime.GetMinute(), MatchTime.GetSecond());

	//����ʱ��	
	MatchStartTime.GetAsSystemTime(pTournamentMatch->MatchStartTime);

	//��ֹ����
	pDatTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_CUTOFF_DATE);
	pDatTimeCtrl->GetTime(MatchDate);

	//��ֹʱ��
	pDatTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_MATCH_CUTOFF_TIME);
	pDatTimeCtrl->GetTime(MatchTime);

	//�ϲ�ʱ��
	CTime MatchCutOffTime(MatchDate.GetYear(), MatchDate.GetMonth(), MatchDate.GetDay(), MatchTime.GetHour(), MatchTime.GetMinute(), MatchTime.GetSecond());

	//��ȡʱ��
	MatchCutOffTime.GetAsSystemTime(pTournamentMatch->MatchCutOffTime);

	//����ʱ��
	GetMatchTime(pTournamentMatch->MatchSignupStart, IDC_COMBO_HOUR_3, IDC_COMBO_MINUTE_3, IDC_COMBO_SECOND_3);
	GetMatchTime(pTournamentMatch->MatchSignupCutOff,IDC_COMBO_HOUR_5, IDC_COMBO_MINUTE_5, IDC_COMBO_SECOND_5);

	//����ʱ��
	GetMatchTime(pTournamentMatch->MatchDurationTime, IDC_COMBO_HOUR_4, IDC_COMBO_MINUTE_4, IDC_COMBO_SECOND_4);

	//��Ʊʱ��
	GetMatchTime(pTournamentMatch->MatchSaleTicketTime, IDC_COMBO_HOUR_6, IDC_COMBO_MINUTE_6, IDC_COMBO_SECOND_6);

	//ѭ������
	if (pTournamentMatch->cbMatchMode == MATCH_MODE_ROUND)
	{
		//���ñ���
		pTournamentMatch->MatchInterval.wDay = 0;
		pTournamentMatch->MatchInterval.wYear = 0;
		pTournamentMatch->MatchInterval.wMonth = 0;
		pTournamentMatch->MatchInterval.wDayOfWeek = 0;

		//��ȡʱ��
		GetMatchTime(pTournamentMatch->MatchInterval, IDC_COMBO_HOUR_1, IDC_COMBO_MINUTE_1, IDC_COMBO_SECOND_1);
	}

	//��ʱ����
	if (pTournamentMatch->cbMatchMode == MATCH_MODE_LOCKTIME)
	{
		//���ñ���
		pTournamentMatch->MatchLockTime.wDay = 0;
		pTournamentMatch->MatchLockTime.wYear = 0;
		pTournamentMatch->MatchLockTime.wMonth = 0;
		pTournamentMatch->MatchLockTime.wDayOfWeek = 0;

		//��������
		pTournamentMatch->cbMatchCycle = MATCH_CYCLE_PER_DAY;

		//��ʼʱ��
		GetMatchTime(pTournamentMatch->MatchLockTime, IDC_COMBO_HOUR_2, IDC_COMBO_MINUTE_2, IDC_COMBO_SECOND_2);

		//���ܿ�
		if (IsDlgButtonChecked(IDC_RADIO_EVERY_WEEK))
		{
			//��ȡ����
			GetComboDataList(&m_ComboWeekDay, pTournamentMatch->cbDayFlagList, CountArray(pTournamentMatch->cbDayFlagList));

			//��������
			pTournamentMatch->cbMatchCycle = MATCH_CYCLE_PER_WEEK;
		}

		//���¿�
		if (IsDlgButtonChecked(IDC_RADIO_EVERY_MONTH))
		{
			//��ȡ����
			GetComboDataList(&m_ComboMonthDay, pTournamentMatch->cbDayFlagList, CountArray(pTournamentMatch->cbDayFlagList));
			GetComboDataList(&m_ComboYearMonth, pTournamentMatch->cbMonthFlagList, CountArray(pTournamentMatch->cbMonthFlagList));

			//��������
			pTournamentMatch->cbMatchCycle = MATCH_CYCLE_PER_MONTH;
		}
	}

	return true;
}

//�����ؼ�
VOID CDlgMatchTournament::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//����ʱ��
VOID CDlgMatchTournament::GetMatchTime(SYSTEMTIME& MatchTime, UINT uHourID, UINT uMinuteID, UINT uSecondID)
{
	//ʱ
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(uHourID);
	if (pComboBox != NULL)
	{
		MatchTime.wHour = 0;
		if (pComboBox->GetCurSel() != -1) MatchTime.wHour = pComboBox->GetItemData(pComboBox->GetCurSel());
	}

	//��
	pComboBox = (CComboBox*)GetDlgItem(uMinuteID);
	if (pComboBox != NULL)
	{
		MatchTime.wMinute = 0;
		if (pComboBox->GetCurSel() != -1) MatchTime.wMinute = pComboBox->GetItemData(pComboBox->GetCurSel());
	}

	//��
	pComboBox = (CComboBox*)GetDlgItem(uSecondID);
	if (pComboBox != NULL)
	{
		MatchTime.wSecond = 0;
		if (pComboBox->GetCurSel() != -1) MatchTime.wSecond = pComboBox->GetItemData(pComboBox->GetCurSel());
	}
}

//����ʱ��
VOID CDlgMatchTournament::SetMatchTime(const SYSTEMTIME& MatchTime, UINT uHourID, UINT uMinuteID, UINT uSecondID)
{
	//ʱ
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(uHourID);
	if (pComboBox != NULL)
	{
		for (int i = 0; i < 1440; i++)
		{
			if (pComboBox->GetItemData(i) == MatchTime.wHour)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//��
	pComboBox = (CComboBox*)GetDlgItem(uMinuteID);
	if (pComboBox != NULL)
	{
		for (int i = 0; i < 60; i++)
		{
			if (pComboBox->GetItemData(i) == MatchTime.wMinute)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//��
	pComboBox = (CComboBox*)GetDlgItem(uSecondID);
	if (pComboBox != NULL)
	{
		for (int i = 0; i < 60; i++)
		{
			if (pComboBox->GetItemData(i) == MatchTime.wSecond)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}
}

//��ȡ���б�
VOID CDlgMatchTournament::GetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount)
{
	//��ȡ����	
	for (INT i = 0; i < pCheckComboBox->GetCount(); i++)
	{
		auto pItemData = (tagItemData*)pCheckComboBox->GetItemDataPtr(i);
		cboDataFlagList[i] = pItemData->bChecked;
		if (i >= cbDataCount) break;
	}
}

//�������б�
VOID CDlgMatchTournament::SetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount)
{
	//��ȡ����	
	for (INT j = 0; j < pCheckComboBox->GetCount(); j++)
	{
		auto pItemData = (tagItemData*)pCheckComboBox->GetItemDataPtr(j);
		if (pItemData->dwDataPtr > cbDataCount) continue;		
		if (cboDataFlagList[pItemData->dwDataPtr-1]==TRUE)
		{
			pItemData->bChecked = TRUE;
		}			
	}	

	//��������
	pCheckComboBox->UpdateData();
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchWizardItem::CDlgMatchWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_bSaveData=true;

	//���ñ���	
	m_pMatchOption=NULL;
	m_pDlgMatchWizard=NULL;
	m_pMatchInfoBuffer=NULL;
	m_pModuleInfoBuffer=NULL;

	return;
}

//��������
CDlgMatchWizardItem::~CDlgMatchWizardItem()
{
}

//��������
bool CDlgMatchWizardItem::SaveItemData()
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
bool CDlgMatchWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
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

	//��������
	OnMatchItemActive();

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//ȷ������
VOID CDlgMatchWizardItem::OnOK() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgMatchWizardItem::OnCancel() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//λ����Ϣ
VOID CDlgMatchWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchWizardStep1::CDlgMatchWizardStep1() : CDlgMatchWizardItem(IDD_MATCH_WIZARD_1)
{
	//��������
	m_cbItemIndex=0xff;
	ZeroMemory(m_pDlgMatchItem,sizeof(m_pDlgMatchItem));

	return;
}

//��������
CDlgMatchWizardStep1::~CDlgMatchWizardStep1()
{
}

//�ؼ���
VOID CDlgMatchWizardStep1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_COMBO_MODULE_NAME,m_ModuleComboBox);
}

//��ʼ������
BOOL CDlgMatchWizardStep1::OnInitDialog()
{
	__super::OnInitDialog();

	//��ʼ������
	InitBaseOption();

	//��ȡ�ؼ�
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_MATCH_OPTION);

	//��������
	pTabCtrl->InsertItem(TAB_OPTION_SIGNUP1,TEXT("������ʽ"));
	pTabCtrl->InsertItem(TAB_OPTION_SIGNUP2,TEXT("��������"));
	//pTabCtrl->InsertItem(TAB_OPTION_RANKINGMODE,TEXT("������ʽ"));
	pTabCtrl->InsertItem(TAB_OPTION_DISTRIBUTE,TEXT("��������"));	
	pTabCtrl->InsertItem(TAB_OPTION_REWARD,TEXT("��������"));	
	//pTabCtrl->InsertItem(TAB_OPTION_ROUND,TEXT("�ִ�����"));	

	//���ô�С
	pTabCtrl->SetItemSize(CSize(120,25));

	//�������
	m_pDlgMatchItem[0]=&m_DlgMatchSignup1;
	m_pDlgMatchItem[1]=&m_DlgMatchSignup2;
	//m_pDlgMatchItem[2]=&m_DlgMatchRankingMode;
	m_pDlgMatchItem[2]=&m_DlgMatchDistribute;	
	m_pDlgMatchItem[3]=&m_DlgMatchReward;	
	//m_pDlgMatchItem[5]=&m_DlgMatchRound;	

	//��������
	CRect rcCreate(0,0,0,0);

	//��������
	for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
	{
		//ָ���ж�
		if (m_pDlgMatchItem[i]==NULL) continue;

		//���ÿؼ�		
		m_pDlgMatchItem[i]->SetMatchOption(m_pMatchOption);
		m_pDlgMatchItem[i]->SetMatchInfoBuffer(m_pMatchInfoBuffer);		

		//��������		
		m_pDlgMatchItem[i]->CreateMatchItem(rcCreate,pTabCtrl);
	}

	//��������
	ActiveOptionItem(TAB_OPTION_SIGNUP1);

	return TRUE;
}

//��������
bool CDlgMatchWizardStep1::SaveInputInfo()
{
	//��������
	if (SaveBaseOption()==false) return false;

	//��������
	for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
	{
		if (m_pDlgMatchItem[i]->SaveInputInfo() == false) return false;
	}

	return true;
}

//��������
VOID CDlgMatchWizardStep1::OnMatchItemActive()
{
	return;
}

//�����ؼ�
VOID CDlgMatchWizardStep1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//��������
VOID CDlgMatchWizardStep1::InitBaseOption()
{
	//��������
	((CEdit *)GetDlgItem(IDC_EDIT_MATCH_NAME))->LimitText(32);		

	//��������
	SetDlgItemText(IDC_EDIT_MATCH_NAME,CW2CT(m_pMatchOption->szMatchName));

	//��������
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_TYPE);
	if (pComboBox!=NULL)
	{
		//��������
		BYTE cbMatchType[]={MATCH_TYPE_LOCKTIME,MATCH_TYPE_IMMEDIATE,MATCH_TYPE_TOURNAMENT };
		LPCTSTR pszMatchType[]={TEXT("��ʱ������"),TEXT("��ʱ������"), TEXT("����������")};

		//��������
		for (INT i=0;i<CountArray(cbMatchType);i++)
		{
			int nIndex=pComboBox->AddString(pszMatchType[i]);
			pComboBox->SetItemData(nIndex,cbMatchType[i]);
			if (m_pMatchOption->cbMatchType==cbMatchType[i])
			{
				pComboBox->SetCurSel(nIndex);
			}
		}

		//Ĭ��ѡ��
		if (pComboBox->GetCurSel()==CB_ERR) pComboBox->SetCurSel(0);
		if (m_pMatchOption->dwMatchID!=0) pComboBox->EnableWindow(FALSE);
	}

	//չʾ����
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SHOW_TYPE);
	if (pComboBox != NULL)
	{
		//��������
		BYTE cbShowType[] = { 0, MATCH_SHOW_TTYPE_DAYLY,MATCH_SHOW_TTYPE_WEEKLY,MATCH_SHOW_TTYPE_PLAYOFF };
		LPCTSTR pszShowType[] = { TEXT("��ͨ����") ,TEXT("��������"),TEXT("��������"),TEXT("����������") };

		//��������
		for (INT i = 0; i < CountArray(cbShowType); i++)
		{
			int nIndex = pComboBox->AddString(pszShowType[i]);
			pComboBox->SetItemData(nIndex, cbShowType[i]);
			if (m_pMatchOption->cbShowType == cbShowType[i])
			{
				pComboBox->SetCurSel(nIndex);
			}
		}

		//Ĭ��ѡ��
		if (pComboBox->GetCurSel() == CB_ERR) pComboBox->SetCurSel(0);		
	}

	//��������
	pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_JOIN_CONDITION);
	if (pComboBox!=NULL)
	{
		//׼������
		OnCbnSelchangeComboMatchType();	

		//����ѡ��
		for (INT i=0;i<pComboBox->GetCount();i++)
		{
			if (pComboBox->GetItemData(i)==m_pMatchOption->cbJoinCondition)
			{
				pComboBox->SetCurSel(i);
				break;
			}
		}
	}

	//ģ���б�
	if (m_pModuleInfoBuffer!=NULL)
	{
		//��������
		tagGameModuleInfo * pGameModuleInfo=NULL;

		//��������
		POSITION Position=m_pModuleInfoBuffer->m_GameModuleInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position!=NULL)
		{
			//��ȡ����
			WORD wModuleID=0L;
			tagGameModuleInfo * pGameModuleInfo=NULL;
			m_pModuleInfoBuffer->m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);

			//�����б�
			ASSERT(pGameModuleInfo!=NULL);
			if (pGameModuleInfo!=NULL)
			{
				int nItemIndex=m_ModuleComboBox.AddString(pGameModuleInfo->szModuleName);
				m_ModuleComboBox.SetItemData(nItemIndex,pGameModuleInfo->wModuleID);
				if (pGameModuleInfo->wModuleID==m_pMatchOption->wKindID)
				{
					m_ModuleComboBox.SetCurSel(nItemIndex);
				}
			}
		}

		//Ĭ��ѡ��
		if (m_ModuleComboBox.GetCurSel()==-1) m_ModuleComboBox.SetCurSel(0);
	}	
}

//��������
bool CDlgMatchWizardStep1::SaveBaseOption()
{
	//������Ϣ
	//m_pMatchOption->lSignupFee=GetDlgItemInt(IDC_EDIT_MATCH_FEE);

	//��������
	TCHAR szMatchName[32];
	GetDlgItemText(IDC_EDIT_MATCH_NAME,szMatchName,CountArray(szMatchName));

	//��������
	StringCchCopy(m_pMatchOption->szMatchName, CountArray(m_pMatchOption->szMatchName),szMatchName);

	//��֤����
	if (m_pMatchOption->szMatchName[0]==0)
	{
		AfxMessageBox(TEXT("�������Ʋ���Ϊ�գ����������룡"));

		//���ý���
		GetDlgItem(IDC_EDIT_MATCH_NAME)->SetFocus();

		return false;
	}

	//��������
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_TYPE);
	m_pMatchOption->cbMatchType=(BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());	

	//չʾ����
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SHOW_TYPE);
	m_pMatchOption->cbShowType = (BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());

	//ģ������
	pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MODULE_NAME);	
	m_pMatchOption->wKindID=pComboBox->GetItemData(pComboBox->GetCurSel());

	//ģ������
	TCHAR szModuleName[LEN_KIND];
	pComboBox->GetLBText(pComboBox->GetCurSel(),szModuleName);

	//��������
	StringCchCopy(m_pMatchOption->szModuleName, CountArray(m_pMatchOption->szModuleName),szModuleName);	

	return true;
}

//��������
VOID CDlgMatchWizardStep1::ActiveOptionItem(BYTE cbItemIndex)
{
	//�ж�����
	if (m_cbItemIndex==cbItemIndex) return;

	//��������
	m_cbItemIndex=cbItemIndex;

	//�ؼ�����
	CRect rcTabCtrl;
	GetDlgItem(IDC_MATCH_OPTION)->GetClientRect(&rcTabCtrl);

	//���ÿؼ�
	if (m_pDlgMatchItem[m_cbItemIndex]->m_hWnd!=NULL)
	{
		m_pDlgMatchItem[m_cbItemIndex]->SetWindowPos(NULL,5,30,rcTabCtrl.Width()-10,rcTabCtrl.Height()-30-5,SWP_NOZORDER|SWP_SHOWWINDOW);
	}

	//���ÿؼ�
	for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
	{
		if (i==m_cbItemIndex) continue;
		if (m_pDlgMatchItem[i]->m_hWnd!=NULL) m_pDlgMatchItem[i]->ShowWindow(SW_HIDE);
	}
}

//ѡ����
void CDlgMatchWizardStep1::OnCbnSelchangeComboMatchType()
{
	//��ȡ����
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_TYPE);
	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	{
		//��������
		BYTE cbMatchType=(BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());

		//�¼�֪ͨ
		for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
		{
			m_pDlgMatchItem[i]->OnEventMatchTypeChanged(cbMatchType);
		}
	}
}

void CDlgMatchWizardStep1::OnTcnSelchangeMatchOption(NMHDR *pNMHDR, LRESULT *pResult)
{
	//��ȡ�ؼ�
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_MATCH_OPTION);
	ActiveOptionItem(pTabCtrl->GetCurSel());	
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CDlgMatchWizardStep2::CDlgMatchWizardStep2() : CDlgMatchWizardItem(IDD_MATCH_WIZARD_2)
{
	//���ñ���
	m_cbItemIndex=0;
	ZeroMemory(&m_MatchRoundItem,sizeof(m_MatchRoundItem));
	ZeroMemory(m_pPromoteModeItem,sizeof(m_pPromoteModeItem));
}

//��������
CDlgMatchWizardStep2::~CDlgMatchWizardStep2()
{
}

//�ؼ���
VOID CDlgMatchWizardStep2::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�����б�
	DDX_Control(pDX,IDC_LIST_ROUND,m_RoundListControl);
}

//��ʼ������
BOOL CDlgMatchWizardStep2::OnInitDialog()
{
	__super::OnInitDialog();

	for (WORD i=0; i<m_pMatchOption->wRoundCount; i++)
	{
		m_RoundInfoBuffer.InsertMatchRoundItem(m_pMatchOption->MatchRoundItem[i]);
	}
	
	for(INT_PTR i=0;i<m_RoundInfoBuffer.m_MatchRoundItemArray.GetCount();i++) 
	{
		m_RoundListControl.InsertMatchRound(m_RoundInfoBuffer.m_MatchRoundItemArray[i]);
	}

	//��������	
	LPCTSTR pszPromoteMode[]={TEXT("��������"),TEXT("���ֻ���"),TEXT("��������")};
	BYTE cbPromoteMode[]={PROMOTE_MODE_OUT_GAME,PROMOTE_MODE_FIXED_GAME,PROMOTE_MODE_FIXED_TABLE};

	//��ȡ�ؼ�
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_PROMOTE_MODE);
	for (INT i=0; i<CountArray(pszPromoteMode); i++)
	{
		pComboBox->SetItemData(pComboBox->AddString(pszPromoteMode[i]),cbPromoteMode[i]);
	}

	//��ȡ�ؼ�
	CWnd * pPromoteCtrl=GetDlgItem(IDC_PROMOTE_RULE);

	//�������
	m_pPromoteModeItem[PROMOTE_MODE_OUT_GAME]=&m_PromoteOutGame;
	m_pPromoteModeItem[PROMOTE_MODE_FIXED_GAME]=&m_PromoteFixedGame;
	m_pPromoteModeItem[PROMOTE_MODE_FIXED_TABLE]=&m_PromoteFixedTable;	

	//��������
	CRect rcCreate(0,0,0,0);

	//��������
	for (int i=0; i<CountArray(m_pPromoteModeItem); i++)
	{
		//ָ���ж�
		if (m_pPromoteModeItem[i]==NULL) continue;

		//���ÿؼ�		
		m_pPromoteModeItem[i]->SetMatchRoundItem(&m_MatchRoundItem);

		//��������		
		m_pPromoteModeItem[i]->CreatePromoteItem(rcCreate,pPromoteCtrl);
	}

	return TRUE;
}

//����ؼ�
VOID CDlgMatchWizardStep2::CleanControl()
{
	//���ÿؼ�	
	SetDlgItemText(IDC_ROUNDID,TEXT(""));
	SetDlgItemText(IDC_START_MIN_COUNT,TEXT(""));
	SetDlgItemText(IDC_START_MAX_COUNT,TEXT(""));
	SetDlgItemText(IDC_PROMOTE_COUNT,TEXT(""));

	//���ÿؼ�	
	SetDlgItemText(IDC_PLAY_COUNT,TEXT(""));
	SetDlgItemText(IDC_EDIT_MAX_BASE_SCORE,TEXT(""));
	SetDlgItemText(IDC_EDIT_INIT_BASE_SCORE,TEXT(""));

	//����ģʽ
	SetDlgItemText(IDC_EDIT_START_INNING,TEXT(""));
	SetDlgItemText(IDC_EDIT_INCREASE_RATIO1,TEXT(""));
	SetDlgItemText(IDC_EDIT_INTERVAL_INNING,TEXT(""));
	
	//����ģʽ
	SetDlgItemText(IDC_EDIT_INCREASE_RATIO2,TEXT(""));
	SetDlgItemText(IDC_EDIT_INTERVAL_SECOND,TEXT(""));
	
	//��������
	SetDlgItemText(IDC_EDIT_INIT_SCORE,TEXT(""));
	SetDlgItemText(IDC_EDIT_RESERVE_RATIO,TEXT(""));

	//���ÿؼ�
	((CButton *)GetDlgItem(IDC_RADIO_INCREASE_MODE1))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_INCREASE_MODE2))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_RESET_SCORE))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_RESERVE_SCORE))->SetCheck(FALSE);
}

//��ʾ����
VOID CDlgMatchWizardStep2::ShowPromoteItem(BYTE cbItemIndex)
{
	//�ж�����
	if (cbItemIndex>=CountArray(m_pPromoteModeItem)) return;

	//��������
	m_cbItemIndex=cbItemIndex;

	//�ؼ�����
	CRect rcTabCtrl;
	GetDlgItem(IDC_PROMOTE_RULE)->GetClientRect(&rcTabCtrl);

	//���ÿؼ�
	if (m_pPromoteModeItem[m_cbItemIndex]->m_hWnd!=NULL)
	{
		m_pPromoteModeItem[m_cbItemIndex]->RefreshControl();
		m_pPromoteModeItem[m_cbItemIndex]->SetWindowPos(NULL,6,16,rcTabCtrl.Width()-12,rcTabCtrl.Height()-16-4,SWP_NOZORDER|SWP_SHOWWINDOW);
	}

	//���ÿؼ�
	for (int i=0; i<CountArray(m_pPromoteModeItem); i++)
	{
		if (i==m_cbItemIndex) continue;
		if (m_pPromoteModeItem[i]->m_hWnd!=NULL) m_pPromoteModeItem[i]->ShowWindow(SW_HIDE);
	}
}

//��������
VOID CDlgMatchWizardStep2::SetMatchRoundItem(tagMatchRoundItem * pMatchRoundItem)
{
	//����У��
	ASSERT(pMatchRoundItem!=NULL);
	if (pMatchRoundItem==NULL) return;	

	//���ñ�ʶ
	SetDlgItemInt(IDC_ROUNDID,(UINT)pMatchRoundItem->wRoundID);	

	//��ȡ�ؼ�
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_PROMOTE_MODE);
	for (INT_PTR i=0; i<pComboBox->GetCount(); i++)
	{
		if (pComboBox->GetItemData(i)==pMatchRoundItem->cbPromoteMode)
		{
			pComboBox->SetCurSel(i);
			break;
		}
	}

	//��ʼ����
	SetDlgItemInt(IDC_START_MIN_COUNT,(UINT)pMatchRoundItem->wStartMinCount);	
	SetDlgItemInt(IDC_START_MAX_COUNT,(UINT)pMatchRoundItem->wStartMaxCount);

	//��������
	SetDlgItemInt(IDC_PROMOTE_COUNT,(UINT)pMatchRoundItem->wPromoteCount);

	//��Ϸ����
	SetDlgItemInt(IDC_PLAY_COUNT,(UINT)pMatchRoundItem->wPlayCount);

	//��Ϸ�׷�		
	SetDlgItemInt(IDC_EDIT_MAX_BASE_SCORE,(UINT)pMatchRoundItem->lMaxBaseScore);
	SetDlgItemInt(IDC_EDIT_INIT_BASE_SCORE,(UINT)pMatchRoundItem->lInitBaseScore);

	//����ģʽ1
	if (pMatchRoundItem->cbIncreaseMode==INCREASE_MODE_INNING)
	{
		SetDlgItemInt(IDC_EDIT_START_INNING,(UINT)pMatchRoundItem->wStartInning);
		SetDlgItemInt(IDC_EDIT_INCREASE_RATIO1,(UINT)pMatchRoundItem->wIncreaseRatio);	
		SetDlgItemInt(IDC_EDIT_INTERVAL_INNING,(UINT)pMatchRoundItem->wIntervalInning);	
		CheckRadioButton(IDC_RADIO_INCREASE_MODE1,IDC_RADIO_INCREASE_MODE2,IDC_RADIO_INCREASE_MODE1);
	}

	//����ģʽ1
	if (pMatchRoundItem->cbIncreaseMode==INCREASE_MODE_TIME)
	{
		SetDlgItemInt(IDC_EDIT_INCREASE_RATIO2,(UINT)pMatchRoundItem->wIncreaseRatio);	
		SetDlgItemInt(IDC_EDIT_INTERVAL_SECOND,(UINT)pMatchRoundItem->wIntervalSecond);
		CheckRadioButton(IDC_RADIO_INCREASE_MODE1,IDC_RADIO_INCREASE_MODE2,IDC_RADIO_INCREASE_MODE2);
	}

	//��������
	if (pMatchRoundItem->cbMScoreSetType==MSCORE_SET_TYPE_RESET)
	{
		SetDlgItemInt(IDC_EDIT_INIT_SCORE,(UINT)pMatchRoundItem->lInitMatchScore);
		CheckRadioButton(IDC_RADIO_RESET_SCORE,IDC_RADIO_RESERVE_SCORE,IDC_RADIO_RESET_SCORE);
	}

	if (pMatchRoundItem->cbMScoreSetType==MSCORE_SET_TYPE_RESERVE)
	{
		SetDlgItemInt(IDC_EDIT_RESERVE_RATIO,(UINT)pMatchRoundItem->wReserveRatio);
		CheckRadioButton(IDC_RADIO_RESET_SCORE,IDC_RADIO_RESERVE_SCORE,IDC_RADIO_RESERVE_SCORE);
	}
}

//��ȡ����
bool CDlgMatchWizardStep2::GetMatchRoundItem(tagMatchRoundItem & MatchRoundItem)
{
	//���ñ���
	ZeroMemory(&MatchRoundItem,sizeof(MatchRoundItem));

	//��ȡ��ʶ
	MatchRoundItem.wRoundID=GetDlgItemInt(IDC_ROUNDID);

	//��ȡ�ؼ�
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_PROMOTE_MODE);
	if (pComboBox->GetCurSel()==-1)
	{
		AfxMessageBox(TEXT("��ѡ�������ʽ��"));
		return false;
	}

	//������ʽ
	MatchRoundItem.cbPromoteMode=pComboBox->GetItemData(pComboBox->GetCurSel());

	//��������
	MatchRoundItem.wStartMinCount=GetDlgItemInt(IDC_START_MIN_COUNT);
	MatchRoundItem.wStartMaxCount=GetDlgItemInt(IDC_START_MAX_COUNT);

	//У������
	if (MatchRoundItem.wStartMinCount==0)
	{
		AfxMessageBox(TEXT("��С��ʼ���������������������ã�"));
		return false;
	}

	//У������
	if (MatchRoundItem.wStartMaxCount==0)
	{
		AfxMessageBox(TEXT("���ʼ���������������������ã�"));
		return false;
	}

	//У������
	if (MatchRoundItem.wStartMaxCount>MAX_MATCH_USER)
	{
		AfxMessageBox(TEXT("��ʼ��������ϵͳ���֧�����������������ã�"));
		return false;
	}

	//У������
	if (MatchRoundItem.wStartMinCount>MatchRoundItem.wStartMaxCount)
	{
		AfxMessageBox(TEXT("��С�������ܳ���������������������ã�"));
		return false;
	}	

	//��ʼ����
	MatchRoundItem.wStartCount=MatchRoundItem.wStartMaxCount;

	//��������
	MatchRoundItem.wPromoteCount=GetDlgItemInt(IDC_PROMOTE_COUNT);
	if (MatchRoundItem.wPromoteCount==0)
	{
		AfxMessageBox(TEXT("�������������������������ã�"));
		return false;
	}

	//��Ϸ����
	MatchRoundItem.wPlayCount=GetDlgItemInt(IDC_PLAY_COUNT);
	if (MatchRoundItem.wPlayCount==0)
	{
		AfxMessageBox(TEXT("��Ϸ���������������������ã�"));
		return false;
	}

	//��������
	if (m_pPromoteModeItem[m_cbItemIndex]->SaveInputInfo()==false)
	{
		return false;
	}

	//��Ϸ�׷�		
	MatchRoundItem.lMaxBaseScore=GetDlgItemInt(IDC_EDIT_MAX_BASE_SCORE);
	MatchRoundItem.lInitBaseScore=GetDlgItemInt(IDC_EDIT_INIT_BASE_SCORE);

	//����ģʽ1
	if (IsDlgButtonChecked(IDC_RADIO_INCREASE_MODE1))
	{
		MatchRoundItem.cbIncreaseMode=INCREASE_MODE_INNING;
		MatchRoundItem.wStartInning=GetDlgItemInt(IDC_EDIT_START_INNING);
		MatchRoundItem.wIncreaseRatio=GetDlgItemInt(IDC_EDIT_INCREASE_RATIO1);
		MatchRoundItem.wIntervalInning=GetDlgItemInt(IDC_EDIT_INTERVAL_INNING);	

		//�������
		if (MatchRoundItem.wIntervalInning==0)
		{
			AfxMessageBox(TEXT("�����������Ϊ0�����������ã�"));
			return false;
		}
	}

	//����ģʽ2
	if (IsDlgButtonChecked(IDC_RADIO_INCREASE_MODE2))
	{
		MatchRoundItem.cbIncreaseMode=INCREASE_MODE_TIME;
		MatchRoundItem.wIncreaseRatio=GetDlgItemInt(IDC_EDIT_INCREASE_RATIO2);
		MatchRoundItem.wIntervalSecond=GetDlgItemInt(IDC_EDIT_INTERVAL_SECOND);	

		//���ʱ��
		if (MatchRoundItem.wIntervalSecond==0)
		{
			AfxMessageBox(TEXT("���ʱ�䲻��Ϊ0�����������ã�"));
			return false;
		}
	}

	//У�����
	if (MatchRoundItem.lInitBaseScore==0)
	{
		AfxMessageBox(TEXT("��Ϸ�׷ֲ���Ϊ0�����������ã�"));
		return false;
	}

	//У�����
	if (MatchRoundItem.lMaxBaseScore<MatchRoundItem.lInitBaseScore)
	{
		AfxMessageBox(TEXT("��Ϸ�ⶥ�׷ֲ���С�ڳ�ʼ�׷֣����������ã�"));
		return false;
	}

	//��������
	if (IsDlgButtonChecked(IDC_RADIO_RESET_SCORE))
	{
		MatchRoundItem.cbMScoreSetType=MSCORE_SET_TYPE_RESET;
		MatchRoundItem.lInitMatchScore=GetDlgItemInt(IDC_EDIT_INIT_SCORE);
	}
	if (IsDlgButtonChecked(IDC_RADIO_RESERVE_SCORE))
	{
		MatchRoundItem.cbMScoreSetType=MSCORE_SET_TYPE_RESERVE;
		MatchRoundItem.wReserveRatio=GetDlgItemInt(IDC_EDIT_RESERVE_RATIO);		
	}

	//��������
	CopyMemory(MatchRoundItem.cbPromoteRule,m_MatchRoundItem.cbPromoteRule,sizeof(MatchRoundItem.cbPromoteRule));

	return true;
}

//��������
bool CDlgMatchWizardStep2::SaveInputInfo()
{
	//��������
	m_pMatchOption->wRoundCount=__min(CountArray(m_pMatchOption->MatchRoundItem),m_RoundInfoBuffer.GetMatchRoundCount());

	for (WORD i=0; i<m_pMatchOption->wRoundCount; i++)
	{
		CopyMemory(&m_pMatchOption->MatchRoundItem[i],m_RoundInfoBuffer.m_MatchRoundItemArray[i],sizeof(m_pMatchOption->MatchRoundItem[i]));
		if (m_pMatchOption->MatchRoundItem[i].wRoundID!=i+1)
		{
			AfxMessageBox(TEXT("�밴�ִ�˳�������ִα�ʶ��"));

			return false;
		}
	}	

	return true;
}

//�����¼�
VOID CDlgMatchWizardStep2::OnMatchItemActive()
{
	//У������
	if (m_pMatchOption->cbMatchType == MATCH_TYPE_TOURNAMENT)
	{
		EnableWindow(FALSE);		
	}
	else
	{
		EnableWindow(TRUE);		
	}
}

//�����ؼ�
VOID CDlgMatchWizardStep2::RectifyControl(INT nWidth, INT nHeight)
{
}

//��ӽ���
VOID CDlgMatchWizardStep2::OnBnClickedAddRound()
{
	//��������	
	tagMatchRoundItem MatchRoundItem;

	//��ȡ����
	if (GetMatchRoundItem(MatchRoundItem)==false) return;

	//�����ж�
	if (m_RoundInfoBuffer.SearchMatchRound(MatchRoundItem.wRoundID)!=NULL)
	{
		AfxMessageBox(TEXT("��Ǹ�����ִα�ʶ�Ѵ��ڣ�"));

		return;
	}		
	
	//�����ִ�
	tagMatchRoundItem * pMatchRoundItem=m_RoundInfoBuffer.InsertMatchRoundItem(MatchRoundItem);
	if (pMatchRoundItem!=NULL)
	{
		m_RoundListControl.InsertMatchRound(pMatchRoundItem);
	}

	//����ؼ�
	CleanControl();
}

//�޸��ִ�
VOID CDlgMatchWizardStep2::OnBnClickedModifyRound()
{
	//��������	
	tagMatchRoundItem MatchRoundItem;

	//��ȡ����
	if (GetMatchRoundItem(MatchRoundItem)==false) return;

	//�����ж�
	if (m_RoundInfoBuffer.SearchMatchRound(MatchRoundItem.wRoundID)==NULL)
	{
		AfxMessageBox(TEXT("��Ǹ�����ִα�ʶ�����ڣ�"));

		return;
	}		
	
	//�޸��ִ�
	tagMatchRoundItem * pMatchRoundItem=m_RoundInfoBuffer.SearchMatchRound(MatchRoundItem.wRoundID);
	if (pMatchRoundItem!=NULL)
	{
		CopyMemory(pMatchRoundItem,&MatchRoundItem,sizeof(MatchRoundItem));
		m_RoundListControl.UpdateMatchRound(pMatchRoundItem);
	}
}

//�޸Ľ���
VOID CDlgMatchWizardStep2::OnBnClickedDeleteRound()
{
	//��������	
	tagMatchRoundItem MatchRoundItem;

	//��ȡ����
	if (GetMatchRoundItem(MatchRoundItem)==false) return;

	//�����ִ�
	tagMatchRoundItem * pMatchRoundItem=m_RoundInfoBuffer.SearchMatchRound(MatchRoundItem.wRoundID);
	if (pMatchRoundItem==NULL)
	{
		AfxMessageBox(TEXT("��Ǹ�����ִα�ʶ�����ڣ�"));
		return;
	}

	//���¿ؼ�
	m_RoundListControl.DeleteMatchRound(pMatchRoundItem);
	m_RoundInfoBuffer.DeleteMatchRoundItem(pMatchRoundItem->wRoundID);

	//����ؼ�
	CleanControl();
}

//ѡ����
void CDlgMatchWizardStep2::OnCbnSelchangeComboPromoteMode()
{
	//��ȡ����
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_PROMOTE_MODE);
	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	{
		//������ʽ
		BYTE cbPromoteMode=(BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());

		//��ʾ����
		ShowPromoteItem(cbPromoteMode);
	}
}

//������
VOID CDlgMatchWizardStep2::OnLvnItemchangedListRound(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//��������
	if (pNMLV->iItem!=LB_ERR)
	{
		//��������
		tagMatchRoundItem * pMatchRoundItem=(tagMatchRoundItem *)m_RoundListControl.GetItemData(pNMLV->iItem);
		if (pMatchRoundItem!=NULL) 
		{
			//��������
			SetMatchRoundItem(pMatchRoundItem);

			//��������
			CopyMemory(&m_MatchRoundItem,pMatchRoundItem,sizeof(m_MatchRoundItem));
			ShowPromoteItem(pMatchRoundItem->cbPromoteMode);
		}
	}

	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchWizardStep3::CDlgMatchWizardStep3() : CDlgMatchWizardItem(IDD_MATCH_WIZARD_3)
{
	//���ñ���
	m_pDlgMatchItem=NULL;
}

//��������
CDlgMatchWizardStep3::~CDlgMatchWizardStep3()
{
}

//�ؼ���
VOID CDlgMatchWizardStep3::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//��ʼ������
BOOL CDlgMatchWizardStep3::OnInitDialog()
{
	__super::OnInitDialog();	
	
	return FALSE;
}

//��������
bool CDlgMatchWizardStep3::SaveInputInfo()
{
	//��������
	if (m_pDlgMatchItem->SaveInputInfo() == false) return false;

	return true;
}

//��������
VOID CDlgMatchWizardStep3::OnMatchItemActive()
{
	//���ش���
	if (m_pDlgMatchItem!=NULL) m_pDlgMatchItem->ShowWindow(SW_HIDE);

	//��ȡλ��
	CRect rcItemRect;
	GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//ѡ������
	switch (m_pMatchOption->cbMatchType)
	{
	case MATCH_TYPE_LOCKTIME:	//��ʱ��
	{
		m_pDlgMatchItem = &m_DlgMatchLockTime;
		break;
	}
	case MATCH_TYPE_IMMEDIATE:	//��ʱ��
	{
		m_pDlgMatchItem = &m_DlgMatchImmediate;
		break;
	}
	case MATCH_TYPE_TOURNAMENT:	//������
	{
		m_pDlgMatchItem = &m_DlgMatchTournament;
		break;
	}
	}

	//���ñ���
	m_pDlgMatchItem->SetMatchOption(m_pMatchOption);

	//��ʾ����
	rcItemRect.DeflateRect(5,5,5,10);
	m_pDlgMatchItem->ShowMatchItem(rcItemRect,this);
}

//�����ؼ�
VOID CDlgMatchWizardStep3::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchWizard::CDlgMatchWizard() : CDialog(IDD_MATCH_WIZARD)
{
	//���ñ���
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_MatchWizardStep1;
	m_pWizardItem[1]=&m_MatchWizardStep2;
	m_pWizardItem[2]=&m_MatchWizardStep3;

	//������Ϣ	
	ZeroMemory(&m_MatchOption,sizeof(m_MatchOption));

	return;
}

//��������
CDlgMatchWizard::~CDlgMatchWizard()
{
}

//��ʼ������
BOOL CDlgMatchWizard::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	CMatchInfoManager MatchInfoManager;
	CModuleInfoManager ModuleInfoManager;

	//��ȡ������
	TCHAR szServiceMachine[LEN_MACHINE_ID];
	CWHService::GetMachineID(szServiceMachine);

	//������Ϣ	
	MatchInfoManager.LoadMatchOption(m_MatchOption.wKindID,szServiceMachine,m_MatchInfoBuffer);

	//ģ����Ϣ
	ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer);

	//������
	ActiveWizardItem(0);

	return FALSE;
}

//ȷ������
VOID CDlgMatchWizard::OnOK()
{
	if ((m_wActiveIndex+1)<CountArray(m_pWizardItem))
	{
		OnBnClickedNext();
	}
	else
	{
		OnBnClickedFinish();
	}

	return;
}


//��ȡ����
VOID CDlgMatchWizard::GetMatchOption(tagMatchOption & MatchOption)
{
	//���ñ���
	MatchOption=m_MatchOption;
}

//��������
VOID CDlgMatchWizard::SetMatchOption(tagMatchOption & MatchOption)
{
	//���ñ���
	m_MatchOption=MatchOption;
}

//������
bool CDlgMatchWizard::ActiveWizardItem(WORD wIndex)
{
	//�ж�״̬
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//�������
	CDlgMatchWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//���ñ���
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//��������
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	}

	//��ȡλ��
	CRect rcItemRect;
	GetDlgItem(IDC_MATCH_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//��������
	m_pWizardItem[wIndex]->m_pDlgMatchWizard=this;
	m_pWizardItem[wIndex]->m_pMatchOption=&m_MatchOption;
	m_pWizardItem[wIndex]->m_pMatchInfoBuffer=&m_MatchInfoBuffer;
	m_pWizardItem[wIndex]->m_pModuleInfoBuffer=&m_ModuleInfoBuffer;	

	//��������
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//��������
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//�������
	CButton * pButtonLast=(CButton * )GetDlgItem(IDC_LAST);
	CButton * pButtonNext=(CButton * )GetDlgItem(IDC_NEXT);
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);

	//���Ƚ���
	pButtonLast->EnableWindow((m_wActiveIndex>0)?TRUE:FALSE);
	pButtonNext->EnableWindow(((m_wActiveIndex+1)<CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonFinish->EnableWindow((m_wActiveIndex==(CountArray(m_pWizardItem)-1))?TRUE:FALSE);	

	//�������
	TCHAR szTitle[128]=TEXT("");
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("���������� --- [ ���� %d ]"),m_wActiveIndex+1);
	
	//���ñ���
	SetWindowText(szTitle);

	return true;
}

//��һ��
VOID CDlgMatchWizard::OnBnClickedLast()
{
	//Ч�����
	ASSERT(m_wActiveIndex>0);
	ASSERT(m_wActiveIndex<CountArray(m_pWizardItem));

	//�л�ҳ��
	ActiveWizardItem(m_wActiveIndex-1);

	return;
}

//��һ��
VOID CDlgMatchWizard::OnBnClickedNext()
{
	//Ч�����
	ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//����ҳ��
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//��ɰ�ť
VOID CDlgMatchWizard::OnBnClickedFinish()
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
	tagCreateMatchInfo CreateMatchInfo;
	ZeroMemory(&CreateMatchInfo,sizeof(CreateMatchInfo));

	//������Ϣ
	CreateMatchInfo.wKindID=m_MatchOption.wKindID;	
	CreateMatchInfo.dwMatchID=m_MatchOption.dwMatchID;
	CreateMatchInfo.dwClubID=m_MatchOption.dwClubID;
	CreateMatchInfo.cbShowType=m_MatchOption.cbShowType;
	CreateMatchInfo.cbMatchType=m_MatchOption.cbMatchType;	

	//������Ϣ
	CreateMatchInfo.lFeeAmount = m_MatchOption.lFeeAmount;
	CreateMatchInfo.wFeeMoneyID=m_MatchOption.wFeeMoneyID;
	CreateMatchInfo.wUseTicketCount=m_MatchOption.wUseTicketCount;
	CreateMatchInfo.wBuyTicketCount=m_MatchOption.wBuyTicketCount;
	CreateMatchInfo.wBuyTicketOption=m_MatchOption.wBuyTicketOption;
	CreateMatchInfo.lBuyTicketAmount=m_MatchOption.lBuyTicketAmount;
	CreateMatchInfo.wBuyTicketMoneyID=m_MatchOption.wBuyTicketMoneyID;	
	CreateMatchInfo.cbSignupMode=m_MatchOption.cbSignupMode;	
	CreateMatchInfo.cbJoinCondition=m_MatchOption.cbJoinCondition;	
	CreateMatchInfo.cbMemberOrder=m_MatchOption.cbMemberOrder;	
	CreateMatchInfo.lExperience=m_MatchOption.lExperience;

	//��������
	CopyMemory(CreateMatchInfo.cbMatchDistribute, m_MatchOption.cbMatchDistribute, sizeof(CreateMatchInfo.cbMatchDistribute));

	//������Ϣ
	StringCchCopy(CreateMatchInfo.szModuleName, CountArray(CreateMatchInfo.szModuleName),m_MatchOption.szModuleName);
	StringCchCopy(CreateMatchInfo.szMatchName, CountArray(CreateMatchInfo.szMatchName), m_MatchOption.szMatchName);

	//��������
	CreateMatchInfo.wRewardCount=m_MatchOption.wRewardCount;
	CopyMemory(CreateMatchInfo.MatchRewardItem,m_MatchOption.MatchRewardItem,sizeof(CreateMatchInfo.MatchRewardItem));

	//�����ִ�
	CreateMatchInfo.wRoundCount=m_MatchOption.wRoundCount;
	CopyMemory(CreateMatchInfo.MatchRoundItem,m_MatchOption.MatchRoundItem,sizeof(CreateMatchInfo.MatchRoundItem));

	//��չ����
	CWHService::GetMachineID(CreateMatchInfo.szServiceMachine);
	CopyMemory(CreateMatchInfo.cbMatchRule,m_MatchOption.cbMatchRule,sizeof(CreateMatchInfo.cbMatchRule));

	//��������
	tagMatchOption GameMatchResult;
	CMatchInfoManager MatchInfoManager;
	
	//�������
	if (CreateMatchInfo.dwMatchID==0)
	{
		if (MatchInfoManager.InsertMatchOption(&CreateMatchInfo,GameMatchResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ���������ɹ�"),TraceLevel_Normal);
	}

	//�޸ı���
	if (CreateMatchInfo.dwMatchID!=0)
	{
		//�޸ķ���
		if (MatchInfoManager.ModifyMatchOption(&CreateMatchInfo,GameMatchResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ�������óɹ�"),TraceLevel_Normal);
	}

	//���ñ�ʶ
	m_MatchOption.dwMatchID=GameMatchResult.dwMatchID;

	//��������
	m_MatchOption.wRewardCount=GameMatchResult.wRewardCount;
	CopyMemory(m_MatchOption.MatchRewardItem,GameMatchResult.MatchRewardItem,sizeof(m_MatchOption.MatchRewardItem));

	//�رմ���
	EndDialog(IDOK);

	return;
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CDlgPromoteModeItem::CDlgPromoteModeItem(UINT nIDTemplate): CDialog(nIDTemplate)
{
	//���ñ���		
	m_pMatchRoundItem=NULL;	
}

//��������
CDlgPromoteModeItem::~CDlgPromoteModeItem()
{
}

//��������
VOID CDlgPromoteModeItem::SetMatchRoundItem(tagMatchRoundItem * pMatchRoundItem)
{
	m_pMatchRoundItem=pMatchRoundItem;
}

//������
bool CDlgPromoteModeItem::ShowPromoteItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) CreatePromoteItem(rcRect,pParentWnd);

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//��������
VOID CDlgPromoteModeItem::CreatePromoteItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//��������
	BOOL bSuccess=Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE);

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));
}

//λ����Ϣ
VOID CDlgPromoteModeItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CDlgPromoteOutGame::CDlgPromoteOutGame() : CDlgPromoteModeItem(IDD_PROMOTE_OUT_GAME)
{
}

//��������
CDlgPromoteOutGame::~CDlgPromoteOutGame()
{
}

//��ʼ������
BOOL CDlgPromoteOutGame::OnInitDialog()
{
	__super::OnInitDialog();

	return FALSE;
}

//��������
bool CDlgPromoteOutGame::SaveInputInfo()
{
	//��ȡ����
	tagPromoteOutGame * pPromoteOutGame=(tagPromoteOutGame * )m_pMatchRoundItem->cbPromoteRule;
	ZeroMemory(pPromoteOutGame,sizeof(tagPromoteOutGame));

	//���ÿؼ�	
	pPromoteOutGame->wEliminateMulti=GetDlgItemInt(IDC_EDIT_ELIMINATE_MULTI);	
	pPromoteOutGame->wDistributeCount=GetDlgItemInt(IDC_EDIT_DISTRIBUTE_COUNT);	

	return true;
}

//ˢ�¿ؼ�
bool CDlgPromoteOutGame::RefreshControl()
{
	//��ȡ����
	tagPromoteOutGame * pPromoteOutGame=(tagPromoteOutGame * )m_pMatchRoundItem->cbPromoteRule;

	//���ÿؼ�	
	SetDlgItemInt(IDC_EDIT_ELIMINATE_MULTI,pPromoteOutGame->wEliminateMulti);
	SetDlgItemInt(IDC_EDIT_DISTRIBUTE_COUNT,pPromoteOutGame->wDistributeCount);

	return true;
}

//�����ؼ�
VOID CDlgPromoteOutGame::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CDlgPromoteFixedGame::CDlgPromoteFixedGame() : CDlgPromoteModeItem(IDD_PROMOTE_FIXED_GAME)
{
}

//��������
CDlgPromoteFixedGame::~CDlgPromoteFixedGame()
{
}

//��ʼ������
BOOL CDlgPromoteFixedGame::OnInitDialog()
{
	__super::OnInitDialog();

	return FALSE;
}

//��������
bool CDlgPromoteFixedGame::SaveInputInfo()
{
	//��ȡ����
	tagPromoteFixedGame * pPromoteFixedGame=(tagPromoteFixedGame * )m_pMatchRoundItem->cbPromoteRule;

	//���ÿؼ�
	pPromoteFixedGame->wSwitchCount=GetDlgItemInt(IDC_EDIT_SWITCH_COUNT);

	return true;
}

//ˢ�¿ؼ�
bool CDlgPromoteFixedGame::RefreshControl()
{
	//��ȡ����
	tagPromoteFixedGame * pPromoteFixedGame=(tagPromoteFixedGame * )m_pMatchRoundItem->cbPromoteRule;

	//���ÿؼ�
	SetDlgItemInt(IDC_EDIT_SWITCH_COUNT,pPromoteFixedGame->wSwitchCount);

	return true;
}

//�����ؼ�
VOID CDlgPromoteFixedGame::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CDlgPromoteFixedTable::CDlgPromoteFixedTable() : CDlgPromoteModeItem(IDD_PROMOTE_FIXED_TABLE)
{

}

//��������
CDlgPromoteFixedTable::~CDlgPromoteFixedTable()
{
}

//��ʼ������
BOOL CDlgPromoteFixedTable::OnInitDialog()
{
	__super::OnInitDialog();

	return FALSE;
}

//��������
bool CDlgPromoteFixedTable::SaveInputInfo()
{
	return true;
}

//ˢ�¿ؼ�
bool CDlgPromoteFixedTable::RefreshControl()
{
	return true;
}

//�����ؼ�
VOID CDlgPromoteFixedTable::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
