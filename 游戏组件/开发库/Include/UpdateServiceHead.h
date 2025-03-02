#ifndef UPDATE_SERVICE_HEAD_HEAD_FILE
#define UPDATE_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "Platform.h"

//组件文件
#include "CommonServiceHead.h"
#include "SkinControlsHead.h"
#include "ClientControlsHead.h"
#include "NetworkServiceHead.h"
//#include "DownloadHead.h"
#include "PlatformDataHead.h"

//////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef UPDATE_SERVICE_CLASS
	#ifdef  UPDATE_SERVICE_DLL
		#define UPDATE_SERVICE_CLASS _declspec(dllexport)
	#else
		#define UPDATE_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define UPDATE_SERVICE_DLL_NAME	TEXT("UpdateService.dll")			//组件 DLL 名字
#else
	#define UPDATE_SERVICE_DLL_NAME	TEXT("UpdateServiceD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//消息定义

#define WM_SETUP_FINISH				(WM_USER+300)						//安装完成

//////////////////////////////////////////////////////////////////////////
//枚举定义

//错误状态
//  enum enDownLoadError
//  {
//  	DownLoadError_Parameter,		//参数错误
//  	DownLoadError_FileSystem,		//文件错误
//  	DownLoadError_HttpServer,		//服务错误
//  	DownLoadError_SocketConnect,	//连接错误
//  };
// 
// //下载状态
// enum enDownLoadStatus
// {
// 	DownLoadStatus_Null,			//未知状态
// 	DownLoadStatus_Error,			//错误状态
// 	DownLoadStatus_Connect,			//连接状态
// 	DownLoadStatus_Service,			//服务状态
// 	DownLoadStatus_Conclude,		//完成状态
// };

//////////////////////////////////////////////////////////////////////////
//结构定义

//任务请求
struct tagMissionRequest
{
	TCHAR							szLocalPath[MAX_PATH];				//本地路径
	TCHAR							szDownLoadUrl[MAX_PATH];			//下载路径
};

//下载请求
struct tagDownLoadRequest
{
	WORD							wKindID;							//游戏标识
	WORD							wServerID;							//房间标识
	TCHAR							szDescribe[128];					//描述信息
	TCHAR							szLocalPath[MAX_PATH];				//本地路径
	TCHAR							szDownLoadUrl[MAX_PATH];			//下载路径
	bool							bSilent;							//后台下载
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDownLoadSocketSink INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadSocketSink={0x64784823,0xd52b,0x43e2,0x0096,0xdd,0x3c,0x59,0xcb,0x44,0x9c,0x02};
#else
	#define VER_IDownLoadSocketSink INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadSocketSink={0xaf1f61c0,0x7879,0x47f5,0x0092,0xbd,0xc7,0xfd,0xe4,0x32,0xdf,0xf0};
#endif

//下载回调
interface IDownLoadSocketSink : public IUnknownEx
{
	//状态接口
public:
	//连接消息
	virtual bool __cdecl OnSocketConnect(INT nErrorCode)=NULL;
	//关闭消息
	virtual bool __cdecl OnSocketClose(bool bCloseByServer)=NULL;

	//数据接口
public:
	//数据包流
	virtual VOID __cdecl OnHttpDataStream(const VOID * pStreamData, WORD wStreamSize)=NULL;
	//数据包头
	virtual VOID __cdecl OnHttpDataHead(const VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDownLoadMission INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadMission={0x0fcaee7a,0x9a00,0x49fd,0x00ae,0x57,0x30,0xe1,0x03,0x92,0x88,0x2e};
#else
	#define VER_IDownLoadMission INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadMission={0x6f4b16b1,0x715c,0x4f1e,0x00af,0xcd,0x31,0xee,0xbc,0xa1,0x5f,0x5b};
#endif

//任务接口
interface IDownLoadMission : public IUnknownEx
{
	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetDownLoadMissionSink(IUnknownEx * pIUnknownEx)=NULL;

	//任务信息
public:
	//任务标识
	virtual DWORD __cdecl GetMissionID()=NULL;
	//下载状态
	//virtual enDownLoadStatus __cdecl GetDownLoadStatus()=NULL;

	//文件信息
public:
	//下载大小
	virtual DWORD __cdecl GetDownLoadSize()=NULL;
	//文件大小
	virtual DWORD __cdecl GetTotalFileSize()=NULL;
	//文件标识
	virtual LPCTSTR __cdecl GetFileEntityTag()=NULL;

	//控制接口
public:
	//结束下载
	virtual bool __cdecl StopDownLoad()=NULL;
	//开始下载
	virtual bool __cdecl StartDownLoad(DWORD dwMissionID, const tagMissionRequest & MissionRequest)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDownLoadMissionSink INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadMissionSink={0x401d0a3d,0x18b0,0x4a6d,0x0098,0x43,0xfb,0x2e,0x91,0x66,0x7a,0x71};
#else
	#define VER_IDownLoadMissionSink INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadMissionSink={0x348ee1a0,0xad11,0x4fcc,0x0095,0xb0,0xfb,0x84,0x6a,0x62,0xe0,0x1f};
#endif

//任务回调
interface IDownLoadMissionSink : public IUnknownEx
{
	////下载开始
	//virtual bool __cdecl OnDownLoadStart(DWORD dwTotalFileSize, IDownLoadMission * pIDownLoadMission)=NULL;
	////下载状态
	//virtual bool __cdecl OnDownLoadStatus(DWORD dwDownLoadSize, IDownLoadMission * pIDownLoadMission)=NULL;
	////下载完成
	//virtual bool __cdecl OnDownLoadComplete(LPCTSTR pszNativeFile, IDownLoadMission * pIDownLoadMission)=NULL;
	////下载异常
	//virtual bool __cdecl OnDownLoadException(enDownLoadError DownLoadError, IDownLoadMission * pIDownLoadMission)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDownLoadService INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadService={0x7e7ab0b3,0x5bf5,0x4beb,0x00b8,0xe7,0xa1,0xc0,0x04,0xac,0x21,0x86};
#else
	#define VER_IDownLoadService INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadService={0x5d56c83d,0xbb69,0x410f,0x0086,0x91,0xaa,0x15,0x89,0x59,0xc9,0xd3};
#endif

//下载服务接口
interface IDownLoadService : public IUnknownEx
{
	//请求下载
	virtual DWORD __cdecl RequestDownLoad(const tagMissionRequest & MissionRequest)=NULL;
	//请求下载
	virtual DWORD __cdecl RequestDownLoad(const tagDownLoadRequest & DownLoadRequest,DWORD dwPlazaStationID,DWORD dwUserStationID)=NULL;
	//设置资源
	virtual	void __cdecl SetResourceInterface(IPlazaResource *pPlazaResource,HINSTANCE hInstance)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDownLoadResultSink INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadResultSink={0xa40a278d,0xc2e1,0x44be,0x0097,0xcb,0xa5,0x17,0x79,0x9a,0xe5,0x84};
#else
	#define VER_IDownLoadResultSink INTERFACE_VERSION(1,1)
	static const GUID IID_IDownLoadResultSink={0x99db3afb,0x0191,0x4a09,0x00a1,0x06,0xda,0x74,0x0c,0xf6,0x94,0x13};
#endif

//下载结果接口
interface IDownLoadResultSink : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IRedirectServiceSink INTERFACE_VERSION(1,1)
static const GUID IID_IRedirectServiceSink={0x5254d505,0xef87,0x4e2b,0x00bd,0xa2,0xfb,0x9c,0xde,0xb5,0x28,0xf1};
#else
#define VER_IRedirectServiceSink INTERFACE_VERSION(1,1)
static const GUID IID_IRedirectServiceSink={0xd8dc3ba9,0xaeb5,0x4ee7,0x0090,0xf2,0x86,0x66,0x45,0x23,0xa4,0x7f};
#endif

//定向回调
interface IRedirectServiceSink : public IUnknownEx
{
	////下载完成
	//virtual bool __cdecl OnRedirectComplete(LPCTSTR pszServerIP, int nParam)=NULL;
	////下载异常
	//virtual bool __cdecl OnRedirectException(enDownLoadError DownLoadError)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IRedirectService INTERFACE_VERSION(1,1)
static const GUID IID_IRedirectService={0xdc79fc0e,0x645e,0x4466,0x008f,0x28,0x50,0xc5,0xd5,0x81,0x59,0x5c};
#else
#define VER_IRedirectService INTERFACE_VERSION(1,1)
static const GUID IID_IRedirectService={0x4adcffa7,0x615,0x4a51,0x00a1,0x67,0x3f,0xfa,0x15,0xfe,0xfc,0x69};
#endif

//重定向服务接口
interface IRedirectService : public IUnknownEx
{
	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetRedirectServiceSink(IUnknownEx * pIUnknownEx)=NULL;

	//控制接口
public:
	//请求定向
	virtual bool __cdecl RequestRedirect(LPCTSTR lpURL)=NULL;
	//停止定向
	virtual VOID __cdecl StopRedirect()=NULL;

	//查询接口
public:
	//获取定向地址
	virtual LPCTSTR __cdecl GetRedirectServer()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IHttpServiceSink INTERFACE_VERSION(1,1)
static const GUID IID_IHttpServiceSink={0xa608002c,0xe78c,0x4d23,0x9c,0xca,0x35,0x1a,0xa7,0x34,0x88,0x91};
#else
#define VER_IHttpServiceSink INTERFACE_VERSION(1,1)
static const GUID IID_IHttpServiceSink={0x9afa5644,0x9680,0x4cc4,0x8f,0x10,0x79,0x49,0x67,0x88,0xea,0x9b};
#endif

//HTTP服务回调
interface IHttpServiceSink
{
	////请求缓冲
	//virtual DWORD __cdecl OnHttpRequestBuffer(DWORD dwRequestSize, LPVOID *ppvBuffer)=NULL;
	////下载完成
	//virtual bool __cdecl OnHttpDataComplete(DWORD dwDataSize)=NULL;
	////下载异常
	//virtual bool __cdecl OnHttpDataException(enDownLoadError DownLoadError)=NULL;
	////连接关闭
	//virtual bool __cdecl OnHttpSocketClose(bool bCloseByServer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IHttpService INTERFACE_VERSION(1,1)
static const GUID IID_IHttpService={0x36854e48,0x54e6,0x4812,0x9c,0xfb,0x61,0xca,0x4d,0xea,0xc5,0xef};
#else
#define VER_IHttpService INTERFACE_VERSION(1,1)
static const GUID IID_IHttpService={0x4125c1f8,0xabcb,0x483d,0xbf,0x57,0x1e,0xc1,0xa3,0xf9,0xf3,0xb7};
#endif

//HTTP服务接口
interface IHttpService : public IUnknownEx
{
	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetHttpServiceSink(IHttpServiceSink * pSink)=NULL;

	//控制接口
public:
	//连接请求
	virtual bool __cdecl ConnectServer(LPCTSTR lpURL)=NULL;
	//停止服务
	virtual VOID __cdecl StopService()=NULL;
};

//////////////////////////////////////////////////////////////////////////
//组件辅助类

DECLARE_MODULE_HELPER(DownLoadMission, DownLoadMission,UPDATE_SERVICE_DLL_NAME,"CreateDownLoadMission")
DECLARE_MODULE_HELPER(DownLoadService, DownLoadService,UPDATE_SERVICE_DLL_NAME,"CreateDownLoadService")
DECLARE_MODULE_HELPER(RedirectService, RedirectService, UPDATE_SERVICE_DLL_NAME,"CreateRedirectService")
DECLARE_MODULE_HELPER(HttpService, HttpService, UPDATE_SERVICE_DLL_NAME,"CreateHttpService")

//////////////////////////////////////////////////////////////////////////

#endif