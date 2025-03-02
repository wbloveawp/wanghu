
#ifndef SQLITE_HEAD_FILE
#define SQLITE_HEAD_FILE

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////////////

#define	CONTROL_CNT						200					//���ƾ���
//������Ϣ
struct tagNewUserControl
{
	DWORD							dwUserId;							 //�û�ID
	DWORD							dwGameId;							 //�û�ID
	WORD							wKindID;						//��ϷID
	BYTE							cbIndex;						//�������е���λ��
	WORD							wRebateRatio;					//�����ʣ�%��
	DWORD							wSelectSampleIndex;				//ѡ����������
	WORD							wTimesCount;					//������Ŀ
	WORD							wTimesList[CONTROL_CNT];		//�����б�
};

//�����洢
struct tagSampleData
{
	int								dwID;							//��ʶID
	WORD							wKindID;						//��ϷID
	CHAR							cbBuffer[512];					//��������
};
//////////////////////////////////////////////////////////////////////////////////

static int  CB_sqlite_count(void* data, int argc, char** argv, char** pszColName)
{
	for (int i = 0; i < argc; i++)
	{
		if (argv == NULL)
		{
			return 0;
		}
		*((int*)data) = atoi(argv[i]);
	}
	return 0;
}

//���
struct tagInventoryInfo
{
	INT								nKindID;							//��ϷID
	INT								nDebugType;							//��������
	INT								nGameID;							//�û�ID/����

	SCORE							lConfigSysStorage;					//����ϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							lConfigPlayerStorage;				//������ҿ��(���ҪӮ��Ǯ)
	INT								lConfigParameterK;					//���õ���ϵ��(ǧ�ֱ�)
	INT								lConfigResetSection;				//ǿ����������(ǧ�ֱ�)

	SCORE							lCurSysStorage;						//��ǰϵͳ���(ϵͳҪӮ��Ǯ)
	SCORE							lCurPlayerStorage;					//��ǰ��ҿ��(���ҪӮ��Ǯ)
	INT								lCurParameterK;						//��ǰ����ϵ��(�ٷֱ�):
	INT								lCurResetSection;					//ǿ����������(�ٷֱ�)

	SCORE							lSysDbgSysWin;						//ϵͳ��Ӯ����
	SCORE							lSysDbgPlayerWin;					//�����Ӯ����
	INT								lWinRatio;							//��ǰ��Ӯ����
	INT								lResetTimes;						//���ô���
};

#pragma pack()

#endif