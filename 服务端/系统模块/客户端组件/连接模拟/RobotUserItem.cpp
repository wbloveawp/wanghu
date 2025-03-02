#include "StdAfx.h"
#include "RobotUserItem.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CTimerItemArray CRobotUserItem::m_TimerItemStorage;					//�������

//////////////////////////////////////////////////////////////////////////

//���캯��
CRobotUserItem::CRobotUserItem()
{
	//״̬����
	m_bStartClient=false;
	m_cbGameStatus=GAME_STATUS_FREE;

	//�ӿڱ���
	m_pGlobalUserData =NULL;
	m_pVirtualLinkItem = NULL;
	m_pIRobotUserItemSink=NULL;

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
	//Ч��״̬
	ASSERT(m_pGlobalUserData != NULL);
	if (m_pGlobalUserData == NULL) return 0;

	return m_pGlobalUserData->dwUserID;
}

//���Ӻ���
WORD CRobotUserItem::GetTableID()
{
	//Ч��״̬
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return INVALID_TABLE;

	return m_pGlobalUserData->wTableID;
}

//���Ӻ���
WORD CRobotUserItem::GetChairID()
{
	//Ч��״̬
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return INVALID_CHAIR;

	return m_pGlobalUserData->wChairID;
}

//�û�״̬
BYTE CRobotUserItem::GetUserStatus()
{
	//Ч��״̬
	ASSERT (m_pGlobalUserData != NULL);
	if (m_pGlobalUserData == NULL) return US_NULL;

	return m_pGlobalUserData->cbUserStatus;
}

//�û�����
LPCTSTR CRobotUserItem::GetAccounts()
{
	//Ч��״̬
	ASSERT(m_pGlobalUserData != NULL);
	if (m_pGlobalUserData == NULL) return NULL;

	return m_pGlobalUserData->szAccounts;
}

//�û�����
SCORE CRobotUserItem::GetUserScore()
{
	//Ч��״̬
	ASSERT(m_pGlobalUserData != NULL);
	if (m_pGlobalUserData == NULL) return 0;

	return m_pGlobalUserData->lGameScore;
}

//���ͺ���
bool CRobotUserItem::SendSocketData(WORD wSubCmdID)
{
	//״̬Ч��
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return false;

	//У������
	ASSERT(m_pVirtualLinkItem != NULL);
	if (m_pVirtualLinkItem == NULL) return false;

	//��������
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//���ͺ���
bool CRobotUserItem::SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return false;

	//У������
	ASSERT (m_pVirtualLinkItem != NULL);
	if (m_pVirtualLinkItem == NULL) return false;

	//��������
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_GAME,wSubCmdID,pData,wDataSize);

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
	//У������
	ASSERT(m_pVirtualLinkItem != NULL);
	if (m_pVirtualLinkItem == NULL) return false;

	//״̬Ч��
	ASSERT((m_pGlobalUserData !=NULL)&&(m_pGlobalUserData->cbUserStatus==US_SIT));
	if ((m_pGlobalUserData ==NULL)||(m_pGlobalUserData->cbUserStatus!=US_SIT)) return false;

	//����׼��
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_FRAME,SUB_GF_USER_READY,pData,wDataSize);

	return true;
}

//��������
bool CRobotUserItem::SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)
{
	//Ч���û�
	ASSERT(m_pGlobalUserData !=NULL);
	if (m_pGlobalUserData ==NULL) return false;

	//У������
	ASSERT (m_pVirtualLinkItem != NULL);
	if (m_pVirtualLinkItem == NULL) return false;

	//������Ϣ
	CMD_GF_UserChat UserChat;
	StringCchCopy(UserChat.szChatString,CountArray(UserChat.szChatString), pszChatMessage);

	//��������
	UserChat.crStringColor=crFontColor;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID= m_pGlobalUserData->dwUserID;
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//��������
	WORD wSendSize=sizeof(UserChat)-sizeof(UserChat.szChatString)+UserChat.wChatLength*sizeof(TCHAR);
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);

	return true;
}

//�û�����
VOID CRobotUserItem::SetGlobalUserData(tagGlobalUserData* pGlobalUserData)
{
	m_pGlobalUserData = pGlobalUserData;
}

//��������
VOID CRobotUserItem::SetVirtualLinkItem(CVirtualLinkItem* pVirtualLinkItem)
{
	m_pVirtualLinkItem = pVirtualLinkItem;
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

//��Ϸ��Ϣ
bool CRobotUserItem::OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��Ϸ����
	if (wMainCmdID==MDM_GF_GAME)
	{
		ASSERT(m_pIRobotUserItemSink!=NULL);
		if (m_pIRobotUserItemSink != NULL)
		{
			return m_pIRobotUserItemSink->OnEventGameMessage(wSubCmdID, pData, wDataSize);
		}		
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
			bool bLookonUser=(m_pGlobalUserData->cbUserStatus==US_LOOKON);
			if (m_pIRobotUserItemSink != NULL)
			{
				return m_pIRobotUserItemSink->OnEventSendGameScene(m_cbGameStatus, bLookonUser, pData, wDataSize);
			}			

			return true;
		}
		case SUB_GF_USER_READY:			//�û�׼��
		{
			if (m_pGlobalUserData->cbUserStatus>=US_READY) return true;

			//���Ϳ�ʼ
			SendUserReady(NULL,0);			

			return true;
		}
		default:
		{
			if (m_pIRobotUserItemSink != NULL)
			{
				return m_pIRobotUserItemSink->OnEventFrameMessage(wSubCmdID, pData, wDataSize);
			}
			break;
		}
		}
	}

	return true;
}

//������Ϸ
VOID CRobotUserItem::StartGameClient()
{
	//״̬�ж�
	ASSERT((m_bStartClient==false)&&(m_pGlobalUserData !=NULL));
	ASSERT((m_pGlobalUserData->wTableID!=INVALID_TABLE)&&(m_pGlobalUserData->wChairID!=INVALID_CHAIR));

	//���ñ���
	m_bStartClient=true;

	//��������
	CMD_GF_GameOption GameOption;
	ZeroMemory(&GameOption,sizeof(GameOption));

	//��������
	GameOption.cbAllowLookon=FALSE;
	GameOption.dwFrameVersion=VERSION_FRAME;
	GameOption.dwClientVersion=m_dwClientVersion;

	//��������
	m_pVirtualLinkItem->SendVirtualData(MDM_GF_FRAME,SUB_GF_GAME_OPTION,&GameOption,sizeof(GameOption));

	return;
}

//�ر���Ϸ
VOID CRobotUserItem::CloseGameClient()
{
	//״̬�ж�
	ASSERT((m_bStartClient==true)&&(m_pGlobalUserData !=NULL));
	ASSERT((m_pGlobalUserData->wTableID==INVALID_TABLE)&&(m_pGlobalUserData->wChairID==INVALID_CHAIR));

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
	m_bStartClient=false;
	m_cbGameStatus=GAME_STATUS_FREE;

	//�û�״̬
	if (m_pGlobalUserData != NULL)
	{
		m_pGlobalUserData->cbUserStatus = US_NULL;
		m_pGlobalUserData->wChairID = INVALID_CHAIR;
		m_pGlobalUserData->wTableID = INVALID_TABLE;
	}

	//�󶨱���
	m_pGlobalUserData = NULL;
	m_pVirtualLinkItem = NULL;

	//ɾ��ʱ��
	m_TimerItemStorage.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//��λ��Ϸ
	if (m_pIRobotUserItemSink!=NULL) m_pIRobotUserItemSink->ResetUserItemSink();

	return;
}

//////////////////////////////////////////////////////////////////////////
