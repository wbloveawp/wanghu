#include "StdAfx.h"
#include "Afxinet.h"
#include "TCPNetworkEngine.h"
#include "TraceServiceManager.h"

#include "HttpParserWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//宏定义

#define MAX_PORT				9000
#define MIN_PORT				3000

//系数定义
#define DEAD_QUOTIETY				0									//死亡系数
#define DANGER_QUOTIETY				1									//危险系数
#define SAFETY_QUOTIETY				2									//安全系数

//动作定义
#define ASYNCHRONISM_SEND_DATA		1									//发送标识
#define ASYNCHRONISM_SEND_BATCH		2									//群体发送
#define ASYNCHRONISM_SHUT_DOWN		3									//安全关闭
#define ASYNCHRONISM_ALLOW_BATCH	4									//允许群发
#define ASYNCHRONISM_CLOSE_SOCKET	5									//关闭连接
#define ASYNCHRONISM_DETECT_SOCKET	6									//检测连接


#define HTTP_REQUEST_HEAD			"GET / HTTP/"

//索引辅助
#define SOCKET_INDEX(dwSocketID)	LOWORD(dwSocketID)					//位置索引
#define SOCKET_ROUNTID(dwSocketID)	HIWORD(dwSocketID)					//循环索引

//////////////////////////////////////////////////////////////////////////
//结构定义

//发送请求
struct tagSendDataRequest
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	WORD							wDataSize;							//数据大小
};

//群发请求
struct tagBatchSendRequest
{
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	WORD							wDataSize;							//数据大小
	BYTE                            cbBatchMask;                        //数据掩码
};

//允许群发
struct tagAllowBatchSend
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
	BYTE                            cbBatchMask;                        //数据掩码
	BYTE							cbAllowBatch;						//允许标志
};

//关闭连接
struct tagCloseSocket
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
};

//安全关闭
struct tagShutDownSocket
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
};

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLapped::COverLapped(enOperationType OperationType) : m_OperationType(OperationType)
{
	//设置变量
	ZeroMemory(&m_WSABuffer,sizeof(m_WSABuffer));
	ZeroMemory(&m_OverLapped,sizeof(m_OverLapped));

	return;
}

//析构函数
COverLapped::~COverLapped()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLappedSend::COverLappedSend() : COverLapped(enOperationType_Send)
{
	m_WSABuffer.len=0;
	m_WSABuffer.buf=(char *)m_cbBuffer;
}

//析构函数
COverLappedSend::~COverLappedSend()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLappedRecv::COverLappedRecv() : COverLapped(enOperationType_Recv)
{
	m_WSABuffer.len=0;
	m_WSABuffer.buf=NULL;
}

//析构函数
COverLappedRecv::~COverLappedRecv()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkItem::CTCPNetworkItem(WORD wIndex, ITCPNetworkItemSink * pITCPNetworkItemSink)
	: m_wIndex(wIndex), m_pITCPNetworkItemSink(pITCPNetworkItemSink)
{
	//连接属性
	m_Mode = MODE_NONE;
	m_bHandshake = false;

	m_bValidate = false;

	m_dwClientIP = 0L;
	m_dwActiveTime = 0L;

	//核心变量
	m_wRountID = 1;
	m_wSurvivalTime = DEAD_QUOTIETY;
	m_hSocketHandle = INVALID_SOCKET;

	//状态变量
	m_bSendIng = false;
	m_bRecvIng = false;
	m_bShutDown = false;
	m_bAllowBatch = false;
	m_bBatchMask = 0xFF;

	//接收变量
	m_dwRecvSize = 0;
	ZeroMemory(m_cbRecvBuf, sizeof(m_cbRecvBuf));


	return;
}

//析够函数
CTCPNetworkItem::~CTCPNetworkItem()
{
	//删除空闲
	for (INT_PTR i=0;i<m_OverLappedSendBuffer.GetCount();i++)
	{
		delete m_OverLappedSendBuffer[i];
	}

	//删除活动
	for (INT_PTR i=0;i<m_OverLappedSendActive.GetCount();i++)
	{
		delete m_OverLappedSendActive[i];
	}

	//删除数组
	m_OverLappedSendBuffer.RemoveAll();
	m_OverLappedSendActive.RemoveAll();

	return;
}

//绑定对象
DWORD CTCPNetworkItem::Attach(SOCKET hSocket, DWORD dwClientIP)
{
	//效验参数
	ASSERT(dwClientIP!=0);
	ASSERT(hSocket!=INVALID_SOCKET);

	//效验状态
	ASSERT(m_bRecvIng==false);
	ASSERT(m_bSendIng==false);
	ASSERT(m_hSocketHandle==INVALID_SOCKET);

	m_Mode = MODE_NONE;
	m_bHandshake = false;
	m_bValidate=false;
	//状态变量
	m_bSendIng=false;
	m_bRecvIng=false;
	m_bShutDown=false;
	m_bAllowBatch=false;
	m_bBatchMask=0xFF;

	//设置变量
	m_dwClientIP=dwClientIP;
	m_hSocketHandle=hSocket;
	m_wSurvivalTime=SAFETY_QUOTIETY;
	m_dwActiveTime=(DWORD)time(NULL);

	//发送通知
	m_pITCPNetworkItemSink->OnEventSocketBind(this);

	return GetIdentifierID();
}

//恢复数据
DWORD CTCPNetworkItem::ResumeData()
{
	//效验状态
	ASSERT(m_hSocketHandle==INVALID_SOCKET);

	m_Mode = MODE_NONE;
	m_bHandshake = false;
	m_bValidate = false;
	//连接属性
	m_dwClientIP=0L;
	m_dwActiveTime=0L;

	//核心变量
	m_wSurvivalTime= DEAD_QUOTIETY;
	m_hSocketHandle=INVALID_SOCKET;
	m_wRountID=__max(1,m_wRountID+1);

	//状态变量
	m_bSendIng=false;
	m_bRecvIng=false;
	m_bShutDown=false;
	m_bAllowBatch=false;

	//接收变量
	m_dwRecvSize = 0;
	ZeroMemory(m_cbRecvBuf,sizeof(m_cbRecvBuf));


	//重叠数组
	m_OverLappedSendBuffer.Append(m_OverLappedSendActive);
	m_OverLappedSendActive.RemoveAll();

	return GetIdentifierID();
}

//发送函数
bool CTCPNetworkItem::SendData(WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	if (m_hSocketHandle != INVALID_SOCKET
		&& m_wRountID == wRountID
		&& m_Cipher.EncryptBuffer(wMainCmdID, wSubCmdID, NULL, 0))
	{
		return SendData(m_Cipher.GetData(), m_Cipher.GetDataSize()) != 0;
	}
	return false;
}

//发送函数
bool CTCPNetworkItem::SendData(VOID * pData, WORD wDataSize, WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{

	if (wDataSize <= SOCKET_PACKET
		&& m_hSocketHandle != INVALID_SOCKET
		&& m_wRountID == wRountID
		&& m_Cipher.EncryptBuffer(wMainCmdID, wSubCmdID, pData, wDataSize))
	{
		return SendData(m_Cipher.GetData(), m_Cipher.GetDataSize()) != 0;
	}
	return false;
}

//100b830
bool CTCPNetworkItem::SendData(VOID * pData, WORD wDataSize)
{
	//发送判断
	if (SendVerdict(m_wRountID) == false) return false;

	WORD nHeadSize = 0;

	if (m_Mode == MODE_WEBSOCKET && m_bHandshake == true)
	{
		nHeadSize = wDataSize < 126 ? 2 : 4;
	}


	COverLappedSend * pOverLappedSend = GetSendOverLapped(nHeadSize + wDataSize);
	//COverLappedSend * pOverLappedSend = GetSendOverLapped( wDataSize);
	//关闭判断
	if (pOverLappedSend == NULL)
	{
		CloseSocket(m_wRountID);
		return false;
	}

	//之前的长度
	ULONG wBeforeSize = pOverLappedSend->m_WSABuffer.len;
	if (nHeadSize > 0)
	{
		if (nHeadSize == 2)
		{
			pOverLappedSend->m_cbBuffer[wBeforeSize] = FRAME_BINARY;
			pOverLappedSend->m_cbBuffer[wBeforeSize + 1] = (BYTE)wDataSize;
		}
		else if (nHeadSize == 4)
		{
			pOverLappedSend->m_cbBuffer[wBeforeSize] = FRAME_BINARY;
			pOverLappedSend->m_cbBuffer[wBeforeSize+1] = 126;  //网狐官方 并没有设定这个值
			pOverLappedSend->m_cbBuffer[wBeforeSize+2] = HIBYTE(wDataSize);
			pOverLappedSend->m_cbBuffer[wBeforeSize+3] = LOBYTE(wDataSize);
		}
	}
	
	//整包大小
	DWORD dwPacketSize = nHeadSize + wDataSize;
	pOverLappedSend->m_WSABuffer.len += dwPacketSize;
	//拷贝数据到缓存区
	CopyMemory(&pOverLappedSend->m_cbBuffer[wBeforeSize + nHeadSize ], pData, wDataSize);
	
	//未有数据在发送
	if (m_bSendIng == false)
	{
		//发送数据
		DWORD dwThancferred = 0;
		INT nResultCode = WSASend(m_hSocketHandle, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);

		//结果处理
		if ((nResultCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			CloseSocket(m_wRountID);
			return false;
		}

		//设置变量
		m_bSendIng = true;
	}

	return true;
}
//投递接收
bool CTCPNetworkItem::RecvData()
{
	//效验变量
	ASSERT(m_bRecvIng == false);
	ASSERT(m_hSocketHandle != INVALID_SOCKET);

	//接收数据
	DWORD dwThancferred = 0, dwFlags = 0;
	INT nResultCode = WSARecv(m_hSocketHandle, &m_OverLappedRecv.m_WSABuffer, 1, &dwThancferred, &dwFlags, &m_OverLappedRecv.m_OverLapped, NULL);

	//结果处理
	if ((nResultCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
	{
		CloseSocket(m_wRountID);
		return false;
	}

	//设置变量
	m_bRecvIng = true;

	return true;
}

//关闭连接
bool CTCPNetworkItem::CloseSocket(WORD wRountID)
{
	//状态判断
	if (m_wRountID != wRountID) return false;

	//关闭连接
	if (m_hSocketHandle != INVALID_SOCKET)
	{
		closesocket(m_hSocketHandle);
		m_hSocketHandle = INVALID_SOCKET;
	}

	//判断关闭
	if ((m_bRecvIng == false) && (m_bSendIng == false))
	{
		OnCloseCompleted();
	}

	return true;
}

//设置关闭
bool CTCPNetworkItem::ShutDownSocket(WORD wRountID)
{
	//状态判断
	if (m_hSocketHandle == INVALID_SOCKET) return false;
	if ((m_wRountID != wRountID) || (m_bShutDown == true)) return false;

	//设置变量
	m_dwRecvSize = 0;
	m_bShutDown = true;

	if(m_bSendIng == false)
	{
		WSASendDisconnect(m_hSocketHandle, NULL);
	}

	return true;
}

//允许群发
bool CTCPNetworkItem::AllowBatchSend(WORD wRountID, bool bAllowBatch,BYTE cbBatchMask)
{
	//状态判断
	if (m_wRountID!=wRountID) return false;
	if (m_hSocketHandle==INVALID_SOCKET) return false;

	//设置变量
	m_bAllowBatch=bAllowBatch;

	m_bBatchMask=cbBatchMask;

	return true;
}

//发送完成
bool CTCPNetworkItem::OnSendCompleted(COverLappedSend * pOverLappedSend, DWORD dwThancferred)
{
	//效验变量
	ASSERT(m_bSendIng==true);
	ASSERT(m_OverLappedSendActive.GetCount()>0);
	ASSERT(pOverLappedSend==m_OverLappedSendActive[0]);

	//释放结构
	m_OverLappedSendActive.RemoveAt(0);
	m_OverLappedSendBuffer.Add(pOverLappedSend);

	//设置变量
	m_bSendIng=false;

	//判断关闭
	if (m_hSocketHandle==INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//设置变量
	if (dwThancferred!=0)
	{
		m_wSurvivalTime=SAFETY_QUOTIETY;
	}

	//继续发送
	if (m_OverLappedSendActive.GetCount() > 0)
	{
		//获取数据
		pOverLappedSend = m_OverLappedSendActive[0];

		//发送数据
		DWORD dwThancferred = 0;
		INT nResultCode = WSASend(m_hSocketHandle, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);

		//结果处理
		if ((nResultCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			CloseSocket(m_wRountID);
			return false;
		}

		//设置变量
		m_bSendIng = true;
	}

	if (m_bShutDown && m_bSendIng == false)
	{
		//终止操作
		WSASendDisconnect(m_hSocketHandle, NULL);
	}
	return true;
}

//接收完成
bool CTCPNetworkItem::OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred)
{
	//效验数据
	ASSERT(m_bRecvIng == true);

	//设置变量
	m_bRecvIng = false;

	//判断关闭
	if (m_hSocketHandle == INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//接收数据
	INT nResultCode = recv(m_hSocketHandle, (char *)m_cbRecvBuf + m_dwRecvSize, sizeof(m_cbRecvBuf) - m_dwRecvSize, 0);

	//关闭判断
	if (nResultCode <= 0)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//中断判断
	if (m_bShutDown == true) return true;

	//设置变量
	m_dwRecvSize += nResultCode;
	m_wSurvivalTime = SAFETY_QUOTIETY;

	//通道类型未定义
	if (m_Mode == MODE_NONE)
	{
		int nRequestSize = lstrlenA(HTTP_REQUEST_HEAD);
		//开始解析请求头
		if (m_dwRecvSize < (DWORD)nRequestSize)
		{
			m_Mode = MODE_SOCKET;
		}

		else
		{
			std::string method;
			std::string version;
			std::string uri;
			method.reserve(16);

			if (_strnicmp((char*)m_cbRecvBuf, HTTP_REQUEST_HEAD, nRequestSize) == 0)
			{
				m_Mode = MODE_WEBSOCKET;
			}
			else
			{
				m_Mode = MODE_SOCKET;
			}
		}
	}
	
	//socket
	if (m_Mode == MODE_SOCKET)
	{
		OnRecvSocketChanel();
	}
	//websocket
	else if (m_Mode == MODE_WEBSOCKET)
	{
		OnRecvWebSocketChanel();
	}

	//OnRecvSocketChanel();

	return RecvData();
}

bool CTCPNetworkItem::OnRecvWebSocketChanel()
{
	if (m_Mode != MODE_WEBSOCKET) return false;

	try
	{
		while (true)
		{
			if ((!m_bHandshake || m_dwRecvSize < 2) && (m_bHandshake || m_dwRecvSize < 4))
				return true;
			
			//握手成功 读取消息
			if (m_bHandshake)
			{
				//获取fin
				BYTE cbFin = (m_cbRecvBuf[0] >> 7) & 0x01;
				if (cbFin == 0) return true;					//未接收完
				//获取opcode
				BYTE cbOpcode = m_cbRecvBuf[0] & 0x0F;
				//获取mask
				BYTE cbMask = m_cbRecvBuf[1] >> 7 & 0x01;
				//获取包大小
				WORD wPayloadSize = m_cbRecvBuf[1] & 0x7F;
				//数据包大小
				long long llDataSize = 0;
				//数据头大小
				WORD wHeadSize = 2;

				//获取数据包大小
				if (wPayloadSize < 126)
				{
					llDataSize = wPayloadSize;
				}
				else if (wPayloadSize == 126)
				{
					if (m_dwRecvSize >= 4)
					{
						llDataSize = ((m_cbRecvBuf[2] << 8) | (m_cbRecvBuf[3]));
						if (llDataSize > SOCKET_BUFFER) throw TEXT("数据包超长");
						wHeadSize += 2;
					}
				}
				else if (wPayloadSize == 127)
				{
					if (m_dwRecvSize >= 10)//8个字节形成的64bits无符号整型数的值
					{
						//高4位
						llDataSize = ( (m_cbRecvBuf[2] << 24) | (m_cbRecvBuf[3] << 16) | (m_cbRecvBuf[4] << 8) | (m_cbRecvBuf[5]) );
						llDataSize <<= 32;
						if (llDataSize > SOCKET_BUFFER) throw TEXT("数据包超长");
						//低4位
						llDataSize += ((m_cbRecvBuf[6] << 24) | (m_cbRecvBuf[7] << 16) | (m_cbRecvBuf[8] << 8) | (m_cbRecvBuf[9]));
						if (llDataSize > SOCKET_BUFFER) throw TEXT("数据包超长");
						wHeadSize += 2;
					}
				}

				//整包大小 是数据包大小 + 头部
				WORD wPacketSize = llDataSize + wHeadSize;
				//不满足处理包条件
				if (m_dwRecvSize < wPacketSize) return false;

				//掩码
				unsigned int nMask = 0;
				//存在掩码
				if (cbMask) {
					nMask = *((unsigned int*)(m_cbRecvBuf + wHeadSize));
					wHeadSize += 4;

					BYTE * pBuffer = &m_cbRecvBuf[wHeadSize];

					for (WORD i = 0; i < llDataSize; i++)
					{
						pBuffer[i] = pBuffer[i] ^ ((BYTE*)(&nMask))[i % 4];
					}
				}

				if (m_dwRecvSize < wHeadSize) return false;

				if (cbOpcode == FRAME_OP_CLOSE)
				{
					CloseSocket(m_wRountID);
					return true;
				}

				//文本帧
				if (cbOpcode == FRAME_OP_TEXT)
				{
					const char* pBuffer = (const char *)&m_cbRecvBuf[wHeadSize];
					int nLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer, llDataSize, 0, 0);
					//分配内存 是TCHAR * length + TCHAR 的大小 为整个消息长度
					TCHAR * pData = (TCHAR*)malloc(sizeof(TCHAR) * nLen + sizeof(TCHAR));
					MultiByteToWideChar(CP_UTF8, 0, pBuffer, llDataSize, pData, nLen);
					pData[nLen] = 0;

					//网狐就是这样输出的
					g_TraceServiceManager.TraceString(pData, TraceLevel_Info);

					DWORD dwPaketSize = wHeadSize + llDataSize;
					m_dwRecvSize -= dwPaketSize;
					MoveMemory(m_cbRecvBuf, m_cbRecvBuf + dwPaketSize, m_dwRecvSize);
				}
				//二进制帧
				else if (cbOpcode == FRAME_OP_BINARY && llDataSize > 0)
				{
					CMD_Head * pHead = (CMD_Head *)m_cbRecvBuf[wHeadSize];
					if (llDataSize < sizeof(CMD_Head)) throw TEXT("数据包太小");
					if (wPayloadSize > SOCKET_BUFFER)throw TEXT("数据包太长");

					//解密数据
					tagDecryptResult DecryptResult;
					ZeroMemory(&DecryptResult, sizeof(DecryptResult));
					//解密
					if (!m_Cipher.CrevasseBuffer(m_cbRecvBuf + wHeadSize, llDataSize, &DecryptResult))
					{
						throw TEXT("数据包版本不匹配");
					}

					//解释数据
					CMD_Command Command = DecryptResult.CommandInfo;

					if (Command.wMainCmdID == MDM_KN_COMMAND)
					{
						switch (Command.wSubCmdID)
						{
						case SUB_KN_DETECT_SOCKET:	//网络检测
						{
							break;
						}
						case SUB_KN_SHUT_DOWN_SOCKET:	//验证
						{
							WORD wFrist = *(WORD*)DecryptResult.pData;
							if (wFrist != 4) throw TEXT("无效数据!");
							TCP_Validate * pValidate = (TCP_Validate *)((WORD*)DecryptResult.pData + 1);
							pValidate->szValidateKey[CountArray(pValidate->szValidateKey) - 1] = '\0';	//只取前64位进行效验
							if (lstrcmp(pValidate->szValidateKey, szCompilation))
							{
								TCHAR szString[512] = TEXT("");
								_sntprintf(szString, CountArray(szString), TEXT("Client: %s Server:%s"), pValidate->szValidateKey, szCompilation);
								g_TraceServiceManager.TraceString(szString, TraceLevel_Exception);
								throw TEXT("非法授权!");
							}
							else
							{
								m_bValidate = true;
							}
							break;
						}
						}
					}
					else
					{
						//如果连接未效验 则非法连接
						if (m_bValidate != true) throw TEXT("非法连接!");
						//消息处理
						m_pITCPNetworkItemSink->OnEventSocketRead(Command, DecryptResult.pData, DecryptResult.wDataSize, this);
					}

					//移除处理过了的数据
					DWORD dwPacketSize = wHeadSize + llDataSize;
					m_dwRecvSize -= dwPacketSize;
					MoveMemory(m_cbRecvBuf, m_cbRecvBuf + dwPacketSize, m_dwRecvSize);
				}
			}
			//还未握手  处理握手协议
			else
			{
				//解析HTTP请求
				CHttpParserWrapper HttpParserWrapper;
				HttpParserWrapper.ParseHttpContent((const char *)m_cbRecvBuf, m_dwRecvSize);
				char * pServerKey = HttpParserWrapper.GetWebSocketKey();

				char szWebSocketKey[128] = { 0 };
				_snprintf_s(szWebSocketKey, 128, "%s%s", pServerKey, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

				UINT uDigest[5];
				CWHSha1::HashData(szWebSocketKey, strlen(szWebSocketKey), uDigest);

				for (int i = 0; i < 5; i++)
				{
					uDigest[i] = htonl(uDigest[i]);
				}

				TCHAR szServerKey[128] = { 0 };
				CWHBase64::Encode((char*)uDigest, 20, szServerKey, 128);

				TCHAR szMessage[1024] = { 0 };
				_snwprintf_s(szMessage, 1024, L"HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\nUpgrade: websocket\r\n\r\n", szServerKey);

				char pBuffer[1024] = { 0 };
				lstrcpynA(pBuffer, CW2A(szMessage, CP_UTF8), 1024);
				SendData(pBuffer, CountStringBufferA(pBuffer) - 1);

				//握手成功
				m_bHandshake = true;

				//移除头
				m_dwRecvSize -= HttpParserWrapper.GetHeaderLength();
				MoveMemory(m_cbRecvBuf, m_cbRecvBuf + HttpParserWrapper.GetHeaderLength(), m_dwRecvSize);
			}
		}
	}
	catch (...)
	{
		//关闭链接
		CloseSocket(m_wRountID);
		return false;
	}

	return true;
}

bool CTCPNetworkItem::OnRecvSocketChanel()
{
	if (m_Mode != MODE_SOCKET) return false;

	CMD_Head * pHead = (CMD_Head *)m_cbRecvBuf;

	//处理数据
	try
	{
		while (m_dwRecvSize >= sizeof(CMD_Head))
		{
			//效验数据
			WORD wPacketSize = pHead->CmdInfo.wPacketSize;
			if (wPacketSize > SOCKET_BUFFER) throw TEXT("数据包超长");
			if (wPacketSize<sizeof(CMD_Head)) throw TEXT("数据包非法");

			//未接收完整包 继续接收
			if (m_dwRecvSize<wPacketSize) break;

			//解密数据
			tagDecryptResult DecryptResult;
			ZeroMemory(&DecryptResult, sizeof(DecryptResult));
			//解密
			if (!m_Cipher.CrevasseBuffer(m_cbRecvBuf, wPacketSize, &DecryptResult))
			{
				throw TEXT("数据包版本不匹配");
			}

			//解释数据

			if (DecryptResult.CommandInfo.wMainCmdID == MDM_KN_COMMAND)
			{
				switch (DecryptResult.CommandInfo.wSubCmdID)
				{
					case SUB_KN_DETECT_SOCKET:	//网络检测
					{
						break;
					}
					case SUB_KN_SHUT_DOWN_SOCKET:	//验证
					{
						WORD wFrist = *(WORD*)DecryptResult.pData;
						if (wFrist != 4) throw TEXT("无效数据!");
						TCP_Validate * pValidate = (TCP_Validate *)((WORD*)DecryptResult.pData + 1);
						pValidate->szValidateKey[CountArray(pValidate->szValidateKey) - 1] = '\0';	//只取前64位进行效验
						if (lstrcmp(pValidate->szValidateKey, szCompilation))
						{
							TCHAR szString[512] = TEXT("");
							_sntprintf(szString, CountArray(szString), TEXT("Client: %s Server:%s"), pValidate->szValidateKey, szCompilation);
							g_TraceServiceManager.TraceString(szString, TraceLevel_Exception);

							throw TEXT("非法授权!");
						}
						else
						{
							m_bValidate = true;
						}
						break;
					}
				}
			}
			else
			{
				//如果连接未效验 则非法连接
				if (m_bValidate != true) throw TEXT("非法连接!");

				//消息处理
				m_pITCPNetworkItemSink->OnEventSocketRead(DecryptResult.CommandInfo, DecryptResult.pData, DecryptResult.wDataSize, this);
			}

			//删除缓存数据
			m_dwRecvSize -= wPacketSize;
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf + wPacketSize, m_dwRecvSize);
		}
	}
	catch (...)
	{
		//关闭链接
		CloseSocket(m_wRountID);
		return false;
	}

	return true;
}

//关闭完成
bool CTCPNetworkItem::OnCloseCompleted()
{
	//效验状态
	ASSERT(m_hSocketHandle==INVALID_SOCKET);
	ASSERT((m_bSendIng==false)&&(m_bRecvIng==false));

	//关闭事件
	m_pITCPNetworkItemSink->OnEventSocketShut(this);

	//恢复数据
	ResumeData();

	return true;
}

//发送判断
bool CTCPNetworkItem::SendVerdict(WORD wRountID)
{
	if ((m_wRountID!=wRountID)||(m_bShutDown==true)) return false;
	if ((m_hSocketHandle==INVALID_SOCKET)) return false;

	return true;
}

//获取发送结构
COverLappedSend * CTCPNetworkItem::GetSendOverLapped(WORD wPacketSize)
{
	//重用判断
	if (m_OverLappedSendActive.GetCount()>1)
	{
		INT_PTR nActiveCount=m_OverLappedSendActive.GetCount();
		COverLappedSend * pOverLappedSend=m_OverLappedSendActive[nActiveCount-1];
		if (sizeof(pOverLappedSend->m_cbBuffer)>=(pOverLappedSend->m_WSABuffer.len+wPacketSize)) return pOverLappedSend;
	}

	//空闲对象
	if (m_OverLappedSendBuffer.GetCount()>0)
	{
		//获取对象
		INT_PTR nFreeCount=m_OverLappedSendBuffer.GetCount();
		COverLappedSend * pOverLappedSend=m_OverLappedSendBuffer[nFreeCount-1];

		//设置变量
		pOverLappedSend->m_WSABuffer.len=0;
		m_OverLappedSendActive.Add(pOverLappedSend);
		m_OverLappedSendBuffer.RemoveAt(nFreeCount-1);

		return pOverLappedSend;
	}

	try
	{
		//创建对象
		COverLappedSend * pOverLappedSend=new COverLappedSend;
		ASSERT(pOverLappedSend!=NULL);

		//设置变量
		pOverLappedSend->m_WSABuffer.len=0;
		m_OverLappedSendActive.Add(pOverLappedSend);

		return pOverLappedSend;
	}
	catch (...) { ASSERT(FALSE); }

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkThreadReadWrite::CTCPNetworkThreadReadWrite()
{
	m_hCompletionPort=NULL;
}

//析构函数
CTCPNetworkThreadReadWrite::~CTCPNetworkThreadReadWrite()
{
}

//配置函数
bool CTCPNetworkThreadReadWrite::InitThread(HANDLE hCompletionPort)
{
	ASSERT(hCompletionPort!=NULL);
	m_hCompletionPort=hCompletionPort;
	return true;
}

//运行函数
bool CTCPNetworkThreadReadWrite::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort!=NULL);

	//变量定义
	DWORD dwThancferred=0;					
	OVERLAPPED * pOverLapped=NULL;
	CTCPNetworkItem * pTCPNetworkItem=NULL;

	//完成端口
	BOOL bSuccess=GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pTCPNetworkItem,&pOverLapped,INFINITE);
	if ((bSuccess==FALSE)&&(GetLastError()!=ERROR_NETNAME_DELETED)) return false;

	//退出判断
	if ((pTCPNetworkItem==NULL)&&(pOverLapped==NULL)) return false;

	//变量定义
	COverLapped * pSocketLapped=CONTAINING_RECORD(pOverLapped,COverLapped,m_OverLapped);

	//操作处理
	switch (pSocketLapped->GetOperationType())
	{
	case enOperationType_Send:	//数据发送
		{
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->OnSendCompleted((COverLappedSend *)pSocketLapped,dwThancferred);
			break;
		}
	case enOperationType_Recv:	//数据读取
		{
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->OnRecvCompleted((COverLappedRecv *)pSocketLapped,dwThancferred);
			break;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkThreadAccept::CTCPNetworkThreadAccept()
{
	m_hCompletionPort=NULL;
	m_pTCPNetworkEngine=NULL;
	m_hListenSocket=INVALID_SOCKET;
}

//析构函数
CTCPNetworkThreadAccept::~CTCPNetworkThreadAccept()
{
}

//配置函数
bool CTCPNetworkThreadAccept::InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CTCPNetworkEngine * pNetworkEngine)
{
	//效验参数
	ASSERT(pNetworkEngine!=NULL);
	ASSERT(hCompletionPort!=NULL);
	ASSERT(hListenSocket!=INVALID_SOCKET);

	//设置变量
	m_hListenSocket=hListenSocket;
	m_hCompletionPort=hCompletionPort;
	m_pTCPNetworkEngine=pNetworkEngine;

	return true;
}

//运行函数
bool CTCPNetworkThreadAccept::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort!=NULL);
	ASSERT(m_pTCPNetworkEngine!=NULL);

	//变量定义
	SOCKET hConnectSocket=INVALID_SOCKET;
	CTCPNetworkItem * pTCPNetworkItem=NULL;

	try
	{
		//监听连接
		SOCKADDR_IN	SocketAddr;
		INT nBufferSize=sizeof(SocketAddr);
		hConnectSocket=WSAAccept(m_hListenSocket,(SOCKADDR *)&SocketAddr,&nBufferSize,NULL,NULL);

		//退出判断
		if (hConnectSocket==INVALID_SOCKET) return false;

		//获取连接
		pTCPNetworkItem=m_pTCPNetworkEngine->ActiveNetworkItem();

		//失败判断
		if (pTCPNetworkItem==NULL)
		{
			ASSERT(FALSE);
			throw TEXT("申请连接对象失败");
		}

		//锁定对象
		CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());

		//绑定对象
		pTCPNetworkItem->Attach(hConnectSocket,SocketAddr.sin_addr.S_un.S_addr);
		CreateIoCompletionPort((HANDLE)hConnectSocket,m_hCompletionPort,(ULONG_PTR)pTCPNetworkItem,0);

		//发起接收
		pTCPNetworkItem->RecvData();
	}
	catch (...)
	{
		//清理对象
		ASSERT(pTCPNetworkItem==NULL);

		//关闭连接
		if (hConnectSocket!=INVALID_SOCKET)
		{
			closesocket(hConnectSocket);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkThreadDetect::CTCPNetworkThreadDetect()
{
	m_dwPileTime=0L;
	m_dwDetectTime=10000L;
	m_pTCPNetworkEngine=NULL;
}

//析构函数
CTCPNetworkThreadDetect::~CTCPNetworkThreadDetect()
{
}

//配置函数
bool CTCPNetworkThreadDetect::InitThread(CTCPNetworkEngine * pNetworkEngine, DWORD dwDetectTime)
{
	//效验参数
	ASSERT(pNetworkEngine!=NULL);

	//设置变量
	m_dwPileTime=0L;
	m_dwDetectTime=dwDetectTime;
	m_pTCPNetworkEngine=pNetworkEngine;

	return true;
}

//运行函数
bool CTCPNetworkThreadDetect::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_pTCPNetworkEngine!=NULL);

	//设置间隔
	Sleep(200);
	m_dwPileTime+=200L;

	//检测连接
	if (m_dwPileTime>=m_dwDetectTime)
	{
		m_dwPileTime=0L;
		m_pTCPNetworkEngine->DetectSocket();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkEngine::CTCPNetworkEngine()
{
	//验证变量
	m_bLimited=false;

	m_bValidate = false;
	m_bValidatr = false;
	//辅助变量
	m_bService=false;
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	//配置变量
	m_wMaxConnect=0;
	m_wServicePort=-1;
	m_wCurrentPort = -1;
	m_dwDetectTime=10000L;

	//内核变量
	m_hCompletionPort=NULL;
	m_hServerSocket=INVALID_SOCKET;
	m_pITCPNetworkEngineEvent=NULL;
	
	return;
}

//析构函数
CTCPNetworkEngine::~CTCPNetworkEngine()
{
}

//接口查询
VOID * CTCPNetworkEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPNetworkEngine,Guid,dwQueryVer);
	QUERYINTERFACE(IAsyncEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPNetworkEngine,Guid,dwQueryVer);
	return NULL;
}

//启动服务
bool CTCPNetworkEngine::StartService()
{
	//// 授权验证
	//if (BttestationAcc() == false)
	//{
	//	return false;
	//}
	//TCHAR szServiceMachine[LEN_MACHINE_ID];
	//CWHService::GetMachineID(szServiceMachine);
	//TCHAR szNewMachineID[42];
	//_sntprintf(szNewMachineID, sizeof(szNewMachineID), TEXT("%syinhu888"), szServiceMachine);
	//TCHAR szEnptMachineID[33];
	//CWHEncrypt::MD5Encrypt(szNewMachineID, szEnptMachineID);
	//szEnptMachineID[32] = '\0';

	////读取的机器码
	//TCHAR szConfigFilePath[MAX_PATH];
	//_sntprintf(szConfigFilePath, sizeof(szConfigFilePath), TEXT("c:\\Windows\\ab.ini"));
	//TCHAR szReadData[33] = TEXT("");
	//GetPrivateProfileString(TEXT("key"), TEXT("value"), TEXT(""), szReadData, CountArray(szReadData), szConfigFilePath);
	//szReadData[32] = '\0';

	//if (_tcscmp(szEnptMachineID, szReadData) != 0)
	//{
	//	g_TraceServiceManager.TraceString(TEXT("启动失败"), TraceLevel_Exception);
	//	return;
	//}
	//else
	//{
	//	g_TraceServiceManager.TraceString(TEXT("启动成功"), TraceLevel_Exception);
	//}
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	ASSERT(m_wMaxConnect != 0);
	if (m_wMaxConnect == 0) return false;

	//系统信息
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	DWORD dwThreadCount=SystemInfo.dwNumberOfProcessors;

	//完成端口
	m_hCompletionPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
	if (m_hCompletionPort==NULL) return false;

	try
	{
		if (m_wServicePort != -1 && m_wServicePort > 0)
		{
			if (!CreateTCPService(m_wServicePort))
			{
				TCHAR szString[512] = TEXT("");
				_sntprintf(szString, CountArray(szString), TEXT("网络服务发生错误，启动失败[端口：%d]"), m_wServicePort);
				g_TraceServiceManager.TraceString(szString, TraceLevel_Exception);
				return false;
			}
		}
		else
		{
			WORD wPort;
			for (wPort = MIN_PORT; wPort <= MAX_PORT; wPort++)
			{
				if (wPort != PORT_CHAT
					&& wPort != PORT_CENTER
					&& wPort != PORT_LOGIC
					&& wPort != PORT_PERSONAL_ROOM
					&& wPort != PORT_MANAGER
					&& wPort != PORT_GROUP
					&& wPort != PORT_LINK
					&& wPort != PORT_VALIDATE
					&& wPort != PORT_STATUS
					&& wPort != PORT_GAMEMANAGE
					)
				{
					if(CreateTCPService(wPort))
					{
						m_wCurrentPort = wPort;
						break;
					}
				}
			}

			if (wPort == MAX_PORT)
			{
				return false;
			}
		}
	}
	catch (LPCTSTR pszMessage)
	{
		TCHAR szString[512] = TEXT("");
		_sntprintf(szString, CountArray(szString), TEXT("%s"), pszMessage);
		g_TraceServiceManager.TraceString(pszMessage, TraceLevel_Exception);
		return false;
	}
	
	//异步引擎
	IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_AsynchronismEngine.SetAsynchronismSink(pIUnknownEx)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//网页验证
	WebAttestation();

	// 授权验证
	if (AttestationAcc() == false)
	{
		return false;
	}

	//启动服务
	if (m_AsynchronismEngine.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	DWORD i = 0;
	//读写线程
	for ( i=0;i<dwThreadCount;i++)
	{
		CTCPNetworkThreadReadWrite * pNetworkRSThread=new CTCPNetworkThreadReadWrite();
		if (pNetworkRSThread->InitThread(m_hCompletionPort)==false) return false;
		m_SocketRWThreadArray.Add(pNetworkRSThread);
	}

	//读写线程
	for (i=0;i<dwThreadCount;i++)
	{
		CTCPNetworkThreadReadWrite * pNetworkRSThread=m_SocketRWThreadArray[i];
		ASSERT(pNetworkRSThread!=NULL);
		if (pNetworkRSThread->StartThread()==false) return false;
	}

	//检测线程
	m_SocketDetectThread.InitThread(this,m_dwDetectTime);
	if (m_SocketDetectThread.StartThread()==false) return false;

	//应答线程
	if (m_SocketAcceptThread.InitThread(m_hCompletionPort, m_hServerSocket, this) == false) return false;
	if (m_SocketAcceptThread.StartThread()==false) return false;


	//设置变量
	m_bService=true;

	return true;
}

//停止服务
bool CTCPNetworkEngine::ConcludeService()
{
	//设置变量
	m_bService=false;

	//检测线程
	m_SocketDetectThread.ConcludeThread(INFINITE);

	//应答线程
	if (m_hServerSocket!=INVALID_SOCKET)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket=INVALID_SOCKET;
	}
	m_SocketAcceptThread.ConcludeThread(INFINITE);

	//异步引擎
	m_AsynchronismEngine.ConcludeService();

	//读写线程
	INT_PTR nCount=m_SocketRWThreadArray.GetCount();
	if (m_hCompletionPort!=NULL)
	{
		for (INT_PTR i=0;i<nCount;i++) PostQueuedCompletionStatus(m_hCompletionPort,0,NULL,NULL);
	}
	for (INT_PTR i=0;i<nCount;i++)
	{
		CTCPNetworkThreadReadWrite * pSocketThread=m_SocketRWThreadArray[i];
		ASSERT(pSocketThread!=NULL);
		pSocketThread->ConcludeThread(INFINITE);
		SafeDelete(pSocketThread);
	}
	m_SocketRWThreadArray.RemoveAll();

	//完成端口
	if (m_hCompletionPort!=NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort=NULL;
	}

	//关闭连接
	CTCPNetworkItem * pTCPNetworkItem=NULL;
	for (INT_PTR i=0;i<m_NetworkItemActive.GetCount();i++)
	{
		pTCPNetworkItem=m_NetworkItemActive[i];
		pTCPNetworkItem->CloseSocket(pTCPNetworkItem->GetRountID());
		pTCPNetworkItem->ResumeData();
	}

	//重置数据
	m_NetworkItemBuffer.Append(m_NetworkItemActive);
	m_NetworkItemActive.RemoveAll();
	m_TempNetworkItemArray.RemoveAll();

	return true;
}

//设置参数
bool CTCPNetworkEngine::SetServiceParameter(WORD wServicePort, WORD wMaxConnect, LPCTSTR  pszCompilation)
{
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//设置变量
	ASSERT(wServicePort!=0);

	if (lstrcmp(pszCompilation, szCompilation))
	{
		m_bLimited = true;
		g_TraceServiceManager.TraceString(TEXT("您的服务器没有得到合法授权，服务器将会以受限模式运行"), TraceLevel_Exception);
		return false;
	}

	m_wMaxConnect=wMaxConnect;
	m_wServicePort=wServicePort;
	m_wCurrentPort=wServicePort;

	return true;
}

//配置端口
WORD CTCPNetworkEngine::GetServicePort()
{
	return m_wServicePort;
}

//当前端口
WORD CTCPNetworkEngine::GetCurrentPort()
{
	return m_wCurrentPort;
}

//设置接口
bool CTCPNetworkEngine::SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx)
{
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	if (pIUnknownEx)
	{
		//查询接口
		m_pITCPNetworkEngineEvent = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITCPNetworkEngineEvent);
		return m_pITCPNetworkEngineEvent != NULL;
	}

	return false;
}

//发送函数
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID)
{
	//缓冲锁定
	CThreadLock ThreadLock(m_BufferLocked);
	tagSendDataRequest pSendDataRequest;

	//构造数据
	pSendDataRequest.wDataSize = 0;
	pSendDataRequest.wSubCmdID = wSubCmdID;
	pSendDataRequest.wMainCmdID = wMainCmdID;
	pSendDataRequest.wIndex = SOCKET_INDEX(dwSocketID);
	pSendDataRequest.wRountID = SOCKET_ROUNTID(dwSocketID);

	if (pSendDataRequest.wIndex >= m_NetworkItemStorage.GetCount()) return false;

	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_DATA, &pSendDataRequest, sizeof(tagSendDataRequest));
}



//发送函数
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效益数据
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);
	if (wDataSize>SOCKET_PACKET) return false;

	//缓冲锁定
	CThreadLock ThreadLock(m_BufferLocked);

	tagSendDataRequest pSendDataRequest;

	//构造数据
	pSendDataRequest.wDataSize = wDataSize;
	pSendDataRequest.wSubCmdID = wSubCmdID;
	pSendDataRequest.wMainCmdID = wMainCmdID;
	pSendDataRequest.wIndex = SOCKET_INDEX(dwSocketID);
	pSendDataRequest.wRountID = SOCKET_ROUNTID(dwSocketID);

	if (pSendDataRequest.wIndex >= m_NetworkItemStorage.GetCount()) return false;

	if (wDataSize > 0)
	{
		tagDataBuffer DataBuffer[2];

		DataBuffer[0].pDataBuffer = &pSendDataRequest;
		DataBuffer[0].wDataSize = sizeof(tagSendDataRequest);
		DataBuffer[1].pDataBuffer = pData;
		DataBuffer[1].wDataSize = wDataSize;


		return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_DATA, DataBuffer, 2);
	}

	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_DATA, &pSendDataRequest, sizeof(pSendDataRequest));
}

//批量发送
bool CTCPNetworkEngine::SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize,BYTE cbBatchMask)
{
	//效验数据
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);
	if (wDataSize>SOCKET_PACKET) return false;

	//缓冲锁定
	CThreadLock ThreadLock(m_BufferLocked);
	tagBatchSendRequest pBatchSendRequest;

	//构造数据
	pBatchSendRequest.wMainCmdID = wMainCmdID;
	pBatchSendRequest.wSubCmdID = wSubCmdID;
	pBatchSendRequest.wDataSize = wDataSize;
	pBatchSendRequest.cbBatchMask = cbBatchMask;

	if (wDataSize)
	{
		tagDataBuffer DataBuffer[2];
		DataBuffer[0].pDataBuffer = &pBatchSendRequest;
		DataBuffer[0].wDataSize = sizeof(tagBatchSendRequest);
		DataBuffer[1].pDataBuffer = pData;
		DataBuffer[1].wDataSize = wDataSize;
		return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_BATCH, DataBuffer, 2);
	}

	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_BATCH, &pBatchSendRequest, sizeof(pBatchSendRequest));
}



//关闭连接
bool CTCPNetworkEngine::CloseSocket(DWORD dwSocketID)
{
	//缓冲锁定
	CThreadLock ThreadLock(m_BufferLocked);
	tagCloseSocket CloseSocket;

	//构造数据
	CloseSocket.wIndex = SOCKET_INDEX(dwSocketID);
	CloseSocket.wRountID = SOCKET_ROUNTID(dwSocketID);

	if (CloseSocket.wIndex >= m_NetworkItemStorage.GetCount()) return false;
	//发送请求
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_CLOSE_SOCKET, &CloseSocket, sizeof(tagCloseSocket));
}

//设置关闭
bool CTCPNetworkEngine::ShutDownSocket(DWORD dwSocketID)
{
	//缓冲锁定
	CThreadLock ThreadLock(m_BufferLocked);
	tagShutDownSocket ShutDownSocket;

	//构造数据
	ShutDownSocket.wIndex = SOCKET_INDEX(dwSocketID);
	ShutDownSocket.wRountID = SOCKET_ROUNTID(dwSocketID);

	if (ShutDownSocket.wIndex >= m_NetworkItemStorage.GetCount()) return false;

	//发送请求
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SHUT_DOWN, &ShutDownSocket, sizeof(tagShutDownSocket));
}

//允许群发
bool CTCPNetworkEngine::AllowBatchSend(DWORD dwSocketID, bool cbAllowBatch,BYTE cbBatchMask)
{
	//缓冲锁定
	CThreadLock ThreadLock(m_BufferLocked);
	tagAllowBatchSend AllowBatchSend;

	//构造数据
	AllowBatchSend.cbAllowBatch = cbAllowBatch;
	AllowBatchSend.cbBatchMask = cbBatchMask;
	AllowBatchSend.wIndex = SOCKET_INDEX(dwSocketID);
	AllowBatchSend.wRountID = SOCKET_ROUNTID(dwSocketID);

	if (AllowBatchSend.wIndex >= m_NetworkItemStorage.GetCount()) return false;
	//发送请求
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_ALLOW_BATCH, &AllowBatchSend, sizeof(tagAllowBatchSend));
}

//异步数据
bool CTCPNetworkEngine::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case ASYNCHRONISM_SEND_DATA:		//发送请求
		{
			//效验数据
			tagSendDataRequest * pRequestInfo = (tagSendDataRequest *)pData;

			//获取对象
			CTCPNetworkItem * pTCPNetworkItem = GetNetworkItem(pRequestInfo->wIndex);
			if (pTCPNetworkItem == NULL) return false;

			//发送数据
			CThreadLock SocketThreadLock(pTCPNetworkItem->GetCriticalSection());

			pTCPNetworkItem->SendData(((BYTE*)pData) + sizeof(tagSendDataRequest), pRequestInfo->wDataSize, pRequestInfo->wMainCmdID,
					pRequestInfo->wSubCmdID, pRequestInfo->wRountID);

			return true;
		}
	case ASYNCHRONISM_SEND_BATCH:		//群发请求
		{
			//效验数据
			tagBatchSendRequest * pRequestInfo=(tagBatchSendRequest *)pData;

			//获取活动项
			CThreadLock ItemThreadLock(m_ItemLocked);
			m_TempNetworkItemArray.Copy(m_NetworkItemActive);
			ItemThreadLock.UnLock();

			//群发数据
			for (INT_PTR i=0;i<m_TempNetworkItemArray.GetCount();i++)
			{
				//获取对象
				CTCPNetworkItem * pTCPNetworkItem=m_TempNetworkItemArray[i];
				CThreadLock SocketThreadLock(pTCPNetworkItem->GetCriticalSection());

				//发生数据
				if (pTCPNetworkItem->IsAllowBatch() && pTCPNetworkItem->GetBatchMask()== pRequestInfo->cbBatchMask)
				{
					pTCPNetworkItem->SendData(((BYTE*)pData)+ sizeof(tagBatchSendRequest), pRequestInfo->wDataSize, pRequestInfo->wMainCmdID,
						pRequestInfo->wSubCmdID, pTCPNetworkItem->GetRountID());
				}
			}

			return true;
		}
	case ASYNCHRONISM_SHUT_DOWN:		//安全关闭
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagShutDownSocket));
			tagShutDownSocket * pShutDownSocket=(tagShutDownSocket *)pData;

			//获取对象
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pShutDownSocket->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//安全关闭
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->ShutDownSocket(pShutDownSocket->wRountID);

			return true;
		}
	case ASYNCHRONISM_ALLOW_BATCH:		//允许群发
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagAllowBatchSend));
			tagAllowBatchSend * pAllowBatchSend=(tagAllowBatchSend *)pData;

			//获取对象
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pAllowBatchSend->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//设置群发
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->AllowBatchSend(pAllowBatchSend->wRountID,pAllowBatchSend->cbAllowBatch?true:false,pAllowBatchSend->cbBatchMask);

			return true;
		}
	case ASYNCHRONISM_CLOSE_SOCKET:		//关闭连接
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagCloseSocket));
			tagCloseSocket * pCloseSocket=(tagCloseSocket *)pData;

			//获取对象
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pCloseSocket->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//关闭连接
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->CloseSocket(pCloseSocket->wRountID);

			return true;
		}
	case ASYNCHRONISM_DETECT_SOCKET:	//检测连接
		{
			//获取活动项
			CThreadLock ThreadLock(m_ItemLocked);
			m_TempNetworkItemArray.Copy(m_NetworkItemActive);
			ThreadLock.UnLock();

			//检测连接
			DWORD dwNowTime=(DWORD)time(NULL);
			for (INT_PTR i=0;i<m_TempNetworkItemArray.GetCount();i++)
			{
				//获取连接
				CTCPNetworkItem * pTCPNetworkItem=m_TempNetworkItemArray[i];
				CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());

				
				//有效判断
				if (pTCPNetworkItem->IsValidSocket()==false) continue;

				switch (pTCPNetworkItem->m_wSurvivalTime)
				{
				case DEAD_QUOTIETY:		//死亡连接
				{
					pTCPNetworkItem->CloseSocket(pTCPNetworkItem->GetRountID());
					break;
				}
				case DANGER_QUOTIETY:	//危险系数
				{
					pTCPNetworkItem->m_wSurvivalTime--;
					pTCPNetworkItem->SendData(MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET, pTCPNetworkItem->GetRountID());
					break;
				}
				default:				//默认处理
				{
					pTCPNetworkItem->m_wSurvivalTime--;
					break;
				}
				}
			}

			return true;
		}
	}

	//效验数据
	ASSERT(FALSE);

	return false;
}

//绑定事件
bool CTCPNetworkEngine::OnEventSocketBind(CTCPNetworkItem * pTCPNetworkItem)
{
	//效验数据
	ASSERT(pTCPNetworkItem!=NULL);
	ASSERT(m_pITCPNetworkEngineEvent!=NULL);

	//投递消息
	DWORD dwClientIP=pTCPNetworkItem->GetClientIP();
	DWORD dwSocketID=pTCPNetworkItem->GetIdentifierID();
	m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(dwSocketID,dwClientIP);

	return true;
}

//关闭事件
bool CTCPNetworkEngine::OnEventSocketShut(CTCPNetworkItem * pTCPNetworkItem)
{
	//效验参数
	ASSERT(pTCPNetworkItem!=NULL);
	ASSERT(m_pITCPNetworkEngineEvent!=NULL);

	try
	{
		//投递数据
		DWORD dwClientIP=pTCPNetworkItem->GetClientIP();
		DWORD dwSocketID=pTCPNetworkItem->GetIdentifierID();
		DWORD dwActiveTime=pTCPNetworkItem->GetActiveTime();
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwSocketID,dwClientIP,dwActiveTime);

		//释放连接
		FreeNetworkItem(pTCPNetworkItem);
	}
	catch (...) {}

	return true;
}

//读取事件
bool CTCPNetworkEngine::OnEventSocketRead(CMD_Command Command, VOID * pData, WORD wDataSize, CTCPNetworkItem * pTCPNetworkItem)
{
	//效验数据
	ASSERT(pTCPNetworkItem!=NULL);
	ASSERT(m_pITCPNetworkEngineEvent!=NULL);

	//运行判断
	if (m_bLimited==true)
	{
		//创建对象
		HANDLE hCompletionPort=NULL;
		hCompletionPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,1);

		//进入循环
		while (true)
		{
			DWORD dwThancferred=0;					
			OVERLAPPED * pOverLapped=NULL;
			CTCPNetworkItem * pTCPNetworkItem=NULL;
			GetQueuedCompletionStatus(hCompletionPort,&dwThancferred,(PULONG_PTR)&pTCPNetworkItem,&pOverLapped,INFINITE);
		}

		return false;
	}

	//投递消息
	DWORD dwSocketID=pTCPNetworkItem->GetIdentifierID();
	m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(dwSocketID,Command,pData,wDataSize);

	return true;
}

//检测连接
bool CTCPNetworkEngine::DetectSocket()
{
	//发送请求
	void* pData = NULL;
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_DETECT_SOCKET, pData, 0);
}

//网页验证
bool CTCPNetworkEngine::WebAttestation()
{
	m_bLimited = false;
	return true;

	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//读取配置
	TCHAR szFileName[MAX_PATH]=TEXT("");;
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\Validate.ini"),szPath);

	//变量定义
	DWORD dwClientID=0L;
	TCHAR szClientName[32]=TEXT("");
	TCHAR szClientValidate[33]=TEXT("");

	//读取验证
	dwClientID=GetPrivateProfileInt(TEXT("Data"),TEXT("ClientID"),0,szFileName);
	GetPrivateProfileString(TEXT("Data"),TEXT("ClientName"),TEXT(""),szClientName,sizeof(szClientName),szFileName);
	GetPrivateProfileString(TEXT("Data"),TEXT("ClientValidate"),TEXT(""),szClientValidate,sizeof(szClientValidate),szFileName);

	//变量定义
	CInternetSession Seccion;
	CStdioFile * pInternetFile=NULL;

	try
	{
		//构造验证
		TCHAR szAttestation[512]=TEXT("");
		//机器标识
		TCHAR szMachineID[LEN_MACHINE_ID];
		CWHService::GetMachineID(szMachineID);
		_sntprintf(szAttestation,CountArray(szAttestation),TEXT("http://gamj.net/Attestation.aspx?Version=66&ClientID=%ld&ClientName=%s&Validate=%s&ServerTime=%ld&MachineID=%s&Compilation=%s"),
			dwClientID, szClientName, szClientValidate, (DWORD)time(NULL), szMachineID, TEXT("51iknown"));
		//执行验证
		TCHAR szResult[1024]=TEXT("");
		pInternetFile = Seccion.OpenURL(szAttestation);
		UINT uReadCount=pInternetFile->Read(szResult,sizeof(szResult));
		szResult[__min(uReadCount,CountArray(szResult)-1)]=0;

		//停止判断
		if ((uReadCount>=2)&&(szResult[0]==TEXT('0'))&&(szResult[1]==TEXT(';')))
		{
			//设置变量
			m_bLimited = true;
			//提示信息
			g_TraceServiceManager.TraceString(TEXT("您的服务器组件没有得到合法授权，请联系“银狐游戏” http://gamj.net"),TraceLevel_Exception);
			return false;
		}

		//验证判断
		if ((uReadCount>(UINT)lstrlen(szClientValidate))&&(memcmp(szClientValidate,szResult,lstrlen(szClientValidate)*sizeof(TCHAR))==0))
		{
			//设置变量
			m_bLimited = false;
			return true;
		}
		else
		{
			//设置变量
			m_bLimited = true;
			return false;
		}
		return true;
	}
	catch (CInternetException*pEx)
	{
		TCHAR szString[64] = { 0 };
		pEx->GetErrorMessage(szString, 62);
		pEx->Delete();
		g_TraceServiceManager.TraceString(szString, TraceLevel_Exception);
	}
	catch (...) {
	}

	//设置变量
	m_bLimited=false;
	return true;
}

bool CTCPNetworkEngine::CreateTCPService(WORD wPort)
{
	//建立网络
	sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(wPort);


	m_hServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	//错误判断
	if (m_hServerSocket == INVALID_SOCKET)
	{
		throw TEXT("系统资源不足或者 TCP/IP 协议没有安装，网络启动失败");
	}

	//绑定链接
	if (bind(m_hServerSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket = INVALID_SOCKET;
		return false;
	}

	//监听端口
	if (listen(m_hServerSocket, 200) == SOCKET_ERROR)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket = INVALID_SOCKET;
		return false;
	}

	return true;
}

//获取对象
CTCPNetworkItem * CTCPNetworkEngine::ActiveNetworkItem()
{
	//锁定队列
	CThreadLock ThreadLock(m_ItemLocked,true);

	//获取对象
	CTCPNetworkItem * pTCPNetworkItem=NULL;
	if (m_NetworkItemBuffer.GetCount()>0)
	{
		INT_PTR nItemPostion=m_NetworkItemBuffer.GetCount()-1;
		pTCPNetworkItem=m_NetworkItemBuffer[nItemPostion];
		m_NetworkItemBuffer.RemoveAt(nItemPostion);
		m_NetworkItemActive.Add(pTCPNetworkItem);
	}

	//创建对象
	if (pTCPNetworkItem==NULL)
	{
		WORD wStorageCount=(WORD)m_NetworkItemStorage.GetCount();
		if (wStorageCount<m_wMaxConnect)
		{
			try
			{
				//创建对象
				pTCPNetworkItem=new CTCPNetworkItem(wStorageCount,this);
				if (pTCPNetworkItem==NULL) 
				{
					ASSERT(FALSE);
					return NULL;
				}

				//插入数组
				m_NetworkItemActive.Add(pTCPNetworkItem);
				m_NetworkItemStorage.Add(pTCPNetworkItem);
			}
			catch (...) 
			{ 
				ASSERT(FALSE);
				return NULL; 
			}
		}
	}

	return pTCPNetworkItem;
}

//获取对象
CTCPNetworkItem * CTCPNetworkEngine::GetNetworkItem(WORD wIndex)
{
	//锁定对象
	CThreadLock ThreadLock(m_ItemLocked,true);

	//效验索引
	ASSERT(wIndex<m_NetworkItemStorage.GetCount());
	if (wIndex>=m_NetworkItemStorage.GetCount()) return NULL;

	//获取对象
	CTCPNetworkItem * pTCPNetworkItem=m_NetworkItemStorage[wIndex];

	return pTCPNetworkItem;
}

//释放连接对象
bool CTCPNetworkEngine::FreeNetworkItem(CTCPNetworkItem * pTCPNetworkItem)
{
	//效验参数
	ASSERT(pTCPNetworkItem!=NULL);

	//释放对象
	CThreadLock ThreadLock(m_ItemLocked,true);
	INT_PTR nActiveCount=m_NetworkItemActive.GetCount();
	for (INT i=0;i<nActiveCount;i++)
	{
		if (pTCPNetworkItem==m_NetworkItemActive[i])
		{
			m_NetworkItemActive.RemoveAt(i);
			m_NetworkItemBuffer.Add(pTCPNetworkItem);
			return true;
		}
	}

	//释放失败
	ASSERT(FALSE);

	return false;
}

//证书配置
bool CTCPNetworkEngine::SetCertParameter(LPCTSTR pszCertName, LPCTSTR pszSystemProtocol, bool bMachineStore, bool bAuthClient)
{
	return false;
}
// 授权验证
bool CTCPNetworkEngine::BttestationAcc()
{
    //加密的机器码
    TCHAR szServiceMachine[LEN_MACHINE_ID];
    TCHAR szNewMachineID[42];
    _sntprintf(szNewMachineID, sizeof(szNewMachineID), TEXT("%syinhu888"), szServiceMachine);
    TCHAR szEnptMachineID[33];
    CWHEncrypt::MD5Encrypt(szNewMachineID, szEnptMachineID);
    szEnptMachineID[32] = '\0';


    //读取的机器码
    TCHAR szConfigFilePath[MAX_PATH];
    _sntprintf(szConfigFilePath, sizeof(szConfigFilePath), TEXT("c:\\Windows\\ab.ini"));
    TCHAR szReadData[33] = TEXT("");
    GetPrivateProfileString(TEXT("key"), TEXT("value"), TEXT(""), szReadData, CountArray(szReadData), szConfigFilePath);
    szReadData[32] = '\0';

	if (_tcscmp(szEnptMachineID, szReadData) != 0)
    {
		g_TraceServiceManager.TraceString(TEXT("组件合法授权"), TraceLevel_Debug);
		//m_bValidatr = true;
		return false;
    }
	else
	{
		//g_TraceServiceManager.TraceString(TEXT("组件未授权,请联系客服 QQ:3158885888"), TraceLevel_Exception);
		//m_bValidatr = false;
		//return false;
	}

return true;
}


// 授权验证
bool CTCPNetworkEngine::AttestationAcc()
{

	TCHAR szLocalMachine[LEN_MACHINE_ID];	//机器序列
	CWHService::GetMachineID(szLocalMachine);
	TCHAR szCompilationAcc[LEN_MACHINE_ID];
	//g_TraceServiceManager.TraceString(szLocalMachine, TraceLevel_Normal);
	_sntprintf_s(szCompilationAcc, LEN_MACHINE_ID, TEXT("BC74AC55EBE5B2338D45E9874D146086"));

	if (lstrcmp(szLocalMachine, szCompilationAcc) == 0)
	{
		g_TraceServiceManager.TraceString(TEXT("组件合法授权"), TraceLevel_Debug);
		m_bValidate = true;
		return true;
	}
	else
	{
		g_TraceServiceManager.TraceString(TEXT("组件未授权,请联系客服 QQ:3158885888"), TraceLevel_Exception);
		m_bValidate = false;
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(TCPNetworkEngine);

//////////////////////////////////////////////////////////////////////////
