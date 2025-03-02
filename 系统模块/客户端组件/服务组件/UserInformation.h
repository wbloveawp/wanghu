#ifndef USER_INFORMATION_HEAD_FILE
#define USER_INFORMATION_HEAD_FILE

#pragma once

#include "ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//类说明
typedef CWHArray<tagCompanionInfo *> CCompanionInfoArray;			//用户关系

//////////////////////////////////////////////////////////////////////////

//用户信息
class CUserInformation : public IUserInformation
{
	//接口变量
protected:
	IUserCompanionSink *			m_pIUserCompanionSink;				//关系回调

	//关系信息
protected:
	CCompanionInfoArray				m_CompanionInfoActive;				//激活子项
	CCompanionInfoArray				m_CompanionInfoBuffer;				//释放子项

	//函数定义
public:
	//构造函数
	CUserInformation();
	//析构函数
	virtual ~CUserInformation();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//加载信息
	virtual bool __cdecl LoadInformation();
	//设置接口
	virtual bool __cdecl SetUserCompanionSink(IUnknownEx * pIUnknownEx);

	//关系接口
public:
	//枚举关系
	virtual tagCompanionInfo * __cdecl EmunCompanionInfo(WORD wIndex);
	//寻找关系
	virtual tagCompanionInfo * __cdecl SearchCompanionInfo(DWORD dwUserID);
	//保存关系
	virtual tagCompanionInfo * __cdecl InsertCompanionInfo(IClientUserItem * pIClientUserItem, BYTE cbCompanion);

	//内部函数
private:
	//创建关系
	tagCompanionInfo * CreateCompanionItem();
	//释放关系
	bool DeleteCompanionItem(tagCompanionInfo * pCompanionInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif