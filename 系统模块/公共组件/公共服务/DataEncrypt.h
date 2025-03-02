#ifndef DATA_ENCRYPT_HEAD_FILE
#define DATA_ENCRYPT_HEAD_FILE

#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define XOR_TIMES					8									//加密倍数
#define ENCRYPT_KEY_LEN				8									//密钥长度

#define MAX_SOURCE_LEN				128									//最大长度
#define MAX_TARGET_LEN				(MAX_SOURCE_LEN*XOR_TIMES+1L)		//最大长度

//////////////////////////////////////////////////////////////////////////

//异或加密
class COMMON_SERVICE_CLASS CXOREncrypt
{
	//函数定义
private:
	//构造函数
	CXOREncrypt() {}

	//功能函数
public:
	//生成密文
	static bool EncryptData(LPCTSTR pszSourceData, LPTSTR pszEncrypData, WORD wMaxCount, WORD wEncryptKey=0);
	//解开密文
	static bool CrevasseData(LPCTSTR pszEncrypData, LPTSTR pszSourceData, WORD wMaxCount, WORD wEncryptKey=0);
};

//////////////////////////////////////////////////////////////////////////

//映射加密
class COMMON_SERVICE_CLASS CMapEncrypt
{
	//函数定义
private:
	//构造函数
	CMapEncrypt() {}

	//功能函数
public:
	//生成密文
	static bool EncryptData(LPVOID pcbSourceData, LPVOID pcbEncrypData, WORD wDataCount);
	//解开密文
	static bool CrevasseData(LPVOID pcbEncrypData, LPVOID pcbSourceData, WORD wDataCount);
};

//////////////////////////////////////////////////////////////////////////

//MD5 加密
class COMMON_SERVICE_CLASS CMD5Encrypt
{
	//函数定义
private:
	//构造函数
	CMD5Encrypt() {}

	//功能函数
public:
	//生成密文
	static VOID EncryptData(LPCSTR pszSourceData, CHAR szMD5Result[33]);
	//生成密文
	static VOID EncryptData(LPCWSTR pszSourceData, WCHAR szMD5Result[33]);
	//文件MD5
	static bool GetFileMD5(const CString &strFileName, TCHAR szMD5Result[LEN_MD5]);
};

//////////////////////////////////////////////////////////////////////////

//MD5 加密
class CMD5Operate
{
	//变量定义
private:
	ULONG							m_lCount[2];						//加密变量
	ULONG							m_lState[4];						//加密变量
	UCHAR							m_cbBuffer[64];						//加密变量
	UCHAR							m_cbPadding[64];					//加密变量

	//函数定义
public:
	//构造函数
	CMD5Operate() { MD5Init(); }

	//功能函数
public:
	//最终结果
	VOID MD5Final(BYTE digest[16]);
	//设置数值
	VOID MD5Update(BYTE * pcbInput, UINT inputLen);

	//内部函数
private:
	//初始化
	VOID MD5Init();
	//置位函数
	VOID MD5Memset(BYTE * pcbOutput, INT nValue, UINT nLen);
	//拷贝函数
	VOID MD5Memcpy(BYTE * pcbOutput, BYTE * pcbInput, UINT nLen);
	//转换函数
	VOID MD5Transform(ULONG lState[4], BYTE cbBlock[64]);

	//编码函数
private:
	//编码函数
	VOID Encode(BYTE * pcbOutput, ULONG * pcbInput, UINT nLen);
	//解码函数
	VOID Decode(ULONG *pcbOutput, BYTE * pcbInput, UINT nLen);
};

//////////////////////////////////////////////////////////////////////////

#endif