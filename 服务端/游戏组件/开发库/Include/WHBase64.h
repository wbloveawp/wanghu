#ifndef WH_BASE64_HEAD_FILE
#define WH_BASE64_HEAD_FILE

#pragma once

//�����ļ�
#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//Base64����
class COMMON_SERVICE_CLASS CWHBase64
{	
	//��������
public:
	//���캯��
	CWHBase64();
	//��������
	virtual ~CWHBase64();	

	//���뺯��
public:	
	//���뺯��
	static int Encode(const CHAR * pData, int nDataLen, CHAR szOutBuffer[], int nBufferLen);	
	//���뺯��
	static int Decode(const CHAR * pszCode, int nCodeLen, CHAR szOutBuffer[], int nBufferLen);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif