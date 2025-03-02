#ifndef GAME_VIDEO_ITEM_HEAD_FILE
#define GAME_VIDEO_ITEM_HEAD_FILE

#pragma once

#include "VideoServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//����˵��
class CGameVideoItem;

//����˵��
typedef CWHArray<CGameVideoItem *>	CGameVideoItemArray;

//////////////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class VIDEO_SERVICE_CLASS CGameVideoItem : public IGameVideoItem
{
	//��Ԫ����
	friend class CGameVideoManager;

	//״̬����
protected:
	bool							m_bRecing;							//¼��״̬
	WORD							m_wTableID;							//���ӱ�ʶ
	DWORD							m_dwPacketPos;						//����λ��
	WCHAR							m_szVideoNumber[LEN_VIDEO_NUMBER];	//¼����

	//���ݱ���
protected:
	DWORD							m_dwDataSize;						//���ݴ�С	
	DWORD							m_dwBufferSize;						//���峤��
	LPBYTE							m_pVideoDataBuffer;					//����ָ��

	//��̬����
private:
	static INT						m_nRefCount;						//���ü���
	static DWORD					m_dwCompressSize;					//ѹ����С
	static LPBYTE					m_pCompressBuffer;					//ѹ������

	//���ñ���
protected:
	tagVideoServer *				m_pVideoServer;						//������Ϣ
	IGameVideoItemSink *			m_pIGameVideoItemSink;				//�ص��ӿ�

	//��������
protected:
	//���캯��
	CGameVideoItem();
	//��������
	virtual ~CGameVideoItem();

	//���κ���
private:
	//��������
	CGameVideoItem(const CGameVideoItem&);
	//��ֵ����
	CGameVideoItem& operator=(const CGameVideoItem&); 

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ¼��
	virtual bool StartVideo();	
	//ֹͣ¼��
	virtual bool ConcludeVideo();	

	//ѹ���ӿ�
public:
	//ѹ����С
	virtual DWORD GetCompressSize() { return m_dwDataSize; }
	//ѹ������
	virtual LPBYTE GetCompressBuffer() { return m_pVideoDataBuffer; }

	//��Ϣ�ӿ�
public:
	//���ӱ�ʶ
	virtual WORD _cdecl GetTableID() { return m_wTableID; }
	//¼����
	virtual LPCTSTR GetVideoNumber() { return m_szVideoNumber; }
	
	//���ܽӿ�
public:	
	//�����û�
	virtual bool InsertVideoUser(IServerUserItem * pIServerUserItem);
	//��¼����
	virtual bool InsertVideoData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��������
protected:
	//ѹ������
	bool CompressData();
	//��������
	VOID ResetVideoItem();
	//��������
	bool RectifyBuffer(WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif