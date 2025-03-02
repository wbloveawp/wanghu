#ifndef FRAME_RESOURCE_HEAD_FILE
#define FRAME_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��ɫ����
#define COLOR_CHAT_BK				RGB(229,249,255)					//���챳��
#define COLOR_SERVER_TX				RGB(10,10,10)						//�б�����
#define COLOR_SERVER_BK				RGB(229,249,255)					//�б��ɫ

//��ɫ����
#define COLOR_SELECT_TX				RGB(0,0,0)							//������ɫ
#define COLOR_SELECT_BK				RGB(212,208,200)					//������ɫ
#define COLOR_NORMAL_TX				RGB(0,0,0)							//������ɫ
#define COLOR_NORMAL_BK				RGB(229,249,255)					//������ɫ

//��ɫ����
#define COLOR_MYSELF_TX				RGB(29,56,117)						//������ɫ
#define COLOR_MYSELF_BK				RGB(199,229,245)					//������ɫ
#define COLOR_MASTER_TX				RGB(95,106,17)						//������ɫ
#define COLOR_MASTER_BK				RGB(225,237,173)					//������ɫ
#define COLOR_MEMBER_TX				RGB(114,45,3)						//������ɫ
#define COLOR_MEMBER_BK				RGB(251,236,166)					//������ɫ

//////////////////////////////////////////////////////////////////////////

//�����Դ
class CFrameResource : public IFrameResource
{
	//��������
public:
	//���캯��
	CFrameResource();
	//��������
	virtual ~CFrameResource();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�ں˽ӿ�
public:
	//��Դ���
	virtual HINSTANCE __cdecl GetResInstance();

	//��Դ��Ϣ
public:
	//��Ϸ����
	virtual VOID __cdecl GetGameBarImage(tagGameBarImage & ResourceInfo);
	//��Ϸ���
	virtual VOID __cdecl GetGameBillImage(tagGameBillImage & ResourceInfo);
	//��Ϸ����
	virtual VOID __cdecl GetGameChatImage(tagGameChatImage & ResourceInfo);
	//��Ϸ���
	virtual VOID __cdecl GetGameFrameImage(tagGameFrameImage & ResourceInfo);
	//��Ϸ�û�
	virtual VOID __cdecl GetGameUserInfoImage(tagGameUserInfoImage & ResourceInfo);
	//��Ϸ�б�
	virtual VOID __cdecl GetGameUserListImage(tagGameUserListImage & ResourceInfo);
	//������Դ
	virtual VOID __cdecl GetBugleImage(tagBugleImage &ResourceInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif