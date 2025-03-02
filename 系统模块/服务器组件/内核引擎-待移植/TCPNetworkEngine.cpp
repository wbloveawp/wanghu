#include "StdAfx.h"
#include "Afxinet.h"
#include "TCPNetworkEngine.h"
#include "TraceServiceManager.h"

#include "HttpParserWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//�궨��

#define MAX_PORT				9000
#define MIN_PORT				3000

//ϵ������
#define DEAD_QUOTIETY				0									//����ϵ��
#define DANGER_QUOTIETY				1									//Σ��ϵ��
#define SAFETY_QUOTIETY				2									//��ȫϵ��

//��������
#define ASYNCHRONISM_SEND_DATA		1									//���ͱ�ʶ
#define ASYNCHRONISM_SEND_BATCH		2									//Ⱥ�巢��
#define ASYNCHRONISM_SHUT_DOWN		3									//��ȫ�ر�
#define ASYNCHRONISM_ALLOW_BATCH	4									//����Ⱥ��
#define ASYNCHRONISM_CLOSE_SOCKET	5									//�ر�����
#define ASYNCHRONISM_DETECT_SOCKET	6									//�������


#define HTTP_REQUEST_HEAD			"GET / HTTP/"

//��������
#define SOCKET_INDEX(dwSocketID)	LOWORD(dwSocketID)					//λ������
#define SOCKET_ROUNTID(dwSocketID)	HIWORD(dwSocketID)					//ѭ������

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//��������
struct tagSendDataRequest
{
	WORD							wIndex;								//��������
	WORD							wRountID;							//ѭ������
	WORD							wMainCmdID;							//��������
	WORD							wSubCmdID;							//��������
	WORD							wDataSize;							//���ݴ�С
};

//Ⱥ������
struct tagBatchSendRequest
{
	WORD							wMainCmdID;							//��������
	WORD							wSubCmdID;							//��������
	WORD							wDataSize;							//���ݴ�С
	BYTE                            cbBatchMask;                        //��������
};

//����Ⱥ��
struct tagAllowBatchSend
{
	WORD							wIndex;								//��������
	WORD							wRountID;							//ѭ������
	BYTE                            cbBatchMask;                        //��������
	BYTE							cbAllowBatch;						//�����־
};

//�ر�����
struct tagCloseSocket
{
	WORD							wIndex;								//��������
	WORD							wRountID;							//ѭ������
};

//��ȫ�ر�
struct tagShutDownSocket
{
	WORD							wIndex;								//��������
	WORD							wRountID;							//ѭ������
};

//////////////////////////////////////////////////////////////////////////

//���캯��
COverLapped::COverLapped(enOperationType OperationType) : m_OperationType(OperationType)
{
	//���ñ���
	ZeroMemory(&m_WSABuffer,sizeof(m_WSABuffer));
	ZeroMemory(&m_OverLapped,sizeof(m_OverLapped));

	return;
}

//��������
COverLapped::~COverLapped()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
COverLappedSend::COverLappedSend() : COverLapped(enOperationType_Send)
{
	m_WSABuffer.len=0;
	m_WSABuffer.buf=(char *)m_cbBuffer;
}

//��������
COverLappedSend::~COverLappedSend()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
COverLappedRecv::COverLappedRecv() : COverLapped(enOperationType_Recv)
{
	m_WSABuffer.len=0;
	m_WSABuffer.buf=NULL;
}

//��������
COverLappedRecv::~COverLappedRecv()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPNetworkItem::CTCPNetworkItem(WORD wIndex, ITCPNetworkItemSink * pITCPNetworkItemSink)
	: m_wIndex(wIndex), m_pITCPNetworkItemSink(pITCPNetworkItemSink)
{
	//��������
	m_Mode = MODE_NONE;
	m_bHandshake = false;

	m_bValidate = false;

	m_dwClientIP = 0L;
	m_dwActiveTime = 0L;

	//���ı���
	m_wRountID = 1;
	m_wSurvivalTime = DEAD_QUOTIETY;
	m_hSocketHandle = INVALID_SOCKET;

	//״̬����
	m_bSendIng = false;
	m_bRecvIng = false;
	m_bShutDown = false;
	m_bAllowBatch = false;
	m_bBatchMask = 0xFF;

	//���ձ���
	m_dwRecvSize = 0;
	ZeroMemory(m_cbRecvBuf, sizeof(m_cbRecvBuf));


	return;
}

//��������
CTCPNetworkItem::~CTCPNetworkItem()
{
	//ɾ������
	for (INT_PTR i=0;i<m_OverLappedSendBuffer.GetCount();i++)
	{
		delete m_OverLappedSendBuffer[i];
	}

	//ɾ���
	for (INT_PTR i=0;i<m_OverLappedSendActive.GetCount();i++)
	{
		delete m_OverLappedSendActive[i];
	}

	//ɾ������
	m_OverLappedSendBuffer.RemoveAll();
	m_OverLappedSendActive.RemoveAll();

	return;
}

//�󶨶���
DWORD CTCPNetworkItem::Attach(SOCKET hSocket, DWORD dwClientIP)
{
	//Ч�����
	ASSERT(dwClientIP!=0);
	ASSERT(hSocket!=INVALID_SOCKET);

	//Ч��״̬
	ASSERT(m_bRecvIng==false);
	ASSERT(m_bSendIng==false);
	ASSERT(m_hSocketHandle==INVALID_SOCKET);

	m_Mode = MODE_NONE;
	m_bHandshake = false;
	m_bValidate=false;
	//״̬����
	m_bSendIng=false;
	m_bRecvIng=false;
	m_bShutDown=false;
	m_bAllowBatch=false;
	m_bBatchMask=0xFF;

	//���ñ���
	m_dwClientIP=dwClientIP;
	m_hSocketHandle=hSocket;
	m_wSurvivalTime=SAFETY_QUOTIETY;
	m_dwActiveTime=(DWORD)time(NULL);

	//����֪ͨ
	m_pITCPNetworkItemSink->OnEventSocketBind(this);

	return GetIdentifierID();
}

//�ָ�����
DWORD CTCPNetworkItem::ResumeData()
{
	//Ч��״̬
	ASSERT(m_hSocketHandle==INVALID_SOCKET);

	m_Mode = MODE_NONE;
	m_bHandshake = false;
	m_bValidate = false;
	//��������
	m_dwClientIP=0L;
	m_dwActiveTime=0L;

	//���ı���
	m_wSurvivalTime= DEAD_QUOTIETY;
	m_hSocketHandle=INVALID_SOCKET;
	m_wRountID=__max(1,m_wRountID+1);

	//״̬����
	m_bSendIng=false;
	m_bRecvIng=false;
	m_bShutDown=false;
	m_bAllowBatch=false;

	//���ձ���
	m_dwRecvSize = 0;
	ZeroMemory(m_cbRecvBuf,sizeof(m_cbRecvBuf));


	//�ص�����
	m_OverLappedSendBuffer.Append(m_OverLappedSendActive);
	m_OverLappedSendActive.RemoveAll();

	return GetIdentifierID();
}

//���ͺ���
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

//���ͺ���
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
	//�����ж�
	if (SendVerdict(m_wRountID) == false) return false;

	WORD nHeadSize = 0;

	if (m_Mode == MODE_WEBSOCKET && m_bHandshake == true)
	{
		nHeadSize = wDataSize < 126 ? 2 : 4;
	}


	COverLappedSend * pOverLappedSend = GetSendOverLapped(nHeadSize + wDataSize);
	//COverLappedSend * pOverLappedSend = GetSendOverLapped( wDataSize);
	//�ر��ж�
	if (pOverLappedSend == NULL)
	{
		CloseSocket(m_wRountID);
		return false;
	}

	//֮ǰ�ĳ���
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
			pOverLappedSend->m_cbBuffer[wBeforeSize+1] = 126;  //�����ٷ� ��û���趨���ֵ
			pOverLappedSend->m_cbBuffer[wBeforeSize+2] = HIBYTE(wDataSize);
			pOverLappedSend->m_cbBuffer[wBeforeSize+3] = LOBYTE(wDataSize);
		}
	}
	
	//������С
	DWORD dwPacketSize = nHeadSize + wDataSize;
	pOverLappedSend->m_WSABuffer.len += dwPacketSize;
	//�������ݵ�������
	CopyMemory(&pOverLappedSend->m_cbBuffer[wBeforeSize + nHeadSize ], pData, wDataSize);
	
	//δ�������ڷ���
	if (m_bSendIng == false)
	{
		//��������
		DWORD dwThancferred = 0;
		INT nResultCode = WSASend(m_hSocketHandle, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);

		//�������
		if ((nResultCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			CloseSocket(m_wRountID);
			return false;
		}

		//���ñ���
		m_bSendIng = true;
	}

	return true;
}
//Ͷ�ݽ���
bool CTCPNetworkItem::RecvData()
{
	//Ч�����
	ASSERT(m_bRecvIng == false);
	ASSERT(m_hSocketHandle != INVALID_SOCKET);

	//��������
	DWORD dwThancferred = 0, dwFlags = 0;
	INT nResultCode = WSARecv(m_hSocketHandle, &m_OverLappedRecv.m_WSABuffer, 1, &dwThancferred, &dwFlags, &m_OverLappedRecv.m_OverLapped, NULL);

	//�������
	if ((nResultCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
	{
		CloseSocket(m_wRountID);
		return false;
	}

	//���ñ���
	m_bRecvIng = true;

	return true;
}

//�ر�����
bool CTCPNetworkItem::CloseSocket(WORD wRountID)
{
	//״̬�ж�
	if (m_wRountID != wRountID) return false;

	//�ر�����
	if (m_hSocketHandle != INVALID_SOCKET)
	{
		closesocket(m_hSocketHandle);
		m_hSocketHandle = INVALID_SOCKET;
	}

	//�жϹر�
	if ((m_bRecvIng == false) && (m_bSendIng == false))
	{
		OnCloseCompleted();
	}

	return true;
}

//���ùر�
bool CTCPNetworkItem::ShutDownSocket(WORD wRountID)
{
	//״̬�ж�
	if (m_hSocketHandle == INVALID_SOCKET) return false;
	if ((m_wRountID != wRountID) || (m_bShutDown == true)) return false;

	//���ñ���
	m_dwRecvSize = 0;
	m_bShutDown = true;

	if(m_bSendIng == false)
	{
		WSASendDisconnect(m_hSocketHandle, NULL);
	}

	return true;
}

//����Ⱥ��
bool CTCPNetworkItem::AllowBatchSend(WORD wRountID, bool bAllowBatch,BYTE cbBatchMask)
{
	//״̬�ж�
	if (m_wRountID!=wRountID) return false;
	if (m_hSocketHandle==INVALID_SOCKET) return false;

	//���ñ���
	m_bAllowBatch=bAllowBatch;

	m_bBatchMask=cbBatchMask;

	return true;
}

//�������
bool CTCPNetworkItem::OnSendCompleted(COverLappedSend * pOverLappedSend, DWORD dwThancferred)
{
	//Ч�����
	ASSERT(m_bSendIng==true);
	ASSERT(m_OverLappedSendActive.GetCount()>0);
	ASSERT(pOverLappedSend==m_OverLappedSendActive[0]);

	//�ͷŽṹ
	m_OverLappedSendActive.RemoveAt(0);
	m_OverLappedSendBuffer.Add(pOverLappedSend);

	//���ñ���
	m_bSendIng=false;

	//�жϹر�
	if (m_hSocketHandle==INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//���ñ���
	if (dwThancferred!=0)
	{
		m_wSurvivalTime=SAFETY_QUOTIETY;
	}

	//��������
	if (m_OverLappedSendActive.GetCount() > 0)
	{
		//��ȡ����
		pOverLappedSend = m_OverLappedSendActive[0];

		//��������
		DWORD dwThancferred = 0;
		INT nResultCode = WSASend(m_hSocketHandle, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);

		//�������
		if ((nResultCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			CloseSocket(m_wRountID);
			return false;
		}

		//���ñ���
		m_bSendIng = true;
	}

	if (m_bShutDown && m_bSendIng == false)
	{
		//��ֹ����
		WSASendDisconnect(m_hSocketHandle, NULL);
	}
	return true;
}

//�������
bool CTCPNetworkItem::OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred)
{
	//Ч������
	ASSERT(m_bRecvIng == true);

	//���ñ���
	m_bRecvIng = false;

	//�жϹر�
	if (m_hSocketHandle == INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//��������
	INT nResultCode = recv(m_hSocketHandle, (char *)m_cbRecvBuf + m_dwRecvSize, sizeof(m_cbRecvBuf) - m_dwRecvSize, 0);

	//�ر��ж�
	if (nResultCode <= 0)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//�ж��ж�
	if (m_bShutDown == true) return true;

	//���ñ���
	m_dwRecvSize += nResultCode;
	m_wSurvivalTime = SAFETY_QUOTIETY;

	//ͨ������δ����
	if (m_Mode == MODE_NONE)
	{
		int nRequestSize = lstrlenA(HTTP_REQUEST_HEAD);
		//��ʼ��������ͷ
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
			
			//���ֳɹ� ��ȡ��Ϣ
			if (m_bHandshake)
			{
				//��ȡfin
				BYTE cbFin = (m_cbRecvBuf[0] >> 7) & 0x01;
				if (cbFin == 0) return true;					//δ������
				//��ȡopcode
				BYTE cbOpcode = m_cbRecvBuf[0] & 0x0F;
				//��ȡmask
				BYTE cbMask = m_cbRecvBuf[1] >> 7 & 0x01;
				//��ȡ����С
				WORD wPayloadSize = m_cbRecvBuf[1] & 0x7F;
				//���ݰ���С
				long long llDataSize = 0;
				//����ͷ��С
				WORD wHeadSize = 2;

				//��ȡ���ݰ���С
				if (wPayloadSize < 126)
				{
					llDataSize = wPayloadSize;
				}
				else if (wPayloadSize == 126)
				{
					if (m_dwRecvSize >= 4)
					{
						llDataSize = ((m_cbRecvBuf[2] << 8) | (m_cbRecvBuf[3]));
						if (llDataSize > SOCKET_BUFFER) throw TEXT("���ݰ�����");
						wHeadSize += 2;
					}
				}
				else if (wPayloadSize == 127)
				{
					if (m_dwRecvSize >= 10)//8���ֽ��γɵ�64bits�޷�����������ֵ
					{
						//��4λ
						llDataSize = ( (m_cbRecvBuf[2] << 24) | (m_cbRecvBuf[3] << 16) | (m_cbRecvBuf[4] << 8) | (m_cbRecvBuf[5]) );
						llDataSize <<= 32;
						if (llDataSize > SOCKET_BUFFER) throw TEXT("���ݰ�����");
						//��4λ
						llDataSize += ((m_cbRecvBuf[6] << 24) | (m_cbRecvBuf[7] << 16) | (m_cbRecvBuf[8] << 8) | (m_cbRecvBuf[9]));
						if (llDataSize > SOCKET_BUFFER) throw TEXT("���ݰ�����");
						wHeadSize += 2;
					}
				}

				//������С �����ݰ���С + ͷ��
				WORD wPacketSize = llDataSize + wHeadSize;
				//�����㴦�������
				if (m_dwRecvSize < wPacketSize) return false;

				//����
				unsigned int nMask = 0;
				//��������
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

				//�ı�֡
				if (cbOpcode == FRAME_OP_TEXT)
				{
					const char* pBuffer = (const char *)&m_cbRecvBuf[wHeadSize];
					int nLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer, llDataSize, 0, 0);
					//�����ڴ� ��TCHAR * length + TCHAR �Ĵ�С Ϊ������Ϣ����
					TCHAR * pData = (TCHAR*)malloc(sizeof(TCHAR) * nLen + sizeof(TCHAR));
					MultiByteToWideChar(CP_UTF8, 0, pBuffer, llDataSize, pData, nLen);
					pData[nLen] = 0;

					//�����������������
					g_TraceServiceManager.TraceString(pData, TraceLevel_Info);

					DWORD dwPaketSize = wHeadSize + llDataSize;
					m_dwRecvSize -= dwPaketSize;
					MoveMemory(m_cbRecvBuf, m_cbRecvBuf + dwPaketSize, m_dwRecvSize);
				}
				//������֡
				else if (cbOpcode == FRAME_OP_BINARY && llDataSize > 0)
				{
					CMD_Head * pHead = (CMD_Head *)m_cbRecvBuf[wHeadSize];
					if (llDataSize < sizeof(CMD_Head)) throw TEXT("���ݰ�̫С");
					if (wPayloadSize > SOCKET_BUFFER)throw TEXT("���ݰ�̫��");

					//��������
					tagDecryptResult DecryptResult;
					ZeroMemory(&DecryptResult, sizeof(DecryptResult));
					//����
					if (!m_Cipher.CrevasseBuffer(m_cbRecvBuf + wHeadSize, llDataSize, &DecryptResult))
					{
						throw TEXT("���ݰ��汾��ƥ��");
					}

					//��������
					CMD_Command Command = DecryptResult.CommandInfo;

					if (Command.wMainCmdID == MDM_KN_COMMAND)
					{
						switch (Command.wSubCmdID)
						{
						case SUB_KN_DETECT_SOCKET:	//������
						{
							break;
						}
						case SUB_KN_SHUT_DOWN_SOCKET:	//��֤
						{
							WORD wFrist = *(WORD*)DecryptResult.pData;
							if (wFrist != 4) throw TEXT("��Ч����!");
							TCP_Validate * pValidate = (TCP_Validate *)((WORD*)DecryptResult.pData + 1);
							pValidate->szValidateKey[CountArray(pValidate->szValidateKey) - 1] = '\0';	//ֻȡǰ64λ����Ч��
							if (lstrcmp(pValidate->szValidateKey, szCompilation))
							{
								TCHAR szString[512] = TEXT("");
								_sntprintf(szString, CountArray(szString), TEXT("Client: %s Server:%s"), pValidate->szValidateKey, szCompilation);
								g_TraceServiceManager.TraceString(szString, TraceLevel_Exception);
								throw TEXT("�Ƿ���Ȩ!");
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
						//�������δЧ�� ��Ƿ�����
						if (m_bValidate != true) throw TEXT("�Ƿ�����!");
						//��Ϣ����
						m_pITCPNetworkItemSink->OnEventSocketRead(Command, DecryptResult.pData, DecryptResult.wDataSize, this);
					}

					//�Ƴ�������˵�����
					DWORD dwPacketSize = wHeadSize + llDataSize;
					m_dwRecvSize -= dwPacketSize;
					MoveMemory(m_cbRecvBuf, m_cbRecvBuf + dwPacketSize, m_dwRecvSize);
				}
			}
			//��δ����  ��������Э��
			else
			{
				//����HTTP����
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

				//���ֳɹ�
				m_bHandshake = true;

				//�Ƴ�ͷ
				m_dwRecvSize -= HttpParserWrapper.GetHeaderLength();
				MoveMemory(m_cbRecvBuf, m_cbRecvBuf + HttpParserWrapper.GetHeaderLength(), m_dwRecvSize);
			}
		}
	}
	catch (...)
	{
		//�ر�����
		CloseSocket(m_wRountID);
		return false;
	}

	return true;
}

bool CTCPNetworkItem::OnRecvSocketChanel()
{
	if (m_Mode != MODE_SOCKET) return false;

	CMD_Head * pHead = (CMD_Head *)m_cbRecvBuf;

	//��������
	try
	{
		while (m_dwRecvSize >= sizeof(CMD_Head))
		{
			//Ч������
			WORD wPacketSize = pHead->CmdInfo.wPacketSize;
			if (wPacketSize > SOCKET_BUFFER) throw TEXT("���ݰ�����");
			if (wPacketSize<sizeof(CMD_Head)) throw TEXT("���ݰ��Ƿ�");

			//δ���������� ��������
			if (m_dwRecvSize<wPacketSize) break;

			//��������
			tagDecryptResult DecryptResult;
			ZeroMemory(&DecryptResult, sizeof(DecryptResult));
			//����
			if (!m_Cipher.CrevasseBuffer(m_cbRecvBuf, wPacketSize, &DecryptResult))
			{
				throw TEXT("���ݰ��汾��ƥ��");
			}

			//��������

			if (DecryptResult.CommandInfo.wMainCmdID == MDM_KN_COMMAND)
			{
				switch (DecryptResult.CommandInfo.wSubCmdID)
				{
					case SUB_KN_DETECT_SOCKET:	//������
					{
						break;
					}
					case SUB_KN_SHUT_DOWN_SOCKET:	//��֤
					{
						WORD wFrist = *(WORD*)DecryptResult.pData;
						if (wFrist != 4) throw TEXT("��Ч����!");
						TCP_Validate * pValidate = (TCP_Validate *)((WORD*)DecryptResult.pData + 1);
						pValidate->szValidateKey[CountArray(pValidate->szValidateKey) - 1] = '\0';	//ֻȡǰ64λ����Ч��
						if (lstrcmp(pValidate->szValidateKey, szCompilation))
						{
							TCHAR szString[512] = TEXT("");
							_sntprintf(szString, CountArray(szString), TEXT("Client: %s Server:%s"), pValidate->szValidateKey, szCompilation);
							g_TraceServiceManager.TraceString(szString, TraceLevel_Exception);

							throw TEXT("�Ƿ���Ȩ!");
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
				//�������δЧ�� ��Ƿ�����
				if (m_bValidate != true) throw TEXT("�Ƿ�����!");

				//��Ϣ����
				m_pITCPNetworkItemSink->OnEventSocketRead(DecryptResult.CommandInfo, DecryptResult.pData, DecryptResult.wDataSize, this);
			}

			//ɾ����������
			m_dwRecvSize -= wPacketSize;
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf + wPacketSize, m_dwRecvSize);
		}
	}
	catch (...)
	{
		//�ر�����
		CloseSocket(m_wRountID);
		return false;
	}

	return true;
}

//�ر����
bool CTCPNetworkItem::OnCloseCompleted()
{
	//Ч��״̬
	ASSERT(m_hSocketHandle==INVALID_SOCKET);
	ASSERT((m_bSendIng==false)&&(m_bRecvIng==false));

	//�ر��¼�
	m_pITCPNetworkItemSink->OnEventSocketShut(this);

	//�ָ�����
	ResumeData();

	return true;
}

//�����ж�
bool CTCPNetworkItem::SendVerdict(WORD wRountID)
{
	if ((m_wRountID!=wRountID)||(m_bShutDown==true)) return false;
	if ((m_hSocketHandle==INVALID_SOCKET)) return false;

	return true;
}

//��ȡ���ͽṹ
COverLappedSend * CTCPNetworkItem::GetSendOverLapped(WORD wPacketSize)
{
	//�����ж�
	if (m_OverLappedSendActive.GetCount()>1)
	{
		INT_PTR nActiveCount=m_OverLappedSendActive.GetCount();
		COverLappedSend * pOverLappedSend=m_OverLappedSendActive[nActiveCount-1];
		if (sizeof(pOverLappedSend->m_cbBuffer)>=(pOverLappedSend->m_WSABuffer.len+wPacketSize)) return pOverLappedSend;
	}

	//���ж���
	if (m_OverLappedSendBuffer.GetCount()>0)
	{
		//��ȡ����
		INT_PTR nFreeCount=m_OverLappedSendBuffer.GetCount();
		COverLappedSend * pOverLappedSend=m_OverLappedSendBuffer[nFreeCount-1];

		//���ñ���
		pOverLappedSend->m_WSABuffer.len=0;
		m_OverLappedSendActive.Add(pOverLappedSend);
		m_OverLappedSendBuffer.RemoveAt(nFreeCount-1);

		return pOverLappedSend;
	}

	try
	{
		//��������
		COverLappedSend * pOverLappedSend=new COverLappedSend;
		ASSERT(pOverLappedSend!=NULL);

		//���ñ���
		pOverLappedSend->m_WSABuffer.len=0;
		m_OverLappedSendActive.Add(pOverLappedSend);

		return pOverLappedSend;
	}
	catch (...) { ASSERT(FALSE); }

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPNetworkThreadReadWrite::CTCPNetworkThreadReadWrite()
{
	m_hCompletionPort=NULL;
}

//��������
CTCPNetworkThreadReadWrite::~CTCPNetworkThreadReadWrite()
{
}

//���ú���
bool CTCPNetworkThreadReadWrite::InitThread(HANDLE hCompletionPort)
{
	ASSERT(hCompletionPort!=NULL);
	m_hCompletionPort=hCompletionPort;
	return true;
}

//���к���
bool CTCPNetworkThreadReadWrite::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_hCompletionPort!=NULL);

	//��������
	DWORD dwThancferred=0;					
	OVERLAPPED * pOverLapped=NULL;
	CTCPNetworkItem * pTCPNetworkItem=NULL;

	//��ɶ˿�
	BOOL bSuccess=GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pTCPNetworkItem,&pOverLapped,INFINITE);
	if ((bSuccess==FALSE)&&(GetLastError()!=ERROR_NETNAME_DELETED)) return false;

	//�˳��ж�
	if ((pTCPNetworkItem==NULL)&&(pOverLapped==NULL)) return false;

	//��������
	COverLapped * pSocketLapped=CONTAINING_RECORD(pOverLapped,COverLapped,m_OverLapped);

	//��������
	switch (pSocketLapped->GetOperationType())
	{
	case enOperationType_Send:	//���ݷ���
		{
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->OnSendCompleted((COverLappedSend *)pSocketLapped,dwThancferred);
			break;
		}
	case enOperationType_Recv:	//���ݶ�ȡ
		{
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->OnRecvCompleted((COverLappedRecv *)pSocketLapped,dwThancferred);
			break;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPNetworkThreadAccept::CTCPNetworkThreadAccept()
{
	m_hCompletionPort=NULL;
	m_pTCPNetworkEngine=NULL;
	m_hListenSocket=INVALID_SOCKET;
}

//��������
CTCPNetworkThreadAccept::~CTCPNetworkThreadAccept()
{
}

//���ú���
bool CTCPNetworkThreadAccept::InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CTCPNetworkEngine * pNetworkEngine)
{
	//Ч�����
	ASSERT(pNetworkEngine!=NULL);
	ASSERT(hCompletionPort!=NULL);
	ASSERT(hListenSocket!=INVALID_SOCKET);

	//���ñ���
	m_hListenSocket=hListenSocket;
	m_hCompletionPort=hCompletionPort;
	m_pTCPNetworkEngine=pNetworkEngine;

	return true;
}

//���к���
bool CTCPNetworkThreadAccept::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_hCompletionPort!=NULL);
	ASSERT(m_pTCPNetworkEngine!=NULL);

	//��������
	SOCKET hConnectSocket=INVALID_SOCKET;
	CTCPNetworkItem * pTCPNetworkItem=NULL;

	try
	{
		//��������
		SOCKADDR_IN	SocketAddr;
		INT nBufferSize=sizeof(SocketAddr);
		hConnectSocket=WSAAccept(m_hListenSocket,(SOCKADDR *)&SocketAddr,&nBufferSize,NULL,NULL);

		//�˳��ж�
		if (hConnectSocket==INVALID_SOCKET) return false;

		//��ȡ����
		pTCPNetworkItem=m_pTCPNetworkEngine->ActiveNetworkItem();

		//ʧ���ж�
		if (pTCPNetworkItem==NULL)
		{
			ASSERT(FALSE);
			throw TEXT("�������Ӷ���ʧ��");
		}

		//��������
		CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());

		//�󶨶���
		pTCPNetworkItem->Attach(hConnectSocket,SocketAddr.sin_addr.S_un.S_addr);
		CreateIoCompletionPort((HANDLE)hConnectSocket,m_hCompletionPort,(ULONG_PTR)pTCPNetworkItem,0);

		//�������
		pTCPNetworkItem->RecvData();
	}
	catch (...)
	{
		//�������
		ASSERT(pTCPNetworkItem==NULL);

		//�ر�����
		if (hConnectSocket!=INVALID_SOCKET)
		{
			closesocket(hConnectSocket);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPNetworkThreadDetect::CTCPNetworkThreadDetect()
{
	m_dwPileTime=0L;
	m_dwDetectTime=10000L;
	m_pTCPNetworkEngine=NULL;
}

//��������
CTCPNetworkThreadDetect::~CTCPNetworkThreadDetect()
{
}

//���ú���
bool CTCPNetworkThreadDetect::InitThread(CTCPNetworkEngine * pNetworkEngine, DWORD dwDetectTime)
{
	//Ч�����
	ASSERT(pNetworkEngine!=NULL);

	//���ñ���
	m_dwPileTime=0L;
	m_dwDetectTime=dwDetectTime;
	m_pTCPNetworkEngine=pNetworkEngine;

	return true;
}

//���к���
bool CTCPNetworkThreadDetect::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_pTCPNetworkEngine!=NULL);

	//���ü��
	Sleep(200);
	m_dwPileTime+=200L;

	//�������
	if (m_dwPileTime>=m_dwDetectTime)
	{
		m_dwPileTime=0L;
		m_pTCPNetworkEngine->DetectSocket();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPNetworkEngine::CTCPNetworkEngine()
{
	//��֤����
	m_bLimited=false;

	m_bValidate = false;
	m_bValidatr = false;
	//��������
	m_bService=false;
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	//���ñ���
	m_wMaxConnect=0;
	m_wServicePort=-1;
	m_wCurrentPort = -1;
	m_dwDetectTime=10000L;

	//�ں˱���
	m_hCompletionPort=NULL;
	m_hServerSocket=INVALID_SOCKET;
	m_pITCPNetworkEngineEvent=NULL;
	
	return;
}

//��������
CTCPNetworkEngine::~CTCPNetworkEngine()
{
}

//�ӿڲ�ѯ
VOID * CTCPNetworkEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPNetworkEngine,Guid,dwQueryVer);
	QUERYINTERFACE(IAsyncEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPNetworkEngine,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CTCPNetworkEngine::StartService()
{
	//// ��Ȩ��֤
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

	////��ȡ�Ļ�����
	//TCHAR szConfigFilePath[MAX_PATH];
	//_sntprintf(szConfigFilePath, sizeof(szConfigFilePath), TEXT("c:\\Windows\\ab.ini"));
	//TCHAR szReadData[33] = TEXT("");
	//GetPrivateProfileString(TEXT("key"), TEXT("value"), TEXT(""), szReadData, CountArray(szReadData), szConfigFilePath);
	//szReadData[32] = '\0';

	//if (_tcscmp(szEnptMachineID, szReadData) != 0)
	//{
	//	g_TraceServiceManager.TraceString(TEXT("����ʧ��"), TraceLevel_Exception);
	//	return;
	//}
	//else
	//{
	//	g_TraceServiceManager.TraceString(TEXT("�����ɹ�"), TraceLevel_Exception);
	//}
	//״̬Ч��
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	ASSERT(m_wMaxConnect != 0);
	if (m_wMaxConnect == 0) return false;

	//ϵͳ��Ϣ
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	DWORD dwThreadCount=SystemInfo.dwNumberOfProcessors;

	//��ɶ˿�
	m_hCompletionPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
	if (m_hCompletionPort==NULL) return false;

	try
	{
		if (m_wServicePort != -1 && m_wServicePort > 0)
		{
			if (!CreateTCPService(m_wServicePort))
			{
				TCHAR szString[512] = TEXT("");
				_sntprintf(szString, CountArray(szString), TEXT("�����������������ʧ��[�˿ڣ�%d]"), m_wServicePort);
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
	
	//�첽����
	IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_AsynchronismEngine.SetAsynchronismSink(pIUnknownEx)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��ҳ��֤
	WebAttestation();

	// ��Ȩ��֤
	if (AttestationAcc() == false)
	{
		return false;
	}

	//��������
	if (m_AsynchronismEngine.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	DWORD i = 0;
	//��д�߳�
	for ( i=0;i<dwThreadCount;i++)
	{
		CTCPNetworkThreadReadWrite * pNetworkRSThread=new CTCPNetworkThreadReadWrite();
		if (pNetworkRSThread->InitThread(m_hCompletionPort)==false) return false;
		m_SocketRWThreadArray.Add(pNetworkRSThread);
	}

	//��д�߳�
	for (i=0;i<dwThreadCount;i++)
	{
		CTCPNetworkThreadReadWrite * pNetworkRSThread=m_SocketRWThreadArray[i];
		ASSERT(pNetworkRSThread!=NULL);
		if (pNetworkRSThread->StartThread()==false) return false;
	}

	//����߳�
	m_SocketDetectThread.InitThread(this,m_dwDetectTime);
	if (m_SocketDetectThread.StartThread()==false) return false;

	//Ӧ���߳�
	if (m_SocketAcceptThread.InitThread(m_hCompletionPort, m_hServerSocket, this) == false) return false;
	if (m_SocketAcceptThread.StartThread()==false) return false;


	//���ñ���
	m_bService=true;

	return true;
}

//ֹͣ����
bool CTCPNetworkEngine::ConcludeService()
{
	//���ñ���
	m_bService=false;

	//����߳�
	m_SocketDetectThread.ConcludeThread(INFINITE);

	//Ӧ���߳�
	if (m_hServerSocket!=INVALID_SOCKET)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket=INVALID_SOCKET;
	}
	m_SocketAcceptThread.ConcludeThread(INFINITE);

	//�첽����
	m_AsynchronismEngine.ConcludeService();

	//��д�߳�
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

	//��ɶ˿�
	if (m_hCompletionPort!=NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort=NULL;
	}

	//�ر�����
	CTCPNetworkItem * pTCPNetworkItem=NULL;
	for (INT_PTR i=0;i<m_NetworkItemActive.GetCount();i++)
	{
		pTCPNetworkItem=m_NetworkItemActive[i];
		pTCPNetworkItem->CloseSocket(pTCPNetworkItem->GetRountID());
		pTCPNetworkItem->ResumeData();
	}

	//��������
	m_NetworkItemBuffer.Append(m_NetworkItemActive);
	m_NetworkItemActive.RemoveAll();
	m_TempNetworkItemArray.RemoveAll();

	return true;
}

//���ò���
bool CTCPNetworkEngine::SetServiceParameter(WORD wServicePort, WORD wMaxConnect, LPCTSTR  pszCompilation)
{
	//״̬Ч��
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//���ñ���
	ASSERT(wServicePort!=0);

	if (lstrcmp(pszCompilation, szCompilation))
	{
		m_bLimited = true;
		g_TraceServiceManager.TraceString(TEXT("���ķ�����û�еõ��Ϸ���Ȩ������������������ģʽ����"), TraceLevel_Exception);
		return false;
	}

	m_wMaxConnect=wMaxConnect;
	m_wServicePort=wServicePort;
	m_wCurrentPort=wServicePort;

	return true;
}

//���ö˿�
WORD CTCPNetworkEngine::GetServicePort()
{
	return m_wServicePort;
}

//��ǰ�˿�
WORD CTCPNetworkEngine::GetCurrentPort()
{
	return m_wCurrentPort;
}

//���ýӿ�
bool CTCPNetworkEngine::SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx)
{
	//״̬Ч��
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	if (pIUnknownEx)
	{
		//��ѯ�ӿ�
		m_pITCPNetworkEngineEvent = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITCPNetworkEngineEvent);
		return m_pITCPNetworkEngineEvent != NULL;
	}

	return false;
}

//���ͺ���
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID)
{
	//��������
	CThreadLock ThreadLock(m_BufferLocked);
	tagSendDataRequest pSendDataRequest;

	//��������
	pSendDataRequest.wDataSize = 0;
	pSendDataRequest.wSubCmdID = wSubCmdID;
	pSendDataRequest.wMainCmdID = wMainCmdID;
	pSendDataRequest.wIndex = SOCKET_INDEX(dwSocketID);
	pSendDataRequest.wRountID = SOCKET_ROUNTID(dwSocketID);

	if (pSendDataRequest.wIndex >= m_NetworkItemStorage.GetCount()) return false;

	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_DATA, &pSendDataRequest, sizeof(tagSendDataRequest));
}



//���ͺ���
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);
	if (wDataSize>SOCKET_PACKET) return false;

	//��������
	CThreadLock ThreadLock(m_BufferLocked);

	tagSendDataRequest pSendDataRequest;

	//��������
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

//��������
bool CTCPNetworkEngine::SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize,BYTE cbBatchMask)
{
	//Ч������
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);
	if (wDataSize>SOCKET_PACKET) return false;

	//��������
	CThreadLock ThreadLock(m_BufferLocked);
	tagBatchSendRequest pBatchSendRequest;

	//��������
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



//�ر�����
bool CTCPNetworkEngine::CloseSocket(DWORD dwSocketID)
{
	//��������
	CThreadLock ThreadLock(m_BufferLocked);
	tagCloseSocket CloseSocket;

	//��������
	CloseSocket.wIndex = SOCKET_INDEX(dwSocketID);
	CloseSocket.wRountID = SOCKET_ROUNTID(dwSocketID);

	if (CloseSocket.wIndex >= m_NetworkItemStorage.GetCount()) return false;
	//��������
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_CLOSE_SOCKET, &CloseSocket, sizeof(tagCloseSocket));
}

//���ùر�
bool CTCPNetworkEngine::ShutDownSocket(DWORD dwSocketID)
{
	//��������
	CThreadLock ThreadLock(m_BufferLocked);
	tagShutDownSocket ShutDownSocket;

	//��������
	ShutDownSocket.wIndex = SOCKET_INDEX(dwSocketID);
	ShutDownSocket.wRountID = SOCKET_ROUNTID(dwSocketID);

	if (ShutDownSocket.wIndex >= m_NetworkItemStorage.GetCount()) return false;

	//��������
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SHUT_DOWN, &ShutDownSocket, sizeof(tagShutDownSocket));
}

//����Ⱥ��
bool CTCPNetworkEngine::AllowBatchSend(DWORD dwSocketID, bool cbAllowBatch,BYTE cbBatchMask)
{
	//��������
	CThreadLock ThreadLock(m_BufferLocked);
	tagAllowBatchSend AllowBatchSend;

	//��������
	AllowBatchSend.cbAllowBatch = cbAllowBatch;
	AllowBatchSend.cbBatchMask = cbBatchMask;
	AllowBatchSend.wIndex = SOCKET_INDEX(dwSocketID);
	AllowBatchSend.wRountID = SOCKET_ROUNTID(dwSocketID);

	if (AllowBatchSend.wIndex >= m_NetworkItemStorage.GetCount()) return false;
	//��������
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_ALLOW_BATCH, &AllowBatchSend, sizeof(tagAllowBatchSend));
}

//�첽����
bool CTCPNetworkEngine::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case ASYNCHRONISM_SEND_DATA:		//��������
		{
			//Ч������
			tagSendDataRequest * pRequestInfo = (tagSendDataRequest *)pData;

			//��ȡ����
			CTCPNetworkItem * pTCPNetworkItem = GetNetworkItem(pRequestInfo->wIndex);
			if (pTCPNetworkItem == NULL) return false;

			//��������
			CThreadLock SocketThreadLock(pTCPNetworkItem->GetCriticalSection());

			pTCPNetworkItem->SendData(((BYTE*)pData) + sizeof(tagSendDataRequest), pRequestInfo->wDataSize, pRequestInfo->wMainCmdID,
					pRequestInfo->wSubCmdID, pRequestInfo->wRountID);

			return true;
		}
	case ASYNCHRONISM_SEND_BATCH:		//Ⱥ������
		{
			//Ч������
			tagBatchSendRequest * pRequestInfo=(tagBatchSendRequest *)pData;

			//��ȡ���
			CThreadLock ItemThreadLock(m_ItemLocked);
			m_TempNetworkItemArray.Copy(m_NetworkItemActive);
			ItemThreadLock.UnLock();

			//Ⱥ������
			for (INT_PTR i=0;i<m_TempNetworkItemArray.GetCount();i++)
			{
				//��ȡ����
				CTCPNetworkItem * pTCPNetworkItem=m_TempNetworkItemArray[i];
				CThreadLock SocketThreadLock(pTCPNetworkItem->GetCriticalSection());

				//��������
				if (pTCPNetworkItem->IsAllowBatch() && pTCPNetworkItem->GetBatchMask()== pRequestInfo->cbBatchMask)
				{
					pTCPNetworkItem->SendData(((BYTE*)pData)+ sizeof(tagBatchSendRequest), pRequestInfo->wDataSize, pRequestInfo->wMainCmdID,
						pRequestInfo->wSubCmdID, pTCPNetworkItem->GetRountID());
				}
			}

			return true;
		}
	case ASYNCHRONISM_SHUT_DOWN:		//��ȫ�ر�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(tagShutDownSocket));
			tagShutDownSocket * pShutDownSocket=(tagShutDownSocket *)pData;

			//��ȡ����
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pShutDownSocket->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//��ȫ�ر�
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->ShutDownSocket(pShutDownSocket->wRountID);

			return true;
		}
	case ASYNCHRONISM_ALLOW_BATCH:		//����Ⱥ��
		{
			//Ч������
			ASSERT(wDataSize==sizeof(tagAllowBatchSend));
			tagAllowBatchSend * pAllowBatchSend=(tagAllowBatchSend *)pData;

			//��ȡ����
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pAllowBatchSend->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//����Ⱥ��
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->AllowBatchSend(pAllowBatchSend->wRountID,pAllowBatchSend->cbAllowBatch?true:false,pAllowBatchSend->cbBatchMask);

			return true;
		}
	case ASYNCHRONISM_CLOSE_SOCKET:		//�ر�����
		{
			//Ч������
			ASSERT(wDataSize==sizeof(tagCloseSocket));
			tagCloseSocket * pCloseSocket=(tagCloseSocket *)pData;

			//��ȡ����
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pCloseSocket->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//�ر�����
			CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->CloseSocket(pCloseSocket->wRountID);

			return true;
		}
	case ASYNCHRONISM_DETECT_SOCKET:	//�������
		{
			//��ȡ���
			CThreadLock ThreadLock(m_ItemLocked);
			m_TempNetworkItemArray.Copy(m_NetworkItemActive);
			ThreadLock.UnLock();

			//�������
			DWORD dwNowTime=(DWORD)time(NULL);
			for (INT_PTR i=0;i<m_TempNetworkItemArray.GetCount();i++)
			{
				//��ȡ����
				CTCPNetworkItem * pTCPNetworkItem=m_TempNetworkItemArray[i];
				CThreadLock ThreadLock(pTCPNetworkItem->GetCriticalSection());

				
				//��Ч�ж�
				if (pTCPNetworkItem->IsValidSocket()==false) continue;

				switch (pTCPNetworkItem->m_wSurvivalTime)
				{
				case DEAD_QUOTIETY:		//��������
				{
					pTCPNetworkItem->CloseSocket(pTCPNetworkItem->GetRountID());
					break;
				}
				case DANGER_QUOTIETY:	//Σ��ϵ��
				{
					pTCPNetworkItem->m_wSurvivalTime--;
					pTCPNetworkItem->SendData(MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET, pTCPNetworkItem->GetRountID());
					break;
				}
				default:				//Ĭ�ϴ���
				{
					pTCPNetworkItem->m_wSurvivalTime--;
					break;
				}
				}
			}

			return true;
		}
	}

	//Ч������
	ASSERT(FALSE);

	return false;
}

//���¼�
bool CTCPNetworkEngine::OnEventSocketBind(CTCPNetworkItem * pTCPNetworkItem)
{
	//Ч������
	ASSERT(pTCPNetworkItem!=NULL);
	ASSERT(m_pITCPNetworkEngineEvent!=NULL);

	//Ͷ����Ϣ
	DWORD dwClientIP=pTCPNetworkItem->GetClientIP();
	DWORD dwSocketID=pTCPNetworkItem->GetIdentifierID();
	m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(dwSocketID,dwClientIP);

	return true;
}

//�ر��¼�
bool CTCPNetworkEngine::OnEventSocketShut(CTCPNetworkItem * pTCPNetworkItem)
{
	//Ч�����
	ASSERT(pTCPNetworkItem!=NULL);
	ASSERT(m_pITCPNetworkEngineEvent!=NULL);

	try
	{
		//Ͷ������
		DWORD dwClientIP=pTCPNetworkItem->GetClientIP();
		DWORD dwSocketID=pTCPNetworkItem->GetIdentifierID();
		DWORD dwActiveTime=pTCPNetworkItem->GetActiveTime();
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwSocketID,dwClientIP,dwActiveTime);

		//�ͷ�����
		FreeNetworkItem(pTCPNetworkItem);
	}
	catch (...) {}

	return true;
}

//��ȡ�¼�
bool CTCPNetworkEngine::OnEventSocketRead(CMD_Command Command, VOID * pData, WORD wDataSize, CTCPNetworkItem * pTCPNetworkItem)
{
	//Ч������
	ASSERT(pTCPNetworkItem!=NULL);
	ASSERT(m_pITCPNetworkEngineEvent!=NULL);

	//�����ж�
	if (m_bLimited==true)
	{
		//��������
		HANDLE hCompletionPort=NULL;
		hCompletionPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,1);

		//����ѭ��
		while (true)
		{
			DWORD dwThancferred=0;					
			OVERLAPPED * pOverLapped=NULL;
			CTCPNetworkItem * pTCPNetworkItem=NULL;
			GetQueuedCompletionStatus(hCompletionPort,&dwThancferred,(PULONG_PTR)&pTCPNetworkItem,&pOverLapped,INFINITE);
		}

		return false;
	}

	//Ͷ����Ϣ
	DWORD dwSocketID=pTCPNetworkItem->GetIdentifierID();
	m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(dwSocketID,Command,pData,wDataSize);

	return true;
}

//�������
bool CTCPNetworkEngine::DetectSocket()
{
	//��������
	void* pData = NULL;
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_DETECT_SOCKET, pData, 0);
}

//��ҳ��֤
bool CTCPNetworkEngine::WebAttestation()
{
	m_bLimited = false;
	return true;

	//��ȡĿ¼
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//��ȡ����
	TCHAR szFileName[MAX_PATH]=TEXT("");;
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\Validate.ini"),szPath);

	//��������
	DWORD dwClientID=0L;
	TCHAR szClientName[32]=TEXT("");
	TCHAR szClientValidate[33]=TEXT("");

	//��ȡ��֤
	dwClientID=GetPrivateProfileInt(TEXT("Data"),TEXT("ClientID"),0,szFileName);
	GetPrivateProfileString(TEXT("Data"),TEXT("ClientName"),TEXT(""),szClientName,sizeof(szClientName),szFileName);
	GetPrivateProfileString(TEXT("Data"),TEXT("ClientValidate"),TEXT(""),szClientValidate,sizeof(szClientValidate),szFileName);

	//��������
	CInternetSession Seccion;
	CStdioFile * pInternetFile=NULL;

	try
	{
		//������֤
		TCHAR szAttestation[512]=TEXT("");
		//������ʶ
		TCHAR szMachineID[LEN_MACHINE_ID];
		CWHService::GetMachineID(szMachineID);
		_sntprintf(szAttestation,CountArray(szAttestation),TEXT("http://gamj.net/Attestation.aspx?Version=66&ClientID=%ld&ClientName=%s&Validate=%s&ServerTime=%ld&MachineID=%s&Compilation=%s"),
			dwClientID, szClientName, szClientValidate, (DWORD)time(NULL), szMachineID, TEXT("51iknown"));
		//ִ����֤
		TCHAR szResult[1024]=TEXT("");
		pInternetFile = Seccion.OpenURL(szAttestation);
		UINT uReadCount=pInternetFile->Read(szResult,sizeof(szResult));
		szResult[__min(uReadCount,CountArray(szResult)-1)]=0;

		//ֹͣ�ж�
		if ((uReadCount>=2)&&(szResult[0]==TEXT('0'))&&(szResult[1]==TEXT(';')))
		{
			//���ñ���
			m_bLimited = true;
			//��ʾ��Ϣ
			g_TraceServiceManager.TraceString(TEXT("���ķ��������û�еõ��Ϸ���Ȩ������ϵ��������Ϸ�� http://gamj.net"),TraceLevel_Exception);
			return false;
		}

		//��֤�ж�
		if ((uReadCount>(UINT)lstrlen(szClientValidate))&&(memcmp(szClientValidate,szResult,lstrlen(szClientValidate)*sizeof(TCHAR))==0))
		{
			//���ñ���
			m_bLimited = false;
			return true;
		}
		else
		{
			//���ñ���
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

	//���ñ���
	m_bLimited=false;
	return true;
}

bool CTCPNetworkEngine::CreateTCPService(WORD wPort)
{
	//��������
	sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(wPort);


	m_hServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	//�����ж�
	if (m_hServerSocket == INVALID_SOCKET)
	{
		throw TEXT("ϵͳ��Դ������� TCP/IP Э��û�а�װ����������ʧ��");
	}

	//������
	if (bind(m_hServerSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket = INVALID_SOCKET;
		return false;
	}

	//�����˿�
	if (listen(m_hServerSocket, 200) == SOCKET_ERROR)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket = INVALID_SOCKET;
		return false;
	}

	return true;
}

//��ȡ����
CTCPNetworkItem * CTCPNetworkEngine::ActiveNetworkItem()
{
	//��������
	CThreadLock ThreadLock(m_ItemLocked,true);

	//��ȡ����
	CTCPNetworkItem * pTCPNetworkItem=NULL;
	if (m_NetworkItemBuffer.GetCount()>0)
	{
		INT_PTR nItemPostion=m_NetworkItemBuffer.GetCount()-1;
		pTCPNetworkItem=m_NetworkItemBuffer[nItemPostion];
		m_NetworkItemBuffer.RemoveAt(nItemPostion);
		m_NetworkItemActive.Add(pTCPNetworkItem);
	}

	//��������
	if (pTCPNetworkItem==NULL)
	{
		WORD wStorageCount=(WORD)m_NetworkItemStorage.GetCount();
		if (wStorageCount<m_wMaxConnect)
		{
			try
			{
				//��������
				pTCPNetworkItem=new CTCPNetworkItem(wStorageCount,this);
				if (pTCPNetworkItem==NULL) 
				{
					ASSERT(FALSE);
					return NULL;
				}

				//��������
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

//��ȡ����
CTCPNetworkItem * CTCPNetworkEngine::GetNetworkItem(WORD wIndex)
{
	//��������
	CThreadLock ThreadLock(m_ItemLocked,true);

	//Ч������
	ASSERT(wIndex<m_NetworkItemStorage.GetCount());
	if (wIndex>=m_NetworkItemStorage.GetCount()) return NULL;

	//��ȡ����
	CTCPNetworkItem * pTCPNetworkItem=m_NetworkItemStorage[wIndex];

	return pTCPNetworkItem;
}

//�ͷ����Ӷ���
bool CTCPNetworkEngine::FreeNetworkItem(CTCPNetworkItem * pTCPNetworkItem)
{
	//Ч�����
	ASSERT(pTCPNetworkItem!=NULL);

	//�ͷŶ���
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

	//�ͷ�ʧ��
	ASSERT(FALSE);

	return false;
}

//֤������
bool CTCPNetworkEngine::SetCertParameter(LPCTSTR pszCertName, LPCTSTR pszSystemProtocol, bool bMachineStore, bool bAuthClient)
{
	return false;
}
// ��Ȩ��֤
bool CTCPNetworkEngine::BttestationAcc()
{
    //���ܵĻ�����
    TCHAR szServiceMachine[LEN_MACHINE_ID];
    TCHAR szNewMachineID[42];
    _sntprintf(szNewMachineID, sizeof(szNewMachineID), TEXT("%syinhu888"), szServiceMachine);
    TCHAR szEnptMachineID[33];
    CWHEncrypt::MD5Encrypt(szNewMachineID, szEnptMachineID);
    szEnptMachineID[32] = '\0';


    //��ȡ�Ļ�����
    TCHAR szConfigFilePath[MAX_PATH];
    _sntprintf(szConfigFilePath, sizeof(szConfigFilePath), TEXT("c:\\Windows\\ab.ini"));
    TCHAR szReadData[33] = TEXT("");
    GetPrivateProfileString(TEXT("key"), TEXT("value"), TEXT(""), szReadData, CountArray(szReadData), szConfigFilePath);
    szReadData[32] = '\0';

	if (_tcscmp(szEnptMachineID, szReadData) != 0)
    {
		g_TraceServiceManager.TraceString(TEXT("����Ϸ���Ȩ"), TraceLevel_Debug);
		//m_bValidatr = true;
		return false;
    }
	else
	{
		//g_TraceServiceManager.TraceString(TEXT("���δ��Ȩ,����ϵ�ͷ� QQ:3158885888"), TraceLevel_Exception);
		//m_bValidatr = false;
		//return false;
	}

return true;
}


// ��Ȩ��֤
bool CTCPNetworkEngine::AttestationAcc()
{

	TCHAR szLocalMachine[LEN_MACHINE_ID];	//��������
	CWHService::GetMachineID(szLocalMachine);
	TCHAR szCompilationAcc[LEN_MACHINE_ID];
	//g_TraceServiceManager.TraceString(szLocalMachine, TraceLevel_Normal);
	_sntprintf_s(szCompilationAcc, LEN_MACHINE_ID, TEXT("BC74AC55EBE5B2338D45E9874D146086"));

	if (lstrcmp(szLocalMachine, szCompilationAcc) == 0)
	{
		g_TraceServiceManager.TraceString(TEXT("����Ϸ���Ȩ"), TraceLevel_Debug);
		m_bValidate = true;
		return true;
	}
	else
	{
		g_TraceServiceManager.TraceString(TEXT("���δ��Ȩ,����ϵ�ͷ� QQ:3158885888"), TraceLevel_Exception);
		m_bValidate = false;
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(TCPNetworkEngine);

//////////////////////////////////////////////////////////////////////////
