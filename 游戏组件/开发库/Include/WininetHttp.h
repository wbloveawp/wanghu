#ifndef WININET_HTTP_HEAD_FILE
#define WININET_HTTP_HEAD_FILE


//引入文件
#include "CommonServiceHead.h"

////////////////////////////////////////////////////////////////////////////////////////

//请求类型
enum emHttpRequest
{
	Hr_Get,									
	Hr_Post
};

//错误类型
enum emHttpInterfaceError
{
	Hi_Success=0,					//成功
	Hi_InitErr,						//初始化失败
	Hi_UrlErr,						//网页错误
	Hi_ConnectErr,					//连接HTTP服务器失败
	Hi_SendErr,						//发送请求失败
	Hi_QueryErr,					//查询HTTP请求头失败
	Hi_404,							//页面不存在
	Hi_IllegalUrl,					//无效的URL
	Hi_CreateFileErr,				//创建文件失败
	Hi_DownloadErr,					//下载失败
	Hi_QueryIPErr,					//获取域名对应的地址失败
	Hi_SocketErr,					//套接字错误
	Hi_UserCancel,					//用户取消下载
	Hi_BufferErr,					//文件太大，缓冲区不足
	Hi_HeaderErr,					//HTTP请求头错误
	Hi_ParamErr,					//参数错误，空指针，空字符
	Hi_UnknowErr,					//未知错误		
};

////////////////////////////////////////////////////////////////////////////////////////
//HTTP请求类
class COMMON_SERVICE_CLASS CWininetHttp
{
	//网络句柄
private:
	HINTERNET					m_hSession;								//会话句柄
	HINTERNET					m_hConnect;								//连接句柄	
	HINTERNET					m_hRequest;								//请求句柄		
	emHttpInterfaceError		m_hiError;								//错误类型

	//函数定义
public:
	//构造函数
	CWininetHttp();
	//析构函数
	virtual ~CWininetHttp();

	//功能函数
public:
	//获取错误
	emHttpInterfaceError GetLastError() { return m_hiError; }
	//请求网页
	CStringA RequestWebPage(LPCSTR pszUrl, emHttpRequest hrType, LPCSTR pszHeader, LPCSTR pszPostData);

	//清理函数
protected:
	//关闭句柄
	void Release();
	//释放句柄
	void ReleaseHandle(HINTERNET & hInternet);

	//解析函数
protected:	
	// 解析URL地址
	bool ParseURLWeb(LPCSTR pszUrl, CStringA& strHostName, CStringA& strPagePath, WORD& wPort);	
};

////////////////////////////////////////////////////////////////////////////////////////

#endif