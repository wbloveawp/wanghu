#include "StdAfx.h"
#include "GameVideoItem.h"


//////////////////////////////////////////////////////////////////////////////////


//ѹ���ļ�
#include "Compress\zlib.h"
#include "Compress\zconf.h"
//�����ļ�
#ifndef _DEBUG
#pragma comment(lib,"Compress\\zlibstatic.lib")
#else
#pragma comment(lib,"Compress\\zlibstaticd.lib")
#endif

// 
//��������
#define MAX_VIDEO_BUFFER							16384*10				//¼�񻺳�

//////////////////////////////////////////////////////////////////////////////////
//��̬����
INT CGameVideoItem::m_nRefCount=0;
DWORD CGameVideoItem::m_dwCompressSize=0;
LPBYTE CGameVideoItem::m_pCompressBuffer=NULL;

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CGameVideoItem::CGameVideoItem()
{
	//��������
	m_nRefCount++;

	//״̬����
	m_bRecing=false;
	m_wTableID=INVALID_TABLE;
	m_szVideoNumber[0]=0;

	//�������
	m_dwDataSize=0;
	m_dwPacketPos=0;
	m_dwBufferSize=0;	
	m_pVideoDataBuffer=NULL;

	//�ӿڱ���
	m_pVideoServer=NULL;
	m_pIGameVideoItemSink=NULL;
}

//��������
CGameVideoItem::~CGameVideoItem()
{
	//��������
	m_nRefCount--;

	//�ͷ���Դ	
	SafeDeleteArray(m_pVideoDataBuffer);	

	//�ͷ���Դ
	if (m_nRefCount==0)
	{
		m_dwCompressSize=0;
		SafeDeleteArray(m_pCompressBuffer);
	}
}

//�ӿڲ�ѯ
VOID * CGameVideoItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameVideoItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameVideoItem,Guid,dwQueryVer);
	return NULL;
}

//��ʼ¼��
bool CGameVideoItem::StartVideo()
{
	//״̬У��
	ASSERT (m_bRecing==false);
	if (m_bRecing==true) return false;

	//����״̬
	m_bRecing=true;

	//����ṹ
	tagVideoHead VideoHead={};
	VideoHead.dwStartTime=(DWORD)time(NULL);

	//�����С
	WORD wDataSize=sizeof(tagVideoHead)+sizeof(tagVideoServer);

	//У�黺��
	if ((m_dwDataSize+wDataSize)>m_dwBufferSize)
	{	
		//��������
		if (RectifyBuffer(wDataSize)==false)
		{
			return false;
		}
	}

	try
	{
		//��������
		CopyMemory(m_pVideoDataBuffer,&VideoHead,sizeof(tagVideoHead));		
		CopyMemory(m_pVideoDataBuffer+sizeof(tagVideoHead),m_pVideoServer,sizeof(tagVideoServer));		

		//���ñ���
		m_dwDataSize=wDataSize;
		m_dwPacketPos=m_dwDataSize;
		
	}
	catch (...) 
	{
		ASSERT (FALSE);
		return false;
	}

	//��ʼ֪ͨ
	if (m_pIGameVideoItemSink)
	{
		m_pIGameVideoItemSink->OnEventVideoStart(this);
	}	

	return true;
}

//ֹͣ¼��
bool CGameVideoItem::ConcludeVideo()
{
	//״̬У��
	ASSERT (m_bRecing==true);
	if (m_bRecing==false) return false;

	//��ʼ֪ͨ
	if (m_pIGameVideoItemSink)
	{
		m_pIGameVideoItemSink->OnEventVideoConclude(this);
	}

	//����״̬
	m_bRecing=false;

	//ѹ������
	if (CompressData()==false)
	{
		return false;
	}

	return true;
}

//�����û�
bool CGameVideoItem::InsertVideoUser(IServerUserItem * pIServerUserItem)
{
	//״̬У��
	ASSERT (m_bRecing && pIServerUserItem!=NULL);
	if (!m_bRecing || pIServerUserItem==NULL) return false;

	//ͷ����Ϣ
	tagVideoHead * pVideoHead=(tagVideoHead *)m_pVideoDataBuffer;

	//У���С
	WORD wUserItemSize=sizeof(tagVideoUser);
	if (m_dwDataSize!=(sizeof(tagVideoHead)+sizeof(tagVideoServer)+pVideoHead->cbUserCount*wUserItemSize))
	{
		return false;
	}

	//У�黺��
	if ((m_dwDataSize+wUserItemSize)>m_dwBufferSize)
	{	
		//��������
		if (RectifyBuffer(wUserItemSize)==false)
		{
			return false;
		}
	}

	//��������
	tagVideoUser VideoUser;
	ZeroMemory(&VideoUser,sizeof(VideoUser));

	//����ṹ
	VideoUser.dwUserID=pIServerUserItem->GetUserID();
	VideoUser.wChairID=pIServerUserItem->GetChairID();
	StringCchCopy(VideoUser.szNickName,CountArray(VideoUser.szNickName), pIServerUserItem->GetAccounts());

	try
	{
		//��������
		CopyMemory(m_pVideoDataBuffer+m_dwDataSize,&VideoUser,wUserItemSize);

		//���ñ���
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

//��������
bool CGameVideoItem::InsertVideoData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//У��״̬
	ASSERT (m_bRecing==true);
	if (m_bRecing==false) return false;

	//У�黺��
	WORD wPacketSize=sizeof(tagVideoPacket)+wDataSize;	
	if ((m_dwDataSize+wPacketSize)>m_dwBufferSize)
	{	
		//��������
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

	//��������
	tagVideoPacket VideoPacket;
	ZeroMemory(&VideoPacket,sizeof(VideoPacket));

	//λ����Ϣ
	VideoPacket.wChairID=wChairID;

	//������Ϣ	
	VideoPacket.wDataSize=wDataSize;
	VideoPacket.wSubCmdID=wSubCmdID;	
	VideoPacket.wMainCmdID=wMainCmdID;

	//����ṹ
	VideoPacket.wPacketSize=wPacketSize;			
	VideoPacket.dwPacketPos=m_dwPacketPos;
	VideoPacket.dwInsertTime=(DWORD)time(NULL);		

	try
	{
		//����λ��
		m_dwPacketPos=m_dwDataSize;

		//��������
		CopyMemory(m_pVideoDataBuffer+m_dwDataSize,&VideoPacket,sizeof(VideoPacket));
		m_dwDataSize+=sizeof(VideoPacket);

		//��������
		CopyMemory(m_pVideoDataBuffer+m_dwDataSize,pData,wDataSize);
		m_dwDataSize+=wDataSize;		

		//ͳ�ư���
		((tagVideoHead *)m_pVideoDataBuffer)->wPacketCount++;		
	}
	catch (...) 
	{
		return false;
	}

	return true;
}

//ѹ������
bool CGameVideoItem::CompressData()
{
	//Ч��ָ��
	if (m_pCompressBuffer==NULL || m_dwDataSize>m_dwCompressSize)
	{
		try
		{
			//�����ڴ�			
			BYTE * pCompressBuffer=new BYTE[m_dwDataSize];
			if (pCompressBuffer==NULL) return false;

			//�û�ָ��
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

	//����ͷ��
	((tagVideoHead *)m_pVideoDataBuffer)->dwDataSize=m_dwDataSize;
	((tagVideoHead *)m_pVideoDataBuffer)->cbCompressKind=COMPRESS_KIND_NONE;

	//��������
	DWORD dwCompressSize=m_dwCompressSize;
	DWORD dwSourceSize=m_dwDataSize-sizeof(tagVideoHead);
	LPBYTE pcbSourceData=m_pVideoDataBuffer+sizeof(tagVideoHead);

	//ѹ������
	if (compress(m_pCompressBuffer,&dwCompressSize,pcbSourceData,dwSourceSize)==0L)
	{
		//��������
		CopyMemory(m_pVideoDataBuffer+sizeof(tagVideoHead),m_pCompressBuffer,dwCompressSize);

		//���ñ���
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

//��������
VOID CGameVideoItem::ResetVideoItem()
{
	//״̬����
	m_bRecing=false;
	m_wTableID=INVALID_TABLE;

	//���ñ���
	m_dwDataSize=0;
	m_dwPacketPos=0;
	m_szVideoNumber[0]=0;
}

//��������
bool CGameVideoItem::RectifyBuffer(WORD wDataSize)
{
	try
	{
		//����У��
		if (MAX_VIDEO_BUFFER<=m_dwBufferSize) return false;

		//���㻺��
		DWORD dwAddBufferSize=__max(m_dwBufferSize/4L,wDataSize)*10;
		DWORD dwNewBufferSize=__min(m_dwBufferSize+dwAddBufferSize,MAX_VIDEO_BUFFER);		

		//�����ڴ�
		BYTE * pNewVideoBuffer=new BYTE [dwNewBufferSize];
		if (pNewVideoBuffer==NULL) return false;

		//��������
		if (m_pVideoDataBuffer!=NULL) 
		{
			CopyMemory(pNewVideoBuffer,m_pVideoDataBuffer,m_dwDataSize);				
		}

		//���ñ���			
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
