#pragma once

#include "KernelEngineHead.h"

struct tagDecryptResult
{
	CMD_Command						CommandInfo;
	WORD							wDataSize;							//���ݰ�ͷ
	void*							pData;								//���ݻ���
};

class CCipher
{
public:
	CCipher();
	~CCipher();

public:
	BYTE						m_cbKind;							//���ܷ�ʽ
	WORD						m_wDataSize;						//���ݳ���
	BYTE						m_cbBuffer[SOCKET_BUFFER];		//���ݻ���

private:
	//�ֽ�ӳ��
	bool MappedBuffer();
	bool UnMappedBuffer();

	//ѹ������
	bool CompressBuffer();
	bool UnCompressBuffer();

public:

	BYTE* GetData() { return m_cbBuffer; }
	WORD GetDataSize() { return m_wDataSize;  }
	void SetDataKind(BYTE cbDataKind);

	bool EncryptBuffer(WORD wMainCmdID, WORD wSubCmdID, void *pData, WORD wDataSize);
	bool CrevasseBuffer(void* pData, WORD wDataSize, tagDecryptResult *pResult);
};

