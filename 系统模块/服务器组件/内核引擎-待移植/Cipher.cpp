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
	if (m_wDataSize < sizeof(CMD_Info)) return false;

	BYTE cbCheckCode = 0;
	BYTE* pData = (BYTE*)m_cbBuffer;
	//for(WORD i = sizeof(TCP_Info); i < m_wDataSize; i++)
	//{
	//	BYTE cbOneKey = pData[i];
	//	cbCheckCode+= cbOneKey;
	//	pData[i] = g_SendByteMap[cbOneKey];
	//	pData[i] = cbOneKey;
	//}
		
	CMD_Head * pHead = (CMD_Head *)m_cbBuffer;
	pHead->CmdInfo.cbCheckCode = ~cbCheckCode + 1;
	pHead->CmdInfo.cbDataMask |= DATA_MASK_CRYPTED;
	pHead->CmdInfo.wPacketSize = m_wDataSize;

	//return cbCheckCode != 0;
	return true;
}

bool CCipher::UnMappedBuffer()
{
	CMD_Info * pInfo = (CMD_Info*)m_cbBuffer;
	//效验包大小
	if (m_wDataSize < sizeof(CMD_Head) || pInfo->wPacketSize != m_wDataSize)
		return false;
	//如果加密方式没有使用字节映射 那么直接返回
	if (!(pInfo->cbDataMask & DATA_MASK_CRYPTED)) return true;

	BYTE cbCheckCode = pInfo->cbCheckCode;;
	BYTE* pBuffer = (BYTE*)m_cbBuffer;
	//for(WORD i = sizeof(TCP_Info); i < m_wDataSize; i++)
	//{
	//	BYTE cbOneByte = g_RecvByteMap[pBuffer[i]];
	//	cbCheckCode+=cbOneByte;
	//	pBuffer[i] = cbOneByte;
	//}
	//校验码
	//return cbCheckCode == 0;
	return true;
}

bool CCipher::CompressBuffer()
{
	if (m_wDataSize >= sizeof(CMD_Head))
	{
		WORD wSourceSize = m_wDataSize - sizeof(CMD_Info);
		LPBYTE pcbSourceData = m_cbBuffer + sizeof(CMD_Info);

		BYTE cbResultData[SOCKET_BUFFER];
		ULONG lResultSize = CWHService::CompressData(pcbSourceData, wSourceSize, cbResultData, sizeof(cbResultData));
		if (lResultSize > 0)
		{
			if (lResultSize < wSourceSize)
			{
				CMD_Info * pInfo = (CMD_Info*)m_cbBuffer;
				pInfo->cbDataMask |= DATA_MASK_CRYPTED;
				pInfo->wPacketSize = ((WORD)lResultSize) + sizeof(CMD_Info);
				m_wDataSize = ((WORD)lResultSize) + sizeof(CMD_Info);
				CopyMemory(m_cbBuffer + sizeof(CMD_Info), cbResultData, lResultSize);
			}
		}

		return true;

	}
	return false;
}

bool CCipher::UnCompressBuffer()
{
	CMD_Info * pInfo = (CMD_Info*)m_cbBuffer;

	if (!(pInfo->cbDataMask & DATA_MASK_CRYPTED))
	{
		return true;
	}

	WORD wSourceSize = m_wDataSize - sizeof(CMD_Info);
	LPBYTE pcbSourceData = m_cbBuffer + sizeof(CMD_Info);

	BYTE cbResultData[SOCKET_BUFFER];
	ULONG lResultSize = CWHService::DecodeString(pcbSourceData, wSourceSize, cbResultData, sizeof(cbResultData));

	if (lResultSize > wSourceSize && wSourceSize + sizeof(CMD_Info) <= SOCKET_TCP_BUFFER)
	{
		pInfo->cbDataMask &= ~DATA_MASK_CRYPTED;
		pInfo->wPacketSize = ((WORD)lResultSize) + sizeof(CMD_Info);
		m_wDataSize = ((WORD)lResultSize) + sizeof(CMD_Info);
		CopyMemory(m_cbBuffer + sizeof(CMD_Info), cbResultData, lResultSize);
		return true;
	}

	return false;
}

bool CCipher::EncryptBuffer(WORD wMainCmdID, WORD wSubCmdID, void* pData, WORD wDataSize)
{
	if (wDataSize <= SOCKET_PACKET)
	{
		m_wDataSize = sizeof(CMD_Head) + wDataSize;
		CMD_Head *pHead = (CMD_Head*)m_cbBuffer;
		pHead->CommandInfo.wMainCmdID = wMainCmdID;
		pHead->CommandInfo.wSubCmdID = wSubCmdID;

		if (wDataSize > 0)
		{
			ASSERT(pData != NULL);
			CopyMemory(pHead+1, pData, wDataSize);
		}

		if ((!(m_cbKind & DATA_MASK_CRYPTED) || MappedBuffer())
			/*&& (!(m_cbKind & DK_COMPRESS) || CompressBuffer())*/ )
			return true;

	}
	return false;
}

bool CCipher::CrevasseBuffer(void* pData, WORD wDataSize, tagDecryptResult *pResult)
{
	if (wDataSize <= sizeof(CMD_Info)) return false;
	if (wDataSize > SOCKET_BUFFER) return false;

	CopyMemory(m_cbBuffer, pData, wDataSize);

	CMD_Head *pHead = (CMD_Head*)m_cbBuffer;
	if (pHead->CmdInfo.wPacketSize != wDataSize) return false;
	m_wDataSize = wDataSize;

	if (!m_cbKind)
	{
		if (pHead->CmdInfo.cbDataMask & DATA_MASK_CRYPTED) m_cbKind = DATA_MASK_CRYPTED;
		if (pHead->CmdInfo.cbDataMask & DATA_MASK_TAKETOKEN) m_cbKind |= DATA_MASK_TAKETOKEN;
		//if (pHead->CmdInfo.cbDataMask & DK_COMPRESS) m_cbKind |= DK_COMPRESS;
		if (!m_cbKind) return false;
	}

	if (UnMappedBuffer()/* && UnCompressBuffer()*/)
	{
		pResult->CommandInfo = pHead->CommandInfo;
		pResult->wDataSize = pHead->CmdInfo.wPacketSize - sizeof(CMD_Head);
		pResult->pData = (pHead+1);

		return true;
	}

	return false;
}