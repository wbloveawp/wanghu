#ifndef GAME_VIDEO_MANAGER_HEAD_FILE
#define GAME_VIDEO_MANAGER_HEAD_FILE

#pragma once

//�����ļ�
#include "GameVideoItem.h"
#include "VideoServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//������
typedef CWHArray<CGameVideoItem *> CGameVideoItemArray;
typedef CMap<WORD,WORD,CGameVideoItem *,CGameVideoItem *> CGameVideoItemMap;

//////////////////////////////////////////////////////////////////////////////////

//¼�����
class VIDEO_SERVICE_CLASS CGameVideoManager : public IGameVideoManager
{
	//���ñ���
protected:
	tagVideoServer					m_VideoServer;						//������Ϣ

	//ϵͳ����
protected:	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������	

	//�û�����
protected:	
	CGameVideoItemArray				m_GameVideoItemActive;				//¼������
	CGameVideoItemArray				m_GameVideoItemBuffer;				//�洢¼��

	//����ӿ�
protected:
	IGameVideoItemSink *			m_pIGameVideoItemSink;				//�ص��ӿ�

	//��������
public:
	//���캯��
	CGameVideoManager();
	//��������
	virtual ~CGameVideoManager();

	//���κ���
private:
	//��������
	CGameVideoManager(const CGameVideoManager&);
	//��ֵ����
	CGameVideoManager& operator=(const CGameVideoManager&);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ú���
public:
	//���ýӿ�
	virtual bool SetGameVideoItemSink(IUnknownEx * pIUnknownEx);
	//�������
	virtual void InitVideoManager(tagGameVideoParameter & GameVideoParameter);

	//������
public:
	//����¼��
	virtual IGameVideoItem * ActiveVideoItem(WORD wTableID);
	//ɾ��¼��
	virtual bool DeleteVideoItem(IGameVideoItem * pIGameVideoItem);	

	//��������
private:
	//¼����
	VOID BuildVideoNumber(WCHAR szBuffer[], WORD wBufferLen);
};

//////////////////////////////////////////////////////////////////////////////////

#endif