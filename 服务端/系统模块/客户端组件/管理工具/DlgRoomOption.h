#ifndef DLG_ROOM_OPTION_HEAD_FILE
#define DLG_ROOM_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgManagerOption.h"

//////////////////////////////////////////////////////////////////////////////////

//房间管理
class CDlgRoomOption : public CDlgOptionItem
{
	//变量定义
protected:
	tagModifyRule					m_ModifyRule;						//修改规则	

	//函数定义
public:
	//构造函数
	CDlgRoomOption();
	//析构函数
	virtual ~CDlgRoomOption();

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
	virtual WORD ConstructManagePacket(BYTE cbBuffer[], WORD wBufferSize);

	//辅助函数
private:
	//初始化控件
	VOID InitControls();

	//事件函数
protected:
	//编辑配置
	VOID OnBnClickedBtModify();

	//消息函数
protected:
	//销毁窗口
	VOID OnDestroy();


	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif