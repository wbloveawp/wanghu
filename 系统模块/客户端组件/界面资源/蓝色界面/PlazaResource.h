#ifndef PLAZA_RESOURCE_HEAD_FILE
#define PLAZA_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//������Դ
class CPlazaResource : public IPlazaResource
{
	//��������
public:
	//���캯��
	CPlazaResource();
	//��������
	virtual ~CPlazaResource();

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
	//��������
	virtual VOID __cdecl GetPlazaChatImage(tagPlazaChatImage & ResourceInfo);
	//��������
	virtual VOID __cdecl GetPlazaTableImage(tagPlazaTableImage & ResourceInfo);
	//�������
	virtual VOID __cdecl GetPlazaFrameImage(tagPlazaFrameImage & ResourceInfo);
	//�����б�
	virtual VOID __cdecl GetPlazaServerImage(tagPlazaServerImage & ResourceInfo);
	//�������
	virtual VOID __cdecl GetPlazaBrowserImage(tagPlazaBrowserImage & ResourceInfo);
	//��������
	virtual VOID __cdecl GetPlazaServiceImage(tagPlazaServiceImage & ResourceInfo);
	//����˽��
	virtual VOID __cdecl GetPlazaWhisperImage(tagPlazaWhisperImage & ResourceInfo);
	//�����û�
	virtual VOID __cdecl GetPlazaUserListImage(tagPlazaUserListImage & ResourceInfo);
	//�����û�
	virtual VOID __cdecl GetPlazaUserInfoImage(tagPlazaUserInfoImage & ResourceInfo);
	//��������
	virtual VOID __cdecl GetPlazaCollocateImage(tagPlazaCollocateImage & ResourceInfo);
	//��ȫ����
	virtual VOID __cdecl GetPlazaSafePromptImage(tagPlazaSafePromptImage & ResourceInfo);
	//������Դ
	virtual VOID __cdecl GetBugleImage(tagBugleImage &ResourceInfo);
	//������Դ
	virtual VOID __cdecl GetDownloadImage(tagDownLoadImage &ResourceInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif