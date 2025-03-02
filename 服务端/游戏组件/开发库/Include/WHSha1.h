#ifndef WH_SHA1_HEAD_FILE
#define WH_SHA1_HEAD_FILE

#pragma once

//�����ļ�
#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//���Ͷ���
enum emShaData
{
	SHA1_DATA_PUSH_SUCCEED			=	 0,
	SHA1_DATA_TOO_LONG				=	-1,
	SHA1_DATA_ALREADY_COMPUTED		=	-2,
	SHA1_FILE_OPEN_FAILED			=	-3,
};

//////////////////////////////////////////////////////////////////////////////////

//Sha����
class COMMON_SERVICE_CLASS CWHSha1
{
	//��������
public:
	//���캯��
	CWHSha1();
	//��������
	virtual ~CWHSha1();			

	//���غ���
public:
	//��ϣ����
	static int HashData(const void * pData, UINT nDataSize, UINT uMsgDigest[5]);		
};

//////////////////////////////////////////////////////////////////////////////////

#endif
