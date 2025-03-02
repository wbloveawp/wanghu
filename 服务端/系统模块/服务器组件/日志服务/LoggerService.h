#ifndef REMOTE_LOOGER_SERVICE_HEAD_FILE
#define REMOTE_LOOGER_SERVICE_HEAD_FILE
#pragma once

#include "Logger.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//日志服务
interface ILoggerSocket
{
  //发送数据
  virtual bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)=NULL;
};

///////////////////////////////////////////////////////////////////////////////////////////////

//日志连接
class LOGGER_SERVICE_CLASS CLoggerSocket : public ILoggerSocket, public ITCPSocketEvent
{
  //状态变量
protected:
    bool							m_bService;							//服务标志
    bool							m_bConnected;						//连接标识

  //连接信息
protected:
    WORD							m_wServicePort;						//服务端口
    TCHAR							m_szServiceHost[LEN_DOMAIN];		//服务地址	

  //组件变量
protected:
    CTCPSocketServiceHelper			m_TCPSocketService;			        //网络服务

  //函数定义
public:
    //构造函数
    CLoggerSocket();
    //析构函数
    virtual ~CLoggerSocket();

  //基础接口
public:
    //释放对象
    virtual VOID Release() { return; }
    //接口查询
    virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //网络接口
protected:
    //连接事件
    virtual bool OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode);
    //关闭事件
    virtual bool OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason);
    //读取事件
    virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID* pData, WORD wDataSize, DWORD* pdwTokenID, WORD wTokenCount);

  //服务管理
public:
    //启动服务
    bool StartService();
    //终止服务
    bool ConcludeService();

  //服务函数
public:
    //服务信息
    void SetLoggerServer(LPCTSTR pszServiceHost, WORD wServicePort);

  //发送数据
public:
    //发送数据
    virtual bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);
};

///////////////////////////////////////////////////////////////////////////////////////////////
//按时间写日志类
class LOGGER_SERVICE_CLASS CTimeWriteT
{
    //标志变量
protected:
    bool                  m_bLocal;                 //本地日志
    bool                  m_bRemote;                //远程日志
    bool                  m_bHasSufix;              //后缀标志
    bool                  m_bHasPrefix;             //前缀标志

    //配置变量
protected:
    TCHAR                 m_szAppName[32];           //应用名称
    TCHAR                 m_szServiceName[32];       //服务名称    
    TCHAR                 m_szFileName[128];         //文件名称 
    TCHAR                 m_szFormat[16];            //时间格式
    TCHAR                 m_szLogPath[MAX_PATH];     //文件路径
    TCHAR                 m_szConcatStr[4];          //连接字符
    TCHAR                 m_szLogType[16];           //日志类型
    TCHAR                 m_szSeparator[16];         //分割字符

    //缓冲变量
protected:
    CStringArray          m_strBuffer;              //缓冲日志

    //通信接口
protected:
    ILoggerSocket*        m_pILoggerSocket;          //通信接口

  //函数定义
public:
    //构造函数
    CTimeWriteT();
    //析构函数
    virtual ~CTimeWriteT();

  //配置函数
public:
    //通信接口
    void SetLoggerSocket(ILoggerSocket* pILoggerSocket);
    //日志信息
    void SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszLogPath, LPCTSTR pszFormat, LPCTSTR pszLogType = TEXT(""));

  //使能函数
public:
    //本地日志开关
    void EnableLocal(bool bEnable) { m_bLocal = bEnable; }
    //远程日志开关
    void EnableRemote(bool bEnable) { m_bRemote = bEnable; }    
    //后缀开关
    void EnableSufix(bool bEnable = true) { m_bHasSufix = bEnable; }  
    //前缀开关
    void EnablePrefix(bool bEnable = true) { m_bHasPrefix = bEnable; } 

  //配置函数
public:
    //设置时间格式
    void SetFormat(LPCTSTR pszFormat);
    //设置分隔符
    void SetSeparator(LPCTSTR pszSeparator);
    //连接符
    void SetConcatStr(LPCTSTR pszConcatStr);  

  //重载函数
public:
    //调用函数
    void operator()(CLoggerFile& LoggerFile, const CStringArray & buffer);
};


///////////////////////////////////////////////////////////////////////////////////////////////
//日志服务类
class LOGGER_SERVICE_CLASS CTimeLoggerService : public CSingleton<CTimeLoggerService, CreateUsingNew, DefaultLifetime>
{
  //类型定义
public:  
    typedef CLogger<CTimeWriteT, CRollByTime> CTimeLogger;    
    typedef CMap<CString, LPCTSTR, CTimeLogger*, CTimeLogger*> CMapLogger;

  //状态变量
protected:
    bool                  m_bService;               //服务状态

  //标志变量
protected:
    bool                  m_bLocal;                 //本地日志
    bool                  m_bRemote;                //远程日志
    bool                  m_bHasSufix;              //后缀标志
    bool                  m_bHasPrefix;             //前缀标志
  
  //配置变量
protected:
    TCHAR                 m_szAppName[32];           //应用名称
    TCHAR                 m_szServiceName[32];       //服务名称    
    TCHAR                 m_szFileName[128];         //文件名称 
    TCHAR                 m_szFormat[16];            //时间格式
    TCHAR                 m_szLogPath[MAX_PATH];     //文件路径
    TCHAR                 m_szConcatStr[4];          //连接字符
    TCHAR                 m_szLogType[16];           //日志类型
    TCHAR                 m_szSeparator[16];         //分割字符

  //变量定义
protected:
    std::mutex			  m_Mutex;					//互斥对象
    CMapLogger            m_MapLoggers;             //日志字典
    CTimeLogger*          m_DefaultLogger;          //缺省日志   

  //组件变量
protected:
    CLoggerThread         m_LocalThread;             //本地线程
    CLoggerThread         m_RemoteThread;            //远程线程
    CLoggerSocket         m_LoggerSocket;            //日志连接

  //函数定义
public:
    //构造函数
    CTimeLoggerService();
    //析构函数
    virtual ~CTimeLoggerService();

  //使能函数
public:
    //本地日志开关
    void EnableLocal(bool bEnable) { m_bLocal = bEnable; }
    //远程日志开关
    void EnableRemote(bool bEnable) { m_bRemote = bEnable; }
    //后缀开关
    void EnableSufix(bool bEnable = true) { m_bHasSufix = bEnable; }
    //前缀开关
    void EnablePrefix(bool bEnable = true) { m_bHasPrefix = bEnable; }

  //使能函数
public:
    //本地开关
    void EnableLocalEx(LPCTSTR pszFileName, bool bEnable);
    //本地开关
    void EnableLocalEx(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, bool bEnable);
    //远程开关
    void EnableRemoteEx(LPCTSTR pszFileName, bool bEnable);
    //远程开关
    void EnableRemoteEx(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, bool bEnable);  

  //配置函数
public:
    //设置时间格式
    void SetFormat(LPCTSTR pszFormat);
    //设置分隔符
    void SetSeparator(LPCTSTR pszSeparator);
    //连接符
    void SetConcatStr(LPCTSTR pszConcatStr);

  //服务函数
public:
    //启动服务
    bool StartService();
    //停止服务
    bool ConcludeService();

  //配置函数
public:
    //日志信息
    void SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszLogPath);  
    //日志服务
    void SetLoggerServer(LPCTSTR pszServiceHost, WORD wServicePort);

  //初始化函数
public:
    //初始化格式
    void InitFormat(LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr = NULL);
    //初始化格式
    void InitFormat(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr = NULL);

  //模板函数
public:
    //初始化格式
    template<class TLogType>
    void InitFormatWithType(LPCTSTR pszFileName, LPCTSTR pszFormat, size_t frequency)
    {
        CLogTypePtr logTypePtr = new TLogType(pszFormat, frequency);
        InitFormat(pszFileName, pszFormat, logTypePtr);
    }

    //初始化格式
    template<class TLogType>
    void InitFormatWithType(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, size_t frequency)
    {
        CLogTypePtr logTypePtr = new TLogType(pszFormat, frequency);
        InitFormat(pszAppName, pszServiceName, pszFileName, pszFormat, logTypePtr);
    }

  //创建日志
public:
    //获取时间日志
    CTimeLogger* Logger(LPCTSTR pszFileName = TEXT(""));
    //获取时间日志
    CTimeLogger* Logger(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName);

  //查找日志
public:
    //存在判断
    bool LoggerExists(LPCTSTR pszFileName);
    //存在判断
    bool LoggerExists(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName);

  //辅助函数
protected:
    //初始化时间日志
    void InitTimeLogger(CTimeLogger* pTimeLogger, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr = NULL);
    //初始化时间日志
    void InitTimeLogger(CTimeLogger* pTimeLogger, LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr = NULL);

  //辅助函数
protected:
    //清理日志
    void CleanLoggers();
};


///////////////////////////////////////////////////////////////////////////////////////////////
//日志服务类
class LOGGER_SERVICE_CLASS CRollLoggerService : public CSingleton<CRollLoggerService, CreateUsingNew, DefaultLifetime>
{
    //类型定义
    typedef CLogger<CWriteT, CRollBySize> CRollLogger;
    typedef CMap<CString, LPCTSTR, CRollLogger*, CRollLogger*> CMapLogger;

  //状态变量
protected:
    bool                  m_bService;              //服务状态

  //配置变量
protected:
    int                   m_wMaxNum;               //最大数量
    int                   m_dwMaxSize;              //最大大小   
    TCHAR                  m_szAppName[32];            //应用名称
    TCHAR                  m_szServiceName[32];          //服务名称    
    TCHAR                  m_szLogPath[128];           //日志路径

  //变量定义
protected:
    std::mutex					      m_Mutex;						    //互斥对象
    CMapLogger               m_MapLoggers;             //日志字典  
    CRollLogger*              m_DefaultLogger;            //默认日志

  //组件变量
protected:
    CLoggerThread              m_LocalThread;             //本地线程  

  //函数定义
public:
    //构造函数
    CRollLoggerService();
    //析构函数
    virtual ~CRollLoggerService();  

  //服务函数
public:
    //启动服务
    bool StartService();
    //停止服务
    bool ConcludeService();

  //配置函数
public:
    //日志信息
    void SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszLogPath, int dwMaxSize = 1024 * 1024 * 50, int wMaxNum = 10);  

  //创建日志
public:
    //获取滚动日志
    CRollLogger* Logger(LPCTSTR pszFileName=NULL);
    //获取滚动日志
    CRollLogger* Logger(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName);

  //辅助函数
protected:
    //初始化时间日志
    void InitRollLogger(CRollLogger* pRollLogger, LPCTSTR pszFileName, int dwMaxSize, int wMaxNum);
    //初始化时间日志
    void InitRollLogger(CRollLogger* pRollLogger, LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, int dwMaxSize, int wMaxNum);
};

///////////////////////////////////////////////////////////////////////////////////////////////
//日志实例
#define g_TimeLoggerService  (CTimeLoggerService::getInstance())                   
#define g_RollLoggerService  (CRollLoggerService::getInstance())                   

//日志宏
#define TLOG         (CTimeLoggerService::getInstance()->Logger())                   
#define FTLOG(x)       (CTimeLoggerService::getInstance()->Logger(x))
#define FFTLOG(x,y,z)     (CTimeLoggerService::getInstance()->Logger(x,y,z))

///////////////////////////////////////////////////////////////////////////////////////////////

#endif

