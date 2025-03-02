#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////

//数据库钩子
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//存储变量
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//缓冲数据

	//组件变量
protected:
	CDataBaseAide					m_GameBattleDBAide;					//平台数据库	
	CDataBaseHelper					m_GameBattleDBModule;				//平台数据库

	//组件变量
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineStop(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//系统功能
protected:
	//用户登录
	bool OnRequestLogonService(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//创建群组
	bool OnRequestCreateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//更新群组
	bool OnRequestUpdateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//删除群组
	bool OnRequestDeleteClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//查找群组
	bool OnRequestSearchClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//转让群组
	bool OnRequestTransferClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//更新成员
	bool OnRequestUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//删除成员
	bool OnRequestDeleteMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//申请请求
	bool OnRequestApplyRequest(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//申请响应
	bool OnRequestApplyRespond(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//申请删除
	bool OnRequestApplyDelete(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//申请撤销
	bool OnRequestApplyRepeal(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//约战结算
	bool OnRequestSettleBattle(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//修改积分
	bool OnRequestModifyScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//赠送积分
	bool OnRequestPresentScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//标记记录
	bool OnRequestDeleteRecord(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//检测名称
	bool OnRequestDetectClubName(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//清理积分
	bool OnRequestCleanupScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//清理税收
	bool OnRequestCleanupRevenue(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//清理设置
	bool OnRequestCleanupSetup(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//清理榜单
	bool OnRequestCleanupRankList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//请求合并
	bool OnRequestRequestMerge(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//响应合并
	bool OnRequestRespondMerge(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//取消代付
	bool OnRequestCancelPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//请求代付
	bool OnRequestRequestPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//响应代付
	bool OnRequestRespondPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);		
	//加载配置
	bool OnRequestLoadClubOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//加载配置
	bool OnRequestLoadForbidWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	
	//辅助函数
protected:		
	//提取群组
	WORD ExtractClubID(LPCTSTR pszClubIDArray, CDWordArray & ClubIDArray);
	//输出异常
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException);
};

//////////////////////////////////////////////////////////////////////////

#endif