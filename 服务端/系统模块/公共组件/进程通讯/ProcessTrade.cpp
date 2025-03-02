#include "StdAfx.h"
#include "ProcessTrade.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

//��ʶ����
#define IDC_PROCESS_TRADE			100									//���ڱ�ʶ

//��Ϣ����
#define WM_ASYNCHRONISM_DATA		(WM_USER+100)						//�첽����

//////////////////////////////////////////////////////////////////////////

//�첽����
struct tagAsynchronismData
{
	HWND							hWndSend;							//���ʹ���
	WORD							wDataSize;							//���ݴ�С
	WORD							wSubCmdID;							//��������
	WORD							wMainCmdID;							//��������
	BYTE							cbBuffer[IPC_PACKET];				//���ݻ���
};

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CProcessTrade, CWnd)
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_ASYNCHRONISM_DATA,OnAsynchronismData)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CProcessTrade::CProcessTrade()
{
	//���ñ���
	m_pIProcessTradeSink=NULL;

	return;
}

//��������
CProcessTrade::~CProcessTrade()
{
	CloseProcessTrade();
}

//�ӿڲ�ѯ
VOID * __cdecl CProcessTrade::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IProcessTrade,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IProcessTrade,Guid,dwQueryVer);
	return NULL;
}

//��ȡ���
HWND __cdecl CProcessTrade::GetTradeKernelWnd()
{
	return m_hWnd;
}

//����ͨѶ
bool __cdecl CProcessTrade::SetProcessTradeSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IProcessTradeSink)!=NULL);
		m_pIProcessTradeSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IProcessTradeSink);

		//�ɹ��ж�
		if (m_pIProcessTradeSink==NULL) return false;
	}
	else m_pIProcessTradeSink=NULL;

	return true;
}

//�ر�ͨѶ
bool __cdecl CProcessTrade::CloseProcessTrade()
{
	//�رմ���
	if (m_hWnd != NULL)
	{
		//��ȡ������
		TCHAR szClassName[64] = { 0 };
		GetClassName(m_hWnd, szClassName, CountArray(szClassName));

		//���ٴ���
		DestroyWindow();		

		//ע��������
		if (szClassName[0] != 0 && !UnregisterClass(szClassName, AfxGetInstanceHandle()))
		{
			ASSERT(FALSE);			
		}
	}

	return true;
}

//��������
bool __cdecl CProcessTrade::CreateProcessTrade(LPCTSTR pszClassName)
{
	//Ч�����
	ASSERT(m_pIProcessTradeSink!=NULL);
	if (m_pIProcessTradeSink==NULL) return false;

	//ע�ᴰ����
	if (pszClassName != NULL)
	{
		//��������
		WNDCLASSEX wndClass;
		ZeroMemory(&wndClass,sizeof(wndClass));

		//��������
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = ::DefWindowProc;
		wndClass.lpszClassName = pszClassName;
		wndClass.hInstance = AfxGetInstanceHandle();
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);			

		//ע������
		if (!RegisterClassEx(&wndClass))
		{
			ASSERT (FALSE);
			return false;
		}
	}	

	//��������
	if (m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		Create(pszClassName,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),IDC_PROCESS_TRADE);
	}

	return true;
}

//��������
bool __cdecl CProcessTrade::SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID)
{
	//Ч�����
	if (IsWindow(hWndServer)==FALSE) return false;

	//��������
	IPC_Head IPCHead;
	IPCHead.wVersion=IPC_VER;
	IPCHead.wSubCmdID=wSubCmdID;
	IPCHead.wMainCmdID=wMainCmdID;
	IPCHead.wPacketSize=sizeof(IPC_Head);

	//��������
	COPYDATASTRUCT CopyDataStruct;
	CopyDataStruct.lpData=&IPCHead;
	CopyDataStruct.dwData=IDT_ASYNCHRONISM;
	CopyDataStruct.cbData=IPCHead.wPacketSize;
	::SendMessage(hWndServer,WM_COPYDATA,(LPARAM)m_hWnd,(WPARAM)&CopyDataStruct);

	return true;
}

//��������
bool __cdecl CProcessTrade::SendData(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)
{
	//Ч�����
	if (IsWindow(hWndServer)==FALSE) return false;

	//��������
	IPC_Buffer IPCBuffer;
	IPCBuffer.Head.wVersion=IPC_VER;
	IPCBuffer.Head.wSubCmdID=wSubCmdID;
	IPCBuffer.Head.wMainCmdID=wMainCmdID;
	IPCBuffer.Head.wPacketSize=sizeof(IPCBuffer.Head);

	//��������
	if (pData!=NULL)
	{
		//Ч�鳤��
		ASSERT(wDataSize<=sizeof(IPCBuffer.cbBuffer));
		if (wDataSize>sizeof(IPCBuffer.cbBuffer)) return false;

		//��������
		IPCBuffer.Head.wPacketSize+=wDataSize;
		CopyMemory(IPCBuffer.cbBuffer,pData,wDataSize);
	}

	//��������
	COPYDATASTRUCT CopyDataStruct;
	CopyDataStruct.lpData=&IPCBuffer;
	CopyDataStruct.dwData=IDT_ASYNCHRONISM;
	CopyDataStruct.cbData=IPCBuffer.Head.wPacketSize;
	::SendMessage(hWndServer,WM_COPYDATA,(LPARAM)m_hWnd,(WPARAM)&CopyDataStruct);

	return true;
}

//ִ������
bool __cdecl CProcessTrade::TransferCommand(HWND hWndServer, WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)
{
	//Ч�����
	if (IsWindow(hWndServer)==FALSE) return false;

	//��������
	IPC_Buffer IPCBuffer;
	IPCBuffer.Head.wVersion=IPC_VER;
	IPCBuffer.Head.wSubCmdID=wSubCmdID;
	IPCBuffer.Head.wMainCmdID=wMainCmdID;
	IPCBuffer.Head.wPacketSize=sizeof(IPCBuffer.Head);

	//��������
	if (pData!=NULL)
	{
		//Ч�鳤��
		ASSERT(wDataSize<=sizeof(IPCBuffer.cbBuffer));
		if (wDataSize>sizeof(IPCBuffer.cbBuffer)) return false;

		//��������
		IPCBuffer.Head.wPacketSize+=wDataSize;
		CopyMemory(IPCBuffer.cbBuffer,pData,wDataSize);
	}

	//��������
	COPYDATASTRUCT CopyDataStruct;
	CopyDataStruct.lpData=&IPCBuffer;
	CopyDataStruct.dwData=IDT_IMMEDIATE;
	CopyDataStruct.cbData=IPCBuffer.Head.wPacketSize;
	::SendMessage(hWndServer,WM_COPYDATA,(LPARAM)m_hWnd,(WPARAM)&CopyDataStruct);

	return true;
}

//֪ͨ��Ϣ
LRESULT	CProcessTrade::OnAsynchronismData(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	tagDataHead DataHead;
	tagAsynchronismData AsynchronismData;
	if (m_DataQueue.DistillData(DataHead,&AsynchronismData,sizeof(AsynchronismData),true)==false)
	{
		ASSERT(FALSE);
		return 0;
	}

	//״̬Ч��
	ASSERT(m_pIProcessTradeSink!=NULL);
	if (m_pIProcessTradeSink==NULL) return 0;

	//���ݴ���
	HWND hWndClient=AsynchronismData.hWndSend;
	WORD wSubCmdID=AsynchronismData.wSubCmdID;
	WORD wMainCmdID=AsynchronismData.wMainCmdID;
	m_pIProcessTradeSink->OnProcessTradeData(wMainCmdID,wSubCmdID,AsynchronismData.cbBuffer,AsynchronismData.wDataSize,hWndClient);

	return 0;
}

//IPC ��Ϣ
BOOL CProcessTrade::OnCopyData(CWnd * pWnd, COPYDATASTRUCT * pCopyDataStruct)
{
	//Ч��ӿ�
	ASSERT(m_pIProcessTradeSink!=NULL);
	if (m_pIProcessTradeSink==NULL) return FALSE;
	ASSERT(pWnd!=NULL);
	if (pWnd==NULL) return FALSE;

	//Ч�����
	ASSERT(pCopyDataStruct->cbData>=sizeof(IPC_Head));
	if (pCopyDataStruct->cbData<sizeof(IPC_Head)) return FALSE;

	//Ч������
	IPC_Head * pHead=(IPC_Head *)pCopyDataStruct->lpData;
	ASSERT((pHead->wVersion==IPC_VER)&&(pHead->wPacketSize==pCopyDataStruct->cbData));
	if ((pHead->wVersion!=IPC_VER)||(pHead->wPacketSize!=pCopyDataStruct->cbData)) return FALSE;

	//���ݴ���
	switch (pCopyDataStruct->dwData)
	{
	case IDT_IMMEDIATE:			//ͬ������
		{
			//���ݴ���
			WORD wDataSize=pHead->wPacketSize-sizeof(IPC_Head);
			m_pIProcessTradeSink->OnProcessTradeData(pHead->wMainCmdID,pHead->wSubCmdID,pHead+1,wDataSize,pWnd->m_hWnd);

			return true;
		}
	case IDT_ASYNCHRONISM:		//�첽����
		{
			//��������
			tagAsynchronismData AsynchronismData;
			AsynchronismData.wSubCmdID=pHead->wSubCmdID;
			AsynchronismData.wMainCmdID=pHead->wMainCmdID;
			AsynchronismData.hWndSend=pWnd->GetSafeHwnd();
			AsynchronismData.wDataSize=pHead->wPacketSize-sizeof(IPC_Head);
			CopyMemory(&AsynchronismData.cbBuffer,pHead+1,pHead->wPacketSize-sizeof(IPC_Head));

			//��������
			WORD wHeadSize=sizeof(AsynchronismData)-sizeof(AsynchronismData.cbBuffer);
			m_DataQueue.InsertData(IDT_ASYNCHRONISM,&AsynchronismData,wHeadSize+AsynchronismData.wDataSize);

			//Ͷ����Ϣ
			PostMessage(WM_ASYNCHRONISM_DATA,(WPARAM)(pWnd->m_hWnd),(LPARAM)(pHead->wPacketSize));

			return true;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(ProcessTrade);

//////////////////////////////////////////////////////////////////////////
