#ifndef VIDEO_SERVICE_HEAD_FILE
#define VIDEO_SERVICE_HEAD_FILE

/////////////////////////////////////////////////////////////////////////////////
//MFC�ļ�
#include <afxtempl.h>

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//ƽ̨�ļ�
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\���������\��Ϸ����\GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef VIDEO_SERVICE_CLASS
	#ifdef VIDEO_SERVICE_DLL
		#define VIDEO_SERVICE_CLASS _declspec(dllexport)
	#else
		#define VIDEO_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define VIDEO_SERVICE_DLL_NAME	TEXT("VideoService.dll")			//�������
#else
	#define VIDEO_SERVICE_DLL_NAME	TEXT("VideoServiceD.dll")			//�������
#endif


//¼�����
struct tagGameVideoParameter
{
	//���ñ���
	tagGameServiceAttrib *			pGameServiceAttrib;					//��������
	tagGameServiceOption *			pGameServiceOption;					//���ò���
};

//////////////////////////////////////////////////////////////////////////////////

//�ӿ�����
interface IGameVideoItem;
interface IGameVideoManager;
interface VER_IGameVideoItemSink;

//////////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IGameVideoItem INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoItem={0x5d8433e7,0x6c57,0x462b,0x8c,0xfe,0x84,0xb7,0xbd,0x97,0x83,0xfa};
#else
#define VER_IGameVideoItem INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoItem={0xf1ff8ad,0x7ea6,0x4097,0x8c,0x89,0xa2,0xcd,0x1e,0x84,0x56,0x97};
#endif

//��Ϸ¼��
interface IGameVideoItem : public IUnknownEx
{
	//���ƽӿ�
public:
	//��ʼ¼��
	virtual bool StartVideo()=NULL;	
	//ֹͣ¼��
	virtual bool ConcludeVideo()=NULL;	

	//���ݽӿ�
public:
	//ѹ����С
	virtual DWORD GetCompressSize()=NULL;
	//ѹ������
	virtual LPBYTE GetCompressBuffer()=NULL;

	//��Ϣ�ӿ�
public:
	//���ӱ�ʶ
	virtual WORD _cdecl GetTableID()=NULL;
	//¼����
	virtual LPCTSTR GetVideoNumber()=NULL;
	
	//���ܽӿ�
public:	
	//�����û�
	virtual bool InsertVideoUser(IServerUserItem * pIServerUserItem)=NULL;
	//��������
	virtual bool InsertVideoData(WORD wChairID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;	
};

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameVideoManager INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoManager={0xe08ed997,0xbbcb,0x4f45,0x96,0x8c,0x69,0x39,0xf4,0xcb,0xec,0x5a};
#else
#define VER_IGameVideoManager INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoManager={0x559cc859,0xbc7f,0x4c6a,0xa5,0x4b,0xab,0x38,0xd0,0xf0,0x98,0xe3};
#endif

//¼�����
interface IGameVideoManager : public IUnknownEx
{
	//���ú���
public:	
	//���ýӿ�
	virtual bool SetGameVideoItemSink(IUnknownEx * pIUnknownEx)=NULL;
	//�������
	virtual void InitVideoManager(tagGameVideoParameter & GameVideoParameter)=NULL;

	//������
public:
	//����¼��
	virtual IGameVideoItem * ActiveVideoItem(WORD wTableID)=NULL;
	//ɾ��¼��
	virtual bool DeleteVideoItem(IGameVideoItem * pIGameVideoItem)=NULL;	
};


//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameVideoItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoItemSink={0x2584ef2a,0xe303,0x476f,0xbc,0xa7,0x9f,0xeb,0xcf,0x28,0xe9,0x54};
#else
#define VER_IGameVideoItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IGameVideoItemSink={0x710114e,0xfda3,0x4c30,0x8a,0x4c,0x31,0x1,0x65,0x45,0x42,0x7f};
#endif

//¼��ص�
interface IGameVideoItemSink :public IUnknownEx
{
	//¼��ӿ�
public:
	//¼��ʼ
	virtual void OnEventVideoStart(IGameVideoItem * pIGameVideoItem)=NULL;
	//¼��ֹͣ
	virtual void OnEventVideoConclude(IGameVideoItem * pIGameVideoItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifndef VIDEO_SERVICE_DLL

	#define	VIDEO_MODULE_HELPER
	
	DECLARE_MODULE_HELPER(GameVideoManager, GameVideoManager, VIDEO_SERVICE_DLL_NAME,"CreateGameVideoManager")

#endif

//////////////////////////////////////////////////////////////////////////////////

#endif