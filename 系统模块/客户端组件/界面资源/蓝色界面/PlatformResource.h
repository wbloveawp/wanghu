#ifndef PLATFORM_RESOURCE_HEAD_FILE
#define PLATFORM_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "SkinResource.h"
#include "PlazaResource.h"
#include "FrameResource.h"

//////////////////////////////////////////////////////////////////////////

//ƽ̨��Դ
class CPlatformResource : public IPlatformResource
{
	//�ں˱���
protected:
	HINSTANCE						m_hInstance;						//��Դ���

	//��Դ����
protected:
	CSkinResource					m_SkinResource;						//������Դ
	CPlazaResource					m_PlazaResource;					//������Դ
	CFrameResource					m_FrameResource;					//�����Դ

	//��������
public:
	//���캯��
	CPlatformResource();
	//��������
	virtual ~CPlatformResource();

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
	//��ȡԤ��
	virtual VOID __cdecl GetSkinPreview(CImage & ImagePreview);

	//������Դ
public:
	//������Դ
	virtual VOID * __cdecl GetSkinResource(REFGUID Guid, DWORD dwQueryVer);
	//������Դ
	virtual VOID * __cdecl GetPlazaResource(REFGUID Guid, DWORD dwQueryVer);
	//�����Դ
	virtual VOID * __cdecl GetFrameResource(REFGUID Guid, DWORD dwQueryVer);
};

//////////////////////////////////////////////////////////////////////////

//ȫ�ֱ���
extern CPlatformResource g_PlatformResource;

//////////////////////////////////////////////////////////////////////////

#endif