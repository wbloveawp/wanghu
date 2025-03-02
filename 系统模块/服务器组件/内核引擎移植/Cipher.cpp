#include "Stdafx.h"
#include "Cipher.h"


CCipher::CCipher()
{
}


CCipher::~CCipher()
{
	m_wDataSize = 0;
	m_cbKind = 0;
	ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
}

void CCipher::SetDataKind(BYTE cbDataKind)
{
	m_cbKind = cbDataKind;
}

bool CCipher::MappedBuffer()
{
	if (m_wDataSize < sizeof(TCP_Info)) return false;

	BYTE cbCheckCode = 0;
	BYTE* pData = (BYTE*)m_cbBuffer;
	for(WORD i = sizeof(TCP_Info); i < m_wDataSize; i++)
	{
		BYTE cbOneKey = pData[i];
		cbCheckCode+= cbOneKey;
		pData[i] = g_SendByteMap[cbOneKey];
	}			
		
	TCP_Head * pHead = (TCP_Head *)m_cbBuffer;
	pHead->TCPInfo.cbCheckCode = ~cbCheckCode + 1;
	pHead->TCPInfo.cbDataKind |= DK_MAPPED;
	pHead->TCPInfo.wPacketSize = m_wDataSize;

	return cbCheckCode != 0;

}

bool CCipher::UnMappedBuffer()
{
	TCP_Info * pInfo = (TCP_Info*)m_cbBuffer;
	//效验包大小
	if (m_wDataSize < sizeof(TCP_Head) || pInfo->wPacketSize != m_wDataSize)
		return false;
	//如果加密方式没有使用字节映射 那么直接返回
	if (!(pInfo->cbDataKind & DK_MAPPED)) return true;

	BYTE cbCheckCode = pInfo->cbCheckCode;;
	BYTE* pBuffer = (BYTE*)m_cbBuffer;
	for(WORD i = sizeof(TCP_Info); i < m_wDataSize; i++)
	{
		BYTE cbOneByte = g_RecvByteMap[pBuffer[i]];
		cbCheckCode+=cbOneByte;
		pBuffer[i] = cbOneByte;
	}
	//校验码
	return cbCheckCode == 0;
}

bool CCipher::CompressBuffer()
{
	if (m_wDataSize >= sizeof(TCP_Head))
	{
		WORD wSourceSize = m_wDataSize - sizeof(TCP_Info);
		LPBYTE pcbSourceData = m_cbBuffer + sizeof(TCP_Info);

		BYTE cbResultData[SOCKET_TCP_BUFFER];
		ULONG lResultSize = CWHService::CompressData(pcbSourceData, wSourceSize, cbResultData, sizeof(cbResultData));
		if (lResultSize > 0)
		{
			if (lResultSize < wSourceSize)
			{
				TCP_Info * pInfo = (TCP_Info*)m_cbBuffer;
				pInfo->cbDataKind |= DK_COMPRESS;
				pInfo->wPacketSize = ((WORD)lResultSize) + sizeof(TCP_Info);
				m_wDataSize = ((WORD)lResultSize) + sizeof(TCP_Info);
				CopyMemory(m_cbBuffer + sizeof(TCP_Info), cbResultData, lResultSize);
			}
		}

		return true;

	}
	return false;
}

bool CCipher::UnCompressBuffer()
{
	TCP_Info * pInfo = (TCP_Info*)m_cbBuffer;

	if (!(pInfo->cbDataKind & 4))
	{
		return true;
	}

	WORD wSourceSize = m_wDataSize - sizeof(TCP_Info);
	LPBYTE pcbSourceData = m_cbBuffer + sizeof(TCP_Info);

	BYTE cbResultData[SOCKET_TCP_BUFFER];
	ULONG lResultSize = CWHService::UnCompressData(pcbSourceData, wSourceSize, cbResultData, sizeof(cbResultData));

	if (lResultSize > wSourceSize && wSourceSize + sizeof(TCP_Info) <= SOCKET_TCP_BUFFER)
	{
		pInfo->cbDataKind &= ~DK_COMPRESS;
		pInfo->wPacketSize = ((WORD)lResultSize) + sizeof(TCP_Info);
		m_wDataSize = ((WORD)lResultSize) + sizeof(TCP_Info);
		CopyMemory(m_cbBuffer + sizeof(TCP_Info), cbResultData, lResultSize);
		return true;
	}

	return false;
}

bool CCipher::EncryptBuffer(WORD wMainCmdID, WORD wSubCmdID, void* pData, WORD wDataSize)
{
	if (wDataSize <= SOCKET_TCP_PACKET)
	{
		m_wDataSize = sizeof(TCP_Head) + wDataSize;
		TCP_Head *pHead = (TCP_Head*)m_cbBuffer;
		pHead->CommandInfo.wMainCmdID = wMainCmdID;
		pHead->CommandInfo.wSubCmdID = wSubCmdID;

		if (wDataSize > 0)
		{
			ASSERT(pData != NULL);
			CopyMemory(pHead+1, pData, wDataSize);
		}

		if (  (!(m_cbKind & DK_MAPPED) || MappedBuffer()) 
			/*&& (!(m_cbKind & DK_COMPRESS) || CompressBuffer())*/ )
			return true;

	}
	return false;
}

bool CCipher::CrevasseBuffer(void* pData, WORD wDataSize, tagDecryptResult *pResult)
{
	if (wDataSize <= sizeof(TCP_Info)) return false;
	if (wDataSize > SOCKET_TCP_BUFFER) return false;

	CopyMemory(m_cbBuffer, pData, wDataSize);

	TCP_Head *pHead = (TCP_Head*)m_cbBuffer;
	if (pHead->TCPInfo.wPacketSize != wDataSize) return false;
	m_wDataSize = wDataSize;

	if (!m_cbKind)
	{
		if (pHead->TCPInfo.cbDataKind & DK_MAPPED) m_cbKind = DK_MAPPED;
		if (pHead->TCPInfo.cbDataKind & DK_ENCRYPT) m_cbKind |= DK_ENCRYPT;
		if (pHead->TCPInfo.cbDataKind & DK_COMPRESS) m_cbKind |= DK_COMPRESS;
		if (!m_cbKind) return false;
	}

	if (UnMappedBuffer()/* && UnCompressBuffer()*/)
	{
		pResult->CommandInfo = pHead->CommandInfo;
		pResult->wDataSize = pHead->TCPInfo.wPacketSize - sizeof(TCP_Head);
		pResult->pData = (pHead+1);

		return true;
	}

	return false;
}