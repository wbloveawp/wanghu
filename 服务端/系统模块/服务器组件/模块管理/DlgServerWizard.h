#ifndef DLG_SERVER_WIZARD_HEAD_FILE
#define DLG_SERVER_WIZARD_HEAD_FILE

#pragma once

#include "ModuleListControl.h"
#include "ModuleInfoManager.h"
#include "ServerInfoManager.h"
#include "ModuleManagerHead.h"
#include "DataBaseListManager.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CDlgServerWizard;

//////////////////////////////////////////////////////////////////////////

//向导基础类
class MODULE_MANAGER_CLASS CDlgServerWizardItem : public CDialog
{
	//友元定义
	friend class CDlgServerWizard;

	//状态变量
private:
	bool							m_bSaveData;						//保存标志

	//变量定义
protected:
	CDlgServerWizard *				m_pDlgServerWizard;					//向导指针
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//函数定义
protected:
	//构造函数
	CDlgServerWizardItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgServerWizardItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo()=NULL;
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//功能函数
private:
	//保存数据
	bool SaveItemData();
	//创建向导
	bool ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd);	

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//房间选项类
class MODULE_MANAGER_CLASS CDlgServerOptionItem : public CDialog
{
	//友元定义
	friend class CDlgServerWizardStep2;

	//变量定义
protected:	
	CDlgServerWizard *				m_pDlgServerWizard;					//向导指针
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置	

	//函数定义
protected:
	//构造函数
	CDlgServerOptionItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgServerOptionItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo()=NULL;
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//事件接口
protected:
	//类型变更
	virtual VOID OnEventServerTypeChange(WORD wServerType) {};

	//功能函数
private:
	//创建子项
	void CreateOptionItem(const CRect & rcRect, CWnd * pParentWnd);

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

//房间选项1
class MODULE_MANAGER_CLASS CDlgServerOption1 : public CDlgServerOptionItem
{
	//函数定义
public:
	//构造函数
	CDlgServerOption1();
	//析构函数
	virtual ~CDlgServerOption1();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//事件接口
protected:
	//类型变更
	virtual VOID OnEventServerTypeChange(WORD wServerType);
};

//////////////////////////////////////////////////////////////////////////

//房间选项2
class MODULE_MANAGER_CLASS CDlgServerOption2 : public CDlgServerOptionItem
{
	//函数定义
public:
	//构造函数
	CDlgServerOption2();
	//析构函数
	virtual ~CDlgServerOption2();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//房间选项3
class MODULE_MANAGER_CLASS CDlgServerOption3 : public CDlgServerOptionItem
{
	//函数定义
public:
	//构造函数
	CDlgServerOption3();
	//析构函数
	virtual ~CDlgServerOption3();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//房间选项4
class MODULE_MANAGER_CLASS CDlgServerOption4 : public CDlgServerOptionItem
{
	//函数定义
public:
	//构造函数
	CDlgServerOption4();
	//析构函数
	virtual ~CDlgServerOption4();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//定制配置
class MODULE_MANAGER_CLASS CDlgServerOptionCustom : public CDlgServerOptionItem
{
	//控件变量
protected:
	HWND							m_hCustomRule;						//定制规则

	//函数定义
public:
	//构造函数
	CDlgServerOptionCustom();
	//析构函数
	virtual ~CDlgServerOptionCustom();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//消息函数
private:
	//消耗消息
	VOID OnNcDestroy();
	//焦点消息
	VOID OnSetFocus(CWnd * pNewWnd);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//公共向导
class MODULE_MANAGER_CLASS CDlgServerWizardStep1 : public CDlgServerWizardItem
{
	//列表变量
protected:
	CStatic							m_StaticPrompt;						//提示控件
	CModuleInfoBuffer				m_ModuleInfoBuffer;					//模块信息
	CModuleListControl				m_ModuleListControl;				//模块列表
	CModuleInfoManager				m_ModuleInfoManager;				//模块管理

	//服务组件
protected:
	tagGameModuleInfo *				m_pGameModuleInfo;					//选择模块
	CGameServiceManagerHelper		m_GameServiceManager;				//游戏模块

	//函数定义
public:
	//构造函数
	CDlgServerWizardStep1();
	//析构函数
	virtual ~CDlgServerWizardStep1();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//功能函数
protected:
	//加载模块
	bool LoadDBModuleItem();

	//控件消息
protected:
	//双击列表
	VOID OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//房间向导
class MODULE_MANAGER_CLASS CDlgServerWizardStep2 : public CDlgServerWizardItem
{
	//控件变量
protected:
	BYTE							m_cbItemIndex;					//子项索引
	CDlgServerOption1				m_DlgServerOption1;				//配置选项
	CDlgServerOption2				m_DlgServerOption2;				//配置选项
	CDlgServerOption3				m_DlgServerOption3;				//配置选项
	CDlgServerOption4				m_DlgServerOption4;				//配置选项
	CDlgServerOptionCustom			m_DlgServerOption5;				//配置选项
	CDlgServerOptionItem *			m_pDlgServerOptionItem[5];		//选项数组

	//函数定义
public:
	//构造函数
	CDlgServerWizardStep2();
	//析构函数
	virtual ~CDlgServerWizardStep2();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//辅助函数
protected:
	//基础配置
	VOID InitBaseOption();
	//保存配置
	bool SaveBaseOption();
	//激活子项
	VOID ActiveOptionItem(BYTE cbItemIndex);

	//消息函数
public:
	//选择变更
	void OnCbnSelchangeServerType();
	//选择变更
	void OnTcnSelchangeServerOption(NMHDR *pNMHDR, LRESULT *pResult);
	
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//房间向导
class MODULE_MANAGER_CLASS CDlgServerWizard : public CDialog
{
	//友元定义
	friend class CDlgServerWizardStep1;
	friend class CDlgServerWizardStep2;
	friend class CDlgServerOptionCustom;

	//变量定义
protected:
	WORD							m_wActiveIndex;						//活动索引
	CDlgServerWizardItem *			m_pWizardItem[2];					//向导指针

	//配置信息
protected:
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数

	//组件变量
protected:
	CDatabaseListManager			m_DBListMgr;						//数据库列表

	//控件变量
protected:
	CDlgServerWizardStep1			m_ServerWizardStep1;				//设置步骤
	CDlgServerWizardStep2			m_ServerWizardStep2;				//设置步骤

	//接口变量
protected:
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IGameServiceCustomRule *		m_pIGameServiceCustomRule;			//自定配置

	//函数定义
public:
	//构造函数
	CDlgServerWizard();
	//析构函数
	virtual ~CDlgServerWizard();

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	//配置函数
public:
	//获取属性
	VOID GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//获取配置
	VOID GetGameServiceOption(tagGameServiceOption & GameServiceOption);
	//设置配置
	VOID SetGameServiceOption(tagGameServiceOption & GameServiceOption);
	//设置接口
	VOID SetGameServiceManager(IGameServiceManager * pIGameServiceManager);

	//内部函数
private:
	//激活向导
	bool ActiveWizardItem(WORD wIndex);

	//消息映射
public:
	//下一步
	VOID OnBnClickedNext();
	//完成按钮
	VOID OnBnClickedFinish();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif