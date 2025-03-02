#ifndef DLG_BANK_HEAD_FILE
#define DLG_BANK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "ClientControlsHead.h"
#include "..\..\全局定义\array.h"
#include "..\界面控件\SkinControlsHead.h"
//#include "..\游戏广场\ServiceManager.h"
//////////////////////////////////////////////////////////////////////////

//类说明
class CBankItem;
struct tagGlobalUserData;

//数组说明
typedef CArrayTemplate<CBankItem *> CBankItemArray;

//////////////////////////////////////////////////////////////////////////

//选择控制
class CLIENT_CONTROLS_CLASS CBankBar : public CWnd
{
	//活动信息
protected:
	bool							m_bHovering;						//盘旋标志
	WORD							m_wCollocateDown;					//按下子项
	WORD							m_wCollocateHover;					//盘旋子项
	WORD							m_wCollocateActive;					//激活子项

	//子项信息
public:
	CRect							m_rcItemCreate;						//创建位置
	CBankItemArray					m_CollocateItemArray;				//控制数组

	//资源变量
protected:
	CSize							m_SizeButton;						//按钮大小
	CBitImage						m_ImageBackGroud;					//背景视图
	IPlatformResource *				m_pIPlatformResource;				//框架资源
	tagPlazaCollocateImage			m_PlazaCollocateImage;				//框架资源

	//函数定义
public:
	//构造函数
	CBankBar(IPlatformResource *pIPlatformResource);
	//析构函数
	virtual ~CBankBar();

	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource();

	//参数控制
public:
	//保存参数
	bool SaveParameter();
	//默认参数
	bool DefaultParameter();

	//功能函数
public:
	//设置区域
	bool SetItemCreateRect(CRect rcItemCreate);
	//插入子项
	bool InsertCollocateItem(CBankItem * pCollocateItem);
	//激活设置
	bool SetActiveCollocateItem(CBankItem * pCollocateItem);
	//取当前项
	CBankItem * GetActiveCollocateItem();

	//内部函数
private:
	//索引切换
	WORD SwitchToButtonIndex(CPoint MousePoint);

	//消息函数
protected:
	//重画消息
	VOID OnPaint();
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标移动
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标离开
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//配置子项
class  CLIENT_CONTROLS_CLASS CBankItem : public CDialog
{
	//环绕资源
protected:
	tagEncircleInfo					m_EncircleInfoFrame;				//框架信息
	tagEncircleImage				m_EncircleImageFrame;				//框架资源
	IPlatformResource *				m_pIPlatformResource;				//框架资源
	tagPlazaCollocateImage			m_PlazaCollocateImage;				//框架资源
	tagGlobalUserData *				m_pGlobalUserData;					//用户指针

public:
	IBankSink *						m_pIBankSink;						//回调指针

	//函数定义
public:
	//构造函数
	CBankItem(UINT uIDTemplate,IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//析构函数
	virtual ~CBankItem();

	//重载函数
protected:
	//确定消息
	virtual VOID OnOK() { GetParent()->PostMessage(WM_COMMAND,IDOK,0); }
	//取消消息
	virtual VOID OnCancel() { GetParent()->PostMessage(WM_COMMAND,IDCANCEL,0); }

	//重载函数
public:
	//保存参数
	virtual bool SaveParameter()=NULL;
	//默认参数
	virtual bool DefaultParameter()=NULL;
	//更新控制
	virtual bool UpdateControlStatus()=NULL;
	//获取标题
	virtual LPCTSTR GetCollocateName()=NULL;
	//设置回调
	virtual void SetBankSink(IBankSink *pIBankSink){m_pIBankSink=pIBankSink;}
	
	//功能函数
public:
	//绘画背景
	VOID DrawBackGround(CDC * pDC, INT nWidth, INT nHeight);
	//创建窗口
	bool CreateCollocateItem(CWnd * pParentWnd, CRect rcCreate);

	//消息函数
protected:
	//背景消息
	BOOL OnEraseBkgnd(CDC * pDC);
	//颜色消息
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//银行存取
class CLIENT_CONTROLS_CLASS CBankAccess : public CBankItem
{
	//函数定义
public:
	//构造函数
	CBankAccess(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//析构函数
	virtual ~CBankAccess();
	enum {IDD=IDD_BANK_ACCESS};

	//重载函数
public:
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);
	//重载函数
public:
	//保存参数
	virtual bool SaveParameter();
	//默认参数
	virtual bool DefaultParameter();
	//更新控制
	virtual bool UpdateControlStatus();
	//获取标题
	virtual LPCTSTR GetCollocateName();

	DECLARE_MESSAGE_MAP()
public:
	CSkinButton m_btStorage;
	CSkinButton m_btDrawout;
	afx_msg void OnBnClickedBtnStorage();
	afx_msg void OnEnChangeEditAmount();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnDrawout();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//////////////////////////////////////////////////////////////////////////

//银行转帐
class CLIENT_CONTROLS_CLASS CBankTransfer : public CBankItem
{
	//函数定义
public:
	//构造函数
	CBankTransfer(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//析构函数
	virtual ~CBankTransfer();
	enum {IDD=IDD_BANK_TRANSFER};

	//重载函数
public:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	
	//重载函数
public:
	//保存参数
	virtual bool SaveParameter();
	//默认参数
	virtual bool DefaultParameter();
	//更新控制
	virtual bool UpdateControlStatus();
	//获取标题
	virtual LPCTSTR GetCollocateName();

	//消息函数
public:
	//代理测试
	VOID OnBnClickedProxyTest();

	DECLARE_MESSAGE_MAP()
	CSkinButton m_btTransfer;
	int 		m_nAccounts;
	afx_msg void OnBnClickedRadioAccount();
	afx_msg void OnBnClickedRadioGameid();
	afx_msg void OnBnClickedBtnTransfer();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//////////////////////////////////////////////////////////////////////////

//修改密码
class CLIENT_CONTROLS_CLASS CBankModifyPass : public CBankItem
{
	//函数定义
public:
	//构造函数
	CBankModifyPass(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//析构函数
	virtual ~CBankModifyPass();
	enum {IDD=IDD_BANK_MODIFYPASS};

	//重载函数
public:
	//保存参数
	virtual bool SaveParameter();
	//默认参数
	virtual bool DefaultParameter();
	//更新控制
	virtual bool UpdateControlStatus();
	//获取标题
	virtual LPCTSTR GetCollocateName();
	//重载函数
public:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);

	DECLARE_MESSAGE_MAP()
	CSkinButton m_btModifyPass;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnModify();
};


//银行窗口
class  CLIENT_CONTROLS_CLASS CDlgBank : public CSkinDialog
{
	//配置变量
protected:
	//IClientUserItem *				m_pMySelfUsreItem;					//用户指针
	IPlatformResource *				m_pIPlatformResource;				//框架资源
	tagGlobalUserData *				m_pGlobalUserData;					//用户指针
	IBankSink		  *				m_pIBankSink;						//回调指针


	//控件变量
protected:
	CSkinButton						m_btOk;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮
	CBankBar						m_CollocateBar;						//子项按钮
	
	//配置子项
public:
	CBankAccess						m_BankAccess;						//银行存取
	CBankTransfer					m_BankTransfer;						//银行转账
	CBankModifyPass					m_BankModifyPass;					//修改密码

	//函数定义
public:
	//构造函数
	CDlgBank(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//析构函数
	virtual ~CDlgBank();
	enum {IDD=IDD_BANK};
	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	//功能函数
public:
	//配置参数
	bool InitBankSink(IBankSink *pIBankSink);
	//刷新界面
	bool UpdateView();

	//按钮消息
protected:
	//默认按钮
	VOID OnBnClickedDefault();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//////////////////////////////////////////////////////////////////////////

#endif