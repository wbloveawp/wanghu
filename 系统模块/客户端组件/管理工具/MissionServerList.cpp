#include "StdAfx.h"
#include "MissionServerList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//��������
#define IDI_AVTIVE_MISSION				100								//��������
#define TIME_AVTIVE_MISSION				600000L							//��������

//////////////////////////////////////////////////////////////////////////
//��̬����

CMissionList * CMissionList::m_pMissionServerList=NULL;		//����ָ��

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMissionList, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CMissionList::CMissionList()
{
	//���ñ���
	m_bOnLineMission=false;

	//���ö���
	ASSERT(m_pMissionServerList==NULL);
	if (m_pMissionServerList==NULL) m_pMissionServerList=this;

	return;
}

//��������
CMissionList::~CMissionList()
{
	//�ͷŶ���
	ASSERT(m_pMissionServerList==this);
	if (m_pMissionServerList==this) m_pMissionServerList=NULL;

	return;
}

//�����¼�
bool CMissionList::OnEventMissionLink(INT nErrorCode)
{
	////�����ж�
	//if (nErrorCode!=0L)
	//{
	//	//״̬Ч��
	//	if ((m_GameKindIDMission.GetCount()==0)&&(m_bOnLineMission==false))
	//	{
	//		ASSERT(FALSE);
	//		return false;
	//	}

	//	//��������
	//	if (m_hWnd==NULL)
	//	{
	//		CRect rcCreate(0,0,0,0);
	//		Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
	//	}

	//	//����ʱ��
	//	SetTimer(IDI_AVTIVE_MISSION,TIME_AVTIVE_MISSION,NULL);

	//	return true;
	//}

	//////��������
	////if (m_bOnLineMission==true)
	////{
	////	//���ñ���
	////	m_bOnLineMission=false;

	////	//��������
	////	CMD_GP_GetOnline GetOnline;
	////	ZeroMemory(&GetOnline,sizeof(GetOnline));

	////	//��������
	////	POSITION Position=NULL;
	////	CMissionManager * pMissionManager=GetMissionManager();
	////	CServerListManager * pServerListManager=CServerListManager::GetInstance();

	////	do
	////	{
	////		//��ȡ����
	////		CGameServerItem * pGameServerItem=pServerListManager->EmunGameServerItem(Position);

	////		//����ж�
	////		if (GetOnline.wServerCount>=CountArray(GetOnline.wOnLineServerID))
	////		{
	////			ASSERT(FALSE);
	////			break;
	////		}

	////		//���÷���
	////		if (pGameServerItem!=NULL)
	////		{
	////			tagPCGameServer * pGameServer=&pGameServerItem->m_GameServer;
	////			GetOnline.wOnLineServerID[GetOnline.wServerCount++]=pGameServer->wServerID;
	////		}

	////	} while (Position!=NULL);

	////	//��������
	////	WORD wHeadSize=sizeof(GetOnline)-sizeof(GetOnline.wOnLineServerID);
	////	WORD wSendSize=wHeadSize+sizeof(GetOnline.wOnLineServerID[0])*GetOnline.wServerCount;
	////	pMissionManager->SendData(MAKECMMD(MDM_GP_SERVER_LIST, SERVICE_MODULE_LOGON),SUB_GP_GET_ONLINE,&GetOnline,wSendSize);
	////}

	//////�б�����
	////if (m_GameKindIDMission.GetCount()>0L)
	////{
	////	//��������
	////	WORD wKindIDItem[MAX_KIND];
	////	WORD wKindIDCount=__min(CountArray(wKindIDItem),m_GameKindIDMission.GetCount());

	////	//��������
	////	WORD wKindIDKey=0;
	////	POSITION KindIDPos=m_GameKindIDMission.GetStartPosition();

	////	//��������
	////	for (WORD i=0;i<wKindIDCount;i++) 
	////	{
	////		m_GameKindIDMission.GetNextAssoc(KindIDPos,wKindIDKey,wKindIDItem[i]);
	////		m_GameKindIDPending[wKindIDItem[i]]=wKindIDItem[i];
	////	}

	////	//ɾ������
	////	m_GameKindIDMission.RemoveAll();

	////	//��������
	////	CMissionManager * pMissionManager=GetMissionManager();
	////	pMissionManager->SendData(MAKECMMD(MDM_GP_SERVER_LIST, SERVICE_MODULE_LOGON),SUB_GP_GET_SERVER,wKindIDItem,wKindIDCount*sizeof(WORD));
	////}

	//CMD_GA_C_LinkService linkService;
	//ZeroMemory(&linkService, sizeof(linkService));

	//linkService.dwStationID = 1000;
	//linkService.wServiceID = INVALID_WORD;
	//linkService.wServiceModule = SERVICE_MODULE_LOGON;

	////��������
	//CMissionManager* pMissionManager = GetMissionManager();
	//pMissionManager->SendData(MAKECMMD(MDM_GA_ACCESS_SERVICE, SERVICE_MODULE_ACCESS), SUB_GA_C_LINK_SERVICE, &linkService, sizeof(linkService));

	return true;
}

//�ر��¼�
bool CMissionList::OnEventMissionShut(BYTE cbShutReason)
{
	////�б�����
	//if (m_GameKindIDPending.GetCount()>0)
	//{
	//	//��������
	//	WORD wKindIDKey=0,wKindID=0;
	//	POSITION Position=m_GameKindIDPending.GetStartPosition();

	//	//��������
	//	while (Position!=NULL)
	//	{
	//		//ö������
	//		m_GameKindIDPending.GetNextAssoc(Position,wKindIDKey,wKindID);

	//		//��������
	//		m_GameKindIDMission[wKindIDKey]=wKindID;
	//	}

	//	//ɾ������
	//	m_GameKindIDPending.RemoveAll();
	//}

	////��������
	//if ((m_GameKindIDMission.GetCount()>0)||(m_bOnLineMission==true))
	//{
	//	//��������
	//	if (m_hWnd==NULL)
	//	{
	//		CRect rcCreate(0,0,0,0);
	//		Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
	//	}

	//	//����ʱ��
	//	SetTimer(IDI_AVTIVE_MISSION,TIME_AVTIVE_MISSION,NULL);
	//}

	return true;
}

//��ȡ�¼�
bool CMissionList::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//WORD wCmd = LOCMMD(Command.wMainCmdID);
	//if (wCmd==MDM_GP_SERVER_LIST)
	//{
	//	switch (Command.wSubCmdID)
	//	{
	//	case SUB_GP_LIST_TYPE:		//������Ϣ
	//		{
	//			//Ч�����
	//			ASSERT(wDataSize%sizeof(tagGameType)==0);
	//			if (wDataSize%sizeof(tagGameType)!=0) return false;

	//			//��������
	//			WORD wItemCount=wDataSize/sizeof(tagGameType);
	//			tagGameType * pGameType=(tagGameType *)(pData);

	//			//��ȡ����
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//��������
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameType(pGameType++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_KIND:		//������Ϣ
	//		{
	//			//Ч�����
	//			ASSERT(wDataSize%sizeof(tagGameKind)==0);
	//			if (wDataSize%sizeof(tagGameKind)!=0) return false;

	//			//��������
	//			WORD wItemCount=wDataSize/sizeof(tagGameKind);
	//			tagGameKind * pGameKind=(tagGameKind *)(pData);

	//			//��ȡ����
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//��������
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameKind(pGameKind++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_NODE:		//�ڵ���Ϣ
	//		{
	//			//Ч�����
	//			ASSERT(wDataSize%sizeof(tagGameNode)==0);
	//			if (wDataSize%sizeof(tagGameNode)!=0) return false;

	//			//��������
	//			WORD wItemCount=wDataSize/sizeof(tagGameNode);
	//			tagGameNode * pGameNode=(tagGameNode *)(pData);

	//			//��ȡ����
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//��������
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameNode(pGameNode++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_SERVER:	//������Ϣ
	//		{
	//			//Ч�����
	//			ASSERT(wDataSize%sizeof(tagPCGameServer)==0);
	//			if (wDataSize%sizeof(tagPCGameServer)!=0) return false;

	//			//��������
	//			WORD wItemCount=wDataSize/sizeof(tagPCGameServer);
	//			tagPCGameServer * pGameServer=(tagPCGameServer *)(pData);

	//			//��ȡ����
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//��������
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameServer(pGameServer++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_CUSTOM:	//������Ϣ
	//		{
	//			//Ч�����
	//			ASSERT(wDataSize%sizeof(tagGameCustom)==0);
	//			if (wDataSize%sizeof(tagGameCustom)!=0) return false;

	//			//��������
	//			WORD wItemCount=wDataSize/sizeof(tagGameCustom);
	//			tagGameCustom * pGameCustom=(tagGameCustom *)(pData);

	//			//��ȡ����
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//��������
	//			for (WORD i=0;i<wItemCount;i++)
	//			{
	//				pServerListManager->InsertGameCustom(pGameCustom++);
	//			}

	//			return true;
	//		}
	//	case SUB_GP_LIST_FINISH:	//�б���Ϣ
	//		{
	//			//�б����
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			if (CServerListManager::GetInstance()!=NULL) CServerListManager::GetInstance()->OnEventListFinish();

	//			//��������
	//			CServerListManager::GetInstance()->UpdateOnlineCount();

	//			return true;
	//		}
	//	case SUB_GP_SERVER_FINISH:	//�������
	//		{
	//			//Ч������
	//			ASSERT(wDataSize%sizeof(WORD)==0);
	//			if (wDataSize%sizeof(WORD)!=0) return false;

	//			//��ȡ����
	//			ASSERT(CServerListManager::GetInstance()!=NULL);
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//������
	//			for (UINT i=0;i<(wDataSize/sizeof(WORD));i++)
	//			{
	//				m_GameKindIDPending.RemoveKey(((WORD *)pData)[i]);
	//				pServerListManager->OnEventKindFinish(((WORD *)pData)[i]);
	//			}

	//			//��ֹ����
	//			if ((m_GameKindIDPending.GetCount()==0L)&&(m_bOnLineMission==false))
	//			{
	//				CMissionManager * pMissionManager=GetMissionManager();
	//				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this);
	//			}

	//			return true;
	//		}
	//	case SUB_GR_KINE_ONLINE:	//��������
	//		{
	//			//��������
	//			CMD_GP_KindOnline * pKindOnline=(CMD_GP_KindOnline *)pData;
	//			WORD wHeadSize=(sizeof(CMD_GP_KindOnline)-sizeof(pKindOnline->OnLineInfoKind));

	//			//Ч������
	//			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pKindOnline->wKindCount*sizeof(tagOnLineInfoKind))));
	//			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pKindOnline->wKindCount*sizeof(tagOnLineInfoKind)))) return false;

	//			//��ȡ����
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//��������
	//			for (WORD i=0;i<pKindOnline->wKindCount;i++)
	//			{
	//				tagOnLineInfoKind * pOnLineInfoKind=&pKindOnline->OnLineInfoKind[i];
	//				pServerListManager->SetKindOnLineCount(pOnLineInfoKind->wKindID,pOnLineInfoKind->dwOnLineCount);
	//			}

	//			//��������
	//			pServerListManager->UpdateOnlineCount();

	//			return true;
	//		}
	//	case SUB_GR_SERVER_ONLINE:	//��������
	//		{
	//			//��������
	//			CMD_GP_ServerOnline * pServerOnline=(CMD_GP_ServerOnline *)pData;
	//			WORD wHeadSize=(sizeof(CMD_GP_ServerOnline)-sizeof(pServerOnline->OnLineInfoServer));

	//			//Ч������
	//			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pServerOnline->wServerCount*sizeof(tagOnLineInfoServer))));
	//			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pServerOnline->wServerCount*sizeof(tagOnLineInfoServer)))) return false;

	//			//��ȡ����
	//			CServerListManager * pServerListManager=CServerListManager::GetInstance();

	//			//��������
	//			for (WORD i=0;i<pServerOnline->wServerCount;i++)
	//			{
	//				tagOnLineInfoServer * pOnLineInfoServer=&pServerOnline->OnLineInfoServer[i];
	//				pServerListManager->SetServerOnLineCount(pOnLineInfoServer->wServerID,pOnLineInfoServer->dwOnLineCount);
	//			}

	//			//��ֹ����
	//			if ((m_GameKindIDPending.GetCount()==0L)&&(m_bOnLineMission==false))
	//			{
	//				CMissionManager * pMissionManager=GetMissionManager();
	//				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this);
	//			}

	//			return true;
	//		}
	//	}
	//}
	//else if (wCmd == MDM_GA_ACCESS_SERVICE)
	//{
	//	if (Command.wSubCmdID == SUB_GA_S_LINK_SUCCESS)
	//	{
	//		//��������
	//		if (m_bOnLineMission == true)
	//		{
	//			//���ñ���
	//			m_bOnLineMission = false;

	//			//��������
	//			CMD_GP_GetOnline GetOnline;
	//			ZeroMemory(&GetOnline, sizeof(GetOnline));

	//			//��������
	//			POSITION Position = NULL;
	//			CMissionManager* pMissionManager = GetMissionManager();
	//			CServerListManager* pServerListManager = CServerListManager::GetInstance();

	//			do
	//			{
	//				//��ȡ����
	//				CGameServerItem* pGameServerItem = pServerListManager->EmunGameServerItem(Position);

	//				//����ж�
	//				if (GetOnline.wServerCount >= CountArray(GetOnline.wOnLineServerID))
	//				{
	//					ASSERT(FALSE);
	//					break;
	//				}

	//				//���÷���
	//				if (pGameServerItem != NULL)
	//				{
	//					tagPCGameServer* pGameServer = &pGameServerItem->m_GameServer;
	//					GetOnline.wOnLineServerID[GetOnline.wServerCount++] = pGameServer->wServerID;
	//				}

	//			} while (Position != NULL);

	//			//��������
	//			WORD wHeadSize = sizeof(GetOnline) - sizeof(GetOnline.wOnLineServerID);
	//			WORD wSendSize = wHeadSize + sizeof(GetOnline.wOnLineServerID[0]) * GetOnline.wServerCount;
	//			pMissionManager->SendData(MAKECMMD(MDM_GP_SERVER_LIST, SERVICE_MODULE_LOGON), SUB_GP_GET_ONLINE, &GetOnline, wSendSize);
	//		}

	//		//�б�����
	//		if (m_GameKindIDMission.GetCount() > 0L)
	//		{
	//			//��������
	//			WORD wKindIDItem[MAX_KIND];
	//			WORD wKindIDCount = __min(CountArray(wKindIDItem), m_GameKindIDMission.GetCount());

	//			//��������
	//			WORD wKindIDKey = 0;
	//			POSITION KindIDPos = m_GameKindIDMission.GetStartPosition();

	//			//��������
	//			for (WORD i = 0; i < wKindIDCount; i++)
	//			{
	//				m_GameKindIDMission.GetNextAssoc(KindIDPos, wKindIDKey, wKindIDItem[i]);
	//				m_GameKindIDPending[wKindIDItem[i]] = wKindIDItem[i];
	//			}

	//			//ɾ������
	//			m_GameKindIDMission.RemoveAll();

	//			//��������
	//			CMissionManager* pMissionManager = GetMissionManager();
	//			pMissionManager->SendData(MAKECMMD(MDM_GP_SERVER_LIST, SERVICE_MODULE_LOGON), SUB_GP_GET_SERVER, wKindIDItem, wKindIDCount * sizeof(WORD));
	//		}
	//	}
	//}

	return true;
}

//��������
VOID CMissionList::UpdateOnLineInfo()
{
	//Ч��״̬
	if (m_bOnLineMission==true) return;

	//���ñ���
	m_bOnLineMission=true;

	//�رմ���
	if (m_hWnd!=NULL) DestroyWindow();

	//��������
	ASSERT(GetMissionManager());
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	return;
}

//��������
VOID CMissionList::UpdateServerInfo(WORD wKindID)
{
	//�رմ���
	if (m_hWnd!=NULL) DestroyWindow();

	//���ñ���
	m_GameKindIDMission[wKindID]=wKindID;

	//��������
	ASSERT(GetMissionManager());
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);
	
	return;
}

//���ط���
VOID CMissionList::LoadServerInfo()
{
	//��������
	ASSERT(GetMissionManager());
	CMissionManager* pMissionManager = GetMissionManager();
	if (pMissionManager != NULL) pMissionManager->AvtiveMissionItem(this, false);

	return;
}

//ʱ����Ϣ
VOID CMissionList::OnTimer(UINT_PTR nIDEvent)
{
	//��������
	if (nIDEvent==IDI_AVTIVE_MISSION)
	{
		//�رմ���
		DestroyWindow();

		//��������
		if (m_GameKindIDMission.GetCount()>0L)
		{
			ASSERT(GetMissionManager());
			CMissionManager * pMissionManager=GetMissionManager();
			if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);
		}

		return;
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
