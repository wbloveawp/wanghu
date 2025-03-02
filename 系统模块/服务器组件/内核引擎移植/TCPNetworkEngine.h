#ifndef TCP_NETWORK_ENGINE_HEAD_FILE
#define TCP_NETWORK_ENGINE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"
#include "AsynchronismEngine.h"
#include "Cipher.h"

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//��������
enum enOperationType
{
	enOperationType_Send,			//��������
	enOperationType_Recv,			//��������
};


enum enNetChannelType
{
	enNetChannelType_None,			//��������
	enNetChannelType_WebSocket,			//��������
	enNetChannelType_Socket
};

//////////////////////////////////////////////////////////////////////////

//��˵��
class COverLappedRecv;
class COverLappedSend;
class CTCPNetworkItem;
class CTCPNetworkEngine;

//���鶨��
typedef class CWHArray<COverLappedSend *>	COverLappedSendPtr;
typedef class CWHArray<COverLappedRecv *>	COverLappedRecvPtr;

//////////////////////////////////////////////////////////////////////////
//�ӿڶ���

//���Ӷ���ص��ӿ�
interface ITCPNetworkItemSink
{
	//���¼�
	virtual bool OnEventSocketBind(CTCPNetworkItem * pTCPNetworkItem)=NULL;
	//�ر��¼�
	virtual bool OnEventSocketShut(CTCPNetworkItem * pTCPNetworkItem)=NULL;
	//��ȡ�¼�
	virtual bool OnEventSocketRead(TCP_Command Command, VOID * pData, WORD wDataSize, CTCPNetworkItem * pTCPNetworkItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//�ص��ṹ��
class COverLapped
{
	//��������
public:
	WSABUF							m_WSABuffer;						//����ָ��
	OVERLAPPED						m_OverLapped;						//�ص��ṹ
	const enOperationType			m_OperationType;					//��������

	//��������
public:
	//���캯��
	COverLapped(enOperationType OperationType);
	//��������
	virtual ~COverLapped();

	//��Ϣ����
public:
	//��ȡ����
	enOperationType GetOperationType() { return m_OperationType; }
};

//////////////////////////////////////////////////////////////////////////

//�����ص��ṹ
class COverLappedSend : public COverLapped
{
	//��������
public:
	BYTE							m_cbBuffer[SOCKET_TCP_BUFFER];			//���ݻ���

	//��������
public:
	//���캯��
	COverLappedSend();
	//��������
	virtual ~COverLappedSend();
};

//////////////////////////////////////////////////////////////////////////

//�����ص��ṹ
class COverLappedRecv : public COverLapped
{
	//��������
public:
	//���캯��
	COverLappedRecv();
	//��������
	virtual ~COverLappedRecv();
};

//////////////////////////////////////////////////////////////////////////

//��������
class CTCPNetworkItem
{
	//��Ԫ����
	friend class CTCPNetworkEngine;

public:
	enum Mode
	{
		MODE_NONE, /// Server-side WebSocket.
		MODE_WEBSOCKET,  /// Client-side WebSocket.
		MODE_SOCKET
	};

	enum FrameFlags
		/// Frame header flags.
	{
		FRAME_FLAG_FIN = 0x80, /// FIN bit: final fragment of a multi-fragment message.
		FRAME_FLAG_RSV1 = 0x40, /// Reserved for future use. Must be zero.
		FRAME_FLAG_RSV2 = 0x20, /// Reserved for future use. Must be zero.
		FRAME_FLAG_RSV3 = 0x10, /// Reserved for future use. Must be zero.
	};

	enum FrameOpcodes
		/// Frame header opcodes.
	{
		FRAME_OP_CONT = 0x00, /// Continuation frame.
		FRAME_OP_TEXT = 0x01, /// Text frame.
		FRAME_OP_BINARY = 0x02, /// Binary frame.
		FRAME_OP_CLOSE = 0x08, /// Close connection.
		FRAME_OP_PING = 0x09, /// Ping frame.
		FRAME_OP_PONG = 0x0a, /// Pong frame.
		FRAME_OP_BITMASK = 0x0f  /// Bit mask for opcodes. 
	};

	enum SendFlags
		/// Combined header flags and opcodes for use with sendFrame().
	{
		FRAME_TEXT = FRAME_FLAG_FIN | FRAME_OP_TEXT,
		/// Use this for sending a single text (UTF-8) payload frame.
		FRAME_BINARY = FRAME_FLAG_FIN | FRAME_OP_BINARY
		/// Use this for sending a single binary payload frame.
	};

	//��������
protected:
	bool							m_bValidate;
	bool							m_bHandshake;
	Mode							m_Mode;						//����ͨ������
	DWORD							m_dwClientIP;						//���ӵ�ַ
	DWORD							m_dwActiveTime;						//����ʱ��

	//�ں˱���
protected:
	WORD							m_wIndex;							//��������
	WORD							m_wRountID;							//ѭ������
	WORD							m_wSurvivalTime;					//����ʱ��
	SOCKET							m_hSocketHandle;					//���Ӿ��
	CCriticalSection				m_CriticalSection;					//��������
	ITCPNetworkItemSink *			m_pITCPNetworkItemSink;				//�ص��ӿ�

	//״̬����
protected:
	bool							m_bSendIng;							//���ͱ�־
	bool							m_bRecvIng;							//���ձ�־
	bool							m_bShutDown;						//�رձ�־
	bool							m_bAllowBatch;						//����Ⱥ��
	BYTE							m_bBatchMask;						//Ⱥ����ʾ

	//���ձ���
protected:
	WORD							m_wRecvSize;						//���ճ���
	BYTE							m_cbRecvBuf[SOCKET_TCP_BUFFER*5];	//���ջ���
	CCipher							m_Cipher;							//�ӽ��ܶ���

	//�ڲ�����
protected:
	COverLappedRecv					m_OverLappedRecv;					//�ص��ṹ
	COverLappedSendPtr				m_OverLappedSendActive;				//�ص��ṹ

	//�������
protected:
	CCriticalSection				m_SendBufferSection;				//��������
	COverLappedSendPtr				m_OverLappedSendBuffer;				//�ص��ṹ

	//��������
public:
	//���캯��
	CTCPNetworkItem(WORD wIndex, ITCPNetworkItemSink * pITCPNetworkItemSink);
	//��������
	virtual ~CTCPNetworkItem();

	//��ʶ����
public:
	//��ȡ����
	inline WORD GetIndex() { return m_wIndex; }
	//��ȡ����
	inline WORD GetRountID() { return m_wRountID; }
	//��ȡ��ʶ
	inline DWORD GetIdentifierID() { return MAKELONG(m_wIndex,m_wRountID); }

	//���Ժ���
public:
	//��ȡ��ַ
	inline DWORD GetClientIP() { return m_dwClientIP; }
	//����ʱ��
	inline DWORD GetActiveTime() { return m_dwActiveTime; }

	//��������
	inline CCriticalSection & GetCriticalSection() { return m_CriticalSection; }

	//״̬����
public:
	//Ⱥ������
	inline bool IsAllowBatch() { return m_bAllowBatch; }
	//�ж�����
	inline bool IsValidSocket() { return (m_hSocketHandle!=INVALID_SOCKET); }
	//Ⱥ����ʾ
	inline BYTE GetBatchMask() { return m_bBatchMask; }

	//������
public:
	//�󶨶���
	DWORD Attach(SOCKET hSocket, DWORD dwClientIP);
	//�ָ�����
	DWORD ResumeData();

	//���ƺ���
public:
	//���ͺ���
	bool SendData(WORD wMainCmdID, WORD wSubCmdID, WORD wRountID);
	//���ͺ���
	bool SendData(VOID * pData, WORD wDataSize, WORD wMainCmdID, WORD wSubCmdID, WORD wRountID);

	bool SendData(VOID * pData, WORD wDataSize);
	//���ղ���
	bool RecvData();
	//�ر�����
	bool CloseSocket(WORD wRountID);

	//״̬����
public:
	//���ùر�
	bool ShutDownSocket(WORD wRountID);
	//����Ⱥ��
	bool AllowBatchSend(WORD wRountID, bool bAllowBatch,BYTE cbBatchMask);

	//֪ͨ�ӿ�
public:
	//�������
	bool OnSendCompleted(COverLappedSend * pOverLappedSend, DWORD dwThancferred);
	//�������
	bool OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred);
	//�ر����
	bool OnCloseCompleted();
	//��������websocket����Ϣ
	bool OnRecvWebSocketChanel();
	//��������socket����Ϣ
	bool OnRecvSocketChanel();



	//��������
private:
	//�����ж�
	inline bool SendVerdict(WORD wRountID);
	//��ȡ�ṹ
	inline COverLappedSend * GetSendOverLapped(WORD wPacketSize);
};

//////////////////////////////////////////////////////////////////////////

//��д�߳���
class CTCPNetworkThreadReadWrite : public CWHThread
{
	//��������
protected:
	HANDLE							m_hCompletionPort;					//��ɶ˿�

	//��������
public:
	//���캯��
	CTCPNetworkThreadReadWrite();
	//��������
	virtual ~CTCPNetworkThreadReadWrite();

	//���ܺ���
public:
	//���ú���
	bool InitThread(HANDLE hCompletionPort);

	//���غ���
private:
	//���к���
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//Ӧ���̶߳���
class CTCPNetworkThreadAccept : public CWHThread
{
	//��������
protected:
	SOCKET							m_hListenSocket;					//��������
	HANDLE							m_hCompletionPort;					//��ɶ˿�
	CTCPNetworkEngine *				m_pTCPNetworkEngine;				//��������

	//��������
public:
	//���캯��
	CTCPNetworkThreadAccept();
	//��������
	virtual ~CTCPNetworkThreadAccept();

	//���ܺ���
public:
	//���ú���
	bool InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CTCPNetworkEngine * pNetworkEngine);

	//���غ���
private:
	//���к���
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//����߳���
class CTCPNetworkThreadDetect : public CWHThread
{
	//��������
protected:
	DWORD							m_dwPileTime;						//����ʱ��
	DWORD							m_dwDetectTime;						//���ʱ��
	CTCPNetworkEngine *				m_pTCPNetworkEngine;				//��������

	//��������
public:
	//���캯��
	CTCPNetworkThreadDetect();
	//��������
	virtual ~CTCPNetworkThreadDetect();

	//���ܺ���
public:
	//���ú���
	bool InitThread(CTCPNetworkEngine * pNetworkEngine, DWORD dwDetectTime);

	//���غ���
private:
	//���к���
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//��˵��
typedef CWHArray<CTCPNetworkItem *> CTCPNetworkItemPtrArray;
typedef CWHArray<CTCPNetworkThreadReadWrite *> CTCPNetworkThreadRWPtrArray;

//��������
class CTCPNetworkEngine : public ITCPNetworkEngine, public IAsynchronismEngineSink, public ITCPNetworkItemSink
{
	friend class CTCPNetworkThreadDetect;
	friend class CTCPNetworkThreadAccept;
	friend class CTCPNetworkThreadReadWrite;

	//��֤����
protected:
	bool							m_bLimited;						//���б�־

	//��������
protected:
	bool							m_bService;							//�����־
	BYTE							m_cbBuffer[MAX_ASYNCHRONISM_DATA];	//��ʱ����

	//���ñ���
protected:
	WORD							m_wMaxConnect;						//�������
	WORD							m_wServicePort;						//�����˿�
	WORD							m_wCurrentPort;
	DWORD							m_dwDetectTime;						//���ʱ��

	//�ں˱���
protected:
	SOCKET							m_hServerSocket;					//���Ӿ��
	HANDLE							m_hCompletionPort;					//��ɶ˿�
	ITCPNetworkEngineEvent *		m_pITCPNetworkEngineEvent;			//�¼��ӿ�

	//�������
protected:
	CCriticalSection				m_ItemLocked;						//��������
	CTCPNetworkItemPtrArray			m_NetworkItemBuffer;				//��������
	CTCPNetworkItemPtrArray			m_NetworkItemActive;				//�����
	CTCPNetworkItemPtrArray			m_NetworkItemStorage;				//�洢����
	CTCPNetworkItemPtrArray			m_TempNetworkItemArray;				//��ʱ����

	//�������
protected:
	CCriticalSection				m_BufferLocked;						//��������
	CAsynchronismEngine				m_AsynchronismEngine;				//�첽����
	CTCPNetworkThreadDetect			m_SocketDetectThread;				//����߳�
	CTCPNetworkThreadAccept			m_SocketAcceptThread;				//Ӧ���߳�
	CTCPNetworkThreadRWPtrArray		m_SocketRWThreadArray;				//��д�߳�

	//��������
public:
	//���캯��
	CTCPNetworkEngine();
	//��������
	virtual ~CTCPNetworkEngine();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

	//��Ϣ�ӿ�
public:
	//���ö˿�
	virtual WORD GetServicePort();
	//��ǰ�˿�
	virtual WORD GetCurrentPort();

	//���ýӿ�
public:
	//���ýӿ�
	virtual bool SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx);
	//���ò���
	virtual bool SetServiceParameter(WORD wServicePort, WORD wMaxConnect, LPCTSTR  pszCompilation);
	
	//���ͽӿ�
public:
	//���ͺ���
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, BYTE cbBatchMask);

	//���ƽӿ�
public:
	//�ر�����
	virtual bool CloseSocket(DWORD dwSocketID);
	//���ùر�
	virtual bool ShutDownSocket(DWORD dwSocketID);
	//����Ⱥ��
	virtual bool AllowBatchSend(DWORD dwSocketID, bool bAllowBatch, BYTE cbBatchMask);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAsynchronismEngineStart() { return true; }
	//ֹͣ�¼�
	virtual bool OnAsynchronismEngineConclude() { return true; }
	//�첽����
	virtual bool OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//�ڲ�֪ͨ
public:
	//���¼�
	virtual bool OnEventSocketBind(CTCPNetworkItem * pTCPNetworkItem);
	//�ر��¼�
	virtual bool OnEventSocketShut(CTCPNetworkItem * pTCPNetworkItem);
	//��ȡ�¼�
	virtual bool OnEventSocketRead(TCP_Command Command, VOID * pData, WORD wDataSize, CTCPNetworkItem * pTCPNetworkItem);

	//��������
private:
	//�������
	bool DetectSocket();

	bool CreateTCPService(WORD wPort);

	//�������
protected:
	//������ж���
	CTCPNetworkItem * ActiveNetworkItem();
	//��ȡ����
	CTCPNetworkItem * GetNetworkItem(WORD wIndex);
	//�ͷ����Ӷ���
	bool FreeNetworkItem(CTCPNetworkItem * pTCPNetworkItem);
};

//////////////////////////////////////////////////////////////////////////

#endif