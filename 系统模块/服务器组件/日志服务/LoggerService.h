#ifndef REMOTE_LOOGER_SERVICE_HEAD_FILE
#define REMOTE_LOOGER_SERVICE_HEAD_FILE
#pragma once

#include "Logger.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//��־����
interface ILoggerSocket
{
  //��������
  virtual bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)=NULL;
};

///////////////////////////////////////////////////////////////////////////////////////////////

//��־����
class LOGGER_SERVICE_CLASS CLoggerSocket : public ILoggerSocket, public ITCPSocketEvent
{
  //״̬����
protected:
    bool							m_bService;							//�����־
    bool							m_bConnected;						//���ӱ�ʶ

  //������Ϣ
protected:
    WORD							m_wServicePort;						//����˿�
    TCHAR							m_szServiceHost[LEN_DOMAIN];		//�����ַ	

  //�������
protected:
    CTCPSocketServiceHelper			m_TCPSocketService;			        //�������

  //��������
public:
    //���캯��
    CLoggerSocket();
    //��������
    virtual ~CLoggerSocket();

  //�����ӿ�
public:
    //�ͷŶ���
    virtual VOID Release() { return; }
    //�ӿڲ�ѯ
    virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //����ӿ�
protected:
    //�����¼�
    virtual bool OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode);
    //�ر��¼�
    virtual bool OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason);
    //��ȡ�¼�
    virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID* pData, WORD wDataSize, DWORD* pdwTokenID, WORD wTokenCount);

  //�������
public:
    //��������
    bool StartService();
    //��ֹ����
    bool ConcludeService();

  //������
public:
    //������Ϣ
    void SetLoggerServer(LPCTSTR pszServiceHost, WORD wServicePort);

  //��������
public:
    //��������
    virtual bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);
};

///////////////////////////////////////////////////////////////////////////////////////////////
//��ʱ��д��־��
class LOGGER_SERVICE_CLASS CTimeWriteT
{
    //��־����
protected:
    bool                  m_bLocal;                 //������־
    bool                  m_bRemote;                //Զ����־
    bool                  m_bHasSufix;              //��׺��־
    bool                  m_bHasPrefix;             //ǰ׺��־

    //���ñ���
protected:
    TCHAR                 m_szAppName[32];           //Ӧ������
    TCHAR                 m_szServiceName[32];       //��������    
    TCHAR                 m_szFileName[128];         //�ļ����� 
    TCHAR                 m_szFormat[16];            //ʱ���ʽ
    TCHAR                 m_szLogPath[MAX_PATH];     //�ļ�·��
    TCHAR                 m_szConcatStr[4];          //�����ַ�
    TCHAR                 m_szLogType[16];           //��־����
    TCHAR                 m_szSeparator[16];         //�ָ��ַ�

    //�������
protected:
    CStringArray          m_strBuffer;              //������־

    //ͨ�Žӿ�
protected:
    ILoggerSocket*        m_pILoggerSocket;          //ͨ�Žӿ�

  //��������
public:
    //���캯��
    CTimeWriteT();
    //��������
    virtual ~CTimeWriteT();

  //���ú���
public:
    //ͨ�Žӿ�
    void SetLoggerSocket(ILoggerSocket* pILoggerSocket);
    //��־��Ϣ
    void SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszLogPath, LPCTSTR pszFormat, LPCTSTR pszLogType = TEXT(""));

  //ʹ�ܺ���
public:
    //������־����
    void EnableLocal(bool bEnable) { m_bLocal = bEnable; }
    //Զ����־����
    void EnableRemote(bool bEnable) { m_bRemote = bEnable; }    
    //��׺����
    void EnableSufix(bool bEnable = true) { m_bHasSufix = bEnable; }  
    //ǰ׺����
    void EnablePrefix(bool bEnable = true) { m_bHasPrefix = bEnable; } 

  //���ú���
public:
    //����ʱ���ʽ
    void SetFormat(LPCTSTR pszFormat);
    //���÷ָ���
    void SetSeparator(LPCTSTR pszSeparator);
    //���ӷ�
    void SetConcatStr(LPCTSTR pszConcatStr);  

  //���غ���
public:
    //���ú���
    void operator()(CLoggerFile& LoggerFile, const CStringArray & buffer);
};


///////////////////////////////////////////////////////////////////////////////////////////////
//��־������
class LOGGER_SERVICE_CLASS CTimeLoggerService : public CSingleton<CTimeLoggerService, CreateUsingNew, DefaultLifetime>
{
  //���Ͷ���
public:  
    typedef CLogger<CTimeWriteT, CRollByTime> CTimeLogger;    
    typedef CMap<CString, LPCTSTR, CTimeLogger*, CTimeLogger*> CMapLogger;

  //״̬����
protected:
    bool                  m_bService;               //����״̬

  //��־����
protected:
    bool                  m_bLocal;                 //������־
    bool                  m_bRemote;                //Զ����־
    bool                  m_bHasSufix;              //��׺��־
    bool                  m_bHasPrefix;             //ǰ׺��־
  
  //���ñ���
protected:
    TCHAR                 m_szAppName[32];           //Ӧ������
    TCHAR                 m_szServiceName[32];       //��������    
    TCHAR                 m_szFileName[128];         //�ļ����� 
    TCHAR                 m_szFormat[16];            //ʱ���ʽ
    TCHAR                 m_szLogPath[MAX_PATH];     //�ļ�·��
    TCHAR                 m_szConcatStr[4];          //�����ַ�
    TCHAR                 m_szLogType[16];           //��־����
    TCHAR                 m_szSeparator[16];         //�ָ��ַ�

  //��������
protected:
    std::mutex			  m_Mutex;					//�������
    CMapLogger            m_MapLoggers;             //��־�ֵ�
    CTimeLogger*          m_DefaultLogger;          //ȱʡ��־   

  //�������
protected:
    CLoggerThread         m_LocalThread;             //�����߳�
    CLoggerThread         m_RemoteThread;            //Զ���߳�
    CLoggerSocket         m_LoggerSocket;            //��־����

  //��������
public:
    //���캯��
    CTimeLoggerService();
    //��������
    virtual ~CTimeLoggerService();

  //ʹ�ܺ���
public:
    //������־����
    void EnableLocal(bool bEnable) { m_bLocal = bEnable; }
    //Զ����־����
    void EnableRemote(bool bEnable) { m_bRemote = bEnable; }
    //��׺����
    void EnableSufix(bool bEnable = true) { m_bHasSufix = bEnable; }
    //ǰ׺����
    void EnablePrefix(bool bEnable = true) { m_bHasPrefix = bEnable; }

  //ʹ�ܺ���
public:
    //���ؿ���
    void EnableLocalEx(LPCTSTR pszFileName, bool bEnable);
    //���ؿ���
    void EnableLocalEx(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, bool bEnable);
    //Զ�̿���
    void EnableRemoteEx(LPCTSTR pszFileName, bool bEnable);
    //Զ�̿���
    void EnableRemoteEx(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, bool bEnable);  

  //���ú���
public:
    //����ʱ���ʽ
    void SetFormat(LPCTSTR pszFormat);
    //���÷ָ���
    void SetSeparator(LPCTSTR pszSeparator);
    //���ӷ�
    void SetConcatStr(LPCTSTR pszConcatStr);

  //������
public:
    //��������
    bool StartService();
    //ֹͣ����
    bool ConcludeService();

  //���ú���
public:
    //��־��Ϣ
    void SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszLogPath);  
    //��־����
    void SetLoggerServer(LPCTSTR pszServiceHost, WORD wServicePort);

  //��ʼ������
public:
    //��ʼ����ʽ
    void InitFormat(LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr = NULL);
    //��ʼ����ʽ
    void InitFormat(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr = NULL);

  //ģ�庯��
public:
    //��ʼ����ʽ
    template<class TLogType>
    void InitFormatWithType(LPCTSTR pszFileName, LPCTSTR pszFormat, size_t frequency)
    {
        CLogTypePtr logTypePtr = new TLogType(pszFormat, frequency);
        InitFormat(pszFileName, pszFormat, logTypePtr);
    }

    //��ʼ����ʽ
    template<class TLogType>
    void InitFormatWithType(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, size_t frequency)
    {
        CLogTypePtr logTypePtr = new TLogType(pszFormat, frequency);
        InitFormat(pszAppName, pszServiceName, pszFileName, pszFormat, logTypePtr);
    }

  //������־
public:
    //��ȡʱ����־
    CTimeLogger* Logger(LPCTSTR pszFileName = TEXT(""));
    //��ȡʱ����־
    CTimeLogger* Logger(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName);

  //������־
public:
    //�����ж�
    bool LoggerExists(LPCTSTR pszFileName);
    //�����ж�
    bool LoggerExists(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName);

  //��������
protected:
    //��ʼ��ʱ����־
    void InitTimeLogger(CTimeLogger* pTimeLogger, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr = NULL);
    //��ʼ��ʱ����־
    void InitTimeLogger(CTimeLogger* pTimeLogger, LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr = NULL);

  //��������
protected:
    //������־
    void CleanLoggers();
};


///////////////////////////////////////////////////////////////////////////////////////////////
//��־������
class LOGGER_SERVICE_CLASS CRollLoggerService : public CSingleton<CRollLoggerService, CreateUsingNew, DefaultLifetime>
{
    //���Ͷ���
    typedef CLogger<CWriteT, CRollBySize> CRollLogger;
    typedef CMap<CString, LPCTSTR, CRollLogger*, CRollLogger*> CMapLogger;

  //״̬����
protected:
    bool                  m_bService;              //����״̬

  //���ñ���
protected:
    int                   m_wMaxNum;               //�������
    int                   m_dwMaxSize;              //����С   
    TCHAR                  m_szAppName[32];            //Ӧ������
    TCHAR                  m_szServiceName[32];          //��������    
    TCHAR                  m_szLogPath[128];           //��־·��

  //��������
protected:
    std::mutex					      m_Mutex;						    //�������
    CMapLogger               m_MapLoggers;             //��־�ֵ�  
    CRollLogger*              m_DefaultLogger;            //Ĭ����־

  //�������
protected:
    CLoggerThread              m_LocalThread;             //�����߳�  

  //��������
public:
    //���캯��
    CRollLoggerService();
    //��������
    virtual ~CRollLoggerService();  

  //������
public:
    //��������
    bool StartService();
    //ֹͣ����
    bool ConcludeService();

  //���ú���
public:
    //��־��Ϣ
    void SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszLogPath, int dwMaxSize = 1024 * 1024 * 50, int wMaxNum = 10);  

  //������־
public:
    //��ȡ������־
    CRollLogger* Logger(LPCTSTR pszFileName=NULL);
    //��ȡ������־
    CRollLogger* Logger(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName);

  //��������
protected:
    //��ʼ��ʱ����־
    void InitRollLogger(CRollLogger* pRollLogger, LPCTSTR pszFileName, int dwMaxSize, int wMaxNum);
    //��ʼ��ʱ����־
    void InitRollLogger(CRollLogger* pRollLogger, LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, int dwMaxSize, int wMaxNum);
};

///////////////////////////////////////////////////////////////////////////////////////////////
//��־ʵ��
#define g_TimeLoggerService  (CTimeLoggerService::getInstance())                   
#define g_RollLoggerService  (CRollLoggerService::getInstance())                   

//��־��
#define TLOG         (CTimeLoggerService::getInstance()->Logger())                   
#define FTLOG(x)       (CTimeLoggerService::getInstance()->Logger(x))
#define FFTLOG(x,y,z)     (CTimeLoggerService::getInstance()->Logger(x,y,z))

///////////////////////////////////////////////////////////////////////////////////////////////

#endif

