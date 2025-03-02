#ifndef GAME_VIDEO_ITEM_HEAD_FILE
#define GAME_VIDEO_ITEM_HEAD_FILE

#pragma once

#include "VideoServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//类型说明
class CGameVideoItem;

//数组说明
typedef CWHArray<CGameVideoItem *>	CGameVideoItemArray;

//////////////////////////////////////////////////////////////////////////////////

//用户信息
class VIDEO_SERVICE_CLASS CGameVideoItem : public IGameVideoItem
{
	//友元定义
	friend class CGameVideoManager;

	//状态变量
protected:
	bool							m_bRecing;							//录制状态
	WORD							m_wTableID;							//桌子标识
	DWORD							m_dwPacketPos;						//数据位置
	WCHAR							m_szVideoNumber[LEN_VIDEO_NUMBER];	//录像编号

	//数据变量
protected:
	DWORD							m_dwDataSize;						//数据大小	
	DWORD							m_dwBufferSize;						//缓冲长度
	LPBYTE							m_pVideoDataBuffer;					//缓冲指针

	//静态变量
private:
	static INT						m_nRefCount;						//引用计数
	static DWORD					m_dwCompressSize;					//压缩大小
	static LPBYTE					m_pCompressBuffer;					//压缩数据

	//配置变量
protected:
	tagVideoServer *				m_pVideoServer;						//房间信息
	IGameVideoItemSink *			m_pIGameVideoItemSink;				//回调接口

	//函数定义
protected:
	//构造函数
	CGameVideoItem();
	//析构函数
	virtual ~CGameVideoItem();

	//屏蔽函数
private:
	//拷贝构造
	CGameVideoItem(const CGameVideoItem&);
	//赋值操作
	CGameVideoItem& operator=(const CGameVideoItem&); 

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//开始录像
	virtual bool StartVideo();	
	//停止录像
	virtual bool ConcludeVideo();	

	//压缩接口
public:
	//压缩大小
	virtual DWORD GetCompressSize() { return m_dwDataSize; }
	//压缩数据
	virtual LPBYTE GetCompressBuffer() { return m_pVideoDataBuffer; }

	//信息接口
public:
	//桌子标识
	virtual WORD _cdecl GetTableID() { return m_wTableID; }
	//录像编号
	virtual LPCTSTR GetVideoNumber() { return m_szVideoNumber; }
	
	//功能接口
public:	
	//插入用户
	virtual bool InsertVideoUser(IServerUserItem * pIServerUserItem);
	//记录数据
	virtual bool InsertVideoData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//压缩数据
	bool CompressData();
	//重置数据
	VOID ResetVideoItem();
	//调整缓冲
	bool RectifyBuffer(WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif