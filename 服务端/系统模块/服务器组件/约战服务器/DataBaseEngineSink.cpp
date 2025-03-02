#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"
#include <afxinet.h>

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{

	try
	{
		//创建对象
		if ((m_GameBattleDBModule.GetInterface() == NULL) && (m_GameBattleDBModule.CreateInstance() == false))
		{
			ASSERT(FALSE);
			return false;
		}

		//变量定义
		tagDataBaseParameter * pGameBattleDBParameter = &m_pInitParameter->m_GameBattleDBParameter;

		//设置连接
		m_GameBattleDBModule->SetConnectionInfo(pGameBattleDBParameter->dwDataBaseAddr, pGameBattleDBParameter->wDataBasePort,
		CW2CT(pGameBattleDBParameter->szDataBaseName),CW2CT(pGameBattleDBParameter->szDataBaseUser),CW2CT(pGameBattleDBParameter->szDataBasePass));
	
		//发起连接
		m_GameBattleDBModule->OpenConnection();
		m_GameBattleDBAide.SetDataBase(m_GameBattleDBModule.GetInterface());		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_GameBattleDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_GameBattleDBModule.GetInterface() != NULL)
	{
		m_GameBattleDBModule->CloseConnection();
		m_GameBattleDBModule.CloseInstance();
	}


	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{		
	case DBR_GR_LOAD_BATTLE_OPTION:		//加载选项
		{
			return OnRequestLoadBattleOption(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
	}

	return false;
}

//加载配置
bool CDataBaseEngineSink::OnRequestLoadBattleOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@wKindID"),0);				
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleBaseOption"),true);

		//加载成功
		if (lReturnValue==DB_SUCCESS)
		{
			//变量定义
			WORD wPacketSize=0;
			BYTE cbBuffer[MAX_ASYNC_DATA];

			//变量定义						
			tagBattleBaseOptionEx * pBattleBaseOption=NULL;

			//开桌配置
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//发送信息
				if ((wPacketSize+sizeof(tagBattleBaseOption))>sizeof(cbBuffer))
				{
					break;
				}

				//读取信息
				pBattleBaseOption=(tagBattleBaseOptionEx *)(cbBuffer+wPacketSize);

				//读取信息
				pBattleBaseOption->wOptionID = m_GameBattleDBAide.GetValue_WORD(TEXT("OptionID"));
				
				//读取数据
				pBattleBaseOption->BattleOption.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
				pBattleBaseOption->BattleOption.cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));
				pBattleBaseOption->BattleOption.wPlayerCount = m_GameBattleDBAide.GetValue_WORD(TEXT("PlayerCount"));
				pBattleBaseOption->BattleOption.cbEmployScene = m_GameBattleDBAide.GetValue_BYTE(TEXT("EmployScene"));
				pBattleBaseOption->BattleOption.wOptionMask = m_GameBattleDBAide.GetValue_WORD(TEXT("OptionMask"));
				pBattleBaseOption->BattleOption.cbItemCount = m_GameBattleDBAide.GetValue_BYTE(TEXT("ItemCount"));
				pBattleBaseOption->BattleOption.wSuportPayType = m_GameBattleDBAide.GetValue_WORD(TEXT("SuportPayType"));
				pBattleBaseOption->BattleOption.wWinnerCount = m_GameBattleDBAide.GetValue_WORD(TEXT("WinnerCount"));
				pBattleBaseOption->BattleOption.lWinnerRevenue = m_GameBattleDBAide.GetValue_LONG(TEXT("WinnerRevenue"));
				pBattleBaseOption->BattleOption.lMinBaseScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MinBaseScore"));
				pBattleBaseOption->BattleOption.lMaxBaseScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MaxBaseScore"));
				pBattleBaseOption->BattleOption.lMinEntryCost = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MinEntryCost"));
				pBattleBaseOption->BattleOption.lMaxEntryCost = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MaxEntryCost"));				
	

				//设置位移
				wPacketSize+=sizeof(tagBattleBaseOptionEx);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}	

			//配置数量			
			WORD wOptionCount = wPacketSize / sizeof(tagBattleBaseOptionEx);

			//设置变量
			WORD wIndex = 0;
			auto pBattleBaseOptionEx = (tagBattleBaseOptionEx*)cbBuffer;

			for (WORD i = 0; i < wOptionCount; i++)
			{
				//构造参数
				m_GameBattleDBAide.ResetParameter();
				m_GameBattleDBAide.AddParameter(TEXT("@wOptionID"), pBattleBaseOptionEx->wOptionID);
				m_GameBattleDBAide.AddParameter(TEXT("@wItemCount"), pBattleBaseOptionEx->BattleOption.cbItemCount);
				LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleOptionItem"), true);

				//加载成功
				if (lReturnValue == DB_SUCCESS)
				{
					//开桌配置
					while (m_GameBattleDBModule->IsRecordsetEnd() == false)
					{
						//读取索引
						wIndex = m_GameBattleDBAide.GetValue_WORD(TEXT("ItemIndex"));
						if (wIndex >= CountArray(pBattleBaseOptionEx->BattleOption.OptionItem)) break;

						//读取配置
						pBattleBaseOptionEx->BattleOption.OptionItem[wIndex].wPlayCount = m_GameBattleDBAide.GetValue_WORD(TEXT("PlayCount"));
						pBattleBaseOptionEx->BattleOption.OptionItem[wIndex].lRoomCard = m_GameBattleDBAide.GetValue_LONG(TEXT("RoomCard"));
						pBattleBaseOptionEx->BattleOption.OptionItem[wIndex].lRoomCardAA = m_GameBattleDBAide.GetValue_LONG(TEXT("RoomCardAA"));						

						//移动记录
						m_GameBattleDBModule->MoveToNext();
					}
				}

				pBattleBaseOptionEx++;
			}

			//基础配置
			if (wOptionCount > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_BASE_OPTION, dwContextID, dwTokenID, cbBuffer, wPacketSize);
			}			

			//构造参数
			m_GameBattleDBAide.ResetParameter();
			m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), 0);
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleGameOption"), true);

			//加载成功
			if (lReturnValue == DB_SUCCESS)
			{
				//设置变量
				wPacketSize = 0;

				//变量定义				
				tagBattleGameOptionEx * pBattleGameOption = NULL;

				//游戏配置
				while (m_GameBattleDBModule->IsRecordsetEnd() == false)
				{
					//发送信息
					if ((wPacketSize + sizeof(tagBattleGameOptionEx)) > sizeof(cbBuffer))
					{
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_GAME_OPTION, dwContextID, dwTokenID, cbBuffer, wPacketSize);
						wPacketSize = 0;
					}

					//读取信息
					pBattleGameOption = (tagBattleGameOptionEx *)(cbBuffer + wPacketSize);

					//读取数据
					pBattleGameOption->BattleGameOption.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
					pBattleGameOption->BattleGameOption.cbEmployScene= m_GameBattleDBAide.GetValue_BYTE(TEXT("EmployScene"));
					m_GameBattleDBAide.GetValue_String(TEXT("CustomOption"), pBattleGameOption->BattleGameOption.szOptionJson, CountArray(pBattleGameOption->BattleGameOption.szOptionJson));

					//设置位移
					wPacketSize += sizeof(tagBattleGameOptionEx);

					//移动记录
					m_GameBattleDBModule->MoveToNext();
				}

				//发送剩余
				if (wPacketSize > 0)
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_GAME_OPTION, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				}
			}	


			//变量定义
			tagBattleGlobalOption BattleGlobalOption;
			ZeroMemory(&BattleGlobalOption,sizeof(BattleGlobalOption));

			//构造参数
			m_GameBattleDBAide.ResetParameter();
			
			//执行查询
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleOption"), true);

			//执行成功
			if (lReturnValue == DB_SUCCESS)
			{
				//茶馆开房间配置
				BattleGlobalOption.wClubMaxTableCount = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubMaxTableCount"));
				BattleGlobalOption.wNormalMaxTableCount = m_GameBattleDBAide.GetValue_WORD(TEXT("NormalMaxTableCount"));	

				//投递数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_GLOBAL_OPTION, dwContextID, dwTokenID, &BattleGlobalOption, sizeof(BattleGlobalOption));
			}			

			//完成通知
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_OPTION_FINISH,dwContextID, dwTokenID,NULL,0);
		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//输出信息
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	//错误信息
	char szDesc[512]="\0";
	_snprintf_s(szDesc,511,"%s(%d):%s",pFile,nLine,pFunction);
	CString strDescribe;
#ifdef _UNICODE
	TCHAR szWDesc[512]=TEXT("\0");
	int nLen=(int)strlen(szDesc);
	MultiByteToWideChar(CP_ACP,0,szDesc,nLen,szWDesc,nLen*2);
	if(pIException!=NULL)
		strDescribe.Format(TEXT("%s:(%s)"),szWDesc,pIException->GetExceptionDescribe());
	else
		strDescribe.Format(TEXT("%s:(未知错误)"),szWDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#else
	if(pIException!=NULL)
		strDescribe.Format(TEXT("%s:(%s)"),szDesc,pIException->GetExceptionDescribe());
	else
		strDescribe.Format(TEXT("%s:(未知错误)"),szDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#endif

}
//////////////////////////////////////////////////////////////////////////

