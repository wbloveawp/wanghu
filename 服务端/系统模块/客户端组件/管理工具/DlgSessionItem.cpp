#include "StdAfx.h"
#include "Resource.h"
#include "DlgSessionItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//��������
#define MIN_INOUTINTERVAL			30*60								//��С���
#define MAX_INOUTINTERVAL			12*3600								//�����

//////////////////////////////////////////////////////////////////////////
//��Ϣӳ��

BEGIN_MESSAGE_MAP(CDlgSessionItem, CDialog)
	ON_WM_SIZE()	
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgSessionOption,CDlgSessionItem)

	//ϵͳ��Ϣ
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgSessionCommand,CDlgSessionItem)	
	ON_BN_CLICKED(IDC_BT_EXECUTE_CMD, OnBnClickedBtExecuteCmd)
	ON_BN_CLICKED(IDC_BT_RELOAD_CONFIG, OnBnClickedBtReloadConfig)
	
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgSessionItem::CDlgSessionItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	m_pServerListManager = NULL;
}

//��������
CDlgSessionItem::~CDlgSessionItem()
{
}

//�л�����
VOID CDlgSessionItem::SwitchViewItem(bool bActive)
{
	m_bActiveStatus = bActive;
}

//��������
VOID CDlgSessionItem::CreateViewItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	BOOL bSuccess=Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
}

//λ����Ϣ
VOID CDlgSessionItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgSessionOption::CDlgSessionOption() : CDlgSessionItem(IDD_SERVER_OPTION)
{
}

//��������
CDlgSessionOption::~CDlgSessionOption()
{
}

//��ʼ����
BOOL CDlgSessionOption::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;  
}

//��������
VOID CDlgSessionOption::DoDataExchange(CDataExchange* pDX)
{
	//�󶨿ؼ�
	DDX_Control(pDX, IDC_COMBO_GAME_LIST, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_SERVER_LIST, m_cbServerList);

	__super::DoDataExchange(pDX);
}

//������Ϣ
BOOL CDlgSessionOption::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD dwCommandID = HIWORD(wParam);
	UINT nCommandID = LOWORD(wParam);
	switch (dwCommandID)
	{
	case CBN_SELCHANGE:
	{
		//��Ϸ�б�
		if (m_cbGameList.m_hWnd == (HWND)lParam)
		{
			//��������
			CWordArray KindArray;
			for (INT i = 0; i < m_cbGameList.GetCount(); i++)
			{
				if (m_cbGameList.GetCheck(i) == static_cast<BOOL>(true))
				{
					tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(i);
					KindArray.Add(pItemData->dwDataPtr);
				}
			}

			//ɾ������				
			while (m_cbServerList.GetCount()) m_cbServerList.DeleteString(0);

			//���뷿��
			bool bInsert = false;
			POSITION Position = NULL;
			CGameServerItem* pGameServerItem = NULL;
			do
			{
				pGameServerItem = m_pServerListManager->EmunGameServerItem(Position);
				if (pGameServerItem != NULL)
				{
					//���ñ�ʶ
					bInsert = false;

					//ƥ������
					for (INT_PTR i = 0; i < KindArray.GetCount(); i++)
					{
						if (pGameServerItem->m_GameServer.wKindID == KindArray[i])
						{
							bInsert = true;
							break;
						}
					}

					if (bInsert == true)
					{
						tagItemData* pItemData = (tagItemData*)m_cbServerList.GetItemDataPtr(m_cbServerList.AddString(pGameServerItem->m_GameServer.szServerName));
						if (pItemData != NULL) pItemData->dwDataPtr = pGameServerItem->m_GameServer.wServerID;						
					}

				}
			} while (Position != NULL);
		}

		return TRUE;
	}
	}

	return __super::OnCommand(wParam, lParam);
}

//��ȡ�¼�
bool CDlgSessionOption::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	if (Command.wMainCmdID == MDM_GP_MANAGER_SERVICE && Command.wSubCmdID == SUB_GP_MANAGE_SUCCESS)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("�����ɹ���"));

		//���ÿؼ�
		EnableControls(TRUE);

		return true;
	}

	return false;
}

//�б����
VOID CDlgSessionOption::OnEventListFinish()
{
	//��������
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	//�������
	while (m_cbGameList.GetCount()) m_cbGameList.DeleteString(0);

	do
	{
		pGameKindItem=m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName));
		if (pItemData != NULL) pItemData->dwDataPtr = pGameKindItem->m_GameKind.wKindID;

	} while (Position!=NULL);
}

//�ؼ�ʹ��
VOID CDlgSessionOption::EnableControls(BOOL bEnabled)
{
	GetDlgItem(IDC_BT_MODIFY)->EnableWindow(bEnabled);
}

//�༭����
VOID CDlgSessionOption::OnBnClickedBtModify()
{
	//��������
	DWORD dwTableRuleAdd=0,dwTableRuleDel=0;
	DWORD dwServerRuleAdd=0,dwServerRuleDel=0;

	//��������
	ZeroMemory(&m_ModifyOption,sizeof(m_ModifyOption));

	//����ռ��
	if (((CButton*)GetDlgItem(IDC_ROBOT_SIMULATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ROBOT_SIMULATE))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_ROBOT_SIMULATE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ROBOT_SIMULATE))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_ROBOT_SIMULATE;
	}

	//�������
	if (((CButton*)GetDlgItem(IDC_ROBOT_ATTEND))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_ROBOT_ATTEND))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_ROBOT_ATTEND;
		if (((CButton*)GetDlgItem(IDC_ALLOW_ROBOT_ATTEND))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_ROBOT_ATTEND;
	}

	//�Թ���Ϸ
	if (((CButton*)GetDlgItem(IDC_LOOKON))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_LOOKON))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_FORBID_LOOKON;
		if (((CButton*)GetDlgItem(IDC_ALLOW_LOOKON))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_FORBID_LOOKON;		
	}

	//���͵ͱ�
	if (((CButton*)GetDlgItem(IDC_BASEENSURE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASEENSURE))->GetCheck()==TRUE)
			dwServerRuleDel|= SR_ALLOW_LARGESS_SUBSIDY;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASEENSURE))->GetCheck()==TRUE)
			dwServerRuleAdd|= SR_ALLOW_LARGESS_SUBSIDY;
	}

	//ǿ������
	if (((CButton*)GetDlgItem(IDC_FORCE_STANDUP))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FORCE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_FORBIT_FORCE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FORCE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_FORBIT_FORCE_STANDUP;		
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_OFFLINE_STANDUP))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_OFFLINE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_FORBIT_OFFLINE_STANDUP;
		if (((CButton*)GetDlgItem(IDC_ALLOW_OFFLINE_STANDUP))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_FORBIT_OFFLINE_STANDUP;		
	}

	//��¼�ɼ�
	if (((CButton*)GetDlgItem(IDC_RECORD_GAME_SCORE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_RECORD_GAME_SCORE))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_RECORD_GAME_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_RECORD_GAME_SCORE))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_RECORD_GAME_SCORE;		
	}

	//��ʱд��
	if (((CButton*)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IMMEDIATE_WRITE_SCORE))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_IMMEDIATE_WRITE_SCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IMMEDIATE_WRITE_SCORE))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_IMMEDIATE_WRITE_SCORE;		
	}

	//��̬����
	if (((CButton*)GetDlgItem(IDC_DYNAMIC_JOINGAME))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_DYNAMIC_JOINGAME))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_DYNAMIC_JOIN_GAME;
		if (((CButton*)GetDlgItem(IDC_ALLOW_DYNAMIC_JOINGAME))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_DYNAMIC_JOIN_GAME;		
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_VOICE_CHAT))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_VOICE_CHAT))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_ALLOW_VOICE_CHAT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_VOICE_CHAT))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_ALLOW_VOICE_CHAT;		
	}

	//��Ϸ¼��
	if (((CButton*)GetDlgItem(IDC_GAME_VIDEO))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_GAME_VIDEO))->GetCheck()==TRUE)
			dwServerRuleDel|=SR_RECORD_GAME_VIDEO;
		if (((CButton*)GetDlgItem(IDC_ALLOW_GAME_VIDEO))->GetCheck()==TRUE)
			dwServerRuleAdd|=SR_RECORD_GAME_VIDEO;		
	}

	//ʤ������
	if (((CButton*)GetDlgItem(IDC_WINRATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_WINRATE))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_WINRATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_WINRATE))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_WINRATE_SET;		
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_FLEERATE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_FLEERATE))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_FLEERATE_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_FLEERATE))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_FLEERATE_SET;		
	}

	//��ַ����
	if (((CButton*)GetDlgItem(IDC_IPSET))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_IPSET))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_IP_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_IPSET))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_IP_LIMIT;		
	}

	//�ɼ�����
	if (((CButton*)GetDlgItem(IDC_SCORELIMIT))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_SCORELIMIT))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_SCORE_LIMIT;
		if (((CButton*)GetDlgItem(IDC_ALLOW_SCORELIMIT))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_SCORE_LIMIT;		
	}

	//���ܻ�ӭ����
	if (((CButton*)GetDlgItem(IDC_BLACK))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BLACK))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_BLACKLIST_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BLACK))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_BLACKLIST_SET;		
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_PASSWORD))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_PASSWORD))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_PASSWORD_SET;
		if (((CButton*)GetDlgItem(IDC_ALLOW_PASSWORD))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_PASSWORD_SET;		
	}	

	//�׷���ʾ
	if (((CButton*)GetDlgItem(IDC_BASE_SCORE))->GetCheck()==TRUE)
	{
		if (((CButton*)GetDlgItem(IDC_LIMIT_BASE_SCORE))->GetCheck()==TRUE)
			dwTableRuleDel|=TR_FORBID_SHOW_BASESCORE;
		if (((CButton*)GetDlgItem(IDC_ALLOW_BASE_SCORE))->GetCheck()==TRUE)
			dwTableRuleAdd|=TR_FORBID_SHOW_BASESCORE;		
	}	
	

	//����У��
	if ((dwTableRuleAdd|dwTableRuleDel|dwServerRuleAdd|dwServerRuleDel)==0)
	{
		return;
	}

	//���ýṹ
	m_ModifyOption.dwTableRuleAdd=dwTableRuleAdd;
	m_ModifyOption.dwTableRuleDel=dwTableRuleDel;
	m_ModifyOption.dwServerRuleAdd=dwServerRuleAdd;
	m_ModifyOption.dwServerRuleDel=dwServerRuleDel;	

	//���嶨��
	CMD_GP_ModifyOption ModifyOption = {};

	//��������
	ModifyOption.ModifyOption = m_ModifyOption;

	//��ȡ����	
	for (INT i = 0; i < m_cbServerList.GetCount(); i++)
	{
		if (m_cbServerList.GetCheck(i) == static_cast<BOOL>(true))
		{
			//��ȡ����
			tagItemData* pItemData = (tagItemData*)m_cbServerList.GetItemData(i);
			ModifyOption.wServerIDList[ModifyOption.wServerCount++] = (WORD)pItemData->dwDataPtr;			
		}
	}

	//���ݴ�С
	WORD wDataSize = sizeof(ModifyOption)-sizeof(ModifyOption.wServerIDList)+ ModifyOption.wServerCount*sizeof(ModifyOption.wServerIDList[0]);

	//��������
	m_pVirtualLinkItem->SendVirtualData(MDM_GP_MANAGER_SERVICE, SUB_GP_MODIFY_OPTION, &ModifyOption, wDataSize);

	//���ÿؼ�
	EnableControls(FALSE);
}

//������Ϣ
VOID CDlgSessionOption::OnDestroy()
{
	__super::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgSessionCommand::CDlgSessionCommand() : CDlgSessionItem(IDD_CUSTOM_OPTION)
{
	//���ñ���
	ZeroMemory(&m_CommandCode,sizeof(m_CommandCode));
}

//��������
CDlgSessionCommand::~CDlgSessionCommand()
{
}

//��ʼ����
BOOL CDlgSessionCommand::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;
}

//��������
VOID CDlgSessionCommand::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//��ȡ�¼�
bool CDlgSessionCommand::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	if (Command.wMainCmdID==MDM_GP_MANAGER_SERVICE && Command.wSubCmdID== SUB_GP_MANAGE_SUCCESS)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("�����ɹ���"));

		//���¿ؼ�
		EnableControls(TRUE);

		return true;	
	}

	return false;
}

//�ؼ�ʹ��
VOID CDlgSessionCommand::EnableControls(BOOL bEnabled)
{
	//ʹ������
	GetDlgItem(IDC_BT_EXECUTE_CMD)->EnableWindow(bEnabled);
}

//�༭����
VOID CDlgSessionCommand::OnBnClickedBtExecuteCmd()
{	
	//��������
	DWORD dwCommandCode=0;
	DWORD dwCommandParam = 0;
	
	//Լս����
	if (((CButton*)GetDlgItem(IDC_BATTLE_SERVICE))->GetCheck()==TRUE)
	{
		//ֹͣ����
		if (((CButton*)GetDlgItem(IDC_STOP_BATTLE_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_BATTLE_SERVICE;
			dwCommandCode&=~CMD_START_BATTLE_SERVICE;
		}

		//��������
		if (((CButton*)GetDlgItem(IDC_START_BATTLE_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_BATTLE_SERVICE;
			dwCommandCode&=~CMD_STOP_BATTLE_SERVICE;
		}
	}

	//���翪��
	if (((CButton*)GetDlgItem(IDC_CLUB_SERVICE))->GetCheck()==TRUE)
	{
		//ֹͣ����
		if (((CButton*)GetDlgItem(IDC_STOP_CLUB_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_CLUB_SERVICE;
			dwCommandCode&=~CMD_START_CLUB_SERVICE;
		}

		//��������
		if (((CButton*)GetDlgItem(IDC_START_CLUB_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_CLUB_SERVICE;
			dwCommandCode&=~CMD_STOP_CLUB_SERVICE;
		}
	}

	//���ز�������
	if (((CButton*)GetDlgItem(IDC_RELOAD_CLUB_DATA))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_RELOAD_CLUB_DATA;
	}

	//��ӡ������Ϣ
	if (((CButton*)GetDlgItem(IDC_PRINT_CLUB_INFO))->GetCheck() == TRUE)
	{
		dwCommandCode |= CMD_PRINT_CLUB_INFO;
	}

	//��������
	if (((CButton*)GetDlgItem(IDC_MATCH_SERVICE))->GetCheck()==TRUE)
	{
		//ֹͣ����
		if (((CButton*)GetDlgItem(IDC_STOP_MATCH_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_STOP_MATCH_SERVICE;
			dwCommandCode&=~CMD_START_MATCH_SERVICE;
		}

		//��������
		if (((CButton*)GetDlgItem(IDC_START_MATCH_SERVICE))->GetCheck()==TRUE)
		{
			dwCommandCode|=CMD_START_MATCH_SERVICE;
			dwCommandCode&=~CMD_STOP_MATCH_SERVICE;
		}
	}

	//�����ж�
	if (dwCommandCode==0)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("������ѡ��һ�����񿪹أ�"));

		return;
	}
	
	//���ýṹ
	m_CommandCode.dwCommandCode=dwCommandCode;
	m_CommandCode.dwCommandParam=dwCommandParam;

	//��������
	m_pVirtualLinkItem->SendVirtualData(MDM_GP_MANAGER_SERVICE, SUB_GP_EXECUTE_COMMAND, &m_CommandCode, sizeof(m_CommandCode));

	//���¿ؼ�
	EnableControls(FALSE);
}

//��������
VOID CDlgSessionCommand::OnBnClickedBtReloadConfig()
{
	//��������
	DWORD dwCommandCode = 0;
	DWORD dwCommandParam = 0;

	//������Ϸ�б�
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_LIST))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_LIST;
	}

	//������Ϸ�ȼ�
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_LEVEL))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_LEVEL;
	}

	//������Ʒ��Ϣ
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_GOODS))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_GOODS;
	}

	//���ػ�������
	if (((CButton*)GetDlgItem(IDC_LOAD_ROBOT_PARAM))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_ROBOT_PARAM;
	}

	//���صͱ�����
	if (((CButton*)GetDlgItem(IDC_LOAD_SUBSIDY_PARAM))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_SUBSIDY_PARAM;
	}

	//���ص�ز���
	if (((CButton*)GetDlgItem(IDC_LOAD_CONTROL_PARAM))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_CONTROL_PARAM;
	}

	//����Լսѡ��
	if (((CButton*)GetDlgItem(IDC_LOAD_BATTLE_OPTION))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_BATTLE_OPTION;
	}

	//������Ϸ�汾
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_VERSION))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_VERSION;
	}

	//���ش����汾
	if (((CButton*)GetDlgItem(IDC_LOAD_PLAZA_VERSION))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_PLAZA_VERSION;
	}

	//�������дʻ�
	if (((CButton*)GetDlgItem(IDC_LOAD_FORBID_WORDS))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_FILTER_WORDS;
	}

	//����ת�˲���
	if (((CButton*)GetDlgItem(IDC_LOAD_TRANSFER_PARAM))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_TRANSFER_PARAM;
	}

	//����ת��ѡ��
	if (((CButton*)GetDlgItem(IDC_LOAD_TURNTABLE_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= OPTION_MASK_TURNTABLE;
	}

	//�������������
	if (((CButton*)GetDlgItem(IDC_LOAD_MARQUEE_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= OPTION_MASK_MARQUEE_NOTICE;
	}

	//���ؿ������
	if (((CButton*)GetDlgItem(IDC_LOAD_STOCK_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_GAME_STOCK;
	}

	//���ش�R����
	if (((CButton*)GetDlgItem(IDC_LOAD_SPLAYER_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_SPLAYER_CONTROL;
	}

	//������Ϸ��������
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_BASIC_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_BASIC_CONFIG;
	}

	//������ϷȨ������
	if (((CButton*)GetDlgItem(IDC_LOAD_GAME_WEIGHT_CONFIG))->GetCheck() == TRUE)
	{
		dwCommandCode = CMD_LOAD_CONFIG_DATA;
		dwCommandParam |= CONFIG_MASK_WEIGHT_CONFIG;
	}

	//�����ж�
	if (dwCommandCode == 0)
	{
		//��Ϣ��ʾ
		CInformation Information;
		Information.ShowMessageBox(TEXT("��ѡ����Ҫ���ص������"));
		return;
	}

	//���ýṹ
	m_CommandCode.dwCommandCode = dwCommandCode;
	m_CommandCode.dwCommandParam = dwCommandParam;

	//��������
	m_pVirtualLinkItem->SendVirtualData(MDM_GP_MANAGER_SERVICE, SUB_GP_EXECUTE_COMMAND, &m_CommandCode, sizeof(m_CommandCode));
}

//////////////////////////////////////////////////////////////////////////