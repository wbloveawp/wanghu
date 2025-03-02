#ifndef BATTLE_LIST_CONTROL_HEAD_FILE
#define BATTLE_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ListControl.h"
#include "ModuleInfoManager.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//数组定义
typedef CArrayTemplate<tagBattleOption *> CBattleOptionArray;

//索引定义
typedef CMap<WORD,WORD,tagBattleOption *,tagBattleOption *> CBattleOptionMap;

//////////////////////////////////////////////////////////////////////////

//约战数据
class MODULE_MANAGER_CLASS CBattleOptionBuffer
{
	//变量定义
public:
	CBattleOptionMap				m_BattleOptionMap;					//约战索引
	CBattleOptionArray				m_BattleOptionArray;				//约战数组

	//函数定义
public:
	//构造函数
	CBattleOptionBuffer();
	//析构函数
	virtual ~CBattleOptionBuffer();

	//管理函数
public:
	//重置数据
	bool ResetBattleOption();
	//删除数据
	bool DeleteBattleOption(WORD wBattleID);
	//插入数据
	bool InsertBattleOption(tagBattleOption * pBattleOption);

	//信息函数
public:
	//获取数目
	DWORD GetBattleOptionCount();
	//查找数据
	tagBattleOption * SearchBattleOption(WORD wBattleID);

	//内部函数
private:
	//创建对象
	tagBattleOption * CreateBattleOption();
};

//////////////////////////////////////////////////////////////////////////

//约战列表
class MODULE_MANAGER_CLASS CBattleListControl : public CListControl
{
	//存储变量
protected:
	CBattleOptionBuffer *				m_pBattleOptionBuffer;			//约战缓冲

	//函数定义
public:
	//构造函数
	CBattleListControl();
	//析构函数
	virtual ~CBattleListControl();

	//重载函数
public:
	//配置列表
	virtual VOID InitListControl();
	//子项排序
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//获取颜色
	virtual VOID GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//功能函数
public:	
	//插入列表
	bool InsertModuleInfo(tagGameModuleInfo * pGameModuleInfo);	
	//设置缓冲
	void SetBattleOptionBuffer(CBattleOptionBuffer * pBattleOptionBuffer);

	//辅助函数
private:
	//插入索引
	WORD GetInsertIndex(tagGameModuleInfo * pGameModuleInfo);
	//描述字符
	LPCTSTR GetDescribeString(tagGameModuleInfo * pGameModuleInfo, WORD wColumnIndex);
};

//////////////////////////////////////////////////////////////////////////

//约战信息
class MODULE_MANAGER_CLASS CBattleOptionManager
{
	//函数定义
public:
	//构造函数
	CBattleOptionManager();
	//析构函数
	virtual ~CBattleOptionManager();

	//数据管理
public:
	//修改配置
	bool UpdateBattleOption(tagBattleOption & BattleOption);
	//插入配置
	bool InsertBattleOption(tagBattleOption & BattleOption);
	//加载配置
	bool LoadBattleOption(CBattleOptionBuffer & BattleOptionBuffer);	
};

//////////////////////////////////////////////////////////////////////////

//约战配置
class MODULE_MANAGER_CLASS CDlgBattleOption : public CDialog
{
	//列表变量
protected:
	CModuleInfoBuffer				m_ModuleInfoBuffer;					//模块信息	
	CModuleInfoManager				m_ModuleInfoManager;				//模块管理

	//列表变量
protected:	
	CBattleListControl				m_BattleListControl;				//约战列表
	CBattleOptionBuffer				m_BattleOptionBuffer;				//约战信息	
	CBattleOptionManager			m_BattleOptionManager;				//约战管理

	//配置变量
public:
	tagBattleOption					m_BattleOption;						//约战配置

	//服务组件
protected:
	tagGameModuleInfo *				m_pGameModuleInfo;					//选择模块

	//函数定义
public:
	//构造函数
	CDlgBattleOption();
	//析构函数
	virtual ~CDlgBattleOption();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//功能函数
protected:
	//加载模块
	bool LoadDBModuleItem();

	//控件消息
protected:
	//单击列表
	VOID OnNMlclkModuleList(NMHDR * pNMHDR, LRESULT * pResult);
	//双击列表
	VOID OnNMlDBclkModuleList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif