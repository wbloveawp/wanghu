#ifndef CLIENT_DEBUG_HEAD_FILE
#define CLIENT_DEBUG_HEAD_FILE
#pragma once

#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////////////////////
// 
//ƽ̨ͷ�ļ�
#include "Platform.h"           
#include "ClientServiceHead.h"                 

//////////////////////////////////////////////////////////////////////////////////////

#define CreateInterface( Hinstance, InterfacePoint, InterfaceName, CallDllName, CallFunctionName )		\
	InterfacePoint = NULL;																					\
	Hinstance = LoadLibrary(_T(CallDllName));															\
	if ( Hinstance )																						\
{																										\
	typedef void * (*CREATE)(); 																		\
	CREATE pFunction = (CREATE)GetProcAddress( Hinstance, CallFunctionName ); 							\
	if ( pFunction )																					\
	{																									\
	InterfacePoint = static_cast< InterfaceName * >( pFunction() );									\
	}																									\
}																										



#define ReleaseInterface(Hinstance, InterfacePoint)	\
	if( InterfacePoint )									\
{														\
	InterfacePoint->Release();							\
	InterfacePoint = NULL;								\
}														\
	if( Hinstance )											\
{														\
	FreeLibrary(Hinstance);								\
	Hinstance = NULL;									\
}	

///////////////////////////////////////////////////////////////////////////////////////
// �����
//��������
#define SUB_C_GET_SERVER_USER						100					//��ȡ�û�

//Ӧ������
#define SUB_S_SERVER_USER_LIST						1000				//�û��б�

///////////////////////////////////////////////////////////////////////////////////////

//�ص��ӿ�
interface IClientDebugCallback
{
	//��������
	virtual bool SendDebugData(WORD nMessageID, WORD wTableID, void * pData, WORD wDataSize) = NULL;	
};

///////////////////////////////////////////////////////////////////////////////////////
// 
// �ͻ��˽ӿ�
interface IClientDebug
{
	//���ƽӿ�
public:
	//�ͷŽӿ�
	virtual void Release() = NULL;
	//��������
	virtual bool Create(CWnd * pParentWnd, IClientDebugCallback * pIClientDebugCallback) = NULL;
	//��ʾ����
	virtual bool ShowWindow(bool bShow) = NULL;

	//��Ϣ�¼�
public:
	//��Ϣ����
	virtual bool OnEventDebugMessage(WORD nMessageID, WORD wTableID, void * pData, WORD nSize) = NULL;

	//�û��¼�
public:
	//�û�����
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem){};
	//�û��뿪
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem){};
	//�û�״̬
	virtual VOID OnEventUserStatus(IClientUserItem *pIClientUserItem){};
};

//////////////////////////////////////////////////////////////////////////////////

#endif
