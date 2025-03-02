#pragma once

#include "Resource.h"
#include "Other\IDispimp.h"
#include "Other\Custsite.h"

//////////////////////////////////////////////////////////////////////////

//应用程序类
class CGamePlazaApp : public CWinApp
{
	//变量定义
protected:
	SOCKET									m_hSocketMutex;				//连接句柄
	ATOM									m_AtomPlazaClass;			//广场原子
	
	//变量定义
public:
	CImpIDispatch *							m_pIDispatch;				//自动接口

	//函数定义
public:
	//构造函数
	CGamePlazaApp();

	//重载函数
public:
	//进入函数
	virtual BOOL InitInstance();
	//退出函数
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//应用程序对象说明
extern CGamePlazaApp theApp;

//////////////////////////////////////////////////////////////////////////
