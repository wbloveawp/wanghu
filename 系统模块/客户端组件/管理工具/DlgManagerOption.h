#ifndef DLG_MANAGER_OPTION_HEAD_FILE
#define DLG_MANAGER_OPTION_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////
//颜色定义
#define CR_NORMAL_TX				RGB(0,0,0)
#define CR_SELECT_TX				RGB(128,64,0)	
#define CR_CONTROL_BACK				RGB(240,240,240)		
#define CR_CONTROL_BORAD			RGB(0,0,0)

//////////////////////////////////////////////////////////////////////////

//类型声明
class CDlgOptionItem;
class CDlgServerOption;
class CDlgCustomOption;

//////////////////////////////////////////////////////////////////////////

//配置子项
class CDlgOptionItem : public CDialog
{
	//函数定义
protected:
	//构造函数
	CDlgOptionItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgOptionItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK() { return; }
	//取消消息
	virtual VOID OnCancel() { return; }

	//配置函数
public:			
	//控件使能
	virtual VOID EnableControls(BOOL bEnabled)=NULL;	
	//构造数据
	virtual WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)=NULL;

	//功能函数
public:
	//显示子项
	VOID ShowViewItem(const CRect & rcRect, CWnd * pParentWnd); 
	//创建子项
	VOID CreateViewItem(const CRect & rcRect, CWnd * pParentWnd);

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);
	//创建颜色
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//房间配置
class CDlgServerOption : public CDlgOptionItem
{
	//变量定义
protected:
	tagModifyRule					m_ModifyRule;						//修改规则	

	//函数定义
public:
	//构造函数
	CDlgServerOption();
	//析构函数
	virtual ~CDlgServerOption();

	//重载函数
protected:
	//初始窗口
	virtual BOOL OnInitDialog();
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);	

	//配置函数
public:			
	//控件使能
	virtual VOID EnableControls(BOOL bEnabled);	
	//构造数据
	virtual WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize);

	//辅助函数
private:
	//初始化控件
	VOID InitControls();	

	//事件函数
protected:
	//编辑子项
	VOID OnBnClickedBtModify();

	//消息函数
protected:
	//销毁窗口
	VOID OnDestroy();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//自定义视图
class CDlgCustomOption : public CDlgOptionItem
{
	//变量定义
protected:
	tagCommandCode					m_CommandCode;						//命令编码

	//函数定义
public:
	//构造函数
	CDlgCustomOption();
	//析构函数
	virtual ~CDlgCustomOption();

	//重载函数
protected:
	//初始窗口
	virtual BOOL OnInitDialog();
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);	

	//配置函数
public:		
	//控件使能
	virtual VOID EnableControls(BOOL bEnabled);	
	//构造数据
	virtual WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize);

	//辅助函数
private:
	//初始化控件
	VOID InitControls();	

	//事件函数
protected:
	//编辑配置
	VOID OnBnClickedBtExecuteCmd();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
#endif