#ifndef SKIN_RICHEDIT_CTRL_HEAD_FILE
#define SKIN_RICHEDIT_CTRL_HEAD_FILE

#pragma once
#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//追踪服务
class CRecordEditCtrl : public CRichEditCtrl
{
	//辅助变量
protected:
	LONG							m_lMaxLineCount;					//最大行数
	LONG							m_lReserveLineCount;				//保留行数

	//线程信息
protected:
	DWORD							m_dwMainThreadID;					//线程标识
	HANDLE							m_hMainThreadHandle;				//线程句柄

	//组件变量
protected:
	CWHDataQueue					m_DataQueue;						//数据队列
	CCriticalSection				m_CriticalSection;					//同步对象

	//辅助变量
protected:

	//函数定义
public:
	//构造函数
	CRecordEditCtrl();
	//析构函数
	virtual ~CRecordEditCtrl();

	//重载函数
protected:
	//绑定函数
	virtual VOID PreSubclassWindow();

	//功能函数
public:
	//加载消息
	bool LoadMessage(LPCTSTR pszFileName);
	//保存信息
	bool SaveMessage(LPCTSTR pszFileName);
	//设置参数
	bool SetParameter(LONG lMaxLineCount, LONG lReserveLineCount);

	//辅助函数
public:
	//配置服务
	VOID InitializeService();	
	//插入字串
	VOID InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat);

	//回调函数
private:
	//加载回调
	static DWORD CALLBACK LoadCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);
	//保存回调
	static DWORD CALLBACK SaveCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);

	//菜单命令
protected:
	//删除信息
	VOID OnClearAll();
	//全部选择
	VOID OnSelectAll();
	//拷贝字符
	VOID OnCopyString();
	//保存信息
	VOID OnSaveString();
	//删除字符
	VOID OnDeleteString();

	//消息映射
protected:
	//建立消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//右键消息
	VOID OnRButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif