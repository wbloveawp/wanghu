#include "StdAfx.h"
#include "RobotUserItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//��̬����
CTimerItemArray CRobotUserItem::m_TimerItemStorage;					//�������

//////////////////////////////////////////////////////////////////////////

//���캯��
CRobotUserItem::CRobotUserItem()
{
	//״̬����
	m_bWaitLeave=false;
	m_bStartClient=false;
	m_wRobotAction=0;
	m_nCurrTimerID=0;
	m_dwPlayInnings=0;
	m_cbGameStatus=GAME_STATUS_FREE;

	//�û�״̬
	m_CurrentUserStatus.cbUserStatus=US_NULL;
	m_CurrentUserStatus.wChairID=INVALID_CHAIR;
	m_CurrentUserStatus.wTableID=INVALID_TABLE;

	//�󶨱���
	m_dwClientVersion=0L;
	m_pIServerUserItem=NULL;
	m_pITableFrameItem=NULL;

	//ʱ�����
	m_dwMinSitInterval=0;
	m_dwMaxSitInterval=0;
	m_dwStandupTickCount=0;
	m_dwReposeTickCount=0;

	//��������
	m_wRoundID=1;
	m_wRobotIndex=INVALID_WORD_INDEX;

	//�ӿڱ���
	m_pIServerUserManager=NULL;
	m_pIRobotUserManager=NULL;
	m_pIRobotUserItemSink=NULL;

	//������Ϣ
	ZeroMemory(&m_RobotService,sizeof(m_RobotService));
	ZeroMemory(&m_RobotItemConfig,sizeof(m_RobotItemConfig));

	return;
}

//��������
CRobotUserItem::~CRobotUserItem()
{
	//ɾ��ʱ��
	KillGameTimer(0);

	//�ͷŶ���
	SafeRelease(m_pIRobotUserItemSink);

	return;
}

//�ӿڲ�ѯ
VOID * CRobotUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRobotUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRobotUserItem,Guid,dwQueryVer);
	return NULL;
}

//��ȡ I D
DWORD CRobotUserItem::GetUserID()
{
	return m_RobotItemConfig.RobotAccountsInfo.dwUserID;
}

//���Ӻ���
WORD CRobotUserItem::GetTableID()
{
	//Ч��״̬
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return INVALID_TABLE;

	return m_pIServerUserItem->GetTableID();
}

//���Ӻ���
WORD CRobotUserItem::GetChairID()
{
	//Ч��״̬
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return INVALID_CHAIR;

	return m_pIServerUserItem->GetChairID();
}

//�û�״̬
BYTE CRobotUserItem::GetUserStatus()
{
	//Ч��״̬
	ASSERT (m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return US_NULL;

	return m_pIServerUserItem->GetUserStatus();
}

//�û�����
LPCTSTR CRobotUserItem::GetAccounts()
{
	//Ч��״̬
	ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return NULL;

	return m_pIServerUserItem->GetAccounts();
}

//�û�����
SCORE CRobotUserItem::GetUserScore()
{
	//Ч��״̬
	ASSERT(m_pIServerUserItem != NULL);
	if (m_pIServerUserItem == NULL) return 0;

	return m_pIServerUserItem->GetUserFreeScore();
}

//���ͺ���
bool CRobotUserItem::SendSocketData(WORD wSubCmdID)
{
	//״̬Ч��
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//��������
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//���ͺ���
bool CRobotUserItem::SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//��������
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//ɾ��ʱ��
bool CRobotUserItem::KillGameTimer(UINT nTimerID)
{
	//ɾ��ʱ��
	if (nTimerID!=0)
{
		//Ѱ������
		for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
	{
			//��ȡʱ��
			tagTimerItem * pTimerItem=m_TimerItemActive[i];

			//ɾ���ж�
			if (pTimerItem->nTimerID==nTimerID)
		{
				m_TimerItemActive.RemoveAt(i);
				m_TimerItemStorage.Add(pTimerItem);

				return true;
			}
		}
	}
	else
{
		m_TimerItemStorage.Append(m_TimerItemActive);
		m_TimerItemActive.RemoveAll();
	}

	return false;
}

//����ʱ��
bool CRobotUserItem::SetGameTimer(UINT nTimerID, UINT nElapse)
{
	//Ѱ������
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
{
		//��ȡʱ��
		tagTimerItem * pTimerItem=m_TimerItemActive[i];

		//�����ж�
		if (pTimerItem->nTimerID==nTimerID)
	{
			pTimerItem->nTimeLeave=nElapse;
			return true;
		}
	}

	//��������
	tagTimerItem * pTimerItem=NULL;
	WORD wStorageCount=(WORD)m_TimerItemStorage.GetCount();

	//�������
	if (wStorageCount>0)
{
		//��ȡ����
		pTimerItem=m_TimerItemStorage[wStorageCount-1];

		//��������
		m_TimerItemActive.Add(pTimerItem);
		m_TimerItemStorage.RemoveAt(wStorageCount-1);
	}

	//��������
	if (pTimerItem==NULL)
{
		try
	{
			//��������
			pTimerItem=new tagTimerItem;
			if (pTimerItem==NULL) return false;

			//��������
			m_TimerItemActive.Add(pTimerItem);
		}
		catch (...)
	{
			ASSERT(FALSE);
			return false;
		}
	}

	//���ñ���
	pTimerItem->nTimerID=nTimerID;
	pTimerItem->nTimeLeave=nElapse;

	return true;
}

//����׼��
bool CRobotUserItem::SendUserReady(VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT((m_pIServerUserItem!=NULL)&&(m_pIServerUserItem->GetUserStatus()==US_SIT));
	if ((m_pIServerUserItem==NULL)||(m_pIServerUserItem->GetUserStatus()!=US_SIT)) return false;

	//У�鶯��
	if (m_wRobotAction&(ROBOT_WAITSTANDUP|ROBOT_WAITLEAVE)) return true;

	//����׼��
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_FRAME,SUB_GF_USER_READY,pData,wDataSize);

	return true;
}

//��������
bool CRobotUserItem::SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)
{
	//Ч���û�
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//������Ϣ
	CMD_GF_UserChat UserChat;
	StringCchCopy(UserChat.szChatString,CountArray(UserChat.szChatString), pszChatMessage);

	//��������
	UserChat.crStringColor=crFontColor;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=m_pIServerUserItem->GetUserID();
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//��������
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	WORD wSendSize=sizeof(UserChat)-sizeof(UserChat.szChatString)+UserChat.wChatLength*sizeof(TCHAR);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);

	return true;
}

//���ö���
void CRobotUserItem::SetRobotAction(WORD wRobotAction, bool bEnable)
{
	if (bEnable == true)
	{
		m_wRobotAction |= wRobotAction;
	}
	else
	{
		m_wRobotAction &= ~wRobotAction;
	}
}

//��������
bool CRobotUserItem::JudgeRobotAction(WORD wAction)
{
	return (m_wRobotAction&wAction) != 0;
}

//���ӽӿ�
VOID CRobotUserItem::SetTableFrameItem(ITableFrameItem* pITableFrameItem)
{
	m_pITableFrameItem = pITableFrameItem;
}

//������Ŀ
WORD CRobotUserItem::GetTableRealUserCount()
{
	if (m_pITableFrameItem == NULL)
	{
		return 0;
	}

	return m_pITableFrameItem->GetRealUserCount();
}

//ͬ���û�
IServerUserItem* CRobotUserItem::GetTableUserItem(WORD wChairID)
{
	if (m_pITableFrameItem == NULL)
	{
		return NULL;
	}

	return m_pITableFrameItem->GetTableUserItem(wChairID);
}

//ʱ���¼�
bool CRobotUserItem::OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//Ѱ������
	for (INT_PTR i=m_TimerItemActive.GetCount()-1;i>=0;i--)
{
		//��������
		tagTimerItem * pTimerItem=m_TimerItemActive[i];

		//ʱ�䴦��
		if (pTimerItem->nTimeLeave<=1L)
	{
			//��������
			m_TimerItemActive.RemoveAt(i);
			m_TimerItemStorage.Add(pTimerItem);

			//���ñ���
			m_nCurrTimerID=pTimerItem->nTimerID;

			//ʱ��֪ͨ
			ASSERT(m_pIRobotUserItemSink!=NULL);
			if (m_pIRobotUserItemSink!=NULL) m_pIRobotUserItemSink->OnEventTimer(pTimerItem->nTimerID);
		}
		else
	{
			//���ñ���
			pTimerItem->nTimeLeave--;
		}
	}

	return true;
}

//������Ϣ
bool CRobotUserItem::OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��¼ʧ��
	if ((wMainCmdID==MDM_GR_LOGON)&&(wSubCmdID==SUB_GR_LOGON_FAILURE))
	{
		return OnSocketSubLogonFailure(pData,wDataSize);
	}

	//��¼���
	if ((wMainCmdID==MDM_GR_LOGON)&&(wSubCmdID==SUB_GR_LOGON_FINISH))
	{
		return OnSocketSubLogonFinish(pData,wDataSize);
	}

	//�û�״̬
	if ((wMainCmdID==MDM_GR_USER)&&(wSubCmdID==SUB_GR_USER_STATUS))
	{
		return OnSocketSubUserStatus(pData,wDataSize);
	}

	//����ʧ��
	if ((wMainCmdID==MDM_GR_USER)&&(wSubCmdID==SUB_GR_REQUEST_FAILURE))
	{
		return OnSocketSubRequestFailure(pData,wDataSize);
	}	

	//��Ϸ��Ϣ
	if ((wMainCmdID==MDM_GF_GAME)||(wMainCmdID==MDM_GF_FRAME))
	{
		return OnSocketGameMessage(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//ϵͳ��Ϣ
	if (wMainCmdID==MDM_CM_SYSTEM && wSubCmdID==SUB_CM_SYSTEM_MESSAGE)
		return OnSocketSubSystemMessage(pData,wDataSize);

	return true;
}

//��Ϸ��Ϣ
bool CRobotUserItem::OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��Ϸ����
	if (wMainCmdID==MDM_GF_GAME)
	{
		ASSERT(m_pIRobotUserItemSink!=NULL);
		return m_pIRobotUserItemSink->OnEventGameMessage(wSubCmdID,pData,wDataSize);
	}

	//��ܴ���
	if (wMainCmdID==MDM_GF_FRAME)
	{
		//Ĭ�ϴ���
		switch (wSubCmdID)
		{
		case SUB_GF_GAME_STATUS:		//��Ϸ״̬
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_GameStatus));
			if (wDataSize!=sizeof(CMD_GF_GameStatus)) return false;

			//��Ϣ����
			CMD_GF_GameStatus * pGameStatus=(CMD_GF_GameStatus *)pData;

			//���ñ���
			m_cbGameStatus=pGameStatus->cbGameStatus;

			return true;
		}
		case SUB_GF_GAME_SCENE:			//��Ϸ����
		{
			//��Ϸ����
			bool bLookonUser=(m_pIServerUserItem->GetUserStatus()==US_LOOKON);
			return m_pIRobotUserItemSink->OnEventSendGameScene(m_cbGameStatus,bLookonUser,pData,wDataSize);
		}
		case SUB_GF_USER_READY:			//�û�׼��
		{
			if (m_pIServerUserItem->GetUserStatus()>=US_READY) return true;				

			//���Ϳ�ʼ
			SendUserReady(NULL,0);			

			return true;
		}
		default:
		{
			if (m_pIRobotUserItemSink!=NULL)
				return m_pIRobotUserItemSink->OnEventFrameMessage(wSubCmdID,pData,wDataSize);
			break;
		}
		}
	}

	return true;
}

//��¼ʧ��
bool CRobotUserItem::OnSocketSubLogonFailure(VOID * pData, WORD wDataSize)
{
	//��ȡ����
	CMD_GR_LogonFailure * pLogonFailure=(CMD_GR_LogonFailure *)pData;

	CTraceService::TraceString(TEXT("�����˵�¼ʧ��, UserID=%d, ErrorMsg=%s, ErrorCode=%d"),TraceLevel_Info, GetUserID(), pLogonFailure->szDescribeString, pLogonFailure->lErrorCode);

	return true;
}

//��¼���
bool CRobotUserItem::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	//���ñ���
	m_cbGameStatus=GAME_STATUS_FREE;
	m_pIServerUserItem=m_pIServerUserManager->SearchUserItem(GetUserID());

	//״̬Ч��
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//�û�״̬
	m_CurrentUserStatus.wChairID=m_pIServerUserItem->GetChairID();
	m_CurrentUserStatus.wTableID=m_pIServerUserItem->GetTableID();
	m_CurrentUserStatus.cbUserStatus=m_pIServerUserItem->GetUserStatus();

	//�������
	CMD_GR_UserRule UserRule;
	ZeroMemory(&UserRule,sizeof(UserRule));

	//���͹���
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GR_USER,SUB_GR_USER_RULE,&UserRule,sizeof(UserRule));

	//����ʱ��
	m_dwStandupTickCount=(DWORD)time(NULL);
	m_dwReposeTickCount=m_dwMinSitInterval+(DWORD)(Random_Double*(__max(m_dwMaxSitInterval-m_dwMinSitInterval,1)));

	//�����ж�
	if (m_pIServerUserItem->GetTableID()!=INVALID_TABLE)
{
		StartGameClient();
	}

	return true;
}

//�û�״̬
bool CRobotUserItem::OnSocketSubUserStatus(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserStatus));
	if (wDataSize<sizeof(CMD_GR_UserStatus)) return false;

	//��������
	CMD_GR_UserStatus * pUserStatus=(CMD_GR_UserStatus *)pData;

	//����ж�
	if (pUserStatus->dwUserID!=m_RobotItemConfig.RobotAccountsInfo.dwUserID)
{
		return true;
	}

	//�û�״̬
	tagUserStatus LastUserStatus;
	CopyMemory(&LastUserStatus,&m_CurrentUserStatus,sizeof(LastUserStatus));
	CopyMemory(&m_CurrentUserStatus,&pUserStatus->UserStatus,sizeof(m_CurrentUserStatus));

	//��ʼ�л�
	if ((LastUserStatus.cbUserStatus!=US_READY)&&(m_CurrentUserStatus.cbUserStatus==US_READY))
{
	}

	//��������
	if(m_CurrentUserStatus.cbUserStatus==US_FREE)
{
		//���þ���
		m_dwPlayInnings=0;
		m_wRobotAction=0;

		//����ʱ��
		m_dwStandupTickCount=(DWORD)time(NULL);
		m_dwReposeTickCount=m_dwMinSitInterval+(DWORD)(Random_Double*(__max(m_dwMaxSitInterval-m_dwMinSitInterval,1)));
	}

	//��Ϸ��ʼ
	if(m_CurrentUserStatus.cbUserStatus==US_PLAYING)
	{
		m_dwPlayInnings++;
	}

	//����״̬
	if(m_CurrentUserStatus.cbUserStatus==US_SIT)
	{
		if(m_RobotService.dwSwitchTableInnings > 0 &&
		m_RobotService.dwSwitchTableInnings<=m_dwPlayInnings)
	{
			//���ö���
			m_dwPlayInnings=0;
			m_wRobotAction|=ROBOT_WAITSTANDUP;
		}
	}

	//����״̬
	if (m_CurrentUserStatus.cbUserStatus==US_FREE || m_CurrentUserStatus.cbUserStatus == US_NULL)
	{
		m_wRobotAction=0;
		m_pITableFrameItem = NULL;
	}

	//�ر��ж�
	if ((m_bStartClient==true)&&(m_CurrentUserStatus.wTableID==INVALID_TABLE))
	{
		//�ر���Ϸ
		CloseGameClient();

		return true;
	}

	//�����ж�
	if ((m_bStartClient==false)&&(m_CurrentUserStatus.wTableID!=INVALID_TABLE))
	{
		//������Ϸ
		StartGameClient();

		return true;
	}

	return true;
}

//����ʧ��
bool CRobotUserItem::OnSocketSubRequestFailure(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GR_RequestFailure));
	if (wDataSize>sizeof(CMD_GR_RequestFailure)) return false;

	//��������
	CMD_GR_RequestFailure * pRequestFailure=(CMD_GR_RequestFailure *)pData;

	//��Ϸ�Ҳ���
	if (pRequestFailure->cbFailureCode==RFC_SCORE_UNENOUGH)
{
		m_pIRobotUserManager->PostponeRobotAction(m_pIServerUserItem->GetUserID(),ROBOT_ACTION_INSURE);
	}

	return true;
}

//ϵͳ��Ϣ
bool CRobotUserItem::OnSocketSubSystemMessage(VOID * pData, WORD wDataSize)
{
	//��������
	CMD_CM_SystemMessage * pSystemMessage=(CMD_CM_SystemMessage *)pData;
	WORD wHeadSize=sizeof(CMD_CM_SystemMessage)-sizeof(pSystemMessage->szString);

	//Ч�����
	ASSERT((wDataSize>wHeadSize)&&(wDataSize==(wHeadSize+pSystemMessage->wLength*sizeof(TCHAR))));
	if ((wDataSize<=wHeadSize)||(wDataSize!=(wHeadSize+pSystemMessage->wLength*sizeof(TCHAR)))) return false;

	//�رմ���
	if ((pSystemMessage->wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0)
{
		//�رմ���
		DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
		m_pIRobotUserManager->DeleteRobotUserItem(dwRobotID,false);
	}

	return true;
}

//������Ϸ
VOID CRobotUserItem::StartGameClient()
{
	//״̬�ж�
	ASSERT((m_bStartClient==false)&&(m_pIServerUserItem!=NULL));
	ASSERT((m_pIServerUserItem->GetTableID()!=INVALID_TABLE)&&(m_pIServerUserItem->GetChairID()!=INVALID_CHAIR));

	//���ñ���
	m_bStartClient=true;

	//��������
	CMD_GF_GameOption GameOption;
	ZeroMemory(&GameOption,sizeof(GameOption));

	//��������
	//ASSERT(m_dwClientVersion!=0);
	GameOption.cbAllowLookon=FALSE;
	GameOption.dwFrameVersion=VERSION_FRAME;
	GameOption.dwClientVersion=m_dwClientVersion;

	//��������
	DWORD dwRobotID=MAKELONG(m_wRobotIndex,m_wRoundID);
	m_pIRobotUserManager->SendDataToServer(dwRobotID,MDM_GF_FRAME,SUB_GF_GAME_OPTION,&GameOption,sizeof(GameOption));

	return;
}

//�ر���Ϸ
VOID CRobotUserItem::CloseGameClient()
{
	//״̬�ж�
	ASSERT((m_bStartClient==true)&&(m_pIServerUserItem!=NULL));
	ASSERT((m_pIServerUserItem->GetTableID()==INVALID_TABLE)&&(m_pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//ɾ��ʱ��
	KillGameTimer(0);

	//���ñ���
	m_bStartClient=false;
	m_cbGameStatus=GAME_STATUS_FREE;

	//��Ϸ��λ
	if (m_pIRobotUserItemSink!=NULL) m_pIRobotUserItemSink->ResetUserItemSink();

	return;
}

//��λ����
VOID CRobotUserItem::RepositUserItem()
{
	//״̬����
	//״̬����
	m_bWaitLeave=false;
	m_bStartClient=false;
	m_wRobotAction=0;
	m_dwPlayInnings=0;
	m_cbGameStatus=GAME_STATUS_FREE;

	//�û�״̬
	m_CurrentUserStatus.cbUserStatus=US_NULL;
	m_CurrentUserStatus.wChairID=INVALID_CHAIR;
	m_CurrentUserStatus.wTableID=INVALID_TABLE;

	//ʱ�����
	m_dwMinSitInterval=0;
	m_dwMaxSitInterval=0;
	m_dwStandupTickCount=0;
	m_dwReposeTickCount=0;

	//״̬��Ϣ
	ZeroMemory(&m_RobotService,sizeof(m_RobotService));
	ZeroMemory(&m_RobotItemConfig,sizeof(m_RobotItemConfig));

	//�󶨱���
	m_pIServerUserItem=NULL;
	m_pITableFrameItem=NULL;

	//ɾ��ʱ��
	m_TimerItemStorage.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//��������
	m_wRoundID=__max(1,m_wRoundID+1);

	//��λ��Ϸ
	if (m_pIRobotUserItemSink!=NULL) m_pIRobotUserItemSink->ResetUserItemSink();

	return;
}

//////////////////////////////////////////////////////////////////////////
