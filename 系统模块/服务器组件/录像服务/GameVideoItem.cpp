#include "StdAfx.h"
#include "GameVideoItem.h"


//////////////////////////////////////////////////////////////////////////////////


//压缩文件
#include "Compress\zlib.h"
#include "Compress\zconf.h"
//链接文件
#ifndef _DEBUG
#pragma comment(lib,"Compress\\zlibstatic.lib")
#else
#pragma comment(lib,"Compress\\zlibstaticd.lib")
#endif

// 
//常量定义
#define MAX_VIDEO_BUFFER							16384*10				//录像缓冲

//////////////////////////////////////////////////////////////////////////////////
//静态变量
INT CGameVideoItem::m_nRefCount=0;
DWORD CGameVideoItem::m_dwCompressSize=0;
LPBYTE CGameVideoItem::m_pCompressBuffer=NULL;

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CGameVideoItem::CGameVideoItem()
{
	//增加引用
	m_nRefCount++;

	//状态变量
	m_bRecing=false;
	m_wTableID=INVALID_TABLE;
	m_szVideoNumber[0]=0;

	//缓冲变量
	m_dwDataSize=0;
	m_dwPacketPos=0;
	m_dwBufferSize=0;	
	m_pVideoDataBuffer=NULL;

	//接口变量
	m_pVideoServer=NULL;
	m_pIGameVideoItemSink=NULL;
}

//析构函数
CGameVideoItem::~CGameVideoItem()
{
	//减少引用
	m_nRefCount--;

	//释放资源	
	SafeDeleteArray(m_pVideoDataBuffer);	

	//释放资源
	if (m_nRefCount==0)
	{
		m_dwCompressSize=0;
		SafeDeleteArray(m_pCompressBuffer);
	}
}

//接口查询
VOID * CGameVideoItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameVideoItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameVideoItem,Guid,dwQueryVer);
	return NULL;
}

//开始录像
bool CGameVideoItem::StartVideo()
{
	//状态校验
	ASSERT (m_bRecing==false);
	if (m_bRecing==true) return false;

	//设置状态
	m_bRecing=true;

	//构造结构
	tagVideoHead VideoHead={};
	VideoHead.dwStartTime=(DWORD)time(NULL);

	//计算大小
	WORD wDataSize=sizeof(tagVideoHead)+sizeof(tagVideoServer);

	//校验缓冲
	if ((m_dwDataSize+wDataSize)>m_dwBufferSize)
	{	
		//调整缓冲
		if (RectifyBuffer(wDataSize)==false)
		{
			return false;
		}
	}

	try
	{
		//拷贝数据
		CopyMemory(m_pVideoDataBuffer,&VideoHead,sizeof(tagVideoHead));		
		CopyMemory(m_pVideoDataBuffer+sizeof(tagVideoHead),m_pVideoServer,sizeof(tagVideoServer));		

		//设置变量
		m_dwDataSize=wDataSize;
		m_dwPacketPos=m_dwDataSize;
		
	}
	catch (...) 
	{
		ASSERT (FALSE);
		return false;
	}

	//开始通知
	if (m_pIGameVideoItemSink)
	{
		m_pIGameVideoItemSink->OnEventVideoStart(this);
	}	

	return true;
}

//停止录像
bool CGameVideoItem::ConcludeVideo()
{
	//状态校验
	ASSERT (m_bRecing==true);
	if (m_bRecing==false) return false;

	//开始通知
	if (m_pIGameVideoItemSink)
	{
		m_pIGameVideoItemSink->OnEventVideoConclude(this);
	}

	//设置状态
	m_bRecing=false;

	//压缩数据
	if (CompressData()==false)
	{
		return false;
	}

	return true;
}

//插入用户
bool CGameVideoItem::InsertVideoUser(IServerUserItem * pIServerUserItem)
{
	//状态校验
	ASSERT (m_bRecing && pIServerUserItem!=NULL);
	if (!m_bRecing || pIServerUserItem==NULL) return false;

	//头部信息
	tagVideoHead * pVideoHead=(tagVideoHead *)m_pVideoDataBuffer;

	//校验大小
	WORD wUserItemSize=sizeof(tagVideoUser);
	if (m_dwDataSize!=(sizeof(tagVideoHead)+sizeof(tagVideoServer)+pVideoHead->cbUserCount*wUserItemSize))
	{
		return false;
	}

	//校验缓冲
	if ((m_dwDataSize+wUserItemSize)>m_dwBufferSize)
	{	
		//调整缓冲
		if (RectifyBuffer(wUserItemSize)==false)
		{
			return false;
		}
	}

	//变量定义
	tagVideoUser VideoUser;
	ZeroMemory(&VideoUser,sizeof(VideoUser));

	//构造结构
	VideoUser.dwUserID=pIServerUserItem->GetUserID();
	VideoUser.wChairID=pIServerUserItem->GetChairID();
	StringCchCopy(VideoUser.szNickName,CountArray(VideoUser.szNickName), pIServerUserItem->GetAccounts());

	try
	{
		//拷贝数据
		CopyMemory(m_pVideoDataBuffer+m_dwDataSize,&VideoUser,wUserItemSize);

		//设置变量
		pVideoHead->cbUserCount++;
		m_dwDataSize+=wUserItemSize;		
		m_dwPacketPos=m_dwDataSize;					
	}
	catch (...) 
	{
		return false;
	}		

	return true;
}

//插入数据
bool CGameVideoItem::InsertVideoData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//校验状态
	ASSERT (m_bRecing==true);
	if (m_bRecing==false) return false;

	//校验缓冲
	WORD wPacketSize=sizeof(tagVideoPacket)+wDataSize;	
	if ((m_dwDataSize+wPacketSize)>m_dwBufferSize)
	{	
		//调整缓冲
		if (RectifyBuffer(wPacketSize)==false)
		{
			return false;
		}
	}

	////if (wMainCmdID != 200 && wMainCmdID != 100)
	//{
		//CString s;
		//s.Format(_T(" InsertVideoData wChairID:%d, wMainCmdID:%d, wSubCmdID:%d, wDataSize:%d,m_dwDataSize:%d"), wChairID, wMainCmdID, wSubCmdID, wDataSize, m_dwDataSize);
		//CTraceService::TraceString(s, TraceLevel_Debug);
	//}

	//变量定义
	tagVideoPacket VideoPacket;
	ZeroMemory(&VideoPacket,sizeof(VideoPacket));

	//位置信息
	VideoPacket.wChairID=wChairID;

	//数据信息	
	VideoPacket.wDataSize=wDataSize;
	VideoPacket.wSubCmdID=wSubCmdID;	
	VideoPacket.wMainCmdID=wMainCmdID;

	//构造结构
	VideoPacket.wPacketSize=wPacketSize;			
	VideoPacket.dwPacketPos=m_dwPacketPos;
	VideoPacket.dwInsertTime=(DWORD)time(NULL);		

	try
	{
		//设置位置
		m_dwPacketPos=m_dwDataSize;

		//拷贝数据
		CopyMemory(m_pVideoDataBuffer+m_dwDataSize,&VideoPacket,sizeof(VideoPacket));
		m_dwDataSize+=sizeof(VideoPacket);

		//拷贝数据
		CopyMemory(m_pVideoDataBuffer+m_dwDataSize,pData,wDataSize);
		m_dwDataSize+=wDataSize;		

		//统计包数
		((tagVideoHead *)m_pVideoDataBuffer)->wPacketCount++;		
	}
	catch (...) 
	{
		return false;
	}

	return true;
}

//压缩数据
bool CGameVideoItem::CompressData()
{
	//效验指针
	if (m_pCompressBuffer==NULL || m_dwDataSize>m_dwCompressSize)
	{
		try
		{
			//申请内存			
			BYTE * pCompressBuffer=new BYTE[m_dwDataSize];
			if (pCompressBuffer==NULL) return false;

			//置换指针
			m_dwCompressSize=m_dwDataSize;			
			SafeDeleteArray(m_pCompressBuffer);
			m_pCompressBuffer=pCompressBuffer;
		}
		catch (...) 
		{
			ASSERT (FALSE);
			return false; 
		}
	}

	//设置头部
	((tagVideoHead *)m_pVideoDataBuffer)->dwDataSize=m_dwDataSize;
	((tagVideoHead *)m_pVideoDataBuffer)->cbCompressKind=COMPRESS_KIND_NONE;

	//变量定义
	DWORD dwCompressSize=m_dwCompressSize;
	DWORD dwSourceSize=m_dwDataSize-sizeof(tagVideoHead);
	LPBYTE pcbSourceData=m_pVideoDataBuffer+sizeof(tagVideoHead);

	//压缩数据
	if (compress(m_pCompressBuffer,&dwCompressSize,pcbSourceData,dwSourceSize)==0L)
	{
		//拷贝数据
		CopyMemory(m_pVideoDataBuffer+sizeof(tagVideoHead),m_pCompressBuffer,dwCompressSize);

		//设置变量
		m_dwDataSize=sizeof(tagVideoHead)+dwCompressSize;		
		((tagVideoHead *)m_pVideoDataBuffer)->cbCompressKind=COMPRESS_KIND_ZIP;
	}

	////////////////////////////////////////////////////////////////
//#ifdef _DEBUG
//	TCHAR szFilePath[MAX_PATH];
//	_sntprintf(szFilePath,CountArray(szFilePath),TEXT("%s.Video"),m_szVideoNumber);
//
//	CFile VideoFile(szFilePath,CFile::modeReadWrite|CFile::modeCreate);
//	VideoFile.Write(m_pVideoDataBuffer,m_dwDataSize);
//#endif
////////////////////////////////////////////////////////////////

	return true;
}

//重置数据
VOID CGameVideoItem::ResetVideoItem()
{
	//状态变量
	m_bRecing=false;
	m_wTableID=INVALID_TABLE;

	//设置变量
	m_dwDataSize=0;
	m_dwPacketPos=0;
	m_szVideoNumber[0]=0;
}

//调整缓冲
bool CGameVideoItem::RectifyBuffer(WORD wDataSize)
{
	try
	{
		//缓冲校验
		if (MAX_VIDEO_BUFFER<=m_dwBufferSize) return false;

		//计算缓冲
		DWORD dwAddBufferSize=__max(m_dwBufferSize/4L,wDataSize)*10;
		DWORD dwNewBufferSize=__min(m_dwBufferSize+dwAddBufferSize,MAX_VIDEO_BUFFER);		

		//申请内存
		BYTE * pNewVideoBuffer=new BYTE [dwNewBufferSize];
		if (pNewVideoBuffer==NULL) return false;

		//拷贝数据
		if (m_pVideoDataBuffer!=NULL) 
		{
			CopyMemory(pNewVideoBuffer,m_pVideoDataBuffer,m_dwDataSize);				
		}

		//设置变量			
		m_dwBufferSize=dwNewBufferSize;
		SafeDeleteArray(m_pVideoDataBuffer);
		m_pVideoDataBuffer=pNewVideoBuffer;
	}
	catch (...) 
	{
		ASSERT (FALSE);
		return false; 
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
