#ifndef CMD_LOGGER_SERVER_HEAD_FILE
#define CMD_LOGGER_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//��������
#define MDM_LS_LOGGER_SERVICE		10										//��������

//////////////////////////////////////////////////////////////////////////////////
//��������
#define SUB_LC_WRITE_LOGGER			100										//д����־

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//��־��Ϣ
struct tagTimeLogInfo
{
	//��־����
	bool							bHasSufix;								//��׺��־
	bool							bHasPrefix;								//ǰ׺��־

	//���ñ���
	TCHAR							szAppName[16];							//Ӧ������
	TCHAR							szServiceName[16];						//��������
	TCHAR							szFileName[128];						//�ļ�����
	TCHAR							szFormat[32];							//ʱ���ʽ
	TCHAR							szConcatStr[4];							//�����ַ�
	TCHAR							szSeparator[4];							//�ָ��ַ�
	TCHAR							szLogType[16];							//��־����
};

//д����־
struct CMD_LC_WriteTimeLogger : public tagTimeLogInfo
{

};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif