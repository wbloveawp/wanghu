#ifndef USER_LIST_CONTROL_HEAD_FILE
#define USER_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ClientControlsHead.h"

//////////////////////////////////////////////////////////////////////////

//常量定义
#define	MAX_COLUMN					32									//最大列数
#define MAX_EXPERIENCE				5									//最大经验

//宽度定义
#define WIDTH_STATUS				20									//列表宽度
#define WIDTH_EXPERIENCE			16									//列表宽度

//索引属性
#define INDEX_STATUS				0									//状态索引
#define COUNT_STATUS				6									//状态数目
#define INDEX_NORMAL				18									//常规索引
#define INDEX_FRIEND				19									//好友索引
#define INDEX_DETEST				20									//厌友索引
#define INDEX_MEMBER				21									//会员索引
#define INDEX_MASTER				43									//管理索引
#define INDEX_INVALID				0xFFFF								//无效索引

//////////////////////////////////////////////////////////////////////////
//默认颜色
#define COLOR_SELECT_TX				RGB(255,234,2)						//字体颜色
#define COLOR_SELECT_BK				RGB(85,72,53)						//背景颜色
#define COLOR_NORMAL_TX				RGB(255,255,255)					//字体颜色
#define COLOR_NORMAL_BK				RGB(50,30,17)						//背景颜色

//默认颜色
#define COLOR_MYSELF_TX				RGB(255,167,132)					//字体颜色
#define COLOR_MYSELF_BK				RGB(85,72,53)						//背景颜色
#define COLOR_MASTER_TX				RGB(255,255,255)					//字体颜色
#define COLOR_MASTER_BK				RGB(90,45,27)						//背景颜色
#define COLOR_MEMBER_TX				RGB(67,44,12)						//字体颜色
#define COLOR_MEMBER_BK				RGB(255,157,2)						//背景颜色
#define COLOR_ANDROID_TX			RGB(255,255,255)					//字体颜色
#define COLOR_ANDROID_BK			RGB(255,0,0)						//背景颜色
//////////////////////////////////////////////////////////////////////////

//颜色定义
struct tagColorUserList
{
	//常规颜色
	COLORREF						crSelectTX;							//字体颜色
	COLORREF						crSelectBK;							//背景颜色
	COLORREF						crNormalTX;							//字体颜色
	COLORREF						crNormalBK;							//背景颜色	

	//特殊颜色
	COLORREF						crMyselfTX;							//字体颜色
	COLORREF						crMyselfBK;							//背景颜色
	COLORREF						crMasterTX;							//字体颜色
	COLORREF						crMasterBK;							//背景颜色
	COLORREF						crMemberTX;							//字体颜色
	COLORREF						crMemberBK;							//背景颜色
};

//////////////////////////////////////////////////////////////////////////

//列头控件
class CLIENT_CONTROLS_CLASS CUserListHeaderCtrl : public CHeaderCtrl
{
	//排序变量
protected:
	INT								m_nColumn;							//当前排列
	enSortType						m_SortType;							//排序类型

	//函数定义
public:
	//构造函数
	CUserListHeaderCtrl();
	//析构函数
	virtual ~CUserListHeaderCtrl();

	//重载函数
protected:
	//控件消息
	virtual BOOL OnChildNotify(UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT * pLResult);

	//消息函数
protected:
	//光标消息
    BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	//功能函数
public:
	//设置标志
	VOID SetSortArrow(INT nSortColumn, enSortType SortType);
	//获取标志
	VOID GetSortArrow(INT& nSortColumn, enSortType& SortType);

	//消息函数
protected:
	//重画消息
	VOID OnPaint();
	//绘画背景
	BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//用户列表
class CLIENT_CONTROLS_CLASS CUserListControl : public CListCtrl
{
	//状态变量
protected:
	bool							m_bAscendSort;						//升序标志
	bool							m_bCreateColumn;					//创建列项
	DWORD							m_dwMySelfUserID;					//自己信息

	//列表信息
protected:
	WORD							m_wColumnCount;						//列表数目
	BYTE							m_cbDataDescribe[MAX_COLUMN];		//数据描述

	//组件变量
protected:
	IGameLevelParser *				m_pIGameLevelParser;				//等级接口
	IGameUserManager *				m_pIGameUserManager;				//用户接口
	CUserListHeaderCtrl				m_UserListHeaderCtrl;				//列头控件
	CSkinScrollBar					m_SkinScrollBar;					//滚动条类

	//资源变量
protected:
	tagColorUserList				m_ColorUserList;					//颜色定义
	static CImageList				m_ExperienceImage;					//经验位图
	static CImageList				m_UserStatusImage;					//状态位图

	//函数定义
public:
	//构造函数
	CUserListControl();
	//析构函数
	virtual ~CUserListControl();

	//重载函数
public:
	//控件绑定
	virtual VOID PreSubclassWindow();
	//绘画函数
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//配置函数
public:
	//设置接口
	bool SetMySelfUserID(DWORD dwMySelfUserID);
	//设置接口
	bool SetGameLevelParser(IUnknownEx * pIUnknownEx);
	//设置接口
	bool SetGameUserManager(IUnknownEx * pIUnknownEx);
	//设置颜色
	bool SetColorUserList(const tagColorUserList & ColorUserList);
	//设置列表
	bool SetColumnDescribe(const tagColumnItem ColumnItem[], WORD wColumnCount);

	//功能函数
public:
	//插入用户
	bool InsertUserItem(IClientUserItem * pIClientUserItem);
	//更新用户
	bool UpdateUserItem(IClientUserItem * pIClientUserItem);
	//删除用户
	bool DeleteUserItem(IClientUserItem * pIClientUserItem);

	//位图函数
protected:
	//获取位置
	INT GetInsertIndex(IClientUserItem * pIClientUserItem);
	//获取位图
	UINT GetImageIndex(IClientUserItem * pIClientUserItem, WORD wColumnIndex);
	//描述字符
	LPCTSTR GetDescribeString(IClientUserItem * pIClientUserItem, WORD wColumnIndex);

	//静态函数
protected:
	//排列函数
	static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	//消息映射
protected:
	//销毁消息
	VOID OnNcDestroy();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//点击列表
	VOID OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//用户列表
class CLIENT_CONTROLS_CLASS CSearchUserListControl : public CListCtrl
{
	//状态变量
protected:
	bool							m_bAscendSort;						//升序标志
	bool							m_bCreateColumn;					//创建列项
	DWORD							m_dwMySelfUserID;					//自己信息

	//列表信息
protected:
	WORD							m_wColumnCount;						//列表数目
	BYTE							m_cbDataDescribe[MAX_COLUMN];		//数据描述

	//组件变量
protected:
	CUserListHeaderCtrl				m_UserListHeaderCtrl;				//列头控件
	CSkinScrollBar					m_SkinScrollBar;					//滚动条类

	//资源变量
protected:
	tagColorUserList				m_ColorUserList;					//颜色定义

	//函数定义
public:
	//构造函数
	CSearchUserListControl();
	//析构函数
	virtual ~CSearchUserListControl();

	//重载函数
public:
	//控件绑定
	virtual VOID PreSubclassWindow();
	//绘画函数
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//配置函数
public:
	//设置接口
	bool SetMySelfUserID(DWORD dwMySelfUserID);
	//设置颜色
	bool SetColorUserList(const tagColorUserList & ColorUserList);
	//设置列表
	bool SetColumnDescribe(const tagColumnItem ColumnItem[], WORD wColumnCount);

	//功能函数
public:
	//插入用户
	bool InsertUserItem(IGlobalUserItem * pIGlobalUserItem);
	//更新用户
	bool UpdateUserItem(IGlobalUserItem * pIGlobalUserItem);
	//删除用户
	bool DeleteUserItem(IGlobalUserItem * pIGlobalUserItem);

	//位图函数
protected:
	//获取位置
	INT GetInsertIndex(IGlobalUserItem * pIGlobalUserItem);
	//获取位图
	UINT GetImageIndex(IGlobalUserItem * pIGlobalUserItem, WORD wColumnIndex);
	//描述字符
	LPCTSTR GetDescribeString(IGlobalUserItem * pIGlobalUserItem, WORD wColumnIndex);

	//静态函数
protected:
	//排列函数
	static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	//消息映射
protected:
	//销毁消息
	VOID OnNcDestroy();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//点击列表
	VOID OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif