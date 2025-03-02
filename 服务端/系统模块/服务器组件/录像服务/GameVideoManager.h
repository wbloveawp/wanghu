#ifndef GAME_VIDEO_MANAGER_HEAD_FILE
#define GAME_VIDEO_MANAGER_HEAD_FILE

#pragma once

//引入文件
#include "GameVideoItem.h"
#include "VideoServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//索引类
typedef CWHArray<CGameVideoItem *> CGameVideoItemArray;
typedef CMap<WORD,WORD,CGameVideoItem *,CGameVideoItem *> CGameVideoItemMap;

//////////////////////////////////////////////////////////////////////////////////

//录像管理
class VIDEO_SERVICE_CLASS CGameVideoManager : public IGameVideoManager
{
	//配置变量
protected:
	tagVideoServer					m_VideoServer;						//房间信息

	//系统配置
protected:	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置	

	//用户变量
protected:	
	CGameVideoItemArray				m_GameVideoItemActive;				//录像数组
	CGameVideoItemArray				m_GameVideoItemBuffer;				//存储录像

	//组件接口
protected:
	IGameVideoItemSink *			m_pIGameVideoItemSink;				//回调接口

	//函数定义
public:
	//构造函数
	CGameVideoManager();
	//析构函数
	virtual ~CGameVideoManager();

	//屏蔽函数
private:
	//拷贝构造
	CGameVideoManager(const CGameVideoManager&);
	//赋值操作
	CGameVideoManager& operator=(const CGameVideoManager&);

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置函数
public:
	//设置接口
	virtual bool SetGameVideoItemSink(IUnknownEx * pIUnknownEx);
	//配置组件
	virtual void InitVideoManager(tagGameVideoParameter & GameVideoParameter);

	//管理函数
public:
	//激活录像
	virtual IGameVideoItem * ActiveVideoItem(WORD wTableID);
	//删除录像
	virtual bool DeleteVideoItem(IGameVideoItem * pIGameVideoItem);	

	//辅助函数
private:
	//录像编号
	VOID BuildVideoNumber(WCHAR szBuffer[], WORD wBufferLen);
};

//////////////////////////////////////////////////////////////////////////////////

#endif