#ifndef PROCESS_TRADE_HEAD_FILE
#define PROCESS_TRADE_HEAD_FILE

#pragma once

#include "ProcessTradeHead.h"

//////////////////////////////////////////////////////////////////////////

//����ͨѶ
class CProcessTrade : public CWnd, public IProcessTrade
{
	//�������
protected:
	CWHDataQueue					m_DataQueue;						//���ݶ���
	IProcessTradeSink *				m_pIProcessTradeSink;				//�ص��ӿ�

	//��������
public:
	//���캯��
	CProcessTrade();
	//��������
	virtual ~CProcessTrade();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//��ȡ���
	virtual HWND __cdecl GetTradeKernelWnd();
	//����ͨѶ
	virtual bool __cdecl SetProcessTradeSink(IUnknownEx * pIUnknownEx);

	//���ӹ���
public:
	//�ر�ͨѶ
	virtual bool __cdecl CloseProcessTrade();
	//��������
	virtual bool __cdecl CreateProcessTrade(LPCTSTR pszClassName);

	//���ͽӿ�
public:
	//��������
	virtual bool __cdecl SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID);
	//��������
	virtual bool __cdecl SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);
	//ִ������
	virtual bool __cdecl TransferCommand(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);

	//��Ϣӳ��
protected:
	//�첽����
	LRESULT	OnAsynchronismData(WPARAM wParam, LPARAM lParam);
	//IPC ��Ϣ
	BOOL OnCopyData(CWnd * pWnd, COPYDATASTRUCT * pCopyDataStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif