#ifndef IPC_MONITOR_SERVER_HEAD_FILE
#define IPC_MONITOR_SERVER_HEAD_FILE


//////////////////////////////////////////////////////////////////////////
//��������
#define MONITOR_CLASS_NAME			 TEXT("WND_MONITOR_#65478")

//////////////////////////////////////////////////////////////////////////
//�����Ϣ

#define IPC_CMD_MS_MONITOR			10									//�����Ϣ

#define IPC_SUB_MS_PROCESS_START	1									//��������
#define IPC_SUB_MS_PROCESS_CLOSE	2									//���̹ر�
#define IPC_SUB_MS_SERVICE_READY	3									//����׼��
#define IPC_SUB_MS_SERVICE_STATUS	4									//����״̬
#define IPC_SUB_MS_DATABASE_EXCEPTION 5									//���ݿ��쳣

#define IPC_SUB_MS_STOP_SERVICE		100									//ֹͣ����
#define IPC_SUB_MS_START_SERVICE	101									//��������
#define IPC_SUB_MS_CLOSE_PROCESS	102									//�رս���

//////////////////////////////////////////////////////////////////////////
//��������
struct CMD_MS_ProcessStart
{
	DWORD							dwProcessID;						//���̱�ʶ	
};

//���̹ر�
struct CMD_MS_ProcessClose
{
	DWORD							dwProcessID;						//���̱�ʶ	
};

//����׼��
struct CMD_MS_ServiceReady
{
	DWORD							dwProcessID;						//���̱�ʶ
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
	WORD							wServiceStatus;						//����״̬
	TCHAR							szServiceName[32];					//��������
};

//����״̬
struct CMD_MS_ServiceStatus
{
	DWORD							dwProcessID;						//���̱�ʶ
	WORD							wServiceStatus;						//����״̬
};

//���ݿ��쳣
struct CMD_MS_DataBaseException
{
	DWORD							dwProcessID;						//���̱�ʶ
	TCHAR							szExceptionInfo[256];				//�쳣��Ϣ
};

//////////////////////////////////////////////////////////////////////////

#endif