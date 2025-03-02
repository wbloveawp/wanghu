#ifndef DATA_ENCRYPT_HEAD_FILE
#define DATA_ENCRYPT_HEAD_FILE

#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//�궨��
#define XOR_TIMES					8									//���ܱ���
#define ENCRYPT_KEY_LEN				8									//��Կ����

#define MAX_SOURCE_LEN				128									//��󳤶�
#define MAX_TARGET_LEN				(MAX_SOURCE_LEN*XOR_TIMES+1L)		//��󳤶�

//////////////////////////////////////////////////////////////////////////

//������
class COMMON_SERVICE_CLASS CXOREncrypt
{
	//��������
private:
	//���캯��
	CXOREncrypt() {}

	//���ܺ���
public:
	//��������
	static bool EncryptData(LPCTSTR pszSourceData, LPTSTR pszEncrypData, WORD wMaxCount, WORD wEncryptKey=0);
	//�⿪����
	static bool CrevasseData(LPCTSTR pszEncrypData, LPTSTR pszSourceData, WORD wMaxCount, WORD wEncryptKey=0);
};

//////////////////////////////////////////////////////////////////////////

//ӳ�����
class COMMON_SERVICE_CLASS CMapEncrypt
{
	//��������
private:
	//���캯��
	CMapEncrypt() {}

	//���ܺ���
public:
	//��������
	static bool EncryptData(LPVOID pcbSourceData, LPVOID pcbEncrypData, WORD wDataCount);
	//�⿪����
	static bool CrevasseData(LPVOID pcbEncrypData, LPVOID pcbSourceData, WORD wDataCount);
};

//////////////////////////////////////////////////////////////////////////

//MD5 ����
class COMMON_SERVICE_CLASS CMD5Encrypt
{
	//��������
private:
	//���캯��
	CMD5Encrypt() {}

	//���ܺ���
public:
	//��������
	static VOID EncryptData(LPCSTR pszSourceData, CHAR szMD5Result[33]);
	//��������
	static VOID EncryptData(LPCWSTR pszSourceData, WCHAR szMD5Result[33]);
	//�ļ�MD5
	static bool GetFileMD5(const CString &strFileName, TCHAR szMD5Result[LEN_MD5]);
};

//////////////////////////////////////////////////////////////////////////

//MD5 ����
class CMD5Operate
{
	//��������
private:
	ULONG							m_lCount[2];						//���ܱ���
	ULONG							m_lState[4];						//���ܱ���
	UCHAR							m_cbBuffer[64];						//���ܱ���
	UCHAR							m_cbPadding[64];					//���ܱ���

	//��������
public:
	//���캯��
	CMD5Operate() { MD5Init(); }

	//���ܺ���
public:
	//���ս��
	VOID MD5Final(BYTE digest[16]);
	//������ֵ
	VOID MD5Update(BYTE * pcbInput, UINT inputLen);

	//�ڲ�����
private:
	//��ʼ��
	VOID MD5Init();
	//��λ����
	VOID MD5Memset(BYTE * pcbOutput, INT nValue, UINT nLen);
	//��������
	VOID MD5Memcpy(BYTE * pcbOutput, BYTE * pcbInput, UINT nLen);
	//ת������
	VOID MD5Transform(ULONG lState[4], BYTE cbBlock[64]);

	//���뺯��
private:
	//���뺯��
	VOID Encode(BYTE * pcbOutput, ULONG * pcbInput, UINT nLen);
	//���뺯��
	VOID Decode(ULONG *pcbOutput, BYTE * pcbInput, UINT nLen);
};

//////////////////////////////////////////////////////////////////////////

#endif