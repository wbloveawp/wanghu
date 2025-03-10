#ifndef TCP_NETWORK_ENGINE_HEAD_FILE
#define TCP_NETWORK_ENGINE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"
#include "AsynchronismEngine.h"
#include "Cipher.h"

//////////////////////////////////////////////////////////////////////////
//枚举定义

//操作类型
enum enOperationType
{
	enOperationType_Send,			//发送类型
	enOperationType_Recv,			//接收类型
};


enum enNetChannelType
{
	enNetChannelType_None,			//发送类型
	enNetChannelType_WebSocket,			//接收类型
	enNetChannelType_Socket
};

//////////////////////////////////////////////////////////////////////////

//类说明
class COverLappedRecv;
class COverLappedSend;
class CTCPNetworkItem;
class CTCPNetworkEngine;

//数组定义
typedef class CWHArray<COverLappedSend *>	COverLappedSendPtr;
typedef class CWHArray<COverLappedRecv *>	COverLappedRecvPtr;

//////////////////////////////////////////////////////////////////////////
//接口定义

//连接对象回调接口
interface ITCPNetworkItemSink
{
	//绑定事件
	virtual bool OnEventSocketBind(CTCPNetworkItem * pTCPNetworkItem)=NULL;
	//关闭事件
	virtual bool OnEventSocketShut(CTCPNetworkItem * pTCPNetworkItem)=NULL;
	//读取事件
	virtual bool OnEventSocketRead(CMD_Command Command, VOID * pData, WORD wDataSize, CTCPNetworkItem * pTCPNetworkItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//重叠结构类
class COverLapped
{
	//变量定义
public:
	WSABUF							m_WSABuffer;						//数据指针
	OVERLAPPED						m_OverLapped;						//重叠结构
	const enOperationType			m_OperationType;					//操作类型

	//函数定义
public:
	//构造函数
	COverLapped(enOperationType OperationType);
	//析构函数
	virtual ~COverLapped();

	//信息函数
public:
	//获取类型
	enOperationType GetOperationType() { return m_OperationType; }
};

//////////////////////////////////////////////////////////////////////////

//发送重叠结构
class COverLappedSend : public COverLapped
{
	//变量定义
public:
	BYTE							m_cbBuffer[SOCKET_BUFFER];			//数据缓冲

	//函数定义
public:
	//构造函数
	COverLappedSend();
	//析构函数
	virtual ~COverLappedSend();
};

//////////////////////////////////////////////////////////////////////////

//接收重叠结构
class COverLappedRecv : public COverLapped
{
	//函数定义
public:
	//构造函数
	COverLappedRecv();
	//析构函数
	virtual ~COverLappedRecv();
};

//////////////////////////////////////////////////////////////////////////

//连接子项
class CTCPNetworkItem
{
	//友元声明
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

	//连接属性
protected:
	bool							m_bValidate;
	bool							m_bHandshake;
	Mode							m_Mode;						//网络通道类型
	DWORD							m_dwClientIP;						//连接地址
	DWORD							m_dwActiveTime;						//激活时间



	//内核变量
protected:
	WORD							m_wIndex;							//连接索引
	WORD							m_wRountID;							//循环索引
	WORD							m_wSurvivalTime;					//生存时间
	SOCKET							m_hSocketHandle;					//连接句柄
	CCriticalSection				m_CriticalSection;					//锁定对象
	ITCPNetworkItemSink *			m_pITCPNetworkItemSink;				//回调接口

	//状态变量
protected:
	bool							m_bSendIng;							//发送标志
	bool							m_bRecvIng;							//接收标志
	bool							m_bShutDown;						//关闭标志
	bool							m_bAllowBatch;						//接受群发
	BYTE							m_bBatchMask;						//群发标示

	//接收变量
protected:
	DWORD							m_dwRecvSize;						//接收长度
	BYTE							m_cbRecvBuf[SOCKET_BUFFER*5];	//接收缓冲
	CCipher							m_Cipher;							//加解密对象

	//内部变量
protected:
	COverLappedRecv					m_OverLappedRecv;					//重叠结构
	COverLappedSendPtr				m_OverLappedSendActive;				//重叠结构

	//缓冲变量
protected:
	CCriticalSection				m_SendBufferSection;				//锁定对象
	COverLappedSendPtr				m_OverLappedSendBuffer;				//重叠结构

	//函数定义
public:
	//构造函数
	CTCPNetworkItem(WORD wIndex, ITCPNetworkItemSink * pITCPNetworkItemSink);
	//析够函数
	virtual ~CTCPNetworkItem();

	//标识函数
public:
	//获取索引
	inline WORD GetIndex() { return m_wIndex; }
	//获取计数
	inline WORD GetRountID() { return m_wRountID; }
	//获取标识
	inline DWORD GetIdentifierID() { return MAKELONG(m_wIndex,m_wRountID); }

	//属性函数
public:
	//获取地址
	inline DWORD GetClientIP() { return m_dwClientIP; }
	//激活时间
	inline DWORD GetActiveTime() { return m_dwActiveTime; }

	//锁定对象
	inline CCriticalSection & GetCriticalSection() { return m_CriticalSection; }

	//状态函数
public:
	//群发允许
	inline bool IsAllowBatch() { return m_bAllowBatch; }
	//判断连接
	inline bool IsValidSocket() { return (m_hSocketHandle!=INVALID_SOCKET); }
	//群发标示
	inline BYTE GetBatchMask() { return m_bBatchMask; }

	//管理函数
public:
	//绑定对象
	DWORD Attach(SOCKET hSocket, DWORD dwClientIP);
	//恢复数据
	DWORD ResumeData();

	//控制函数
public:
	//发送函数
	bool SendData(WORD wMainCmdID, WORD wSubCmdID, WORD wRountID);
	//发送函数
	bool SendData(VOID * pData, WORD wDataSize, WORD wMainCmdID, WORD wSubCmdID, WORD wRountID);

	bool SendData(VOID * pData, WORD wDataSize);
	//接收操作
	bool RecvData();
	//关闭连接
	bool CloseSocket(WORD wRountID);

	//状态管理
public:
	//设置关闭
	bool ShutDownSocket(WORD wRountID);
	//允许群发
	bool AllowBatchSend(WORD wRountID, bool bAllowBatch,BYTE cbBatchMask);

	//通知接口
public:
	//发送完成
	bool OnSendCompleted(COverLappedSend * pOverLappedSend, DWORD dwThancferred);
	//接收完成
	bool OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred);
	//关闭完成
	bool OnCloseCompleted();
	//接收来自websocket的消息
	bool OnRecvWebSocketChanel();
	//接收来自socket的消息
	bool OnRecvSocketChanel();



	//辅助函数
private:
	//发送判断
	inline bool SendVerdict(WORD wRountID);
	//获取结构
	inline COverLappedSend * GetSendOverLapped(WORD wPacketSize);
};

//////////////////////////////////////////////////////////////////////////

//读写线程类
class CTCPNetworkThreadReadWrite : public CWHThread
{
	//变量定义
protected:
	HANDLE							m_hCompletionPort;					//完成端口

	//函数定义
public:
	//构造函数
	CTCPNetworkThreadReadWrite();
	//析构函数
	virtual ~CTCPNetworkThreadReadWrite();

	//功能函数
public:
	//配置函数
	bool InitThread(HANDLE hCompletionPort);

	//重载函数
private:
	//运行函数
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//应答线程对象
class CTCPNetworkThreadAccept : public CWHThread
{
	//变量定义
protected:
	SOCKET							m_hListenSocket;					//监听连接
	HANDLE							m_hCompletionPort;					//完成端口
	CTCPNetworkEngine *				m_pTCPNetworkEngine;				//网络引擎

	//函数定义
public:
	//构造函数
	CTCPNetworkThreadAccept();
	//析构函数
	virtual ~CTCPNetworkThreadAccept();

	//功能函数
public:
	//配置函数
	bool InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CTCPNetworkEngine * pNetworkEngine);

	//重载函数
private:
	//运行函数
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//检测线程类
class CTCPNetworkThreadDetect : public CWHThread
{
	//变量定义
protected:
	DWORD							m_dwPileTime;						//积累时间
	DWORD							m_dwDetectTime;						//检测时间
	CTCPNetworkEngine *				m_pTCPNetworkEngine;				//网络引擎

	//函数定义
public:
	//构造函数
	CTCPNetworkThreadDetect();
	//析构函数
	virtual ~CTCPNetworkThreadDetect();

	//功能函数
public:
	//配置函数
	bool InitThread(CTCPNetworkEngine * pNetworkEngine, DWORD dwDetectTime);

	//重载函数
private:
	//运行函数
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//类说明
typedef CWHArray<CTCPNetworkItem *> CTCPNetworkItemPtrArray;
typedef CWHArray<CTCPNetworkThreadReadWrite *> CTCPNetworkThreadRWPtrArray;

//网络引擎
class CTCPNetworkEngine : public ITCPNetworkEngine, public IAsyncEngineSink, public ITCPNetworkItemSink
{
	friend class CTCPNetworkThreadDetect;
	friend class CTCPNetworkThreadAccept;
	friend class CTCPNetworkThreadReadWrite;

	//验证变量
protected:
	bool							m_bLimited;							//运行标志
	bool							m_bValidate;						//验证标志
	bool							m_bValidatr;						//验证标志
	//辅助变量
protected:
	bool							m_bService;							//服务标志
	BYTE							m_cbBuffer[MAX_ASYNCHRONISM_DATA];	//临时对象

	//配置变量
protected:
	WORD							m_wMaxConnect;						//最大连接
	WORD							m_wServicePort;						//监听端口
	WORD							m_wCurrentPort;
	DWORD							m_dwDetectTime;						//检测时间

	//内核变量
protected:
	SOCKET							m_hServerSocket;					//连接句柄
	HANDLE							m_hCompletionPort;					//完成端口
	ITCPNetworkEngineEvent *		m_pITCPNetworkEngineEvent;			//事件接口

	//子项变量
protected:
	CCriticalSection				m_ItemLocked;						//子项锁定
	CTCPNetworkItemPtrArray			m_NetworkItemBuffer;				//空闲连接
	CTCPNetworkItemPtrArray			m_NetworkItemActive;				//活动连接
	CTCPNetworkItemPtrArray			m_NetworkItemStorage;				//存储连接
	CTCPNetworkItemPtrArray			m_TempNetworkItemArray;				//临时连接

	//组件变量
protected:
	CCriticalSection				m_BufferLocked;						//缓冲锁定
	CAsyncEngine				    m_AsynchronismEngine;				//异步对象
	CTCPNetworkThreadDetect			m_SocketDetectThread;				//检测线程
	CTCPNetworkThreadAccept			m_SocketAcceptThread;				//应答线程
	CTCPNetworkThreadRWPtrArray		m_SocketRWThreadArray;				//读写线程

	//函数定义
public:
	//构造函数
	CTCPNetworkEngine();
	//析构函数
	virtual ~CTCPNetworkEngine();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//服务接口
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();

	//信息接口
public:
	//配置端口
	virtual WORD GetServicePort();
	//当前端口
	virtual WORD GetCurrentPort();

	//配置接口
public:
	//设置接口
	virtual bool SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx);
	//设置参数
	virtual bool SetServiceParameter(WORD wServicePort, WORD wMaxConnect, LPCTSTR  pszCompilation);
	//证书配置
	virtual bool SetCertParameter(LPCTSTR pszCertName, LPCTSTR pszSystemProtocol, bool bMachineStore = false, bool bAuthClient = false);
	
	virtual bool AttestationAcc();
	virtual bool BttestationAcc();
	//发送接口
public:
	//发送函数
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//批量发送
	virtual bool SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, BYTE cbBatchMask);

	//控制接口
public:
	//关闭连接
	virtual bool CloseSocket(DWORD dwSocketID);
	//设置关闭
	virtual bool ShutDownSocket(DWORD dwSocketID);
	//允许群发
	virtual bool AllowBatchSend(DWORD dwSocketID, bool bAllowBatch, BYTE cbBatchMask);

	//异步接口
public:
	//启动事件
	virtual bool OnAsynchronismEngineStart() { return true; }
	//停止事件
	virtual bool OnAsyncEngineConclude() { return true; }
	//异步数据
	virtual bool OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//内部通知
public:
	//绑定事件
	virtual bool OnEventSocketBind(CTCPNetworkItem * pTCPNetworkItem);
	//关闭事件
	virtual bool OnEventSocketShut(CTCPNetworkItem * pTCPNetworkItem);
	//读取事件
	virtual bool OnEventSocketRead(CMD_Command Command, VOID * pData, WORD wDataSize, CTCPNetworkItem * pTCPNetworkItem);

	//辅助函数
private:
	//检测连接
	bool DetectSocket();
	//网页验证
	bool WebAttestation();

	bool CreateTCPService(WORD wPort);

	//对象管理
protected:
	//激活空闲对象
	CTCPNetworkItem * ActiveNetworkItem();
	//获取对象
	CTCPNetworkItem * GetNetworkItem(WORD wIndex);
	//释放连接对象
	bool FreeNetworkItem(CTCPNetworkItem * pTCPNetworkItem);
};

//////////////////////////////////////////////////////////////////////////

#endif