#ifndef SKIN_RESOURCE_HEAD_FILE
#define SKIN_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��Դ������
class CSkinResource : public ISkinResource
{
	//��������
public:
	//���캯��
	CSkinResource();
	//��������
	virtual ~CSkinResource();

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

	//������Դ
public:
	//��������
	virtual bool __cdecl GetFontResource(CFont & Font);
	//��ȡ��Ϣ
	virtual bool __cdecl GetEncircleResource(tagSkinEncircleResource & SkinEncircleResource);

	//������Դ
public:
	//��ȡ��Ϣ
	virtual bool __cdecl GetMenuResource(tagSkinMenuResource & ResourceInfo);
	//��ȡ��Ϣ
	virtual bool __cdecl GetEditResource(tagSkinEditResource & ResourceInfo);
	//��ȡ��Ϣ
	virtual bool __cdecl GetDialogResource(tagSkinDialogResource & ResourceInfo);
	//��ȡ��Ϣ
	virtual bool __cdecl GetButtonResource(tagSkinButtonResource & ResourceInfo);
	//��ȡ��Ϣ
	virtual bool __cdecl GetComboBoxResource(tagSkinComboBoxResource & ResourceInfo);
	//��ȡ��Ϣ
	virtual bool __cdecl GetSplitterResource(tagSkinSplitterResource & ResourceInfo);
	//��ȡ��Ϣ
	virtual bool __cdecl GetListCtrlResource(tagSkinListCtrlResource & ResourceInfo);
	//��ȡ��Ϣ
	virtual bool __cdecl GetHyperLinkResource(tagSkinHyperLinkResource & ResourceInfo);
	//��ȡ��Ϣ
	virtual bool __cdecl GetTabControlResource(tagSkinTabControlResource & ResourceInfo);
	//��ȡ��Ϣ
	virtual	bool __cdecl GetScrollResource(tagSkinScrollResource &ResourceInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif