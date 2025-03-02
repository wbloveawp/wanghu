#pragma once

#include "KernelEngineHead.h"

struct tagDecryptResult
{
	CMD_Command						CommandInfo;
	WORD							wDataSize;							//数据包头
	void*							pData;								//数据缓冲
};

class CCipher
{
public:
	CCipher();
	~CCipher();

public:
	BYTE						m_cbKind;							//加密方式
	WORD						m_wDataSize;						//数据长度
	BYTE						m_cbBuffer[SOCKET_BUFFER];		//数据缓存

private:
	//字节映射
	bool MappedBuffer();
	bool UnMappedBuffer();

	//压缩数据
	bool CompressBuffer();
	bool UnCompressBuffer();

public:

	BYTE* GetData() { return m_cbBuffer; }
	WORD GetDataSize() { return m_wDataSize;  }
	void SetDataKind(BYTE cbDataKind);

	bool EncryptBuffer(WORD wMainCmdID, WORD wSubCmdID, void *pData, WORD wDataSize);
	bool CrevasseBuffer(void* pData, WORD wDataSize, tagDecryptResult *pResult);
};

