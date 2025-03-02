#include "StdAfx.h"
#include "RobotUserManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//��������
#define ROBOT_LOGON_COUNT			32									//��¼��Ŀ
#define ROBOT_PLAY_DRAW				10                 //��Ϸ���� 

//ʱ�䳣��
#define ROBOT_UNIN_TIME				10*60								//������ʱ��
#define ROBOT_UNLOAD_TIME			30*60								//������ʱ��
#define ROBOT_UNDRAW_TIME			20*60								//��ȡ��ʱ��

//ʱ���ʶ
#define IDI_ROBOT_IN				(IDI_ROBOT_MANAGER_START+0)			//����ʱ��
#define IDI_ROBOT_OUT				(IDI_ROBOT_MANAGER_START+MAX_BATCH)	//�˳�ʱ��
#define IDI_ROBOT_PULSE				(IDI_ROBOT_MANAGER_START+2*MAX_BATCH) //����ʱ��
#define IDI_ROBOT_ACTION			(IDI_ROBOT_MANAGER_START+2*MAX_BATCH+1)//����ʱ��

//����ʱ��
#define TIME_ROBOT_INOUT			15L									//��¼ʱ��
#define TIME_ROBOT_PULSE			1L									//����ʱ��
#define TIME_ROBOT_REPOSE_TIME		1800L								//����ʱ��

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CRobotUserManager::CRobotUserManager()
{
	//ϵͳ����
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//����ӿ�
	m_pITimerEngine=NULL;
	m_pIServiceFunction=NULL;
	m_pIServerUserManager=NULL;
	m_pIGameServiceManager=NULL;
	m_pIGameServiceSustomTime=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//���ñ���	
	m_dwMinSitInterval=0;
	m_dwMaxSitInterval=0;
	m_bServiceContinue=false;
	m_wAutoRobotCount=ROBOT_LOGON_COUNT;

	//������
	m_wStockCount=0;
	ZeroMemory(&m_RobotItemConfig,sizeof(m_RobotItemConfig));

	//��������
	m_ROBOTUserItemMap.InitHashTable(PRIME_ROBOT_USER);
	m_ROBOTUserItemMap.RemoveAll();

	return;
}

//��������
CRobotUserManager::~CRobotUserManager()
{
	//�������
	ASSERT(m_ROBOTUserItemMap.GetCount()==0L);
	ASSERT(m_ROBOTParameterArray.GetCount()==0L);
	ASSERT(m_ROBOTUserItemArray.GetCount()==0L);
	ASSERT(m_ROBOTUserItemBuffer.GetCount()==0L);

	//�ͷ���Դ
	for (INT_PTR i=0; i<CRobotUserItem::m_TimerItemStorage.GetCount(); i++)
{
		SafeDelete(CRobotUserItem::m_TimerItemStorage[i]);
	}
	CRobotUserItem::m_TimerItemStorage.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CRobotUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRobotUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRobotUserManager,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CRobotUserManager::StartService()
{
	//ʱ����
	DWORD dwTimeCell=TIME_CELL;
	DWORD dwElapse=TIME_ROBOT_PULSE*1000L;

	//����ʱ��
	if (m_pIGameServiceSustomTime!=NULL)
{		
		dwTimeCell=m_pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell=TIME_CELL;

		dwElapse=m_pIGameServiceSustomTime->GetRobotTimerPulse();		
		if (dwElapse<dwTimeCell) dwElapse=dwTimeCell;
	}

	//����ʱ��
	m_pITimerEngine->SetTimer(IDI_ROBOT_PULSE,dwElapse,TIMES_INFINITY,0);

	return true;
}

//ֹͣ����
bool CRobotUserManager::ConcludeService()
{
	//ɾ���洢
	for (INT_PTR i=0;i<m_ROBOTUserItemArray.GetCount();i++)
{
		SafeRelease(m_ROBOTUserItemArray[i]);
	}

	//ɾ���洢
	for (INT_PTR i=0;i<m_ROBOTParameterArray.GetCount();i++)
{
		SafeDelete(m_ROBOTParameterArray[i]);
	}

	//��������
	m_ROBOTUserItemMap.RemoveAll();
	m_ROBOTParameterArray.RemoveAll();
	m_ROBOTUserItemArray.RemoveAll();
	m_ROBOTUserItemBuffer.RemoveAll();

	//����û�
	m_wStockCount=0;
	ZeroMemory(&m_RobotItemConfig,sizeof(m_RobotItemConfig));

	return true;
}

//�Ƴ�����
bool CRobotUserManager::RemoveRobotParameter(DWORD dwBatchID)
{
	//��ȡ����
	tagRobotParameterEx * pRobotParameter = GetRobotParameter(dwBatchID);
	if(pRobotParameter==NULL) return false;

	//��������
	pRobotParameter->bIsValided=false;
	pRobotParameter->RobotParameter.dwLeaveTime=GetTodayTickCount();

	//��ȡ����
	WORD wParameterIndex=GetRobotParameterIndex(pRobotParameter);
	if (wParameterIndex==INVALID_WORD) return true;

	//�رն�ʱ��
	m_pITimerEngine->KillTimer(IDI_ROBOT_IN+wParameterIndex);

	//ɾ�����
	for (int i=m_wStockCount-1;i>=0;i--)
{
		//�����ж�
		if(m_RobotItemConfig[i].pRobotParameter!=&pRobotParameter->RobotParameter) continue;

		//ɾ�����
		m_wStockCount--;
		m_RobotItemConfig[i]=m_RobotItemConfig[m_wStockCount];
	}

	//�����ж�
	if(pRobotParameter->bIsLoadRobot==false)
{
		//�ͷ���Դ
		SafeDelete(pRobotParameter);
		m_ROBOTParameterArray[wParameterIndex]=NULL;

		return true;
	}

	//������ʱ��
	m_pITimerEngine->SetTimer(IDI_ROBOT_OUT+wParameterIndex,TIME_ROBOT_INOUT*1000L,TIMES_INFINITY,(WPARAM)pRobotParameter);

	return true;
}

//���ò���
bool CRobotUserManager::AddRobotParameter(tagRobotParameter RobotParameter[], WORD wParameterCount)
{
	//��������
	tagRobotParameterEx * pRobotParameter=NULL;

	for(WORD wIndex=0;wIndex<wParameterCount;wIndex++)
{
		//������Դ
		pRobotParameter = GetRobotParameter(RobotParameter[wIndex].dwBatchID);
		if(pRobotParameter==NULL)
	{
			//������Դ
			pRobotParameter = new tagRobotParameterEx;
			if (pRobotParameter==NULL) return false;

			//���ñ���
			pRobotParameter->bIsValided=true;
			pRobotParameter->bIsLoadRobot=false;

			//��������
			bool bVacancyExist=false;

			//���ҿ�ȱ
			for (INT_PTR nIndex=0;nIndex<m_ROBOTParameterArray.GetCount();nIndex++)
		{
				if (m_ROBOTParameterArray[nIndex]==NULL)
			{
					bVacancyExist=true;
					m_ROBOTParameterArray[nIndex]=pRobotParameter;
					break;
				}
			}

			//�����ڿ�ȱ
			if (bVacancyExist==false)
		{
				//��Ŀ����
				if(m_ROBOTParameterArray.GetCount()>=MAX_BATCH) break;
				m_ROBOTParameterArray.Add(pRobotParameter);
			}
		}

		//��������
		CopyMemory(pRobotParameter,&RobotParameter[wIndex],sizeof(tagRobotParameter));
	}

	return true;
}

//�������
bool CRobotUserManager::InsertRobotInfo(tagRobotAccountsInfo RobotAccountsInfo[],WORD wRobotCount,DWORD dwBatchID)
{
	//��ȡ����
	tagRobotParameterEx * pRobotParameter = GetRobotParameter(dwBatchID);
	if(pRobotParameter==NULL) return false;

	//��¼���	
	CRobotUserItem * pRobotUserItem=NULL;
	WORD wStockCount=m_wStockCount;
	bool bRobotExist=false;

	CString str;
	str.Format(TEXT("�����˼��سɹ�, BatchID=%d,RobotCount=%d"),dwBatchID,wRobotCount);
	CTraceService::TraceString(str,TraceLevel_Info);

	//����»���
	for (WORD wIndex=0;wIndex<wRobotCount;wIndex++)
{
		//���ñ���
		bRobotExist=false;

		//�ӿ���в���
		for (WORD wItem=0;wItem<wStockCount;wItem++)
	{
			if (m_RobotItemConfig[wItem].RobotAccountsInfo.dwUserID==RobotAccountsInfo[wIndex].dwUserID)
		{
				bRobotExist=true;
				break;
			}
		}

		//�����ж�
		if (bRobotExist==true) continue;

		//��ӳ���в���
		if (m_ROBOTUserItemMap.Lookup(RobotAccountsInfo[wIndex].dwUserID,pRobotUserItem)==TRUE)
	{
			continue;
		}

		//��ӻ���
		m_RobotItemConfig[m_wStockCount].RobotAccountsInfo=RobotAccountsInfo[wIndex];
		m_RobotItemConfig[m_wStockCount].pRobotParameter=&pRobotParameter->RobotParameter;

		//���ӿ��
		m_wStockCount++;
  }

	//��ȡ����
	WORD wParameterIndex=GetRobotParameterIndex(pRobotParameter);
	if (wParameterIndex==INVALID_WORD) return true;

	//������ʱ��
	m_pITimerEngine->KillTimer(IDI_ROBOT_IN+wParameterIndex);
	m_pITimerEngine->SetTimer(IDI_ROBOT_IN+wParameterIndex,TIME_ROBOT_INOUT*1000L,TIMES_INFINITY,(WPARAM)pRobotParameter);

	return true;
}

//�ӻ�����
bool CRobotUserManager::PostponeRobotAction(DWORD dwUserID,WORD wActionCode)
{
	//�����ж�
	for (INT_PTR i=0; i<m_ROBOTActionArray.GetCount(); i++)
{
		if (m_ROBOTActionArray[i].dwUserID==dwUserID)
	{
			return false;
		}
	}

	//��������
	tagRobotAction RobotAction;
	ZeroMemory(&RobotAction,sizeof(RobotAction));

	//���һ���
	CRobotUserItem * pRobotUserItem=NULL;
	m_ROBOTUserItemMap.Lookup(dwUserID,pRobotUserItem);

	//�жϽ��
	if (pRobotUserItem==NULL) return false;

	//��������
	tagRobotService * pRobotService=pRobotUserItem->GetRobotService();

	//����ȡ��
	if (wActionCode==ROBOT_ACTION_INSURE)
{
		//�ж�ȡ�����
		if (pRobotService->wInsureDrawCount==0)
	{
			//�޸Ķ���
			wActionCode=ROBOT_ACTION_LEAVE;
		}
		else
	{
			//�ݼ�����
			pRobotService->wInsureDrawCount--;

			//����ṹ
			RobotAction.dwUserID=dwUserID;
			RobotAction.wActionCode=ROBOT_ACTION_INSURE;
			RobotAction.dwGenerateTime=(DWORD)time(NULL);			
			RobotAction.dwDelayTime=TIME_ROBOT_INSURE_MIN+DWORD(Random_Double*(TIME_ROBOT_INSURE_MAX-TIME_ROBOT_INSURE_MIN));			
		}
	}

	//�뿪����
	if (wActionCode==ROBOT_ACTION_LEAVE)
{
		//����ṹ
		RobotAction.dwUserID=dwUserID;
		RobotAction.wActionCode=ROBOT_ACTION_LEAVE;
		RobotAction.dwGenerateTime=(DWORD)time(NULL);
		RobotAction.dwDelayTime=TIME_ROBOT_LEAVE_MIN+DWORD(Random_Double*(TIME_ROBOT_LEAVE_MAX-TIME_ROBOT_LEAVE_MIN));
	}

	//��������
	if (wActionCode==ROBOT_ACTION_STANDUP)
{
		//����ṹ	
		RobotAction.dwUserID=dwUserID;
		RobotAction.wActionCode=ROBOT_ACTION_STANDUP;
		RobotAction.dwGenerateTime=(DWORD)time(NULL);
		RobotAction.dwDelayTime=TIME_ROBOT_STANDUP_MIN+DWORD(Random_Double*(TIME_ROBOT_STANDUP_MAX-TIME_ROBOT_STANDUP_MIN));
	}	

	//��Ӷ���
	m_ROBOTActionArray.Add(RobotAction);

	//���ö�ʱ��
	m_pITimerEngine->SetTimer(IDI_ROBOT_ACTION,1000L,TIMES_INFINITY,0);

	return true;
}

//ɾ������
bool CRobotUserManager::DeleteRobotUserItem(DWORD dwRobotID,bool bStockRetrieve)
{
	//��ȡ����
	WORD wIndex=LOWORD(dwRobotID);
	CRobotUserItem * pRobotUserItem=GetRobotUserItem(wIndex);

	//����Ч��
	//ASSERT((pRobotUserItem!=NULL)&&(pRobotUserItem->m_wRoundID==HIWORD(dwRobotID)));
	if ((pRobotUserItem==NULL)||(pRobotUserItem->m_wRoundID!=HIWORD(dwRobotID))) return false;

	//�ر��¼�
	try
{
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwRobotID,0,0L);
	}
	catch (...)
{
		//�������
		ASSERT(FALSE);
	}

	//��������
	tagRobotItemConfig RobotItemConfig=pRobotUserItem->m_RobotItemConfig;

	//ɾ������
	FreeRobotUserItem(pRobotUserItem->GetUserID());

	//������
	if(bStockRetrieve==true)
{		
		InsertRobotInfo(&RobotItemConfig.RobotAccountsInfo,1,RobotItemConfig.pRobotParameter->dwBatchID);
	}

	return true;
}

//���һ���
IRobotUserItem * CRobotUserManager::SearchRobotUserItem(DWORD dwUserID, DWORD dwContextID)
{
	//���һ���
	CRobotUserItem * pRobotUserItem=NULL;
	m_ROBOTUserItemMap.Lookup(dwUserID,pRobotUserItem);

	//�����ж�
	if (pRobotUserItem!=NULL)
{
		WORD wRoundID=pRobotUserItem->m_wRoundID;
		WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;
		if (MAKELONG(wRobotIndex,wRoundID)==dwContextID) return pRobotUserItem;
	}

	return NULL;
}

//��������
IRobotUserItem * CRobotUserManager::CreateRobotUserItem(tagRobotItemConfig & RobotItemConfig)
{
	//Ч�����
	ASSERT (RobotItemConfig.RobotAccountsInfo.dwUserID!=0L);
	if (RobotItemConfig.RobotAccountsInfo.dwUserID==0L) return NULL;

	//��������
	CRobotUserItem * pRobotUserItem=ActiveRobotUserItem(RobotItemConfig);
	if (pRobotUserItem==NULL) return NULL;

	//���Ա���
	WORD wRoundID=pRobotUserItem->m_wRoundID;
	WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;

	//����ģ��
	try
{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(MAKELONG(wRobotIndex,wRoundID),0L)==false)
	{
			throw 0;
		}
	}
	catch (...)
{
		//�������
		ASSERT(FALSE);

		//�ͷ��û�
		FreeRobotUserItem(RobotItemConfig.RobotAccountsInfo.dwUserID);

		return NULL;
	}

	//��������
	CMD_GR_LogonRobot LogonRobot;
	ZeroMemory(&LogonRobot,sizeof(LogonRobot));

	//��������	
	LogonRobot.dwPlazaVersion=VERSION_PLAZA;
	LogonRobot.dwFrameVersion=VERSION_FRAME;
	LogonRobot.dwProcessVersion=m_pGameServiceAttrib->dwClientVersion;

	//��������	
	LogonRobot.dwUserID=RobotItemConfig.RobotAccountsInfo.dwUserID;
	StringCchCopy(LogonRobot.szPassword,CountArray(LogonRobot.szPassword), RobotItemConfig.RobotAccountsInfo.szPassword);

	//��������
	CMD_Command Command;
	Command.wMainCmdID=MDM_GR_LOGON;
	Command.wSubCmdID=SUB_GR_LOGON_ROBOT;

	//��Ϣ����
	try
{
		//��������
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(MAKELONG(wRobotIndex,wRoundID),Command,&LogonRobot,sizeof(LogonRobot),0)==false)
	{
			throw 0;
		}
	}
	catch (...) 
{
		//�������
		ASSERT(FALSE);

		//ɾ������
		DeleteRobotUserItem(MAKELONG(wRobotIndex,wRoundID),false);

		CTraceService::TraceString(TEXT("CreateRobotUserItem Exception"), TraceLevel_Exception);

		return NULL;
	}

	return pRobotUserItem;
}

//�����¼�
bool CRobotUserManager::OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//���봦��
	if (dwTimerID>=IDI_ROBOT_IN && dwTimerID<IDI_ROBOT_IN+MAX_BATCH)			
{
		//��ȡʱ��		
		DWORD dwTodayTickCount= GetTodayTickCount();

		//��ȡ����
		tagRobotParameter * pRobotParameter=(tagRobotParameter *)dwBindParameter;
		if(pRobotParameter==NULL)
	{
			//�رն�ʱ��
			m_pITimerEngine->KillTimer(dwTimerID);

			return true;
		}

		//��������
		bool bAllowRobotAttend=CServerRule::IsAllowRobotAttend(m_pGameServiceOption->dwServerRule);
		bool bAllowRobotSimulate=CServerRule::IsAllowRobotSimulate(m_pGameServiceOption->dwServerRule);

		//�ɹ���ʶ
		bool bLogonSuccessed=false;

		//��¼����
		if ((bAllowRobotAttend==true)||(bAllowRobotSimulate==true))
	{
			//Ѱ�һ���
			for (WORD i=0;i<m_wStockCount;i++)
		{
				//�����ж�
				if(m_RobotItemConfig[i].pRobotParameter!=pRobotParameter) continue;

				//ģ���ж�
				if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
					&& (pRobotParameter->dwServiceMode&ROBOT_SIMULATE)!=0
					&& (pRobotParameter->dwServiceMode&ROBOT_PASSIVITY)==0
					&& (pRobotParameter->dwServiceMode&ROBOT_INITIATIVE)==0)
			{
					continue;
				}

				//��������
				if (CreateRobotUserItem(m_RobotItemConfig[i])!=NULL)
			{					
					//ɾ�����
					m_wStockCount--;
					bLogonSuccessed=true;
					m_RobotItemConfig[i]=m_RobotItemConfig[m_wStockCount];

					break;
				}
			}
		}

		//�رն�ʱ��
		m_pITimerEngine->KillTimer(dwTimerID);

		//��ʱ������
		if(bLogonSuccessed==true)
	{
			//����ʱ��
			DWORD dwElapse = pRobotParameter->dwEnterMinInterval+(DWORD)(Random_Double*(pRobotParameter->dwEnterMaxInterval-pRobotParameter->dwEnterMinInterval));
			if(dwElapse==0) dwElapse=TIME_ROBOT_INOUT;

			//���ö�ʱ��
			m_pITimerEngine->SetTimer(dwTimerID,dwElapse*1000L,TIMES_INFINITY,dwBindParameter);
		}
		else
	{
			//ʣ��ʱ��
			DWORD dwRemaindTime = BatchServiceRemaindTime(pRobotParameter,dwTodayTickCount);
			if(dwRemaindTime<=ROBOT_UNIN_TIME)
		{
				//���ö�ʱ��
				m_pITimerEngine->SetTimer(IDI_ROBOT_OUT+(dwTimerID-IDI_ROBOT_IN),TIME_ROBOT_INOUT*1000L,TIMES_INFINITY,dwBindParameter);				
			}
			else
		{
				//���ö�ʱ��
				m_pITimerEngine->SetTimer(dwTimerID,TIME_ROBOT_INOUT*1000L,TIMES_INFINITY,dwBindParameter);				
			}
		}

		return true;
	}

	//�˳�����
	if (dwTimerID>=IDI_ROBOT_OUT && dwTimerID<IDI_ROBOT_OUT+MAX_BATCH)
{
		//��ȡ����
		tagRobotParameterEx * pRobotParameter=(tagRobotParameterEx *)dwBindParameter;
		if(pRobotParameter==NULL)
	{
			//�رն�ʱ��
			m_pITimerEngine->KillTimer(dwTimerID);

			return true;
		}

		//��ȡʱ��
		DWORD dwTodayTickCount= GetTodayTickCount();	

		//ʱ���ж�
		if (BatchServiceRemaindTime(&pRobotParameter->RobotParameter,dwTodayTickCount)>0) 
	{			
			return true;
		}

		//��������
		bool bLogoutMarked=false;
		bool bLogoutFinished=true;	

		//��������
		DWORD dwUserID=0L;
		INT_PTR nActiveCount=m_ROBOTUserItemMap.GetCount();
		POSITION Position=m_ROBOTUserItemMap.GetStartPosition();

		//�˳�����		
		while (Position!=NULL)
	{
			//��ȡ����
			CRobotUserItem * pRobotUserItem=NULL;
			m_ROBOTUserItemMap.GetNextAssoc(Position,dwUserID,pRobotUserItem);

			//�˳��ж�
			if (pRobotUserItem->GetRobotParameter()==&pRobotParameter->RobotParameter)
		{
				//���ñ���
				bLogoutFinished=false;

				//��������
				IServerUserItem * pIServerUserItem=pRobotUserItem->m_pIServerUserItem;
				if (pIServerUserItem!=NULL)
			{
					//����״̬
					BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
					BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
					if ((cbUserStatus!=US_FREE)&&(cbUserStatus!=US_SIT))
				{
						if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
					{
							if (cbUserMatchStatus!=MUS_NULL && (pRobotUserItem->m_wRobotAction&ROBOT_WAITLEAVE)==0)
						{
								pRobotUserItem->m_wRobotAction|=ROBOT_WAITLEAVE;
							}
						}
						else
					{
							//��Ƕ���
							if(bLogoutMarked==false && pRobotUserItem->m_wRobotAction==0)
						{
								bLogoutMarked=true;
								pRobotUserItem->m_wRobotAction|=ROBOT_WAITLEAVE;
							}							
						}

						continue;
					}
					else
				{
						if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
					{
							if ((cbUserMatchStatus!=MUS_NULL || (pRobotUserItem->m_wRobotAction&ROBOT_WAITLEAVE)==0) && pIServerUserItem->GetTableID()!=INVALID_TABLE)
						{
								pRobotUserItem->m_wRobotAction|=ROBOT_WAITLEAVE;
								continue;
							}
						}
					}
				}

				//ɾ���û�
				WORD wRoundID=pRobotUserItem->m_wRoundID;
				WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;
				DeleteRobotUserItem(MAKELONG(wRobotIndex,wRoundID),false);				

				//���ñ�ʶ
				if(pIServerUserItem!=NULL) break;
			}			
		}

		//�رն�ʱ��
		m_pITimerEngine->KillTimer(dwTimerID);

		//��ʱ������
		if(bLogoutFinished==false)
	{
			//��������ʱ��
			DWORD dwElapse = pRobotParameter->RobotParameter.dwLeaveMinInterval+(DWORD)(Random_Double*(pRobotParameter->RobotParameter.dwLeaveMaxInterval-pRobotParameter->RobotParameter.dwLeaveMinInterval));
			if(dwElapse==0) dwElapse=TIME_ROBOT_INOUT;

			//���ö�ʱ��
			m_pITimerEngine->SetTimer(dwTimerID,dwElapse*1000L,TIMES_INFINITY,dwBindParameter);
		}
		else
	{
			if(pRobotParameter->bIsValided==false)
		{
				//��������
				WORD wParameterIndex=(WORD)(dwTimerID-IDI_ROBOT_OUT);
				if(wParameterIndex==GetRobotParameterIndex(pRobotParameter))
			{
					CString str;
					str.Format(TEXT("�����λ�����ȫ���˳�,�����ν���ɾ��, ����ID=%d"),pRobotParameter->RobotParameter.dwBatchID);
					CTraceService::TraceString(str,TraceLevel_Info);

					//�ͷ���Դ
					SafeDelete(m_ROBOTParameterArray[wParameterIndex]);
					m_ROBOTParameterArray[wParameterIndex]=NULL;
				}

			}
			else
		{
				//�ָ���ʶ
				pRobotParameter->bIsLoadRobot=false;
				
				CString str;
				str.Format(TEXT("�����λ�����ȫ���˳�, ����ID=%d"),pRobotParameter->RobotParameter.dwBatchID);
				CTraceService::TraceString(str,TraceLevel_Info);
			}
		}

		return true;
	}

	//��������
	if (dwTimerID==IDI_ROBOT_ACTION)
{
		//��ȡʱ��
		DWORD dwElapseTime=INVALID_DWORD;
		DWORD dwCurrentTime=(DWORD)time(NULL);

		//��������
		for (INT_PTR i=m_ROBOTActionArray.GetCount()-1; i>=0; i--)
	{
			//��ȡ����
			tagRobotAction * pRobotAction=&m_ROBOTActionArray[i];

			//�ж�ʱ��
			if (pRobotAction->dwGenerateTime+pRobotAction->dwDelayTime>dwCurrentTime)
		{
				DWORD dwLeaveTime=pRobotAction->dwGenerateTime+pRobotAction->dwDelayTime-dwCurrentTime;
				if (dwLeaveTime<dwElapseTime) dwElapseTime=dwLeaveTime;
				continue;
			}

			//���һ���
			CRobotUserItem * pRobotUserItem=NULL;
			m_ROBOTUserItemMap.Lookup(pRobotAction->dwUserID,pRobotUserItem);

			//����ָ��
			IServerUserItem * pIServerUserItem=m_pIServerUserManager->SearchUserItem(pRobotAction->dwUserID);

			if (pIServerUserItem!=NULL && pRobotUserItem!=NULL)
		{
				switch (pRobotAction->wActionCode)
			{
				case ROBOT_ACTION_LEAVE:		//�뿪����
				{
						//ɾ������
						DeleteRobotUserItem(MAKELONG(pRobotUserItem->m_wRobotIndex,pRobotUserItem->m_wRoundID),false);

						break;
					}
				case ROBOT_ACTION_STANDUP:	//��������
				{
						//ǿ������
						ForceRobotUserItemStandup(pRobotUserItem);

						break;
					}
				case ROBOT_ACTION_INSURE:		//ȡ���
				{
						//��ȡʱ��		
						DWORD dwTodayTickCount=GetTodayTickCount();

						//��ȡ����
						tagRobotParameter * pRobotParameter=pRobotUserItem->GetRobotParameter();

						//ʣ��ʱ��
						DWORD dwRemaindTime = BatchServiceRemaindTime(pRobotParameter,dwTodayTickCount);
						if (dwRemaindTime<=ROBOT_UNDRAW_TIME)
					{
							//ɾ������
							DeleteRobotUserItem(MAKELONG(pRobotUserItem->m_wRobotIndex,pRobotUserItem->m_wRoundID),false);
						}
						else
					{
							//ɾ������
							DeleteRobotUserItem(MAKELONG(pRobotUserItem->m_wRobotIndex,pRobotUserItem->m_wRoundID),true);
						}

						break;
					}
				}

				//ɾ������
				m_ROBOTActionArray.RemoveAt(i);
			}

		}

		//���ö�ʱ��
		if (m_ROBOTActionArray.GetCount()==0)
	{
			m_pITimerEngine->KillTimer(dwTimerID);
		}
		else
	{
			m_pITimerEngine->SetTimer(IDI_ROBOT_ACTION,dwElapseTime*1000L,TIMES_INFINITY,0);
		}

		return true;
	}

	//�û�����
	if (dwTimerID==IDI_ROBOT_PULSE)		
{
		//��������
		DWORD dwUserID=0L;
		CRobotUserItem * pRobotUserItem=NULL;
		POSITION Position=m_ROBOTUserItemMap.GetStartPosition();

		//�û�����
		while (Position!=NULL)
	{
			//��ȡ����
			m_ROBOTUserItemMap.GetNextAssoc(Position,dwUserID,pRobotUserItem);

			//ʱ�䴦��
			try
		{
				if (pRobotUserItem->m_pIServerUserItem!=NULL) pRobotUserItem->OnTimerPulse(dwTimerID,dwBindParameter);
			}
			catch (...)
		{
				//��������
				TCHAR szException[128];
				_sntprintf_s(szException,CountArray(szException),TEXT("������ʱ�ӵ����쳣: TimerID=%d"),pRobotUserItem->m_nCurrTimerID);

				//��ʾ��Ϣ
				CTraceService::TraceString(szException,TraceLevel_Exception);

				//ɾ������
				DeleteRobotUserItem(MAKELONG(pRobotUserItem->m_wRobotIndex,pRobotUserItem->m_wRoundID),true);				
			}
		}

		return true;
	}

	return false;
}

//���ػ���
bool CRobotUserManager::GetRobotLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount)
{
	//��Ŀ�ж�
	if(m_ROBOTParameterArray.GetCount()==0) return false;
	
	//��������
	DWORD dwRemaindTime;
	DWORD dwTodayTickCount= GetTodayTickCount();
	tagRobotParameterEx * pRobotParameter=NULL;		

	//�˳�����
	for (INT_PTR nIndex=0;nIndex<m_ROBOTParameterArray.GetCount();nIndex++)
{
		//��ȡ����		
		pRobotParameter = m_ROBOTParameterArray[nIndex];
		if (pRobotParameter==NULL) continue;

		//�����ж�
		if (pRobotParameter->bIsLoadRobot==true) continue;

		//ʣ��ʱ��
		dwRemaindTime=BatchServiceRemaindTime(&pRobotParameter->RobotParameter,dwTodayTickCount);

		//���ػ���
		if (dwRemaindTime>ROBOT_UNLOAD_TIME)
	{
			//���ò���
			dwBatchID = pRobotParameter->RobotParameter.dwBatchID;
			dwLoadCount = __min(pRobotParameter->RobotParameter.dwRobotCount,dwRemaindTime/pRobotParameter->RobotParameter.dwEnterMaxInterval);

			if (dwLoadCount>0) 
		{
				//���ñ�ʶ
				pRobotParameter->bIsLoadRobot=true;

				return true;
			}
		}
	}

	return false;
}

//�û�״��
WORD CRobotUserManager::GetRobotUserInfo(tagRobotUserInfo & RobotUserInfo, DWORD dwServiceMode)
{
	//���ñ���
	ZeroMemory(&RobotUserInfo,sizeof(RobotUserInfo));

	//��������
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTimeMask=(1L<<SystemTime.wHour);
	POSITION Position=m_ROBOTUserItemMap.GetStartPosition();

	//��ȡʱ��
	DWORD dwTodayTickCount= GetTodayTickCount();	

	//ö�ٶ���
	while (Position!=NULL)
{
		//��ȡ����
		DWORD dwUserID=0L;
		CRobotUserItem * pRobotUserItem=NULL;
		m_ROBOTUserItemMap.GetNextAssoc(Position,dwUserID,pRobotUserItem);

		//Ч�����
		ASSERT((dwUserID!=0L)&&(pRobotUserItem!=NULL));
		if ((dwUserID==0L)||(pRobotUserItem==NULL)) break;

		//���ж�
		if(pRobotUserItem->m_pIServerUserItem==NULL) continue;

		//��������
		IServerUserItem * pIServerUserItem=pRobotUserItem->m_pIServerUserItem;
		tagRobotParameter * pRobotParameter=pRobotUserItem->GetRobotParameter();

		//�뿪�ж�
		if ((pRobotUserItem->m_wRobotAction&ROBOT_WAITLEAVE)!=0) continue;

		//ģʽ�ж�
		if ((pRobotParameter->dwServiceMode&dwServiceMode)==0L) continue;		

		//�뿪�ж�
		if (!m_bServiceContinue && BatchServiceRemaindTime(pRobotParameter,dwTodayTickCount)==0) continue;

		//��Ϣʱ��
		DWORD dwCurrTime=(DWORD)time(NULL);
		if((pRobotUserItem->m_dwStandupTickCount+pRobotUserItem->m_dwReposeTickCount)>dwCurrTime) continue;

		//״̬�ж�
		switch (pIServerUserItem->GetUserStatus())
	{
		case US_FREE:
		{
				ASSERT(RobotUserInfo.wFreeUserCount<CountArray(RobotUserInfo.pIRobotUserFree));
				RobotUserInfo.pIRobotUserFree[RobotUserInfo.wFreeUserCount++]=pRobotUserItem;
				break;
			}
		case US_SIT:
		case US_READY:
		{
				ASSERT(RobotUserInfo.wSitdownUserCount<CountArray(RobotUserInfo.pIRobotUserSitdown));
				RobotUserInfo.pIRobotUserSitdown[RobotUserInfo.wSitdownUserCount++]=pRobotUserItem;
				break;
			}
		case US_PLAYING:
		case US_OFFLINE:
		{
				ASSERT(RobotUserInfo.wPlayUserCount<CountArray(RobotUserInfo.pIRobotUserPlay));
				RobotUserInfo.pIRobotUserPlay[RobotUserInfo.wPlayUserCount++]=pRobotUserItem;
				break;
			}
		}
	};

	return RobotUserInfo.wFreeUserCount+RobotUserInfo.wPlayUserCount+RobotUserInfo.wSitdownUserCount;
}

//��������
bool CRobotUserManager::SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	for (INT_PTR i=0;i<m_ROBOTUserItemArray.GetCount();i++)
{
		//��ȡ����
		CRobotUserItem * pRobotUserItem=m_ROBOTUserItemArray[i];

		//״̬�ж�
		if (pRobotUserItem->m_pIServerUserItem==NULL) continue;
		if (pRobotUserItem->m_RobotItemConfig.RobotAccountsInfo.dwUserID==0L) continue;

		//��Ϣ����
		try
	{
			if (pRobotUserItem->OnSocketRead(wMainCmdID,wSubCmdID,pData,wDataSize)==false)
		{
				throw 0;
			}
		}
		catch (...) 
	{
			//�������
			ASSERT(FALSE);
			CTraceService::TraceString(TEXT("SendDataToClientA Exception"), TraceLevel_Exception);

			//�Ͽ��û�
			WORD wRoundID=pRobotUserItem->m_wRoundID;
			WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;
			DeleteRobotUserItem(MAKELONG(wRobotIndex,wRoundID),true);
		}
	}

	return true;
}

//��������
bool CRobotUserManager::SendDataToClient(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	WORD wIndex=LOWORD(dwRobotID);
	CRobotUserItem * pRobotUserItem=GetRobotUserItem(wIndex);

	//����Ч��
	//ASSERT((pRobotUserItem!=NULL)&&(pRobotUserItem->m_wRoundID==HIWORD(dwRobotID)));
	if ((pRobotUserItem==NULL)||(pRobotUserItem->m_wRoundID!=HIWORD(dwRobotID))) return false;

	//��Ϣ����
	try
{
		if (pRobotUserItem->OnSocketRead(wMainCmdID,wSubCmdID,pData,wDataSize)==false)
	{
			ASSERT(FALSE);
			throw 0;
		}
	}
	catch (...) 
{
		ASSERT(FALSE);
		DeleteRobotUserItem(dwRobotID,true);

		CTraceService::TraceString(TEXT("SendDataToClientB Exception"), TraceLevel_Exception);
	}

	return true;
}

//��������
bool CRobotUserManager::SendDataToServer(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	CMD_Command Command;
	Command.wSubCmdID=wSubCmdID;
	Command.wMainCmdID=wMainCmdID;

	//��Ϣ����
	try
{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(dwRobotID,Command,pData,wDataSize,0)==false)
	{
			throw 0;
		}
	}
	catch (...) 
{
		ASSERT(FALSE);
		DeleteRobotUserItem(dwRobotID,true);

		CTraceService::TraceString(TEXT("SendDataToServer Exception"), TraceLevel_Exception);
	}

	return true;
}

//��ȡ����
CRobotUserItem * CRobotUserManager::GetRobotUserItem(WORD wIndex)
{
	//Ч������
	ASSERT(wIndex>=INDEX_ROBOT);
	if (wIndex<INDEX_ROBOT) return NULL;

	//Ч������
	ASSERT((wIndex-INDEX_ROBOT)<m_ROBOTUserItemArray.GetCount());
	if (((wIndex-INDEX_ROBOT)>=m_ROBOTUserItemArray.GetCount())) return NULL;

	//��ȡ����
	WORD wBufferIndex=wIndex-INDEX_ROBOT;
	CRobotUserItem * pRobotUserItem=m_ROBOTUserItemArray[wBufferIndex];

	return pRobotUserItem;
}

//�ͷŶ���
VOID CRobotUserManager::FreeRobotUserItem(DWORD dwUserID)
{
	//Ч�����
	ASSERT(dwUserID!=0);
	if (dwUserID==0) return;

	//��������
	CRobotUserItem * pRobotUserItem=NULL;
	m_ROBOTUserItemMap.Lookup(dwUserID,pRobotUserItem);

	//�����ж�
	if (pRobotUserItem==NULL)
{
		ASSERT(FALSE);
		return;
	}

	//��λ����
	pRobotUserItem->RepositUserItem();

	//��������
	m_ROBOTUserItemMap.RemoveKey(dwUserID);
	m_ROBOTUserItemBuffer.Add(pRobotUserItem);

	return;
}

//��ȡ����
tagRobotParameterEx * CRobotUserManager::GetRobotParameter(DWORD dwBatchID)
{
	//��������
	for (INT_PTR nIndex=0;nIndex<m_ROBOTParameterArray.GetCount();nIndex++)
{
		if (m_ROBOTParameterArray[nIndex]==NULL) continue;
		if (m_ROBOTParameterArray[nIndex]->RobotParameter.dwBatchID==dwBatchID)
	{
			return m_ROBOTParameterArray[nIndex];
		}
	}

	return NULL;
}

//��ȡ����
WORD CRobotUserManager::GetRobotParameterIndex(tagRobotParameterEx * pRobotParameter)
{
	//����У��
	if (pRobotParameter==NULL) return INVALID_WORD; 

	//��������
	for (INT_PTR nIndex=0;nIndex<m_ROBOTParameterArray.GetCount();nIndex++)
{
		if (m_ROBOTParameterArray[nIndex]==pRobotParameter)
	{
			return (WORD)nIndex;
		}
	}

	return INVALID_WORD;
}

//ת��ʱ��
DWORD CRobotUserManager::GetTodayTickCount()
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	return SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;
}

//ʣ��ʱ��
DWORD CRobotUserManager::BatchServiceRemaindTime(tagRobotParameter * pRobotParameter,DWORD dwTodayTickCount)
{
	//����У��
	ASSERT(pRobotParameter!=NULL);
	if(pRobotParameter==NULL) return 0;

	//��������
	DWORD dwEnterTime,dwLeaveTime;
	DWORD dwRemaindTime=0;

	//���ñ���
	dwEnterTime=pRobotParameter->dwEnterTime;
	dwLeaveTime=pRobotParameter->dwLeaveTime;

	//����ʱ��
	if(dwLeaveTime>dwEnterTime)
{
		if(dwTodayTickCount>=dwEnterTime && dwTodayTickCount<dwLeaveTime)
	{
			dwRemaindTime=dwLeaveTime-dwTodayTickCount;
		}
	}
	else if(dwLeaveTime<dwEnterTime)
{
		//��һ��
		if(dwTodayTickCount>=dwEnterTime)
	{
			dwRemaindTime = dwLeaveTime+24*3600-dwTodayTickCount;
		}

		//�ڶ���
		if(dwTodayTickCount< dwLeaveTime)
	{
			dwRemaindTime = dwLeaveTime-dwTodayTickCount;
		}
	}

	return dwRemaindTime;
}

//�ȴ�ʱ��
DWORD CRobotUserManager::BatchServiceWaitTime(tagRobotParameter * pRobotParameter,DWORD dwTodayTickCount)
{
	//����У��
	ASSERT(pRobotParameter!=NULL);
	if(pRobotParameter==NULL) return 0;
	if(((tagRobotParameterEx *)pRobotParameter)->bIsLoadRobot==false) return -1;

	//��������
	DWORD dwEnterTime,dwLeaveTime;

	//���ñ���
	dwEnterTime=pRobotParameter->dwEnterTime;
	dwLeaveTime=pRobotParameter->dwLeaveTime;

	//����ʱ��
	if(dwEnterTime<dwLeaveTime)
{
		if(dwTodayTickCount<dwEnterTime)
	{
			return dwEnterTime-dwTodayTickCount;
		}
	}
	else
{
		if(dwTodayTickCount<dwEnterTime && dwTodayTickCount>dwLeaveTime)
	{
			return dwEnterTime-dwTodayTickCount;
		}
	}

	return 0;
}

//�������
CRobotUserItem * CRobotUserManager::ActiveRobotUserItem(tagRobotItemConfig & RobotItemConfig)
{
	//��������
	CRobotUserItem * pRobotUserItem=NULL;
	INT_PTR nFreeItemCount=m_ROBOTUserItemBuffer.GetCount();

	//��ȡ����
	if (nFreeItemCount>0)
{
		//��ȡ����
		INT_PTR nItemPostion=nFreeItemCount-1;
		pRobotUserItem=m_ROBOTUserItemBuffer[nItemPostion];

		//ɾ������
		m_ROBOTUserItemBuffer.RemoveAt(nItemPostion);
	}

	//��������
	if (pRobotUserItem==NULL)
{
		//��Ŀ�ж�
		if (m_ROBOTUserItemArray.GetCount()>=MAX_ROBOT)
	{
			ASSERT(FALSE);
			return NULL;
		}

		try
	{
			//��������
			IRobotUserItemSink * pIRobotUserItemSink=NULL;
			
			//��������
			if (CServerRule::IsAllowRobotAttend(m_pGameServiceOption->dwServerRule))
		{
				pIRobotUserItemSink=(IRobotUserItemSink *)m_pIGameServiceManager->CreateRobotUserItemSink(IID_IRobotUserItemSink,VER_IRobotUserItemSink);
			}

			//��������
			try
		{
				pRobotUserItem=new CRobotUserItem;
			}
			catch (...)
		{
				//�������
				ASSERT(FALSE);

				//ɾ������
				SafeRelease(pIRobotUserItemSink);
			}

			//�����ж�
			if (pRobotUserItem==NULL)
		{
				//�������
				ASSERT(FALSE);

				//ɾ������
				SafeRelease(pIRobotUserItemSink);

				return NULL;
			}

			//�����û�
			if ((pIRobotUserItemSink!=NULL)&&(pIRobotUserItemSink->InitUserItemSink(pRobotUserItem)==false))
			{
				//�������
				ASSERT(FALSE);

				//ɾ������
				SafeRelease(pRobotUserItem);
				SafeRelease(pIRobotUserItemSink);

				return NULL;
			}

			//��������
			WORD wCurrentIndex=(WORD)m_ROBOTUserItemArray.Add(pRobotUserItem);

			//���ñ���
			pRobotUserItem->m_dwMinSitInterval=m_dwMinSitInterval;
			pRobotUserItem->m_dwMaxSitInterval=m_dwMaxSitInterval;

			//���ýӿ�
			pRobotUserItem->m_wRobotIndex=wCurrentIndex+INDEX_ROBOT;
			pRobotUserItem->m_pIServerUserManager=m_pIServerUserManager;
			pRobotUserItem->m_pIRobotUserItemSink=pIRobotUserItemSink;
			pRobotUserItem->m_pIRobotUserManager=QUERY_ME_INTERFACE(IRobotUserManager);
		}
		catch (...) 
	{ 
			ASSERT(FALSE);
			return NULL; 
		}
	}

	//��Ϸ����
	if((RobotItemConfig.pRobotParameter->dwSwitchMinInnings!=0L)&&(RobotItemConfig.pRobotParameter->dwSwitchMaxInnings!=0L))
{
		//��������
		DWORD dwSwitchMinInnings=RobotItemConfig.pRobotParameter->dwSwitchMinInnings;
		DWORD dwSwitchMaxInnings=RobotItemConfig.pRobotParameter->dwSwitchMaxInnings;

		//��������
		if ((dwSwitchMaxInnings-dwSwitchMinInnings)>0L)
			pRobotUserItem->m_RobotService.dwSwitchTableInnings=dwSwitchMinInnings+(DWORD)(Random_Double*(dwSwitchMaxInnings-dwSwitchMinInnings));
		else
			pRobotUserItem->m_RobotService.dwSwitchTableInnings=dwSwitchMinInnings;
	}
	else
		pRobotUserItem->m_RobotService.dwSwitchTableInnings=0;

	//ȡ�����
	pRobotUserItem->m_RobotService.wInsureDrawCount=1+(WORD)(Random_Double*3);

	//��������
	if(pRobotUserItem->m_pIRobotUserItemSink==NULL && (RobotItemConfig.pRobotParameter->dwServiceMode&ROBOT_SIMULATE)!=0)
		RobotItemConfig.pRobotParameter->dwServiceMode=ROBOT_SIMULATE;		

	//���ñ���
	pRobotUserItem->m_RobotItemConfig=RobotItemConfig;

	//��������
	m_ROBOTUserItemMap[RobotItemConfig.RobotAccountsInfo.dwUserID]=pRobotUserItem;

	return pRobotUserItem;
}

//ǿ������
VOID CRobotUserManager::ForceRobotUserItemStandup(CRobotUserItem * pRobotUserItem)
{
	//У�����
	ASSERT(pRobotUserItem!=NULL);
	if(pRobotUserItem==NULL) return;

	//���Ա���
	WORD wRoundID=pRobotUserItem->m_wRoundID;
	WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;

	//����ṹ
	CMD_GR_UserStandUp UserStandUp;
	UserStandUp.cbForceLeave=true;
	UserStandUp.wTableID=pRobotUserItem->m_pIServerUserItem->GetTableID();
	UserStandUp.wChairID=pRobotUserItem->m_pIServerUserItem->GetChairID();	

	//��������
	SendDataToServer(MAKELONG(wRobotIndex,wRoundID),MDM_GR_USER,SUB_GR_USER_STANDUP,&UserStandUp,sizeof(UserStandUp));

	return;
}


//////////////////////////////////////////////////////////////////////////////////
