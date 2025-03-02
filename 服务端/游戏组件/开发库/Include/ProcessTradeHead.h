#ifndef PROCESS_TRADE_HEAD_HEAD_FILE
#define PROCESS_TRADE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨�ļ�
#include "Platform.h"           

//��ͷ�ļ�
#include "CommonServiceHead.h"                

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef PROCESS_TRADE_CLASS
	#ifdef  PROCESS_TRADE_DLL
		#define PROCESS_TRADE_CLASS _declspec(dllexport)
	#else
		#define PROCESS_TRADE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define PROCESS_TRADE_DLL_NAME	TEXT("ProcessTrade.dll")			//��� DLL ����
#else
	#define PROCESS_TRADE_DLL_NAME	TEXT("ProcessTradeD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IProcessTrade INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessTrade={0xc4e4c3b5,0x6204,0x4ec5,0x008c,0xf1,0xe8,0x30,0x6e,0xf1,0xad,0xf7};
#else
	#define VER_IProcessTrade INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessTrade={0xeb98b2ba,0xfbb6,0x46bb,0x008f,0x95,0x78,0x45,0xb2,0x60,0x63,0x90};
#endif

//����ͨѶ
interface IProcessTrade : public IUnknownEx
{
	//���ýӿ�
public:
	//��ȡ���
	virtual HWND __cdecl GetTradeKernelWnd()=NULL;
	//�������
	virtual bool __cdecl SetProcessTradeSink(IUnknownEx * pIUnknownEx)=NULL;

	//���ӹ���
public:
	//�ر�ͨѶ
	virtual bool __cdecl CloseProcessTrade()=NULL;
	//��������
	virtual bool __cdecl CreateProcessTrade(LPCTSTR pszClassName)=NULL;

	//���ͽӿ�
public:
	//��������
	virtual bool __cdecl SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//��������
	virtual bool __cdecl SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)=NULL;
	//ִ������
	virtual bool __cdecl TransferCommand(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IProcessTradeSink INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessTradeSink={0x29ff40d6,0xbe04,0x4ecc,0x00b0,0x06,0x86,0x91,0x18,0xa3,0xed,0xdd};
#else
	#define VER_IProcessTradeSink INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessTradeSink={0x0505dc10,0x3e03,0x4d4f,0x0080,0xe3,0x7a,0xc3,0x88,0x9c,0x58,0xda};
#endif

//���ݽӿ�
interface IProcessTradeSink : public IUnknownEx
{
	//��������
	virtual bool __cdecl OnProcessTradeData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, HWND hWndSend)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IIPCRecvDataSink INTERFACE_VERSION(1,1)
	static const GUID IID_IIPCRecvDataSink={0x653995d6,0xa5f6,0x435a,0x009f,0xb1,0xb8,0xff,0xd4,0x1d,0xfd,0x52};
#else
	#define VER_IIPCRecvDataSink INTERFACE_VERSION(1,1)
	static const GUID IID_IIPCRecvDataSink={0x06711045,0x5e25,0x4ed6,0x0082,0xfc,0xc2,0xf9,0x19,0x7e,0x2f,0x0b};
#endif

//���ݽӿ�
interface IIPCRecvDataSink : public IUnknownEx
{
	//ͬ������
	virtual bool __cdecl OnImmediateData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, HWND hWndSend)=NULL;
	//�첽����
	virtual bool __cdecl OnAsynchronismData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, HWND hWndSend)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//���������

DECLARE_MODULE_HELPER(ProcessTrade, ProcessTrade, PROCESS_TRADE_DLL_NAME,"CreateProcessTrade")

//////////////////////////////////////////////////////////////////////////
//�����ļ�

#ifndef PROCESS_TRADE_DLL
	#include "IPCDataControl.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif