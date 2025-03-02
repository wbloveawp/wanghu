#ifndef CHAT_PHRASE_HEAD_FILE
#define CHAT_PHRASE_HEAD_FILE

#pragma once

#include "ClientControlsHead.h"

//////////////////////////////////////////////////////////////////////////

//短语列表
class CChatPhraseList : public CSkinListCtrl
{
	//状态变量
protected:
	bool							m_bAscendSort;						//升序标志

	//资源变量
protected:
	static CImageList				m_PhraseFlagImage;					//标志位图

	//函数定义
public:
	//构造函数
	CChatPhraseList();
	//析构函数
	virtual ~CChatPhraseList();

	//配置函数
public:
	//配置列表
	bool InitListControl();

	//内部函数
protected:
	//排列函数
	static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	//消息映射
protected:
	//销毁消息
	VOID OnNcDestroy();
	//点击列表
	VOID OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//聊天动作
class CLIENT_CONTROLS_CLASS CChatPhrase : public CSkinDialog
{
	//控件变量
protected:
	CSkinButton						m_btSelect;							//选择按钮
	CSkinButton						m_btCancel;							//取消按钮
	CChatPhraseList					m_ChatPhraseList;					//短语列表

	//函数定义
public:
	//构造函数
	CChatPhrase();
	//析构函数
	virtual ~CChatPhrase();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK() { return; }

	//功能函数
protected:
	//调整控件
	VOID RectifyControl(INT nWidth, INT nHeight);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif