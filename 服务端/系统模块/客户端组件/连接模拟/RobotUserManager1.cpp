#include "StdAfx.h"
#include "RobotUserManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//常量定义
#define ROBOT_LOGON_COUNT			32									//登录数目
#define ROBOT_PLAY_DRAW				10                 //游戏局数 

//时间常亮
#define ROBOT_UNIN_TIME				10*60								//不进入时间
#define ROBOT_UNLOAD_TIME			30*60								//不加载时间
#define ROBOT_UNDRAW_TIME			20*60								//不取款时间

//时间标识
#define IDI_ROBOT_IN				(IDI_ROBOT_MANAGER_START+0)			//进入时间
#define IDI_ROBOT_OUT				(IDI_ROBOT_MANAGER_START+MAX_BATCH)	//退出时间
#define IDI_ROBOT_PULSE				(IDI_ROBOT_MANAGER_START+2*MAX_BATCH) //脉冲时间
#define IDI_ROBOT_ACTION			(IDI_ROBOT_MANAGER_START+2*MAX_BATCH+1)//脉冲时间

//机器时间
#define TIME_ROBOT_INOUT			15L									//登录时间
#define TIME_ROBOT_PULSE			1L									//脉冲时间
#define TIME_ROBOT_REPOSE_TIME		1800L								//服务时间

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CRobotUserManager::CRobotUserManager()
{
	//系统参数
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件接口
	m_pITimerEngine=NULL;
	m_pIServiceFunction=NULL;
	m_pIServerUserManager=NULL;
	m_pIGameServiceManager=NULL;
	m_pIGameServiceSustomTime=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//配置变量	
	m_dwMinSitInterval=0;
	m_dwMaxSitInterval=0;
	m_bServiceContinue=false;
	m_wAutoRobotCount=ROBOT_LOGON_COUNT;

	//库存变量
	m_wStockCount=0;
	ZeroMemory(&m_RobotItemConfig,sizeof(m_RobotItemConfig));

	//设置索引
	m_ROBOTUserItemMap.InitHashTable(PRIME_ROBOT_USER);
	m_ROBOTUserItemMap.RemoveAll();

	return;
}

//析构函数
CRobotUserManager::~CRobotUserManager()
{
	//清理断言
	ASSERT(m_ROBOTUserItemMap.GetCount()==0L);
	ASSERT(m_ROBOTParameterArray.GetCount()==0L);
	ASSERT(m_ROBOTUserItemArray.GetCount()==0L);
	ASSERT(m_ROBOTUserItemBuffer.GetCount()==0L);

	//释放资源
	for (INT_PTR i=0; i<CRobotUserItem::m_TimerItemStorage.GetCount(); i++)
{
		SafeDelete(CRobotUserItem::m_TimerItemStorage[i]);
	}
	CRobotUserItem::m_TimerItemStorage.RemoveAll();

	return;
}

//接口查询
VOID * CRobotUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRobotUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRobotUserManager,Guid,dwQueryVer);
	return NULL;
}

//启动服务
bool CRobotUserManager::StartService()
{
	//时间间隔
	DWORD dwTimeCell=TIME_CELL;
	DWORD dwElapse=TIME_ROBOT_PULSE*1000L;

	//调整时间
	if (m_pIGameServiceSustomTime!=NULL)
{		
		dwTimeCell=m_pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell=TIME_CELL;

		dwElapse=m_pIGameServiceSustomTime->GetRobotTimerPulse();		
		if (dwElapse<dwTimeCell) dwElapse=dwTimeCell;
	}

	//启动时间
	m_pITimerEngine->SetTimer(IDI_ROBOT_PULSE,dwElapse,TIMES_INFINITY,0);

	return true;
}

//停止服务
bool CRobotUserManager::ConcludeService()
{
	//删除存储
	for (INT_PTR i=0;i<m_ROBOTUserItemArray.GetCount();i++)
{
		SafeRelease(m_ROBOTUserItemArray[i]);
	}

	//删除存储
	for (INT_PTR i=0;i<m_ROBOTParameterArray.GetCount();i++)
{
		SafeDelete(m_ROBOTParameterArray[i]);
	}

	//清理数组
	m_ROBOTUserItemMap.RemoveAll();
	m_ROBOTParameterArray.RemoveAll();
	m_ROBOTUserItemArray.RemoveAll();
	m_ROBOTUserItemBuffer.RemoveAll();

	//库存用户
	m_wStockCount=0;
	ZeroMemory(&m_RobotItemConfig,sizeof(m_RobotItemConfig));

	return true;
}

//移除参数
bool CRobotUserManager::RemoveRobotParameter(DWORD dwBatchID)
{
	//获取参数
	tagRobotParameterEx * pRobotParameter = GetRobotParameter(dwBatchID);
	if(pRobotParameter==NULL) return false;

	//调整参数
	pRobotParameter->bIsValided=false;
	pRobotParameter->RobotParameter.dwLeaveTime=GetTodayTickCount();

	//获取索引
	WORD wParameterIndex=GetRobotParameterIndex(pRobotParameter);
	if (wParameterIndex==INVALID_WORD) return true;

	//关闭定时器
	m_pITimerEngine->KillTimer(IDI_ROBOT_IN+wParameterIndex);

	//删掉库存
	for (int i=m_wStockCount-1;i>=0;i--)
{
		//批次判断
		if(m_RobotItemConfig[i].pRobotParameter!=&pRobotParameter->RobotParameter) continue;

		//删除存存
		m_wStockCount--;
		m_RobotItemConfig[i]=m_RobotItemConfig[m_wStockCount];
	}

	//加载判断
	if(pRobotParameter->bIsLoadRobot==false)
{
		//释放资源
		SafeDelete(pRobotParameter);
		m_ROBOTParameterArray[wParameterIndex]=NULL;

		return true;
	}

	//开启定时器
	m_pITimerEngine->SetTimer(IDI_ROBOT_OUT+wParameterIndex,TIME_ROBOT_INOUT*1000L,TIMES_INFINITY,(WPARAM)pRobotParameter);

	return true;
}

//设置参数
bool CRobotUserManager::AddRobotParameter(tagRobotParameter RobotParameter[], WORD wParameterCount)
{
	//变量定义
	tagRobotParameterEx * pRobotParameter=NULL;

	for(WORD wIndex=0;wIndex<wParameterCount;wIndex++)
{
		//设置资源
		pRobotParameter = GetRobotParameter(RobotParameter[wIndex].dwBatchID);
		if(pRobotParameter==NULL)
	{
			//申请资源
			pRobotParameter = new tagRobotParameterEx;
			if (pRobotParameter==NULL) return false;

			//设置变量
			pRobotParameter->bIsValided=true;
			pRobotParameter->bIsLoadRobot=false;

			//变量定义
			bool bVacancyExist=false;

			//查找空缺
			for (INT_PTR nIndex=0;nIndex<m_ROBOTParameterArray.GetCount();nIndex++)
		{
				if (m_ROBOTParameterArray[nIndex]==NULL)
			{
					bVacancyExist=true;
					m_ROBOTParameterArray[nIndex]=pRobotParameter;
					break;
				}
			}

			//不存在空缺
			if (bVacancyExist==false)
		{
				//数目限制
				if(m_ROBOTParameterArray.GetCount()>=MAX_BATCH) break;
				m_ROBOTParameterArray.Add(pRobotParameter);
			}
		}

		//拷贝数据
		CopyMemory(pRobotParameter,&RobotParameter[wIndex],sizeof(tagRobotParameter));
	}

	return true;
}

//插入机器
bool CRobotUserManager::InsertRobotInfo(tagRobotAccountsInfo RobotAccountsInfo[],WORD wRobotCount,DWORD dwBatchID)
{
	//获取参数
	tagRobotParameterEx * pRobotParameter = GetRobotParameter(dwBatchID);
	if(pRobotParameter==NULL) return false;

	//记录库存	
	CRobotUserItem * pRobotUserItem=NULL;
	WORD wStockCount=m_wStockCount;
	bool bRobotExist=false;

	CString str;
	str.Format(TEXT("机器人加载成功, BatchID=%d,RobotCount=%d"),dwBatchID,wRobotCount);
	CTraceService::TraceString(str,TraceLevel_Info);

	//添加新机器
	for (WORD wIndex=0;wIndex<wRobotCount;wIndex++)
{
		//设置变量
		bRobotExist=false;

		//从库存中查找
		for (WORD wItem=0;wItem<wStockCount;wItem++)
	{
			if (m_RobotItemConfig[wItem].RobotAccountsInfo.dwUserID==RobotAccountsInfo[wIndex].dwUserID)
		{
				bRobotExist=true;
				break;
			}
		}

		//存在判断
		if (bRobotExist==true) continue;

		//从映射中查找
		if (m_ROBOTUserItemMap.Lookup(RobotAccountsInfo[wIndex].dwUserID,pRobotUserItem)==TRUE)
	{
			continue;
		}

		//添加机器
		m_RobotItemConfig[m_wStockCount].RobotAccountsInfo=RobotAccountsInfo[wIndex];
		m_RobotItemConfig[m_wStockCount].pRobotParameter=&pRobotParameter->RobotParameter;

		//增加库存
		m_wStockCount++;
  }

	//获取索引
	WORD wParameterIndex=GetRobotParameterIndex(pRobotParameter);
	if (wParameterIndex==INVALID_WORD) return true;

	//开启定时器
	m_pITimerEngine->KillTimer(IDI_ROBOT_IN+wParameterIndex);
	m_pITimerEngine->SetTimer(IDI_ROBOT_IN+wParameterIndex,TIME_ROBOT_INOUT*1000L,TIMES_INFINITY,(WPARAM)pRobotParameter);

	return true;
}

//延缓动作
bool CRobotUserManager::PostponeRobotAction(DWORD dwUserID,WORD wActionCode)
{
	//存在判断
	for (INT_PTR i=0; i<m_ROBOTActionArray.GetCount(); i++)
{
		if (m_ROBOTActionArray[i].dwUserID==dwUserID)
	{
			return false;
		}
	}

	//变量定义
	tagRobotAction RobotAction;
	ZeroMemory(&RobotAction,sizeof(RobotAction));

	//查找机器
	CRobotUserItem * pRobotUserItem=NULL;
	m_ROBOTUserItemMap.Lookup(dwUserID,pRobotUserItem);

	//判断结果
	if (pRobotUserItem==NULL) return false;

	//机器服务
	tagRobotService * pRobotService=pRobotUserItem->GetRobotService();

	//银行取款
	if (wActionCode==ROBOT_ACTION_INSURE)
{
		//判断取款次数
		if (pRobotService->wInsureDrawCount==0)
	{
			//修改动作
			wActionCode=ROBOT_ACTION_LEAVE;
		}
		else
	{
			//递减次数
			pRobotService->wInsureDrawCount--;

			//构造结构
			RobotAction.dwUserID=dwUserID;
			RobotAction.wActionCode=ROBOT_ACTION_INSURE;
			RobotAction.dwGenerateTime=(DWORD)time(NULL);			
			RobotAction.dwDelayTime=TIME_ROBOT_INSURE_MIN+DWORD(Random_Double*(TIME_ROBOT_INSURE_MAX-TIME_ROBOT_INSURE_MIN));			
		}
	}

	//离开动作
	if (wActionCode==ROBOT_ACTION_LEAVE)
{
		//构造结构
		RobotAction.dwUserID=dwUserID;
		RobotAction.wActionCode=ROBOT_ACTION_LEAVE;
		RobotAction.dwGenerateTime=(DWORD)time(NULL);
		RobotAction.dwDelayTime=TIME_ROBOT_LEAVE_MIN+DWORD(Random_Double*(TIME_ROBOT_LEAVE_MAX-TIME_ROBOT_LEAVE_MIN));
	}

	//起立动作
	if (wActionCode==ROBOT_ACTION_STANDUP)
{
		//构造结构	
		RobotAction.dwUserID=dwUserID;
		RobotAction.wActionCode=ROBOT_ACTION_STANDUP;
		RobotAction.dwGenerateTime=(DWORD)time(NULL);
		RobotAction.dwDelayTime=TIME_ROBOT_STANDUP_MIN+DWORD(Random_Double*(TIME_ROBOT_STANDUP_MAX-TIME_ROBOT_STANDUP_MIN));
	}	

	//添加动作
	m_ROBOTActionArray.Add(RobotAction);

	//设置定时器
	m_pITimerEngine->SetTimer(IDI_ROBOT_ACTION,1000L,TIMES_INFINITY,0);

	return true;
}

//删除机器
bool CRobotUserManager::DeleteRobotUserItem(DWORD dwRobotID,bool bStockRetrieve)
{
	//获取对象
	WORD wIndex=LOWORD(dwRobotID);
	CRobotUserItem * pRobotUserItem=GetRobotUserItem(wIndex);

	//对象效验
	//ASSERT((pRobotUserItem!=NULL)&&(pRobotUserItem->m_wRoundID==HIWORD(dwRobotID)));
	if ((pRobotUserItem==NULL)||(pRobotUserItem->m_wRoundID!=HIWORD(dwRobotID))) return false;

	//关闭事件
	try
{
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwRobotID,0,0L);
	}
	catch (...)
{
		//错误断言
		ASSERT(FALSE);
	}

	//机器配置
	tagRobotItemConfig RobotItemConfig=pRobotUserItem->m_RobotItemConfig;

	//删除对象
	FreeRobotUserItem(pRobotUserItem->GetUserID());

	//库存回收
	if(bStockRetrieve==true)
{		
		InsertRobotInfo(&RobotItemConfig.RobotAccountsInfo,1,RobotItemConfig.pRobotParameter->dwBatchID);
	}

	return true;
}

//查找机器
IRobotUserItem * CRobotUserManager::SearchRobotUserItem(DWORD dwUserID, DWORD dwContextID)
{
	//查找机器
	CRobotUserItem * pRobotUserItem=NULL;
	m_ROBOTUserItemMap.Lookup(dwUserID,pRobotUserItem);

	//机器判断
	if (pRobotUserItem!=NULL)
{
		WORD wRoundID=pRobotUserItem->m_wRoundID;
		WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;
		if (MAKELONG(wRobotIndex,wRoundID)==dwContextID) return pRobotUserItem;
	}

	return NULL;
}

//创建机器
IRobotUserItem * CRobotUserManager::CreateRobotUserItem(tagRobotItemConfig & RobotItemConfig)
{
	//效验参数
	ASSERT (RobotItemConfig.RobotAccountsInfo.dwUserID!=0L);
	if (RobotItemConfig.RobotAccountsInfo.dwUserID==0L) return NULL;

	//创建对象
	CRobotUserItem * pRobotUserItem=ActiveRobotUserItem(RobotItemConfig);
	if (pRobotUserItem==NULL) return NULL;

	//属性变量
	WORD wRoundID=pRobotUserItem->m_wRoundID;
	WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;

	//连接模拟
	try
{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(MAKELONG(wRobotIndex,wRoundID),0L)==false)
	{
			throw 0;
		}
	}
	catch (...)
{
		//错误断言
		ASSERT(FALSE);

		//释放用户
		FreeRobotUserItem(RobotItemConfig.RobotAccountsInfo.dwUserID);

		return NULL;
	}

	//变量定义
	CMD_GR_LogonRobot LogonRobot;
	ZeroMemory(&LogonRobot,sizeof(LogonRobot));

	//设置数据	
	LogonRobot.dwPlazaVersion=VERSION_PLAZA;
	LogonRobot.dwFrameVersion=VERSION_FRAME;
	LogonRobot.dwProcessVersion=m_pGameServiceAttrib->dwClientVersion;

	//设置数据	
	LogonRobot.dwUserID=RobotItemConfig.RobotAccountsInfo.dwUserID;
	StringCchCopy(LogonRobot.szPassword,CountArray(LogonRobot.szPassword), RobotItemConfig.RobotAccountsInfo.szPassword);

	//构造数据
	CMD_Command Command;
	Command.wMainCmdID=MDM_GR_LOGON;
	Command.wSubCmdID=SUB_GR_LOGON_ROBOT;

	//消息处理
	try
{
		//发送数据
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(MAKELONG(wRobotIndex,wRoundID),Command,&LogonRobot,sizeof(LogonRobot),0)==false)
	{
			throw 0;
		}
	}
	catch (...) 
{
		//错误断言
		ASSERT(FALSE);

		//删除机器
		DeleteRobotUserItem(MAKELONG(wRobotIndex,wRoundID),false);

		CTraceService::TraceString(TEXT("CreateRobotUserItem Exception"), TraceLevel_Exception);

		return NULL;
	}

	return pRobotUserItem;
}

//脉冲事件
bool CRobotUserManager::OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//进入处理
	if (dwTimerID>=IDI_ROBOT_IN && dwTimerID<IDI_ROBOT_IN+MAX_BATCH)			
{
		//获取时间		
		DWORD dwTodayTickCount= GetTodayTickCount();

		//获取参数
		tagRobotParameter * pRobotParameter=(tagRobotParameter *)dwBindParameter;
		if(pRobotParameter==NULL)
	{
			//关闭定时器
			m_pITimerEngine->KillTimer(dwTimerID);

			return true;
		}

		//变量定义
		bool bAllowRobotAttend=CServerRule::IsAllowRobotAttend(m_pGameServiceOption->dwServerRule);
		bool bAllowRobotSimulate=CServerRule::IsAllowRobotSimulate(m_pGameServiceOption->dwServerRule);

		//成功标识
		bool bLogonSuccessed=false;

		//登录处理
		if ((bAllowRobotAttend==true)||(bAllowRobotSimulate==true))
	{
			//寻找机器
			for (WORD i=0;i<m_wStockCount;i++)
		{
				//批次判断
				if(m_RobotItemConfig[i].pRobotParameter!=pRobotParameter) continue;

				//模拟判断
				if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
					&& (pRobotParameter->dwServiceMode&ROBOT_SIMULATE)!=0
					&& (pRobotParameter->dwServiceMode&ROBOT_PASSIVITY)==0
					&& (pRobotParameter->dwServiceMode&ROBOT_INITIATIVE)==0)
			{
					continue;
				}

				//创建机器
				if (CreateRobotUserItem(m_RobotItemConfig[i])!=NULL)
			{					
					//删除存存
					m_wStockCount--;
					bLogonSuccessed=true;
					m_RobotItemConfig[i]=m_RobotItemConfig[m_wStockCount];

					break;
				}
			}
		}

		//关闭定时器
		m_pITimerEngine->KillTimer(dwTimerID);

		//定时器处理
		if(bLogonSuccessed==true)
	{
			//流逝时间
			DWORD dwElapse = pRobotParameter->dwEnterMinInterval+(DWORD)(Random_Double*(pRobotParameter->dwEnterMaxInterval-pRobotParameter->dwEnterMinInterval));
			if(dwElapse==0) dwElapse=TIME_ROBOT_INOUT;

			//设置定时器
			m_pITimerEngine->SetTimer(dwTimerID,dwElapse*1000L,TIMES_INFINITY,dwBindParameter);
		}
		else
	{
			//剩余时间
			DWORD dwRemaindTime = BatchServiceRemaindTime(pRobotParameter,dwTodayTickCount);
			if(dwRemaindTime<=ROBOT_UNIN_TIME)
		{
				//设置定时器
				m_pITimerEngine->SetTimer(IDI_ROBOT_OUT+(dwTimerID-IDI_ROBOT_IN),TIME_ROBOT_INOUT*1000L,TIMES_INFINITY,dwBindParameter);				
			}
			else
		{
				//设置定时器
				m_pITimerEngine->SetTimer(dwTimerID,TIME_ROBOT_INOUT*1000L,TIMES_INFINITY,dwBindParameter);				
			}
		}

		return true;
	}

	//退出处理
	if (dwTimerID>=IDI_ROBOT_OUT && dwTimerID<IDI_ROBOT_OUT+MAX_BATCH)
{
		//获取参数
		tagRobotParameterEx * pRobotParameter=(tagRobotParameterEx *)dwBindParameter;
		if(pRobotParameter==NULL)
	{
			//关闭定时器
			m_pITimerEngine->KillTimer(dwTimerID);

			return true;
		}

		//获取时间
		DWORD dwTodayTickCount= GetTodayTickCount();	

		//时间判断
		if (BatchServiceRemaindTime(&pRobotParameter->RobotParameter,dwTodayTickCount)>0) 
	{			
			return true;
		}

		//变量定义
		bool bLogoutMarked=false;
		bool bLogoutFinished=true;	

		//变量定义
		DWORD dwUserID=0L;
		INT_PTR nActiveCount=m_ROBOTUserItemMap.GetCount();
		POSITION Position=m_ROBOTUserItemMap.GetStartPosition();

		//退出处理		
		while (Position!=NULL)
	{
			//获取对象
			CRobotUserItem * pRobotUserItem=NULL;
			m_ROBOTUserItemMap.GetNextAssoc(Position,dwUserID,pRobotUserItem);

			//退出判断
			if (pRobotUserItem->GetRobotParameter()==&pRobotParameter->RobotParameter)
		{
				//设置变量
				bLogoutFinished=false;

				//变量定义
				IServerUserItem * pIServerUserItem=pRobotUserItem->m_pIServerUserItem;
				if (pIServerUserItem!=NULL)
			{
					//服务状态
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
							//标记动作
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

				//删除用户
				WORD wRoundID=pRobotUserItem->m_wRoundID;
				WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;
				DeleteRobotUserItem(MAKELONG(wRobotIndex,wRoundID),false);				

				//设置标识
				if(pIServerUserItem!=NULL) break;
			}			
		}

		//关闭定时器
		m_pITimerEngine->KillTimer(dwTimerID);

		//定时器处理
		if(bLogoutFinished==false)
	{
			//计算流逝时间
			DWORD dwElapse = pRobotParameter->RobotParameter.dwLeaveMinInterval+(DWORD)(Random_Double*(pRobotParameter->RobotParameter.dwLeaveMaxInterval-pRobotParameter->RobotParameter.dwLeaveMinInterval));
			if(dwElapse==0) dwElapse=TIME_ROBOT_INOUT;

			//设置定时器
			m_pITimerEngine->SetTimer(dwTimerID,dwElapse*1000L,TIMES_INFINITY,dwBindParameter);
		}
		else
	{
			if(pRobotParameter->bIsValided==false)
		{
				//计算索引
				WORD wParameterIndex=(WORD)(dwTimerID-IDI_ROBOT_OUT);
				if(wParameterIndex==GetRobotParameterIndex(pRobotParameter))
			{
					CString str;
					str.Format(TEXT("本批次机器人全部退出,该批次将被删除, 批次ID=%d"),pRobotParameter->RobotParameter.dwBatchID);
					CTraceService::TraceString(str,TraceLevel_Info);

					//释放资源
					SafeDelete(m_ROBOTParameterArray[wParameterIndex]);
					m_ROBOTParameterArray[wParameterIndex]=NULL;
				}

			}
			else
		{
				//恢复标识
				pRobotParameter->bIsLoadRobot=false;
				
				CString str;
				str.Format(TEXT("本批次机器人全部退出, 批次ID=%d"),pRobotParameter->RobotParameter.dwBatchID);
				CTraceService::TraceString(str,TraceLevel_Info);
			}
		}

		return true;
	}

	//机器动作
	if (dwTimerID==IDI_ROBOT_ACTION)
{
		//获取时间
		DWORD dwElapseTime=INVALID_DWORD;
		DWORD dwCurrentTime=(DWORD)time(NULL);

		//遍历动作
		for (INT_PTR i=m_ROBOTActionArray.GetCount()-1; i>=0; i--)
	{
			//获取动作
			tagRobotAction * pRobotAction=&m_ROBOTActionArray[i];

			//判断时间
			if (pRobotAction->dwGenerateTime+pRobotAction->dwDelayTime>dwCurrentTime)
		{
				DWORD dwLeaveTime=pRobotAction->dwGenerateTime+pRobotAction->dwDelayTime-dwCurrentTime;
				if (dwLeaveTime<dwElapseTime) dwElapseTime=dwLeaveTime;
				continue;
			}

			//查找机器
			CRobotUserItem * pRobotUserItem=NULL;
			m_ROBOTUserItemMap.Lookup(pRobotAction->dwUserID,pRobotUserItem);

			//查找指针
			IServerUserItem * pIServerUserItem=m_pIServerUserManager->SearchUserItem(pRobotAction->dwUserID);

			if (pIServerUserItem!=NULL && pRobotUserItem!=NULL)
		{
				switch (pRobotAction->wActionCode)
			{
				case ROBOT_ACTION_LEAVE:		//离开动作
				{
						//删除机器
						DeleteRobotUserItem(MAKELONG(pRobotUserItem->m_wRobotIndex,pRobotUserItem->m_wRoundID),false);

						break;
					}
				case ROBOT_ACTION_STANDUP:	//起立动作
				{
						//强制起立
						ForceRobotUserItemStandup(pRobotUserItem);

						break;
					}
				case ROBOT_ACTION_INSURE:		//取款动作
				{
						//获取时间		
						DWORD dwTodayTickCount=GetTodayTickCount();

						//获取配置
						tagRobotParameter * pRobotParameter=pRobotUserItem->GetRobotParameter();

						//剩余时间
						DWORD dwRemaindTime = BatchServiceRemaindTime(pRobotParameter,dwTodayTickCount);
						if (dwRemaindTime<=ROBOT_UNDRAW_TIME)
					{
							//删除机器
							DeleteRobotUserItem(MAKELONG(pRobotUserItem->m_wRobotIndex,pRobotUserItem->m_wRoundID),false);
						}
						else
					{
							//删除机器
							DeleteRobotUserItem(MAKELONG(pRobotUserItem->m_wRobotIndex,pRobotUserItem->m_wRoundID),true);
						}

						break;
					}
				}

				//删除动作
				m_ROBOTActionArray.RemoveAt(i);
			}

		}

		//设置定时器
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

	//用户脉冲
	if (dwTimerID==IDI_ROBOT_PULSE)		
{
		//变量定义
		DWORD dwUserID=0L;
		CRobotUserItem * pRobotUserItem=NULL;
		POSITION Position=m_ROBOTUserItemMap.GetStartPosition();

		//用户处理
		while (Position!=NULL)
	{
			//获取对象
			m_ROBOTUserItemMap.GetNextAssoc(Position,dwUserID,pRobotUserItem);

			//时间处理
			try
		{
				if (pRobotUserItem->m_pIServerUserItem!=NULL) pRobotUserItem->OnTimerPulse(dwTimerID,dwBindParameter);
			}
			catch (...)
		{
				//变量定义
				TCHAR szException[128];
				_sntprintf_s(szException,CountArray(szException),TEXT("机器人时钟调度异常: TimerID=%d"),pRobotUserItem->m_nCurrTimerID);

				//提示消息
				CTraceService::TraceString(szException,TraceLevel_Exception);

				//删除机器
				DeleteRobotUserItem(MAKELONG(pRobotUserItem->m_wRobotIndex,pRobotUserItem->m_wRoundID),true);				
			}
		}

		return true;
	}

	return false;
}

//加载机器
bool CRobotUserManager::GetRobotLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount)
{
	//数目判断
	if(m_ROBOTParameterArray.GetCount()==0) return false;
	
	//变量定义
	DWORD dwRemaindTime;
	DWORD dwTodayTickCount= GetTodayTickCount();
	tagRobotParameterEx * pRobotParameter=NULL;		

	//退出处理
	for (INT_PTR nIndex=0;nIndex<m_ROBOTParameterArray.GetCount();nIndex++)
{
		//获取对象		
		pRobotParameter = m_ROBOTParameterArray[nIndex];
		if (pRobotParameter==NULL) continue;

		//加载判断
		if (pRobotParameter->bIsLoadRobot==true) continue;

		//剩余时间
		dwRemaindTime=BatchServiceRemaindTime(&pRobotParameter->RobotParameter,dwTodayTickCount);

		//加载机器
		if (dwRemaindTime>ROBOT_UNLOAD_TIME)
	{
			//设置参数
			dwBatchID = pRobotParameter->RobotParameter.dwBatchID;
			dwLoadCount = __min(pRobotParameter->RobotParameter.dwRobotCount,dwRemaindTime/pRobotParameter->RobotParameter.dwEnterMaxInterval);

			if (dwLoadCount>0) 
		{
				//设置标识
				pRobotParameter->bIsLoadRobot=true;

				return true;
			}
		}
	}

	return false;
}

//用户状况
WORD CRobotUserManager::GetRobotUserInfo(tagRobotUserInfo & RobotUserInfo, DWORD dwServiceMode)
{
	//设置变量
	ZeroMemory(&RobotUserInfo,sizeof(RobotUserInfo));

	//变量定义
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTimeMask=(1L<<SystemTime.wHour);
	POSITION Position=m_ROBOTUserItemMap.GetStartPosition();

	//获取时间
	DWORD dwTodayTickCount= GetTodayTickCount();	

	//枚举对象
	while (Position!=NULL)
{
		//获取对象
		DWORD dwUserID=0L;
		CRobotUserItem * pRobotUserItem=NULL;
		m_ROBOTUserItemMap.GetNextAssoc(Position,dwUserID,pRobotUserItem);

		//效验参数
		ASSERT((dwUserID!=0L)&&(pRobotUserItem!=NULL));
		if ((dwUserID==0L)||(pRobotUserItem==NULL)) break;

		//绑定判断
		if(pRobotUserItem->m_pIServerUserItem==NULL) continue;

		//变量定义
		IServerUserItem * pIServerUserItem=pRobotUserItem->m_pIServerUserItem;
		tagRobotParameter * pRobotParameter=pRobotUserItem->GetRobotParameter();

		//离开判断
		if ((pRobotUserItem->m_wRobotAction&ROBOT_WAITLEAVE)!=0) continue;

		//模式判断
		if ((pRobotParameter->dwServiceMode&dwServiceMode)==0L) continue;		

		//离开判断
		if (!m_bServiceContinue && BatchServiceRemaindTime(pRobotParameter,dwTodayTickCount)==0) continue;

		//休息时间
		DWORD dwCurrTime=(DWORD)time(NULL);
		if((pRobotUserItem->m_dwStandupTickCount+pRobotUserItem->m_dwReposeTickCount)>dwCurrTime) continue;

		//状态判断
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

//发送数据
bool CRobotUserManager::SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//发送数据
	for (INT_PTR i=0;i<m_ROBOTUserItemArray.GetCount();i++)
{
		//获取对象
		CRobotUserItem * pRobotUserItem=m_ROBOTUserItemArray[i];

		//状态判断
		if (pRobotUserItem->m_pIServerUserItem==NULL) continue;
		if (pRobotUserItem->m_RobotItemConfig.RobotAccountsInfo.dwUserID==0L) continue;

		//消息处理
		try
	{
			if (pRobotUserItem->OnSocketRead(wMainCmdID,wSubCmdID,pData,wDataSize)==false)
		{
				throw 0;
			}
		}
		catch (...) 
	{
			//错误断言
			ASSERT(FALSE);
			CTraceService::TraceString(TEXT("SendDataToClientA Exception"), TraceLevel_Exception);

			//断开用户
			WORD wRoundID=pRobotUserItem->m_wRoundID;
			WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;
			DeleteRobotUserItem(MAKELONG(wRobotIndex,wRoundID),true);
		}
	}

	return true;
}

//发送数据
bool CRobotUserManager::SendDataToClient(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//获取对象
	WORD wIndex=LOWORD(dwRobotID);
	CRobotUserItem * pRobotUserItem=GetRobotUserItem(wIndex);

	//对象效验
	//ASSERT((pRobotUserItem!=NULL)&&(pRobotUserItem->m_wRoundID==HIWORD(dwRobotID)));
	if ((pRobotUserItem==NULL)||(pRobotUserItem->m_wRoundID!=HIWORD(dwRobotID))) return false;

	//消息处理
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

//发送数据
bool CRobotUserManager::SendDataToServer(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//构造数据
	CMD_Command Command;
	Command.wSubCmdID=wSubCmdID;
	Command.wMainCmdID=wMainCmdID;

	//消息处理
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

//获取对象
CRobotUserItem * CRobotUserManager::GetRobotUserItem(WORD wIndex)
{
	//效验索引
	ASSERT(wIndex>=INDEX_ROBOT);
	if (wIndex<INDEX_ROBOT) return NULL;

	//效验索引
	ASSERT((wIndex-INDEX_ROBOT)<m_ROBOTUserItemArray.GetCount());
	if (((wIndex-INDEX_ROBOT)>=m_ROBOTUserItemArray.GetCount())) return NULL;

	//获取对象
	WORD wBufferIndex=wIndex-INDEX_ROBOT;
	CRobotUserItem * pRobotUserItem=m_ROBOTUserItemArray[wBufferIndex];

	return pRobotUserItem;
}

//释放对象
VOID CRobotUserManager::FreeRobotUserItem(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0);
	if (dwUserID==0) return;

	//变量定义
	CRobotUserItem * pRobotUserItem=NULL;
	m_ROBOTUserItemMap.Lookup(dwUserID,pRobotUserItem);

	//对象判断
	if (pRobotUserItem==NULL)
{
		ASSERT(FALSE);
		return;
	}

	//复位对象
	pRobotUserItem->RepositUserItem();

	//设置索引
	m_ROBOTUserItemMap.RemoveKey(dwUserID);
	m_ROBOTUserItemBuffer.Add(pRobotUserItem);

	return;
}

//获取参数
tagRobotParameterEx * CRobotUserManager::GetRobotParameter(DWORD dwBatchID)
{
	//查找数组
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

//获取索引
WORD CRobotUserManager::GetRobotParameterIndex(tagRobotParameterEx * pRobotParameter)
{
	//参数校验
	if (pRobotParameter==NULL) return INVALID_WORD; 

	//查找数组
	for (INT_PTR nIndex=0;nIndex<m_ROBOTParameterArray.GetCount();nIndex++)
{
		if (m_ROBOTParameterArray[nIndex]==pRobotParameter)
	{
			return (WORD)nIndex;
		}
	}

	return INVALID_WORD;
}

//转换时间
DWORD CRobotUserManager::GetTodayTickCount()
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	return SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;
}

//剩余时间
DWORD CRobotUserManager::BatchServiceRemaindTime(tagRobotParameter * pRobotParameter,DWORD dwTodayTickCount)
{
	//参数校验
	ASSERT(pRobotParameter!=NULL);
	if(pRobotParameter==NULL) return 0;

	//变量定义
	DWORD dwEnterTime,dwLeaveTime;
	DWORD dwRemaindTime=0;

	//设置变量
	dwEnterTime=pRobotParameter->dwEnterTime;
	dwLeaveTime=pRobotParameter->dwLeaveTime;

	//计算时间
	if(dwLeaveTime>dwEnterTime)
{
		if(dwTodayTickCount>=dwEnterTime && dwTodayTickCount<dwLeaveTime)
	{
			dwRemaindTime=dwLeaveTime-dwTodayTickCount;
		}
	}
	else if(dwLeaveTime<dwEnterTime)
{
		//第一天
		if(dwTodayTickCount>=dwEnterTime)
	{
			dwRemaindTime = dwLeaveTime+24*3600-dwTodayTickCount;
		}

		//第二天
		if(dwTodayTickCount< dwLeaveTime)
	{
			dwRemaindTime = dwLeaveTime-dwTodayTickCount;
		}
	}

	return dwRemaindTime;
}

//等待时间
DWORD CRobotUserManager::BatchServiceWaitTime(tagRobotParameter * pRobotParameter,DWORD dwTodayTickCount)
{
	//参数校验
	ASSERT(pRobotParameter!=NULL);
	if(pRobotParameter==NULL) return 0;
	if(((tagRobotParameterEx *)pRobotParameter)->bIsLoadRobot==false) return -1;

	//变量定义
	DWORD dwEnterTime,dwLeaveTime;

	//设置变量
	dwEnterTime=pRobotParameter->dwEnterTime;
	dwLeaveTime=pRobotParameter->dwLeaveTime;

	//计算时间
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

//激活对象
CRobotUserItem * CRobotUserManager::ActiveRobotUserItem(tagRobotItemConfig & RobotItemConfig)
{
	//变量定义
	CRobotUserItem * pRobotUserItem=NULL;
	INT_PTR nFreeItemCount=m_ROBOTUserItemBuffer.GetCount();

	//获取对象
	if (nFreeItemCount>0)
{
		//获取对象
		INT_PTR nItemPostion=nFreeItemCount-1;
		pRobotUserItem=m_ROBOTUserItemBuffer[nItemPostion];

		//删除数组
		m_ROBOTUserItemBuffer.RemoveAt(nItemPostion);
	}

	//创建对象
	if (pRobotUserItem==NULL)
{
		//数目判断
		if (m_ROBOTUserItemArray.GetCount()>=MAX_ROBOT)
	{
			ASSERT(FALSE);
			return NULL;
		}

		try
	{
			//变量定义
			IRobotUserItemSink * pIRobotUserItemSink=NULL;
			
			//创建陪玩
			if (CServerRule::IsAllowRobotAttend(m_pGameServiceOption->dwServerRule))
		{
				pIRobotUserItemSink=(IRobotUserItemSink *)m_pIGameServiceManager->CreateRobotUserItemSink(IID_IRobotUserItemSink,VER_IRobotUserItemSink);
			}

			//创建对象
			try
		{
				pRobotUserItem=new CRobotUserItem;
			}
			catch (...)
		{
				//错误断言
				ASSERT(FALSE);

				//删除对象
				SafeRelease(pIRobotUserItemSink);
			}

			//错误判断
			if (pRobotUserItem==NULL)
		{
				//错误断言
				ASSERT(FALSE);

				//删除对象
				SafeRelease(pIRobotUserItemSink);

				return NULL;
			}

			//设置用户
			if ((pIRobotUserItemSink!=NULL)&&(pIRobotUserItemSink->InitUserItemSink(pRobotUserItem)==false))
			{
				//错误断言
				ASSERT(FALSE);

				//删除对象
				SafeRelease(pRobotUserItem);
				SafeRelease(pIRobotUserItemSink);

				return NULL;
			}

			//设置数组
			WORD wCurrentIndex=(WORD)m_ROBOTUserItemArray.Add(pRobotUserItem);

			//配置变量
			pRobotUserItem->m_dwMinSitInterval=m_dwMinSitInterval;
			pRobotUserItem->m_dwMaxSitInterval=m_dwMaxSitInterval;

			//设置接口
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

	//游戏局数
	if((RobotItemConfig.pRobotParameter->dwSwitchMinInnings!=0L)&&(RobotItemConfig.pRobotParameter->dwSwitchMaxInnings!=0L))
{
		//变量定义
		DWORD dwSwitchMinInnings=RobotItemConfig.pRobotParameter->dwSwitchMinInnings;
		DWORD dwSwitchMaxInnings=RobotItemConfig.pRobotParameter->dwSwitchMaxInnings;

		//调整局数
		if ((dwSwitchMaxInnings-dwSwitchMinInnings)>0L)
			pRobotUserItem->m_RobotService.dwSwitchTableInnings=dwSwitchMinInnings+(DWORD)(Random_Double*(dwSwitchMaxInnings-dwSwitchMinInnings));
		else
			pRobotUserItem->m_RobotService.dwSwitchTableInnings=dwSwitchMinInnings;
	}
	else
		pRobotUserItem->m_RobotService.dwSwitchTableInnings=0;

	//取款次数
	pRobotUserItem->m_RobotService.wInsureDrawCount=1+(WORD)(Random_Double*3);

	//服务类型
	if(pRobotUserItem->m_pIRobotUserItemSink==NULL && (RobotItemConfig.pRobotParameter->dwServiceMode&ROBOT_SIMULATE)!=0)
		RobotItemConfig.pRobotParameter->dwServiceMode=ROBOT_SIMULATE;		

	//设置变量
	pRobotUserItem->m_RobotItemConfig=RobotItemConfig;

	//设置索引
	m_ROBOTUserItemMap[RobotItemConfig.RobotAccountsInfo.dwUserID]=pRobotUserItem;

	return pRobotUserItem;
}

//强制起立
VOID CRobotUserManager::ForceRobotUserItemStandup(CRobotUserItem * pRobotUserItem)
{
	//校验参数
	ASSERT(pRobotUserItem!=NULL);
	if(pRobotUserItem==NULL) return;

	//属性变量
	WORD wRoundID=pRobotUserItem->m_wRoundID;
	WORD wRobotIndex=pRobotUserItem->m_wRobotIndex;

	//构造结构
	CMD_GR_UserStandUp UserStandUp;
	UserStandUp.cbForceLeave=true;
	UserStandUp.wTableID=pRobotUserItem->m_pIServerUserItem->GetTableID();
	UserStandUp.wChairID=pRobotUserItem->m_pIServerUserItem->GetChairID();	

	//发送数据
	SendDataToServer(MAKELONG(wRobotIndex,wRoundID),MDM_GR_USER,SUB_GR_USER_STANDUP,&UserStandUp,sizeof(UserStandUp));

	return;
}


//////////////////////////////////////////////////////////////////////////////////
