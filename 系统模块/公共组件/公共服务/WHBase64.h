#ifndef WH_BASE64_HEAD_FILE
#define WH_BASE64_HEAD_FILE

#pragma once

//引入文件
#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//Base64编码
class COMMON_SERVICE_CLASS CWHBase64
{	
	//函数定义
public:
	//构造函数
	CWHBase64();
	//析构函数
	virtual ~CWHBase64();	

	//编码函数
public:	
	//编码函数
	static int Encode(const CHAR * pData, int nDataLen, CHAR szOutBuffer[], int nBufferLen);	
	//解码函数
	static int Decode(const CHAR * pszCode, int nCodeLen, CHAR szOutBuffer[], int nBufferLen);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif