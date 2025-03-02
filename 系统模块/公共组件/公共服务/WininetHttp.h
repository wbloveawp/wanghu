#ifndef WININET_HTTP_HEAD_FILE
#define WININET_HTTP_HEAD_FILE


//�����ļ�
#include "CommonServiceHead.h"

////////////////////////////////////////////////////////////////////////////////////////

//��������
enum emHttpRequest
{
	Hr_Get,									
	Hr_Post
};

//��������
enum emHttpInterfaceError
{
	Hi_Success=0,					//�ɹ�
	Hi_InitErr,						//��ʼ��ʧ��
	Hi_UrlErr,						//��ҳ����
	Hi_ConnectErr,					//����HTTP������ʧ��
	Hi_SendErr,						//��������ʧ��
	Hi_QueryErr,					//��ѯHTTP����ͷʧ��
	Hi_404,							//ҳ�治����
	Hi_IllegalUrl,					//��Ч��URL
	Hi_CreateFileErr,				//�����ļ�ʧ��
	Hi_DownloadErr,					//����ʧ��
	Hi_QueryIPErr,					//��ȡ������Ӧ�ĵ�ַʧ��
	Hi_SocketErr,					//�׽��ִ���
	Hi_UserCancel,					//�û�ȡ������
	Hi_BufferErr,					//�ļ�̫�󣬻���������
	Hi_HeaderErr,					//HTTP����ͷ����
	Hi_ParamErr,					//�������󣬿�ָ�룬���ַ�
	Hi_UnknowErr,					//δ֪����		
};

////////////////////////////////////////////////////////////////////////////////////////
//HTTP������
class COMMON_SERVICE_CLASS CWininetHttp
{
	//������
private:
	HINTERNET					m_hSession;								//�Ự���
	HINTERNET					m_hConnect;								//���Ӿ��	
	HINTERNET					m_hRequest;								//������		
	emHttpInterfaceError		m_hiError;								//��������

	//��������
public:
	//���캯��
	CWininetHttp();
	//��������
	virtual ~CWininetHttp();

	//���ܺ���
public:
	//��ȡ����
	emHttpInterfaceError GetLastError() { return m_hiError; }
	//������ҳ
	CStringA RequestWebPage(LPCSTR pszUrl, emHttpRequest hrType, LPCSTR pszHeader, LPCSTR pszPostData);

	//������
protected:
	//�رվ��
	void Release();
	//�ͷž��
	void ReleaseHandle(HINTERNET & hInternet);

	//��������
protected:	
	// ����URL��ַ
	bool ParseURLWeb(LPCSTR pszUrl, CStringA& strHostName, CStringA& strPagePath, WORD& wPort);	
};

////////////////////////////////////////////////////////////////////////////////////////

#endif