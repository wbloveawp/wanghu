#include "StdAfx.h"
#include "WHBase64.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//

//常量定义
const CHAR CHAR_63 = '+';
const CHAR CHAR_64 = '/';
const CHAR CHAR_PAD = '=';

//字符表
const CHAR alph[] = 
{ 
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9',CHAR_63,CHAR_64
};

//缓冲定义
union unBuffer
{
	unsigned char bytes[ 4 ];
	unsigned int block;
}; 

//获取长度
inline int GetDataLength(int nCodeLength)
{
	return nCodeLength-nCodeLength/4;
}

//获取长度
inline int GetCodeLength(int nDataLength)
{
	int len=nDataLength+nDataLength/3 + (int)(nDataLength%3!=0);
	if (len % 4) len += 4-len%4;
	return len;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CWHBase64::CWHBase64()
{
}

//析构函数
CWHBase64::~CWHBase64()
{
}

//编码函数
int CWHBase64::Encode(const CHAR * pData, int nDataLen, CHAR szOutBuffer[], int nBufferLen)
{
	//计算长度
	int nCodeLen=GetCodeLength(nDataLen);
	if (nBufferLen<nCodeLen) return 0;

	//变量定义
	unBuffer Buffer;
	const int nMask=0x3F;	

	for (int i=0,j=0,left=nDataLen; i<nDataLen; i+=3,j+=4,left-=3)
	{
		Buffer.bytes[2]=pData[i];

		if (left>1)
		{
			Buffer.bytes[1]=pData[i+1];
			if (left>2 )
				Buffer.bytes[0]=pData[i+2];
			else
				Buffer.bytes[0]=0;
		}
		else
		{
			Buffer.bytes[1]=0;
			Buffer.bytes[0]=0;
		}

		szOutBuffer[j]=alph[(Buffer.block>>18)&nMask];
		szOutBuffer[j+1]=alph[(Buffer.block>>12)&nMask];
		if (left>1)
		{
			szOutBuffer[j+2]=alph[(Buffer.block>>6)&nMask];
			if ( left > 2 )
				szOutBuffer[j+3]=alph[Buffer.block&nMask];
			else
				szOutBuffer[j+3]=CHAR_PAD;
		}
		else
		{
			szOutBuffer[j+2]=CHAR_PAD;
			szOutBuffer[j+3]=CHAR_PAD;
		}
	}

	//设置结束符
	szOutBuffer[nCodeLen]=0;	

	return nCodeLen;
}

// 解码函数
int CWHBase64::Decode(const CHAR * pszCode, int nCodeLen, CHAR szOutBuffer[], int nBufferLen)
{
	//计算长度
	int nDataLen=GetDataLength(nCodeLen);
	if (nBufferLen<nDataLen) return 0;

	//变量定义
	unBuffer Buffer;
	Buffer.block=0;

	int j=0;
	for (int i=0; i<nCodeLen; i++)
	{
		int m=i%4;
		int val=0;
		char x=pszCode[i];

		if (x>='A' && x<='Z')
			val=x-'A';
		else if (x>='a' && x<='z')
			val=x-'a'+'Z'-'A'+1;
		else if (x>='0' && x<='9')
			val=x-'0'+('Z'-'A'+1)*2;
		else if (x==CHAR_63)
			val=62;
		else if (x==CHAR_64)
			val=63;

		if (x!=CHAR_PAD)
			Buffer.block|=val<<(3-m)*6;
		else
			m--;

		if (m==3 || x==CHAR_PAD)
		{
			szOutBuffer[j++]=Buffer.bytes[2];
			if (x!=CHAR_PAD || m>1)
			{
				szOutBuffer[j++]=Buffer.bytes[1];
				if (x!=CHAR_PAD || m>2) szOutBuffer[j++]=Buffer.bytes[0];
			}

			Buffer.block=0;
		}

		if (x==CHAR_PAD) break;
		if (j==nBufferLen) break;
	}

	//设置结束符
	szOutBuffer[j]=0;

	return j;
}

//////////////////////////////////////////////////////////////////////////////////////////////