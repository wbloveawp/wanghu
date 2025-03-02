#include "StdAfx.h"
#include "GlobalUnits.h"
#include "SimulateClient.h"
#include "SimulateLinkDlg.h"


//////////////////////////////////////////////////////////////////////////
//��Ϣӳ��

BEGIN_MESSAGE_MAP(CSimulateClient, CWnd)

	//ϵͳ��Ϣ
	ON_WM_TIMER()

	//��¼��Ϣ
	ON_MESSAGE(WM_LOGON_LINK_SHUT, OnLogonLinkShut)
	ON_MESSAGE(WM_LOGON_LINK_FAILURE, OnLogonLinkFailure)
	ON_MESSAGE(WM_LOGON_LOGON_FINISH, OnLogonLogonFinish)
	ON_MESSAGE(WM_LOGON_LOGON_FAILURE, OnLogonLogonFailure)

	//������Ϣ
	ON_MESSAGE(WM_MATCH_LINK_SHUT, OnMatchLinkShut)
	ON_MESSAGE(WM_MATCH_LINK_FAILURE, OnMatchLinkFailure)
	ON_MESSAGE(WM_MATCH_LOGON_FINISH, OnMatchLogonFinish)
	ON_MESSAGE(WM_MATCH_LOGON_FAILURE, OnMatchLogonFailure)

	//������Ϣ
	ON_MESSAGE(WM_CLUB_LINK_SHUT, OnClubLinkShut)
	ON_MESSAGE(WM_CLUB_LINK_FAILURE, OnClubLinkFailure)
	ON_MESSAGE(WM_CLUB_LOGON_FINISH, OnClubLogonFinish)
	ON_MESSAGE(WM_CLUB_LOGON_FAILURE, OnClubLogonFailure)

	//����㳡
	ON_MESSAGE(WM_CLUBPLAZA_LINK_SHUT, OnClubPlazaLinkShut)
	ON_MESSAGE(WM_CLUBPLAZA_LINK_FAILURE, OnClubPlazaLinkFailure)
	ON_MESSAGE(WM_CLUBPLAZA_LOGON_FINISH, OnClubPlazaLogonFinish)
	ON_MESSAGE(WM_CLUBPLAZA_LOGON_FAILURE, OnClubPlazaLogonFailure)

	//Լս����
	ON_MESSAGE(WM_BATTLE_LINK_SHUT, OnBattleLinkShut)
	ON_MESSAGE(WM_BATTLE_LINK_FAILURE, OnBattleLinkFailure)
	ON_MESSAGE(WM_BATTLE_LOGON_FINISH, OnBattleLogonFinish)
	ON_MESSAGE(WM_BATTLE_LOGON_FAILURE, OnBattleLogonFailure)

	//������Ϣ
	ON_MESSAGE(WM_SERVER_LINK_SHUT, OnServerLinkShut)
	ON_MESSAGE(WM_SERVER_LINK_FAILURE, OnServerLinkFailure)
	ON_MESSAGE(WM_SERVER_LOGON_FINISH, OnServerLogonFinish)
	ON_MESSAGE(WM_SERVER_LOGON_FAILURE, OnServerLogonFailure)
	ON_MESSAGE(WM_SERVER_MATCH_COMPLETED, OnServerMatchCompleted)	

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CSimulateClient::CSimulateClient():
	m_ModuleLogon(this),
	m_ModuleMatch(this),
	m_ModuleServer(this),
	m_ModuleBattle(this),
	m_ModuleClub(this),
	m_ModuleClubPlaza(this)
{
	//���ñ���
	m_szAccounts[0] = 0;
	m_szPassword[0] = 0;
	ZeroMemory(&m_GlobalUserData, sizeof(m_GlobalUserData));

	//���ýӿ�
	m_VirtualLinkManager.SetVirtualManagerSink(this);

	//�������
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleLogon);
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleMatch);
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleServer);	
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleBattle);
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleClub);
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleClubPlaza);
}

//��������
CSimulateClient::~CSimulateClient()
{
}

//����֪ͨ
bool CSimulateClient::OnEventTCPSocketLink(INT nErrorCode)
{
	//���ӳɹ�
	if (nErrorCode == 0)
	{
		//��ӡ��Ϣ
		CString strMessage;
		strMessage.Format(TEXT("Accounts=%s: �������ӳɹ�!"), m_szAccounts);
		InsertString(strMessage.GetString(), RGB(128, 128, 128));

		m_ModuleLogon.LogonByAccounts(m_szAccounts,m_szPassword);
	}

	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}	 

	return true;
}

//�ر�֪ͨ
bool CSimulateClient::OnEventTCPSocketShut(BYTE cbShutReason)
{
	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ������������ӶϿ���!"), m_szAccounts);
	InsertString(strMessage.GetString(), RGB(255, 0, 0));

	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return true;
}

//����ʱ��
VOID CSimulateClient::SetGameTimer(UINT uTimerID, DWORD dwElapse)
{
	SetTimer(uTimerID, dwElapse, NULL);
}

//����ʱ��
VOID CSimulateClient::KillGameTimer(UINT uTimerID)
{
	KillTimer(uTimerID);
}

//������Ϣ
VOID CSimulateClient::InsertString(LPCTSTR pszString, COLORREF crColor)
{
	//��������
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat, sizeof(CharFormat));

	//���ñ���
	CharFormat.cbSize = sizeof(CharFormat);
	CharFormat.dwMask = CFM_COLOR;
	CharFormat.crTextColor = crColor;

	//������Ϣ
	CString strContent;
	strContent.Format(TEXT("[%I64d]%s"), GetTickCount64(), pszString);

	//������Ϣ
	if (AfxGetMainWnd() != NULL)
	{
		auto pRichEditCtrl = &((CSimulateLinkDlg*)AfxGetMainWnd())->m_RichEditCtrl;
		if (pRichEditCtrl->GetLineCount()>=1000)
		{
			//ɾ����Ϣ
			pRichEditCtrl->SetSel(0L, -1L);
			pRichEditCtrl->ReplaceSel(NULL);			
		}

		pRichEditCtrl->InsertString(strContent.GetString(), CharFormat);
		pRichEditCtrl->InsertString(TEXT("\n"), CharFormat);
	}	

	return;
}

//ʱ����Ϣ
VOID CSimulateClient::OnTimer(UINT_PTR nIDEvent)
{
	//У���ʶ
	if (nIDEvent >= IDI_MODULE_START)
	{
		//ģ���б�
		IModuleItem* ModuleItemList[] = { &m_ModuleLogon,&m_ModuleMatch,&m_ModuleServer,&m_ModuleBattle,&m_ModuleClub,&m_ModuleClubPlaza};

		//У���ʶ
		for (int i = 0; i < CountArray(ModuleItemList); i++)
		{
			auto pIModuleItem = ModuleItemList[i];

			//��¼ģ��
			if (nIDEvent >= pIModuleItem->GetTimerStart() && nIDEvent <= pIModuleItem->GetTimerRange())
			{
				pIModuleItem->OnEventTimer(nIDEvent - pIModuleItem->GetTimerStart(), 0);
				break;
			}
		}		
	}
	else
	{

	}

	__super::OnTimer(nIDEvent);
}

//Ͷ����Ϣ
VOID CSimulateClient::PostWindowMessage(UINT MsgID, WPARAM wParam, LPARAM lParam)
{
	if (m_hWnd != NULL) PostMessage(MsgID, wParam, lParam);
}

//�رտͻ���
VOID CSimulateClient::CloseCleint()
{
	//У��״̬
	if (m_VirtualLinkManager.GetSocketStatus() == SOCKET_STATUS_CONNECT)
	{
		m_VirtualLinkManager.PerformClose();
	}	
}

//�����ͻ���
bool CSimulateClient::StartClient(LPCTSTR pszServerAddr, WORD wServerPort)
{
	//У���û�
	ASSERT (m_szAccounts[0]!=0 && m_szPassword[0]!=0);
	if ((m_szAccounts[0] == 0 || m_szPassword[0] == 0)) return false;

	//У��״̬
	if (m_VirtualLinkManager.GetSocketStatus() == SOCKET_STATUS_CONNECT)
	{
		return false;
	}

	//��������
	if (m_hWnd == NULL)
	{
		CRect rcCreate(0, 0, 0, 0);
		Create(NULL, NULL, WS_CHILD, rcCreate, GetDesktopWindow(), 100);
	}

	//��������
	m_VirtualLinkManager.PerformConnect(pszServerAddr, wServerPort);

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: �������ӷ�����..."),m_szAccounts);
	InsertString(strMessage.GetString(),RGB(128,128,128));

	return true;
}

//�����˺�
VOID CSimulateClient::SetAccountInfo(LPCTSTR pszAccounts, LPCTSTR pszPassword)
{
	//��������
	StringCchCopy(m_szAccounts,CountArray(m_szAccounts), pszAccounts);
	StringCchCopy(m_szPassword, CountArray(m_szPassword), pszPassword);
}

//���ӹر�
LRESULT CSimulateClient::OnLogonLinkShut(WPARAM wParam, LPARAM lParam)
{
	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//����ʧ��
LRESULT CSimulateClient::OnLogonLinkFailure(WPARAM wParam, LPARAM lParam)
{
	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//��¼���
LRESULT CSimulateClient::OnLogonLogonFinish(WPARAM wParam, LPARAM lParam)
{
	//����ģʽ
	switch (g_GlobalUnits.m_cbTestMode)
	{
	case TEST_MODE_GAME:		//��Ϸ����
	{
		//��������
		if (m_GlobalUserData.wLockServerID != 0)
		{
			m_ModuleServer.EnterServer(g_GlobalUnits.m_wKindID, m_GlobalUserData.wLockServerID, g_GlobalUnits.m_wServerLevel);

			return 0;
		}

		switch (g_GlobalUnits.m_wServerType)
		{
		case GAME_GENRE_GOLD:		//��ҷ���
		{
			m_ModuleServer.EnterServer(g_GlobalUnits.m_wKindID, INVALID_WORD, g_GlobalUnits.m_wServerLevel);
			break;
		}
		case GAME_GENRE_MATCH:		//��������
		{
			m_ModuleMatch.EnterService();
			break;
		}
		}

		break;
	}
	case TEST_MODE_CLUB:			//�������
	{
		m_ModuleClubPlaza.EnterService();

		break;
	}
	case TEST_MODE_BATTLE:			//Լս����
	{
		m_ModuleBattle.EnterService();

		break;
	}
	}		

	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//��¼ʧ��
LRESULT CSimulateClient::OnLogonLogonFailure(WPARAM wParam, LPARAM lParam)
{
	//����ģ��
	m_ModuleServer.ResetModule();

	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//���ӹر�
LRESULT CSimulateClient::OnMatchLinkShut(WPARAM wParam, LPARAM lParam)
{
	m_ModuleMatch.ResetModule();

	return 0;
}

//����ʧ��
LRESULT CSimulateClient::OnMatchLinkFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleMatch.ResetModule();

	return 0;
}

//��¼���
LRESULT CSimulateClient::OnMatchLogonFinish(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

//��¼ʧ��
LRESULT CSimulateClient::OnMatchLogonFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleMatch.ResetModule();


	return 0;
}

//���ӹر�
LRESULT CSimulateClient::OnClubLinkShut(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClub.ResetModule();

	return 0;
}

//����ʧ��
LRESULT CSimulateClient::OnClubLinkFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClub.ResetModule();

	return 0;
}

//��¼���
LRESULT CSimulateClient::OnClubLogonFinish(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//��¼ʧ��
LRESULT CSimulateClient::OnClubLogonFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClub.ResetModule();

	return 0;
}

//���ӹر�
LRESULT CSimulateClient::OnClubPlazaLinkShut(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClubPlaza.ResetModule();

	return 0;
}

//����ʧ��
LRESULT CSimulateClient::OnClubPlazaLinkFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClubPlaza.ResetModule();

	return 0;
}

//��¼���
LRESULT CSimulateClient::OnClubPlazaLogonFinish(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//��¼ʧ��
LRESULT CSimulateClient::OnClubPlazaLogonFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClubPlaza.ResetModule();

	return 0;
}

//���ӹر�
LRESULT CSimulateClient::OnBattleLinkShut(WPARAM wParam, LPARAM lParam)
{
	m_ModuleBattle.ResetModule();

	return 0;
}

//����ʧ��
LRESULT CSimulateClient::OnBattleLinkFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleBattle.ResetModule();

	return 0;
}

//��¼���
LRESULT CSimulateClient::OnBattleLogonFinish(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//��¼ʧ��
LRESULT CSimulateClient::OnBattleLogonFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleBattle.ResetModule();

	return 0;
}

//���ӹر�
LRESULT CSimulateClient::OnServerLinkShut(WPARAM wParam, LPARAM lParam)
{
	//����ģ��
	m_ModuleServer.ResetModule();

	//��ȡ��ʶ
	auto wServiceID = (WORD)lParam;
	auto bGameStarted = (bool)wParam;

	//�����ж�
	if (bGameStarted == true && wServiceID != 0 && g_GlobalUnits.m_bConnectStatus==true)
	{
		m_ModuleServer.EnterServer(0,wServiceID,0);
	}

	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//����ʧ��
LRESULT CSimulateClient::OnServerLinkFailure(WPARAM wParam, LPARAM lParam)
{
	//�ر�����
	m_ModuleServer.CloseVirtualLink();

	//����ģ��
	m_ModuleServer.ResetModule();

	switch (g_GlobalUnits.m_wServerType)
	{
	case GAME_GENRE_MATCH:		//��������
	{
		m_ModuleMatch.EnterService();
		break;
	}
	}

	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//��¼���
LRESULT CSimulateClient::OnServerLogonFinish(WPARAM wParam, LPARAM lParam)
{
	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//��¼ʧ��
LRESULT CSimulateClient::OnServerLogonFailure(WPARAM wParam, LPARAM lParam)
{
	//����״̬
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//�������
LRESULT CSimulateClient::OnServerMatchCompleted(WPARAM wParam, LPARAM lParam)
{
	//�رշ���
	m_ModuleServer.CloseVirtualLink();

	//�������
	if (m_ModuleMatch.GetLinkStatus()==false) m_ModuleMatch.EnterService();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
