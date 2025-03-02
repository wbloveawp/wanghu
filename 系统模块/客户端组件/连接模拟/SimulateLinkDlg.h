#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "SimulateClient.h"
#include "SkinRichEditCtrl.h"
#include "RobotUserManager.h"

//////////////////////////////////////////////////////////////////////////
//时钟定义
#define IDI_CONNECT			1											//连接标识

//时间定义
#define TIME_CONNECT		25											//连接时间

//消息定义
#define WM_STATUS_CLIENT_USER		(WM_USER+100)						//状态消息

//连接模式
#define CONNECT_MODE_QUEUE				1								//队列模式	
#define CONNECT_MODE_CONCURRENT			2								//并发模式	

//测试模式
#define TEST_MODE_LOGON					1								//登录测试
#define TEST_MODE_GAME					2								//游戏测试
#define TEST_MODE_CLUB					3								//茶社测试
#define TEST_MODE_BATTLE				4								//约战测试

//////////////////////////////////////////////////////////////////////////
//类型定义

//游戏子项
struct tagGameItem
{
	WORD							wKindID;							//类型标识	
	DWORD							dwVersionID;						//游戏版本
	TCHAR							szGameName[32];						//游戏名称
	TCHAR							szRobotDllName[64];					//组件名称
};

//////////////////////////////////////////////////////////////////////////
//类型定义 
typedef CWHStockList<tagGameItem>  CStockGameList;
typedef CWHArray<CSimulateClient*> CSimulateClientArray;

//////////////////////////////////////////////////////////////////////////

//主对话框
class CSimulateLinkDlg : public CDialog
{
	//友元定义
	friend class CSimulateClient;

	//状态变量
protected:
	WORD							m_wConnectIndex;					//连接索引

	//连接信息
protected:
	WORD							m_wAccountsNum;						//用户数目	

	//配置变量
protected:
	WORD							m_wServerPort;						//服务断开
	TCHAR							m_szServerAddr[32];					//服务地址

	//控件变量
protected:
	CStockGameList					m_StockGameList;					//游戏列表	
	CSimulateClientArray			m_SimulateClientArray;				//用户数组

	//控件变量
public:
	CSkinRichEditCtrl				m_RichEditCtrl;						//信息窗口

	//函数定义
public:
	//构造函数
	CSimulateLinkDlg();
    //析构函数
	virtual ~CSimulateLinkDlg();

	//重载函数
protected:
	//创建函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);

	//按钮消息
protected:
	//停止按钮
	VOID OnBnClickedStop();
	//开始按钮
	VOID OnBnClickedStart();

	//辅助函数
protected:
	//加载配置
	VOID LoadConfig();

	//系统消息
protected:
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);
	//状态消息
	LRESULT OnStatusClientUser(WPARAM wParam, LPARAM lParam);

	//消息事件
public:
	//变更事件
	afx_msg void OnCbnSelchangeComboGameName();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
