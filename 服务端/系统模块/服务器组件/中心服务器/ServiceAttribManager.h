#ifndef SERVICE_ATTRIB_MANAGER_HEAD_FILE
#define SERVICE_ATTRIB_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//服务属性管理
class CServiceAttribManager
{
	//服务属性
protected:
	static tagServiceAttrib		m_ServiceAttribArray[];			//服务属性

	//函数定义
public:
	//构造函数
	CServiceAttribManager();
	//析构函数
	virtual ~CServiceAttribManager();

	//功能函数
public:
	//枚举服务
	static tagServiceAttrib* EnumServiceAttrib(WORD wIndex);
	//获取服务
	static tagServiceAttrib* SearchServiceAttrib(WORD wServiceModule);	
};

//////////////////////////////////////////////////////////////////////////

#endif
