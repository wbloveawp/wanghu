#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

//��Ϸ����
struct tagGameType
{
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wTypeID;							//��������
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//��Ϸ����
struct tagGameKind
{
	WORD							wModuleID;							//���ͱ�ʶ
	WORD							wTypeID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	DWORD							dwOnLineCount;						//��������
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
	TCHAR							szProcessName[LEN_PROCESS];			//��������
};

//��������
struct tagServiceAttrib
{
	WORD							wServiceModule;						//����ģ��
	TCHAR							szServiceName[32];					//��������
	bool							bIsStateful;						//״̬��ʾ
	bool							bIsExternal;						//�����ʾ
	WORD							dwMaxInstanceNum;					//ʵ����Ŀ
};

//��������
struct tagServiceItem
{
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
	WORD							wServicePort;						//����˿�
	WORD							wMaxConnect;						//�������
	DWORD							dwServiceAddr;						//�����ַ
	WORD							wServiceKindID;						//���ͱ�ʶ
	TCHAR							szServiceName[LEN_SERVER];			//��������
	TCHAR							szServiceDomain[LEN_DOMAIN];		//�����ַ		
};

//��������
struct tagServiceOption
{
	//������Ϣ
	WORD							wServiceID;							//�����ʶ
	WORD							wServicePort;						//����˿�
	WORD							wMaxConnect;						//�������
	TCHAR							szServiceName[LEN_SERVER];			//��������
	TCHAR							szServiceDomain[LEN_DOMAIN];		//�����ַ	

	//��չ��Ϣ
	WORD							wServiceKindID;						//���ͱ�ʶ
	TCHAR							szServiceKindName[LEN_SERVER];		//��������
};

//��������
struct tagServiceKind
{
	//��չ��Ϣ
	WORD							wServiceKindID;						//���ͱ�ʶ
	TCHAR							szServiceKindName[LEN_SERVER];		//��������
};

//��Ϸ����
struct tagGameKindEx : public tagGameKind
{
	WORD							wSupportMask;						//֧������	
};

//��Ϸ�ڵ�
struct tagGameNode
{
	WORD							wKindID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wNodeID;							//�ڵ�����
	TCHAR							szNodeName[LEN_NODE];				//�ڵ�����
};

//������Ϣ
struct tagSocketInfo
{
	WORD							wGateID;							//���ر�ʶ
	DWORD							dwTokenID;							//���Ʊ�ʶ
};

//��Ϸ����
struct tagGameServer
{
	WORD							wModuleID;							//���ͱ�ʶ
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wVisibleMask;						//��������
	WORD							wServerID;							//��������
	WORD							wServerPort;						//����˿�
	WORD							wServerType;						//��������
	WORD							wServerLevel;						//����ȼ�
	DWORD							dwMaxPlayer;						//�������
	DWORD							dwOnLineCount;						//��������
	DWORD							dwServerRule;						//�������
	DWORD							dwServerAddr;						//�����ַ
	DWORD							dwMasterRule;						//�������			
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR							szServerDomain[LEN_DOMAIN];			//��������

	//��������
	SCORE							lCellScore;							//��λ����	
	SCORE							lMinEnterScore;						//��ͽ���
	SCORE							lMaxEnterScore;						//��߽���
};


//�ṹ����
struct tagPCGameServer
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wServerID;							//��������
	WORD							wServerPort;						//����˿�
	WORD							wServerType;						//��������
	DWORD							dwOnLineCount;						//��������
	DWORD							dwServerAddrDX;						//���ŵ�ַ
	DWORD							dwServerAddrWT;						//��ͨ��ַ
	DWORD							dwServerAddrTT;						//��ͨ��ַ
	TCHAR							szServerName[LEN_SERVER];			//��������

	//��������
	SCORE							lCellScore;							//��λ����	
	SCORE							lMinEnterScore;						//��ͽ���
	SCORE							lMaxEnterScore;						//��߽���
};

//�ṹ����
struct tagMobileGameServer
{
	WORD							wKindID;							//��������
	WORD							wSortID;							//��������
	WORD							wServerID;							//��������
	WORD							wServerPort;						//����˿�
	WORD							wServerType;						//��������
	WORD							wServerLevel;						//����ȼ�
	DWORD							dwMaxPlayer;						//�������
	DWORD							dwOnLineCount;						//��������
	DWORD							dwServerAddr;						//���ŵ�ַ	
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR							szServerDomain[LEN_DOMAIN];			//��������

	//��������
	SCORE							lCellScore;							//��λ����	
	SCORE							lMinEnterScore;						//��ͽ���
	SCORE							lMaxEnterScore;						//��߽���
};

//ģ��汾
struct tagModuleVersion
{
	bool							IsCurrent;							//��ǰ�汾
	DWORD							dwDeviceKind;						//�豸����
	WORD							wModuleID;							//ģ���ʶ	
	DWORD							dwVersion;							//�ն˰汾
};


//��������
struct tagGameCustom
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wPageID;							//��������
	WORD							wOperateType;						//��������
	TCHAR							szDisplayName[LEN_CUSTOM];			//��������
	TCHAR							szReponseUrl[256];					//��ת��ַ
};

//��������
struct tagGameFilter
{
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wServerID;							//��������
	WORD							wFilterMask;						//��������
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//������Ϣ
struct tagOnLineInfoKind
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwOnLineCount;						//��������
};

//������Ϣ
struct tagOnLineInfoServer
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwOnLineCount;						//��������
};

//�û���Ϣ
struct tagGlobalUserInfo
{
	//�û�����
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

	//״̬��Ϣ
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	DWORD							dwTokenID;							//���Ʊ�ʶ
};

///////////////////////////////////////////////////////////////////////////////////////////////
//��Ϣ����

//��Ϣ����
struct tagMessageItem
{
	WORD							wMessageID;							//��Ϣ��ʶ
	WORD							wServicePort;						//����˿�
	TCHAR							szServiceAddr[LEN_DOMAIN];			//�����ַ	
};

//��������
struct tagNoticeRollOption
{
	bool							bExclusive;						//��ռ��ʶ
	BYTE							cbPriority;						//���ȼ���
	WORD							wRollTimes;						//��������	
	WORD							wViewPlace;						//��ʾλ��
	BYTE							cbNoticeKind;					//��������
	DWORD							dwDurationTime;					//����ʱ��
};

//������Ϣ
struct tagMarqueeNoticeInfo
{
	//������Ϣ
	DWORD							dwNoticeID;						//�����ʶ
	BYTE							cbNoticeKind;					//��������

	//����ѡ��
	bool							bExclusive;						//��ռ��ʶ
	BYTE							cbPriority;						//���ȼ���		
	WORD							wViewPlace;						//��ʾλ��
	WORD							wRollTimes;						//��������	
	DWORD							dwEndRollTime;					//����ʱ��
	DWORD							dwStartRollTime;				//��ʼʱ��			

	//��������
	WORD							wNoticeSize;					//�����С
	BYTE							cbNoticeContent[512];			//��������	
};

//ϵͳ�๫��
struct tagNoticeKind_System
{
	TCHAR							szNoticeContent[256];			//��������
};

//��¼�๫��
struct tagNoticeKind_UserLogon
{
	WORD							wRankID;						//������ʶ
	BYTE							cbListType;						//������
	TCHAR							szNickName[LEN_NICKNAME];		//�û��ǳ�
};

//��Ϸ��ʤ�๫��
struct tagNoticeKind_BigWinner
{
	SCORE							lWinScore;						//Ӯȡ����
	TCHAR							szGameKind[LEN_KIND];			//��Ϸ����	
	TCHAR							szNickName[LEN_NICKNAME];		//�û��ǳ�	
};

//�����н��๫��
struct tagNoticeKind_PrizePool
{
	SCORE							lAwardAmount;					//�������	
	TCHAR							szGameKind[LEN_KIND];			//��Ϸ����	
	TCHAR							szNickName[LEN_NICKNAME];		//�û��ǳ�	
};

///////////////////////////////////////////////////////////////////////////////////////////////
//��������

//��������
struct tagMatchRankItem
{
	WORD							wRankID;							//���α�ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	SCORE							lMatchScore;						//��������
	TCHAR							szNickName[LEN_ACCOUNTS];			//����ǳ�
};

//�����û�
struct tagMatchUserItem
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wServerID;							//�����ʶ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ�ܴ���
	LONG							lMaxWinStreak;						//��ʤ����
	SCORE							lMatchScore;						//��������
	WORD							wSignupCount;						//��������
	DWORD							dwSignupTime;						//����ʱ��
};

//�����û�
struct tagSignupUserItem
{
	DWORD							dwUserID;							//�����û�
	DWORD							dwSignupTime;						//����ʱ��
};

//�ִ�����
struct tagMatchRoundItem
{
	WORD							wRoundID;							//�ִα�ʶ
	WORD							wPlayCount;							//��Ϸ����
	WORD							wStartCount;						//��ʼ����		
	WORD							wStartMinCount;						//��ʼ����		
	WORD							wStartMaxCount;						//��ʼ����		

	//��������
	BYTE							cbMScoreSetType;					//���ַ�ʽ
	LONG							lInitMatchScore;					//��ʼ�ȷ�
	WORD							wReserveRatio;						//��������

	//��Ϸ�׷�
	LONG							lMaxBaseScore;						//��Ϸ�׷�
	LONG							lInitBaseScore;						//��Ϸ�׷�
	BYTE							cbIncreaseMode;						//����ģʽ
	WORD							wStartInning;						//��ʼ����
	WORD							wIntervalInning;					//�������
	WORD							wIntervalSecond;					//�������
	WORD							wIncreaseRatio;						//��������		
	
	//������Ϣ
	WORD							wPromoteCount;						//��������
	BYTE							cbPromoteMode;						//����ģʽ
	BYTE							cbPromoteRule[16];					//��������
};

//��ʱ��
struct tagLockTimeMatch
{
	//��������
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchCutOffTime;					//��ֹʱ��

	//������ʽ
	BYTE							cbMatchMode;						//����ģʽ	
	BYTE							cbMatchCycle;						//��������
	BYTE							cbDayFlagList[MAX_DAY];				//��ʶ�б�	
	SYSTEMTIME						MatchLockTime;						//����ʱ��
	SYSTEMTIME						MatchInterval;						//�������
	SYSTEMTIME						MatchSignupTime;					//����ʱ��

	//��Ϸ����
	WORD							wMinPlayerCount;					//��Ϸ����
	WORD							wMaxPlayerCount;					//��Ϸ����

	//��������	
	WORD							wFeeMoneyID;						//��������
	LONG							lFeeAmount;							//���ý��
	LONG							lReviveMulti;						//�����
	BYTE							cbReviveTimes;						//�������	
};

//��ʱ��
struct tagImmediateMatch
{
	//��������
	WORD							wMinPlayerCount;					//��Ϸ����
	WORD							wMaxPlayerCount;					//��Ϸ����

	//����ʱ��
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchCutOffTime;					//��ֹʱ��
};

//������
struct tagTournamentMatch
{
	//��������
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchCutOffTime;					//��ֹʱ��

	//������ʽ
	BYTE							cbMatchMode;						//����ģʽ
	BYTE							cbMatchCycle;						//��������
	BYTE							cbDayFlagList[MAX_DAY];				//��ʶ�б�		
	SYSTEMTIME						MatchLockTime;						//����ʱ��
	SYSTEMTIME						MatchInterval;						//�������
	SYSTEMTIME						MatchDurationTime;					//����ʱ��
	SYSTEMTIME						MatchSaleTicketTime;				//��Ʊʱ��
	SYSTEMTIME						MatchSignupStart;					//������ʼ
	SYSTEMTIME						MatchSignupCutOff;					//������ֹ

	//��������
	WORD							wPlayCount;							//��Ϸ����
	WORD							wMaxTableCount;						//�������
	SCORE							lInitBaseScore;						//��ʼ�׷�
	SCORE							lInitMatchScore;					//��ʼ����
	WORD							wMinPlayerCount;					//��Ϸ����
	WORD							wMaxPlayerCount;					//��Ϸ����

	//������ʽ
	BYTE							cbMonthFlagList[MAX_MONTH];			//��ʶ�б�
};

//��ʱ������
struct tagLockTimeGroupDetail
{
	//ʱ�䶨��	
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchSignupStart;					//������ʼ
	SYSTEMTIME						MatchSignupCutOff;					//������ֹ
	SYSTEMTIME						ServerLocalTime;					//����ʱ��
};

//����������
struct tagTournamentGroupDetail
{
	//ʱ�䶨��	
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchSignupStart;					//������ʼ
	SYSTEMTIME						MatchSignupCutOff;					//������ֹ
	SYSTEMTIME						ServerLocalTime;					//����ʱ��
	SYSTEMTIME						MatchSaleTicketTime;				//��Ʊʱ��
};

//��ʱ������
struct tagImmediateGroupDetail
{

};

//��������
struct tagMatchRewardItem
{
	WORD							wEndRankID;							//��������
	WORD							wStartRankID;						//��������	
	//SCORE							lRewardGold;						//�������
	//SCORE							lRewardIngot;						//������ʯ
	//TCHAR							szRewardEntity[16];					//����ʵ��

	TCHAR							szRewardEntity[128];				//ʵ�ｱ��
	TCHAR							szRewardGoods[256];					//��Ʒ����
};

//��������
struct tagMatchOption
{
	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwMatchID;							//������ʶ	
	DWORD							dwGroupID;							//��ݱ�ʶ
	BYTE							cbShowType;							//չʾ����
	BYTE							cbMatchStatus;						//����״̬	
	BYTE							cbMatchType;						//��������		
	TCHAR							szMatchName[32];					//��������
	TCHAR							szModuleName[LEN_KIND];				//ģ������	
	
	//������Ϣ
	SCORE							lFeeAmount;							//���ý��
	WORD							wFeeMoneyID;						//���ұ�ʶ
	WORD							wUseTicketCount;					//��ȯ����
	WORD							wBuyTicketCount;					//��ȯ����
	SCORE							lBuyTicketAmount;					//���ý��
	WORD							wBuyTicketMoneyID;					//���ұ�ʶ
	WORD							wBuyTicketOption;					//��Ʊѡ��
	WORD							wSignupCount;						//��������
	BYTE							cbSignupMode;						//������ʽ
	BYTE							cbJoinCondition;					//��������
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	LONG							lExperience;						//��Ҿ���
	
	//��������
	WORD							wRewardCount;						//������Ŀ
	tagMatchRewardItem				MatchRewardItem[8];					//������Ϣ

	//�����ִ�
	WORD							wRoundCount;						//�ִ���Ŀ
	tagMatchRoundItem				MatchRoundItem[5];					//�ִ���Ϣ

	//��������
	BYTE							cbMatchRule[256];					//��������

	//��������
	BYTE							cbMatchDistribute[64];				//��������
};

//��������
struct tagMatchDistribute
{
	WORD							wDistributeRule;					//�������
	WORD							wDistributeMinUser;					//�����û�
	WORD							wDistributeInterval;				//�������		
	WORD							wMinPartakeGameUser;				//��������
	WORD							wMaxPartakeGameUser;				//�������
};

//��������
struct tagMatchSessionItem
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	BYTE							cbMatchStatus;						//����״̬

	//������Ϣ	
	WORD							wServerID;							//�����ʶ	
};

//������Ϣ
struct tagMatchViewOption
{
	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwGroupID;							//��ݱ�ʶ
	BYTE							cbShowType;							//չʾ����	
	BYTE							cbMatchType;						//��������	
	TCHAR							szMatchName[32];					//��������
	BYTE							cbOptionStatus;						//����״̬

	//������Ϣ
	SCORE							lFeeAmount;							//���ý��
	WORD							wFeeMoneyID;						//���ұ�ʶ
	WORD							wUseTicketCount;					//��ȯ����
	WORD							wBuyTicketCount;					//��ȯ����
	SCORE							lBuyTicketAmount;					//���ý��
	WORD							wBuyTicketMoneyID;					//���ұ�ʶ
	WORD							wSignupCount;						//��������
	BYTE							cbSignupMode;						//������ʽ
	BYTE							cbJoinCondition;					//��������
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	LONG							lExperience;						//��Ҿ���

	//����ʱ��
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchCutOffTime;					//��ֹʱ��

	//������Ϣ
	WORD							wRewardCount;						//������Ŀ
	tagMatchRewardItem				MatchRewardItem[8];					//��������
};

//��������
struct tagPromoteOutGame
{
	//WORD							wEliminateCount;					//��̭����	
	WORD							wEliminateMulti;					//��̭����
	WORD							wDistributeCount;					//��������
};

//���ֻ���
struct tagPromoteFixedGame
{
	WORD							wSwitchCount;						//��������
};

//������¼�û�����
struct tagMatchRecordUserItem
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wRankID;							//��������
	SCORE							lMatchScore;						//��������
	TCHAR							szRewardEntity[64];					//��������
	TCHAR							szRewardGoods[128];					//��������
};

//������¼����
struct tagMatchRecordItem
{
	SCORE							lMatchNo;							//��������
	DWORD							dwMatchID;							//������ʶ
	BYTE							cbShowType;							//չʾ����
	TCHAR							szMatchName[32];					//��������	
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchFinishTime;					//����ʱ��
	tagMatchRecordUserItem			MatchRecordUserItem;				//�Լ���¼
};


//����������Ϣ
struct tagMatchTableInfo
{
	WORD							wServerID;							//�����ʶ	
	WORD							wTableID;							//���ӱ�ʶ
	BYTE							cbGameStatus;						//��Ϸ״̬
	BYTE							cbTeamCount;						//�Ŷ�����
	SCORE							lTeamScore[4];						//�Ŷӻ���
	WORD							wUserCount;							//�û�����
	DWORD							dwUserIDList[4];					//��ʶ����					
};

//////////////////////////////////////////////////////////////////////////////////////

//Ⱥ����Ϣ
struct tagGroupInfo
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwCreaterID;						//������ʶ
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������
};

//������Ϣ
struct tagGameKindInfo
{
	WORD							wKindID;							//���ͱ�ʶ
	TCHAR							szKindName[LEN_KIND];				//��������
};

//��������
struct tagRobotParameter
{
	//���Ա���
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwServiceMode;						//����ģʽ
	DWORD							dwRobotCount;						//������Ŀ
	DWORD							dwEnterTime;						//����ʱ��
	DWORD							dwLeaveTime;						//�뿪ʱ��
	SCORE							lTakeMinScore;						//Я������
	SCORE							lTakeMaxScore;						//Я������
	DWORD							dwEnterMinInterval;					//������
	DWORD							dwEnterMaxInterval;					//������
	DWORD							dwLeaveMinInterval;					//�뿪���
	DWORD							dwLeaveMaxInterval;					//�뿪���	
	DWORD							dwSwitchMinInnings;					//��������
	DWORD							dwSwitchMaxInnings;					//��������		
};

//������Ϣ
struct tagRobotBatchConfig
{
	//���Ա���
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwEnterTime;						//����ʱ��
	DWORD							dwLeaveTime;						//�뿪ʱ��	
};

//Ⱥ���Ա��Ϣ
struct tagGroupMemberInfo
{
	DWORD							dwUserID;							//������ʶ	
	DWORD							dwGameID;							//��Ϸ��ʶ	
	DWORD							dwGroupID;							//Ⱥ���ʶ
	TCHAR							szNickName[LEN_ACCOUNTS];			//�����ǳ�
	WORD							wFaceID;							//ͼ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwCustomID;							//ͷ���ʶ
	DWORD							dwParentID;							//�ϼ���ʶ
	BYTE							cbMemberType;						//��Ա����	
};

//���γ�Ա��Ϣ
struct tagBatchMemberInfo
{
	//�˺���Ϣ
	DWORD							dwUserID;							//������ʶ	
	DWORD							dwBatchID;							//���α�ʶ

	//������Ϣ
	DWORD							dwEnterTime;						//����ʱ��
	DWORD							dwLeaveTime;						//�뿪ʱ��
	WORD							wPlayKindID[3];						//��������
	DWORD							dwSupplyTemplateID;					//����ģ��
	DWORD							dwActionTemplateID[3];				//��Ϊģ��	
};

//ģ������
struct tagActionTemplateData
{
	LONG							lScoreArea1[2];						//�׷ַ�Χ
	BYTE							cbModeRate1[4];						//ģʽ����
	LONG							lScoreArea2[2];						//�׷ַ�Χ
	BYTE							cbModeRate2[4];						//ģʽ����
	LONG							lScoreArea3[2];						//�׷ַ�Χ
	BYTE							cbModeRate3[4];						//ģʽ����
	LONG							lScoreArea4[2];						//�׷ַ�Χ
	BYTE							cbModeRate4[4];						//ģʽ����
	BYTE							cbModeData1[32];					//ģʽ����	
	BYTE							cbModeData2[32];					//ģʽ����	
	BYTE							cbModeData3[32];					//ģʽ����	
	BYTE							cbModeData4[32];					//ģʽ����		
};

//��Ϊģ��
struct tagRobotActionTemplate
{
	DWORD							dwTemplateID;						//ģ���ʶ
	WORD							wGameKindID;						//���ͱ�ʶ	
	TCHAR							szTemplateName[32];					//ģ������	
	TCHAR							szTemplateRemarks[128];				//ģ�屸ע
	tagActionTemplateData			ActionTemplateData;					//ģ������
};

//ģ������
struct tagSupplyTemplateData
{
	BYTE							cbSupplyRule1[64];					//��������
	BYTE							cbSupplyRule2[64];					//��������	
	BYTE							cbSupplyRule3[64];					//��������
	BYTE							cbSupplyRule4[64];					//��������	
};

//����ģ��
struct tagRobotSupplyTemplate
{
	DWORD							dwTemplateID;						//ģ���ʶ
	TCHAR							szTemplateName[32];					//ģ������	
	TCHAR							szTemplateRemarks[128];				//ģ�屸ע
	tagSupplyTemplateData			SupplyTemplateData;					//ģ������
};

//��������
struct tagReduceScoreItem
{
	LONG							lScore;								//ʣ�����
	BYTE							cbReduceRate;						//���ָ���
	LONG							lMinReduceScore;					//��С����
	LONG							lMaxReduceScore;					//������
};

//�ӷ�����
struct tagIncreaseScoreItem
{
	WORD							wWaitTime;							//�ȴ�ʱ��
	WORD							wScoreMultiple;						//�׷ֱ���	
	BYTE							cbIncreaseRate;						//�ӷָ���
	LONG							lMinIncreaseScore;					//��С����
	LONG							lMaxIncreaseScore;					//������
};

//////////////////////////////////////////////////////////////////////////
//��λ��Ϣ

//��������
struct tagRankingTableItem
{
	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wServerID;							//�����ʶ
	DWORD							dwRankingNum;						//��λ���		
	TCHAR							szPassword[LEN_PASSWORD];			//��������

	//������Ϣ	
	DWORD							dwCreateTime;						//����ʱ��	
};

//��������
struct tagRankingTableItemEx
{
	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wServerID;							//�����ʶ
	DWORD							dwRankingNum;						//��λ���	
	TCHAR							szPassword[LEN_PASSWORD];			//��������

	//������Ϣ	
	DWORD							dwCreateTime;						//����ʱ��	
};

//�����û�
struct tagRankingTableUser
{
	//�û���Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwCustomID;							//ͷ���ʶ
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�

	//��λ��Ϣ
	WORD							wRankOrder;							//��λ�ȼ�	
};

//��λ����
struct tagRankingConfig
{
	//��������
	WORD							wPlayCount;							//��Ϸ����
	WORD							wNotifyMask;						//֪ͨ����		
	LONG							lRankingCost;						//��λ����

	//ת������
	WORD							wWinConvertRate;					//ת������
	WORD							wLoseConvertRate;					//ת������

	//��������
	LONG							lRiseLoveHeartCount;				//��������
	LONG							lLostLoveHeartCount;				//ʧ������	
};

//��������
struct tagRankingCreateParam
{
	//������Ϣ
	DWORD							dwRankingNum;						//Լս���
	DWORD							dwCreateTime;						//����ʱ��
	TCHAR							szTablePassword[LEN_PASSWORD];		//��������

	//������Ϣ
	tagRankingConfig				RankingConfig;						//��λ����
};

//��������
struct tagRankingRankItem
{
    WORD							wRankID;							//���α�ʶ
    DWORD							dwUserID;							//�û���ʶ
    //DWORD							dwGameID;							//��Ϸ��ʶ
    SCORE							lScore;								//��λ����
    //TCHAR							szNickName[LEN_ACCOUNTS];			//����ǳ�
};

//////////////////////////////////////////////////////////////////////////
//Լս��Ϣ

//�û���Ϣ
struct tagGateUserInfo
{
	DWORD							dwUserID;							//�û���ʶ		
	DWORD							dwTokenID;							//���Ʊ�ʶ
};

//Լս��Ϸѡ��
struct tagBattleGameOption
{
	WORD							wKindID;							//���ͱ�ʶ	
	BYTE							cbEmployScene;						//ʹ�ó���
	TCHAR							szOptionJson[4000];					//��������													
};

//�Զ���ѡ��
struct tagBattleGameOptionEx
{
	tagBattleGameOption				BattleGameOption;					//��Ϸ����
};

//Լս��������
struct tagBattleBaseOption
{
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wPlayerCount;						//��Ϸ����
	BYTE							cbEmployScene;						//ʹ�ó���
	WORD							wOptionMask;						//��������
	SCORE							lMinEntryCost;						//�볡����
	SCORE							lMaxEntryCost;		 				//�볡����			
};

//Լսѡ��
struct tagBattleBaseOptionEx
{
	WORD							wOptionID;							//ѡ���ʶ
	tagBattleBaseOption				BattleOption;						//Լսѡ��
};

//Լս����
struct tagBattleConfig
{
	//��������
	WORD							wKindID;							//���ͱ�ʶ
	SCORE							lEntryCost;							//�볡����
	WORD							wPlayerCount;						//��Ϸ����	
	BYTE							cbEmployScene;						//ʹ�ó���			

	//ѡ������
	WORD							wCardBackID;						//�Ʊ���ʶ
	WORD							wTableBackID;						//������ʶ
	BYTE							cbAllowVoice;						//��������
	BYTE							cbAllowSameIP;						//����ͬIP
	BYTE							cbAllowLookon;						//�����Թ�
	BYTE							cbShowSeatMode;						//��λģʽ	

	//��Ϸ����
	BYTE							cbOptionSize;						//���ô�С
	BYTE							cbGameOption[64];					//��Ϸ����	
};

//��������
struct tagBattleConfigEx : public tagBattleConfig
{	
	//������Ϣ	
	SCORE							lWinScore;							//Ӯȡ����
	TCHAR							szPassword[LEN_PASSWORD];			//��������

	//ϵͳ����
	WORD							wPayType;							//֧������
	LONG							lBaseScore;							//�����׷�	
	WORD							wPlayCount;							//��Ϸ����
	WORD							wRevenueRate;						//�������	

	//������Ϣ
	WORD							wNotifyMask;						//֪ͨ����
	WORD							wBattleOptionID;					//���ñ�ʶ	
};

//��������
struct tagBattleCreateParam
{
	//������Ϣ	
	DWORD							dwOwnerID;							//�û���ʶ
	DWORD							dwBattleNum;						//Լս���
	DWORD							dwCreateTime;						//����ʱ��
	TCHAR							szTablePassword[LEN_PASSWORD];		//��������

	//������Ϣ	
	tagBattleConfigEx				BattleConfig;						//����ѡ��
};

//�޸Ĳ���
struct tagBattleModifyParam
{
	//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwBattleNum;						//Լս���

	//������Ϣ	
	tagBattleConfigEx				BattleConfig;						//��������
};

//�����û�
struct tagBattleTableUser
{
	//�û���Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	WORD							wChairID;							//�û���λ
	BYTE							cbGender;							//�û��Ա�
	WORD							wFaceFrameID;						//�������
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	DWORD							dwCustomID;							//ͷ���ʶ
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�

	//�ȼ���Ϣ
	WORD							wGameOrder;							//��Ϸ�ȼ�
	WORD							wMemberOrder;						//��Ա�ȼ�	
};

//���Ӳ���
struct tagBattleTableParam
{
	DWORD							dwBattleNum;						//Լս���	

	//�û���Ϣ
	WORD							wUserCount;							//�û�����
	tagBattleTableUser				TableUserItem[0];					//�û�����
};

//��������
struct tagBattleTableItem
{
	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wServerID;							//�����ʶ
	DWORD							dwOwnerID;							//�û���ʶ	
	DWORD							dwBattleNum;						//Լս���		

	//������Ϣ	
	SCORE							lWinScore;							//Ӯȡ����	
	bool							bLockState;							//����״̬			
	DWORD							dwCreateTime;						//����ʱ��
	tagBattleConfig					BattleConfig;						//Լս����
};

//��������
struct tagBattleTableItemEx
{
	//������Ϣ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wServerID;							//�����ʶ
	DWORD							dwOwnerID;							//�û���ʶ
	DWORD							dwBattleNum;						//Լս���
	TCHAR							szPassword[LEN_PASSWORD];			//��������

	//������Ϣ
	DWORD							dwCreateTime;						//����ʱ��
	tagBattleConfigEx				BattleConfig;						//Լս����
};

//��ҳɼ�
struct tagUserGradeItem
{		
	WORD							wChairID;							//���ӱ�ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lAddupScore;						//�û��ɼ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
};

//��������
struct tagBattleScoreItem
{
	//�û���Ϣ
	WORD							wChairID;							//�û���λ
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwGameID;							//��Ϸ��ʶ	
	BYTE							cbUserStatus;						//�û�״̬
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�

	//�û�����
	WORD							wFaceID;							//ͷ���ʶ	
	DWORD							dwCustomID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�

	//������Ϣ
	SCORE							lOverScore;							//��������

	//�ɼ���Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lRevenue;							//�û�˰��		
	LONG							lWinCount;							//ʤ����Ŀ
	LONG							lLostCount;							//ʧ����Ŀ
	LONG							lDrawCount;							//ƽ����Ŀ	

	//ժҪ��Ϣ
	TCHAR							szBattleAbstract[128];				//ԼսժҪ
};

//�û�����
struct tagBattleUserItem : public tagBattleScoreItem
{	
};

//�û�����
struct tagRankingUserItem
{
	//�û���Ϣ
	WORD							wChairID;							//�û���λ
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwGameID;							//��Ϸ��ʶ	
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�

	//�û�����
	WORD							wFaceID;							//ͷ���ʶ	
	DWORD							dwCustomID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�

	//��λ�ɼ�
	SCORE							lInitScore;							//��ʼ����
	SCORE							lOverScore;							//��������	
};

//////////////////////////////////////////////////////////////////////////
//IM����

//״̬����
struct tagStatusItem
{
	WORD							wStatusID;							//״̬��ʶ
	WORD							wServicePort;						//����˿�
	TCHAR							szServiceAddr[32];					//�����ַ
};

//�߼�����
struct tagLogicItem
{
	WORD							wLogicID;							//�߼���ʶ
	WORD							wServicePort;						//����˿�
	TCHAR							szServiceAddr[32];					//�����ַ
};

//Ⱥ������
struct tagGroupItem
{
	WORD							wGroupID;							//Ⱥ���ʶ
	WORD							wServicePort;						//����˿�
	TCHAR							szServiceAddr[32];					//�����ַ
};

//////////////////////////////////////////////////////////////////////////
//�û���Ϣ

//����״̬
struct tagTableStatus
{
	BYTE							cbTableLock;						//������־
	BYTE							cbPlayStatus;						//��Ϸ��־
};

//�û�״̬
struct tagUserStatus
{
	WORD							wTableID;							//��������
	WORD							wChairID;							//����λ��
	BYTE							cbUserStatus;						//�û�״̬
};

//�û�����
struct tagServerUserScore
{
	//������Ϣ
	SCORE							lScore;								//�û�����	
	
	//��Ϸ��Ϣ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ������
	LONG							lDrawCount;							//�;�����
	LONG							lFleeCount;							//��������
	LONG							lExperience;						//�û�����
	LONG							lWinStreak;							//��ʤ����
	LONG							lMaxWinStreak;						//�����ʤ
	SCORE							lBetAmount;							//��ע���
};

//�û�����
struct tagClientUserScore
{
	//������Ϣ
	SCORE							lScore;								//�û�����
	
	//��Ӯ��Ϣ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ������
	LONG							lDrawCount;							//�;�����
	LONG							lFleeCount;							//��������
	LONG							lExperience;						//�û�����
	LONG							lWinStreak;							//��ʤ����
};

//�û���Ϣ
struct tagServerUserInfo
{
	//��������
	WORD							wFaceID;							//ͷ������
	WORD							wActorID;							//��ɫ����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	WORD							wFaceFrameID;						//�������
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	WORD							wCardBackID;						//�Ʊ���ʶ

	//�û�����
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMasterOrder;						//����ȼ�
	DWORD							dwMemberPoint;						//��Ա����
	TCHAR							szArea[LEN_AREA];					//������Ϣ
	TCHAR							szCity[LEN_CITY];					//������Ϣ
	TCHAR							szProvince[LEN_PROVINCE];			//ʡ����Ϣ
	
	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lGold;								//�û����	
	SCORE							lScore;								//�û�����
	SCORE							lIngot;								//�û���ʯ
	SCORE							lTicket;							//�û���ȯ
	SCORE							lInsureGold;						//���н��
	LONG							lLoveHeart;							//��������

	//��Ϸ��Ϣ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ������
	LONG							lDrawCount;							//�;�����
	LONG							lFleeCount;							//��������
	LONG							lExperience;						//�û�����
	LONG							lWinStreak;							//��ʤ����
	LONG							lMaxWinStreak;						//��ʤ����		

	//�Զ�ͷ��
	DWORD							dwCustomID;							//�Զ���ʶ
};

//�û���Ϣ
struct tagClientUserInfo
{
	//��������	
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	bool							bIsRobot;							//������ʶ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��	

	//�û�����
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMasterOrder;						//����ȼ�
	DWORD							dwMemberPoint;						//��Ա����
	TCHAR							szArea[LEN_AREA];					//������Ϣ
	TCHAR							szCity[LEN_CITY];					//������Ϣ
	TCHAR							szProvince[LEN_PROVINCE];			//ʡ����Ϣ

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lGold;								//�û����	
	SCORE							lScore;								//�û�����
	SCORE							lInsureGold;						//���н��

	//��Ϸ��Ϣ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ������
	LONG							lDrawCount;							//�;�����
	LONG							lFleeCount;							//��������
	LONG							lExperience;						//�û�����
	LONG							lWinStreak;							//��ʤ����
	LONG							lMaxWinStreak;						//��ʤ����

	//�Զ�ͷ��
	WORD							wFaceID;							//ͷ������
	DWORD							dwCustomID;							//�Զ���ʶ
};

//�û���Ϣ
struct tagPCUserInfoHead
{
	//�û�����
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwUserID;							//�û� I D
	bool							bIsRobot;							//������ʶ

	//ͷ����Ϣ
	WORD							wFaceID;							//ͷ������
	DWORD							dwCustomID;							//�Զ���ʶ

	//�û�����
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMasterOrder;						//����ȼ�

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lGold;								//�û��ƽ�
	SCORE							lScore;								//�û�����
	SCORE							lInsureGold;						//���н��

	//��Ϸ��Ϣ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ������
	LONG							lDrawCount;							//�;�����
	LONG							lFleeCount;							//��������
	LONG							lWinStreak;							//��ʤ����
	LONG							lExperience;						//�û�����	
};

//�û���Ϣ
struct tagMBUserInfoHead
{
	//�û�����
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwUserID;							//�û� I D

	//ͷ����Ϣ
	WORD							wFaceID;							//ͷ������
	WORD							wActorID;							//��ɫ����
	DWORD							dwCustomID;							//�Զ���ʶ
	WORD							wFaceFrameID;						//�������
	WORD							wCardBackID;						//�Ʊ���ʶ

	//�û�����
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMasterOrder;						//����ȼ�

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lGold;								//�û��ƽ�
	SCORE							lScore;								//�û�����
																		
	//��Ϸ��Ϣ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ������
	LONG							lDrawCount;							//�;�����
	LONG							lFleeCount;							//��������
	LONG							lWinStreak;							//��ʤ����
	LONG							lExperience;						//�û�����	

	//λ����Ϣ
	DOUBLE							dGlobalPosLat;						//γ����Ϣ
	DOUBLE							dGlobalPosLng;						//������Ϣ	
};

//��λ��Ϣ
struct tagUserRankingInfo
{
	DWORD							dwRankID;							//������ʶ	
	WORD							wSeasonID;							//������ʶ
	LONG							lWinCount;							//Ӯ����Ŀ
	LONG							lLostCount;							//������Ŀ
	LONG							lWinStreakCount;					//��ʤ����
	LONG							lLoveHeart;							//��������
	SCORE							lRankScore;							//��λ����	
};

//ͷ����Ϣ
struct tagCustomFaceInfo
{
	DWORD							dwDataSize;							//���ݴ�С
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//ͼƬ��Ϣ
};

//�û���Ϣ
struct tagDebugUserInfo
{
	//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	bool							bIsRobot;							//������ʶ
	SCORE							lHistoryKindIDWin;					//����Ϸ��ʷ����Ӯ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�

	//�Ƹ���Ϣ
	SCORE							lUserScore;							//�û�����
	SCORE							lInsureScore;						//���з���	

	//״̬��Ϣ
	WORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬
};

//////////////////////////////////////////////////////////////////////////
//IM�ṹ����

//������Ϣ
struct tagIMUserPublicInfo
{
	//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	BYTE							cbGender;							//�û��Ա�
	WORD							wMasterOrder;						//����ȼ�	
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�

	//������Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	WORD							wFaceFrameID;						//����ʶ
	WORD							wChatFrameID;						//������ʶ
	DWORD							dwCustomID;							//ͷ���ʶ	

	//��Ϸ��Ϣ
	SCORE							lRankScore;							//��λ����
	DWORD							dwExperience;						//�û�����
	DWORD							dwMemberPoint;						//��Ա����	
	DWORD							dwLoveliness;						//����ֵ
	
	//������Ϣ
	BYTE							cbInfoPublic;						//������־
	DWORD							dwLikedNumber;						//��������
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
};

//�������
struct tagIMGroupOption
{
	//Լս����
	BYTE							cbPayTypeMask;						//֧������
	BYTE							cbCurrencyKindMask;					//��������	

	//�������
	WORD							wMinRevenueRate;					//���˰��
	WORD							wMaxRevenueRate;					//���˰��
	WORD							wMaxJoinGroupCount;					//��������
	WORD							wMaxCreateGroupCount;				//��������	
	WORD							wMaxBattleConfigCount;				//�淨����
};

//Ⱥ��Ƹ�
struct tagIMGroupWealth
{
	SCORE							lIngot;								//ʣ����ʯ
	SCORE							lRoomCard;							//ʣ�෿��
};

//Ⱥ�鶯̬
struct tagIMGroupDynamic
{
	WORD							wTableCount;						//��������
};

//Ⱥ������
struct tagIMGroupProperty
{
	//������Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ	
	DWORD							dwCreaterID;						//Ⱥ����ʶ
	DWORD							dwCreaterGameID;					//Ⱥ����ʶ	
	WORD							wGroupLevel;						//Ⱥ�鼶��
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������
	TCHAR							szCreaterNickName[LEN_ACCOUNTS];	//Ⱥ���ǳ�

	//��Ա��Ϣ
	WORD							wMemberCount;						//��Ա����
	WORD							wMaxMemberCount;					//��Ա����

	//������Ϣ
	DWORD							dwPayrollerID;						//�����߱�ʶ

	//������Ϣ
	SYSTEMTIME						CreateDateTime;						//����ʱ��
	TCHAR							szGroupIntroduce[LEN_GROUP_INTRODUCE];//Ⱥ�����	
};

//Ⱥ����Ƭ
struct tagIMGroupNameCard
{
	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ		
	WORD							wGroupLevel;						//Ⱥ�鼶��
	WORD							wMemberCount;						//��Ա����	
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������	

	//Ⱥ����Ϣ
	DWORD							dwCreaterID;						//Ⱥ����ʶ
	DWORD							dwCreaterGameID;					//Ⱥ����ʶ	
	TCHAR							szCreaterNickName[LEN_ACCOUNTS];	//Ⱥ���ǳ�
	BYTE							cbCreaterGender;					//�û��Ա�
	WORD							wCreaterFaceID;						//ϵͳͷ��	
	DWORD							dwCreaterCustomID;					//�Զ���ʶ			
};

//Լս�淨
struct tagIMBattleConfig
{
	WORD							wKindID;							//���ͱ�ʶ	
	DWORD							dwConfigID;							//�淨��ʶ	
	TCHAR							szBattleConfig[256];				//Լս����
};

//��ݹ���
struct tagIMGroupRules
{	
	BYTE							cbPayType;							//֧������
	BYTE							cbCurrencyKind;						//��������
	BYTE							cbBattleMask;						//Լս����
	WORD							wRevenueRate;						//˰�ձ���
	BYTE							cbScoreRight;						//����Ȩ��
	BYTE							cbScoreAdjust;						//���ֵ���
	WORD							wSwitchOption;						//����ѡ��
	BYTE							cbAutoCleanup;						//�Զ�����
};

//�������
struct tagIMGroupItem	
{	
	tagIMGroupRules					GroupRules;							//Ⱥ�����
	tagIMGroupWealth				GroupWealth;						//Ⱥ��Ƹ�	
	tagIMGroupProperty				GroupProperty;						//Ⱥ������	
};

//�û���Ϣ
struct tagIMUserInfo
{
	//��������	
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D	
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbUserStatus;						//�û�״̬
	TCHAR							szNickName[LEN_ACCOUNTS];			//����ǳ�	

	//�Զ�ͷ��
	WORD							wFaceID;							//ϵͳͷ��
	DWORD							dwCustomID;							//�Զ���ʶ

	//������
	DWORD							dwInviteCode;						//�������ʶ
};

//Ⱥ���Ա
struct tagIMGroupMember
{	
	//��Ա��Ϣ
	DWORD							dwMemberID;							//��Ա��ʶ
	DWORD							dwParentID;							//�ϼ���ʶ
	WORD							wMemberRate;						//�ֳɱ���
	BYTE							cbMemberType;						//��Ա����
	BYTE							cbMemberStatus;						//��Ա״̬	
	SYSTEMTIME						JoinDateTime;						//����ʱ��
	SYSTEMTIME						LastBattleTime;						//Լսʱ��

	//Լս��Ϣ
	SCORE							lGroupScore;						//��ݷ���
	SCORE							lGroupCowry;						//��ݱ���
	SCORE							lRevenueCount;						//�ۼ�˰��
	DWORD							dwCreateCount;						//��������
	DWORD							dwBattleCount;						//�������
};

//Ⱥ���Ա
struct tagIMGroupMemberUser
{	
	tagIMUserInfo					UserInfo;							//�û���Ϣ
	tagIMGroupMember				MemberInfo;							//��Ա��Ϣ	
};

//�û���Ϣ
struct tagIMUserItem
{
	//������Ϣ
	DWORD							dwUserID;							//�û� I D
	tagSocketInfo					SocketInfo;							//������Ϣ

	//״̬��Ϣ
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��
};

//������Ϣ
struct tagIMApplyMessage
{
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageStatus;					//��Ϣ״̬

	//������Ϣ
	DWORD							dwApplyerID;						//�û���ʶ	
	DWORD							dwApplyerGameID;					//��Ϸ��ʶ
	BYTE							cbApplyerGender;					//�û��Ա�
	WORD							wApplyerFaceID;						//ϵͳͷ��
	DWORD							dwApplyerCustomID;					//�Զ�ͷ��
	TCHAR							szApplyerNickName[LEN_ACCOUNTS];	//�û��ǳ�
	SYSTEMTIME						ApplyDateTime;						//����ʱ��

	//Ⱥ����Ϣ
	DWORD							dwGroupID;							//Ⱥ���ʶ	
	DWORD							dwResponderID;						//�����߱�ʶ
	TCHAR							szGroupName[LEN_GROUP_NAME];		//Ⱥ������	
};

///////////////////////////////////////////////////////////////////////////////////////////////
//��Ϣ����

//�洢����
struct tagStoreItem
{
	WORD							wStoreID;							//�洢��ʶ
	WORD							wServicePort;						//����˿�
	TCHAR							szServiceAddr[LEN_DOMAIN];			//�����ַ	
};

//////////////////////////////////////////////////////////////////////////
//�����ṹ

//�㳡����
struct tagPlazaItem
{
	DWORD							dwServiceAddr;						//�����ַ
	TCHAR							szServerName[32];					//��������
};

//��������
struct tagLevelItem
{
	LONG							lLevelScore;						//�������
	TCHAR							szLevelName[16];					//��������
};

//��Ա����
struct tagMemberItem
{
	BYTE							cbMemberOrder;						//�ȼ�����
	TCHAR							szMemberName[16];					//�ȼ�����
};

//��������
struct tagMasterItem
{
	BYTE							cbMasterOrder;						//�ȼ�����
	TCHAR							szMasterName[16];					//�ȼ�����
};

//�б�����
struct tagColumnItem
{
	BYTE							cbColumnWidth;						//�б���
	BYTE							cbDataDescribe;						//�ֶ�����
	TCHAR							szColumnName[16];					//�б�����
};

//������ʶ
struct tagComputerID
{
	DWORD							dwSystemVer;						//ϵͳ�汾
	DWORD							dwComputerID[3];					//��������
};

//��������
struct tagAgentItem
{
	WORD							wAgentKind;							//��������
};

//��ַ��Ϣ
struct tagAddressInfo
{
	DWORD							dwServiceAddr;						//���ŵ�ַ
	TCHAR							szServiceDomain[LEN_DOMAIN];		//��������
};

//���ݿ���Ϣ
struct tagDataBaseParameter
{
	WORD							wDataBasePort;						//���ݿ�˿�
	DWORD							dwDataBaseAddr;						//���ݿ��ַ
	TCHAR							szDataBaseUser[32];					//���ݿ��û�
	TCHAR							szDataBasePass[32];					//���ݿ�����
	TCHAR							szDataBaseName[32];					//���ݿ�����
};

//����ѡ��
struct tagRedisConnOption
{
	WORD							wDB;								//��������	
	WORD							wPort;								//���Ӷ˿�	
	TCHAR							szHost[32];							//���ӵ�ַ
	TCHAR							szUser[32];							//�����û�
	TCHAR							szPassword[32];						//��������	
};

//��Ϸ����
struct tagGameServerItem
{
	WORD							wServerID;							//��������
	WORD							wServerPort;						//����˿�
	DWORD							dwServerAddr;						//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//��¼��ַ
struct tagLogonServerItem
{
	WORD							wServerPort;						//����˿�
	DWORD							dwServiceAddr;						//�����ַ	
	TCHAR							szServiceDomain[LEN_DOMAIN];		//��������

};

//�Ƹ�����
struct tagWealthItem
{
	WORD							wMoneyID;							//���ұ�ʶ
	SCORE							lBalance;							//�������
};

//�û��ȼ�
struct tagUserLevelItem
{
	WORD							wLevelID;							//�ȼ���ʶ
	LONG							lNeedPoint;							//�������
	TCHAR							szLevelName[32];					//�ȼ�����
	TCHAR							szRewards[256];						//��������
};

//��Ϸ�ȼ�
struct tagGameLevelItem
{
	WORD							wLevelID;							//�ȼ���ʶ	
	LONG							lNeedPoint;							//�������
	TCHAR							szRewards[256];						//��������
};

//��λ�ȼ�
struct tagRankLevelItem
{
	//�ȼ���Ϣ
	WORD							wLevelID;							//�ȼ���ʶ	
	LONG							lNeedScore;							//�������
	TCHAR							szLevelName[32];					//�ȼ�����

	//��λ��Ϣ
	BYTE							cbMainOrder;						//��λ˳��
	BYTE							cbSubOrder;							//����˳��
	BYTE							cbStarOrder;						//�Ǽ�˳��
};

//////////////////////////////////////////////////////////////////////////
//���붨��


//����ѡ��
struct tagGateOption
{
	WORD							wGateID;							//���ر�ʶ	
	TCHAR							szGateName[32];						//��������
	WORD							wMaxConnect;						//�������	
	WORD							wServicePort;						//����˿�
	TCHAR							szServiceAddr[32];					//�����ַ	
};

//�߼�ѡ��
struct tagLogicOption
{
	WORD							wLogicID;							//�߼���ʶ		
	WORD							wMaxConnect;						//�������	
	WORD							wServicePort;						//����˿�
	TCHAR							szServiceAddr[32];					//�����ַ	
};

//Ⱥ��ѡ��
struct tagGroupOption
{
	WORD							wGroupID;							//Ⱥ���ʶ		
	WORD							wMaxConnect;						//�������	
	WORD							wServicePort;						//����˿�
	TCHAR							szServiceAddr[32];					//�����ַ	
};
//////////////////////////////////////////////////////////////////////////

//�޸Ĺ���
struct tagModifyRule
{
	DWORD							dwTableRuleAdd;						//���ӹ���	
	DWORD							dwTableRuleDel;						//���ӹ���	
	DWORD							dwServerRuleAdd;					//�������		
	DWORD							dwServerRuleDel;					//�������
};

//�������
struct tagCommandCode
{
	DWORD							dwCommandCode;						//�������
};

//////////////////////////////////////////////////////////////////////////
//¼����

//¼������ͷ
struct tagVideoHead
{
	DWORD							dwStartTime;						//��ʼʱ��
	DWORD							dwDataSize;							//���ݴ�С	
	WORD							wPacketCount;						//������Ŀ
	BYTE							cbUserCount;						//�û�����
	BYTE							cbCompressKind;						//ѹ������	
};

//¼���û�
struct tagVideoUser
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wChairID;							//���ӷ�λ
	TCHAR							szNickName[LEN_ACCOUNTS];			//����ǳ�
};

//¼�����ݰ�
struct tagVideoPacket
{	
	//������Ϣ
	WORD							wPacketSize;						//���ݴ�С
	DWORD							dwPacketPos;						//����λ��
	DWORD							dwInsertTime;						//����ʱ��	

	//λ����Ϣ
	WORD							wChairID;							//��ҷ�λ

	//������Ϣ
	WORD							wMainCmdID;							//��������
	WORD							wSubCmdID;							//��������
	WORD							wDataSize;							//���ݴ�С
};

//�ṹ����
struct tagVideoServer
{
	//������Ϣ
	WORD							wKindID;							//���ƺ���
	WORD							wChairCount;						//������Ŀ
	TCHAR							szProcessName[LEN_PROCESS];			//��������

	//������Ϣ
	WORD							wServerID;							//�������
	WORD							wServerType;						//��������		
	DWORD							dwServerRule;						//�������
	TCHAR							szServerName[LEN_SERVER];			//��������	
};

//////////////////////////////////////////////////////////////////////////

//��Ʒ����
struct tagGoodsItem
{
	WORD							wGoodsID;							//��Ʒ��ʶ
	WORD							wGoodsLevel;						//��Ʒ�ȼ�
	DWORD							dwGoodsCount;						//��Ʒ����
	DWORD							dwGoodsIndate;						//��Ʒ��Ч��
};

//��Ʒ����
struct tagGoodsCountItem
{
	WORD							wGoodsID;							//��Ʒ��ʶ
	DWORD							dwGoodsCount;						//��Ʒ����
};

//��Ʒ��Ϣ
struct tagGoodsInfo
{
	WORD							wGoodsID;							//��Ʒ��ʶ
	WORD							wGoodsKind;							//��Ʒ����
	bool							bHasIndate;							//��Ч��
	WORD							wCanTradeType;						//��������
	WORD							wCanStorePlace;						//���λ��	
	TCHAR							szGoodsName[LEN_GOODS_NAME];		//��Ʒ����
	TCHAR							szGoodsOption[LEN_GOODS_OPTION];	//��Ʒѡ��
};

//��Ʒ������Ϣ
struct tagGoodsHoldInfo
{
	WORD							wGoodsID;							//��Ʒ��ʶ
	WORD							wGoodsKind;							//��Ʒ����
	WORD							wGoodsLevel;						//��Ʒ�ȼ�
	DWORD							dwGoodsCount;						//��Ʒ����
	DWORD							dwGoodsIndate;						//��Ʒ��Ч��	
	DWORD							dwExpireTime;						//����ʱ��
};

//����������Ϣ
struct tagGiftInfo
{
    DWORD							dwRecordID;							//��¼ID
    DWORD							dwUserID;							//�û���ʶ
    DWORD							dwGoodsID;							//��Ʒ��ʶ
    DWORD							dwGoodsCount;						//��Ʒ����
	DWORD							dwLoveLiness;						//��������
    SYSTEMTIME						SendDate;							//����ʱ��	
    TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ʺ�
};

//////////////////////////////////////////////////////////////////////////
//������

//ӳ������
struct tagMappedItem
{
	WORD							wServicePort;						//�����˿�
	WORD							wRemotePort;						//Զ�̶˿�
	DWORD							dwRemoteAddr;						//Զ�̵�ַ
};

//�˿ڶ�
struct tagPortPair
{
	WORD							wServicePort;						//�����˿�
	WORD							wForwardPort;						//ת���˿�
};

//����ѡ��
struct tagAgentOption
{
	WORD							wAgentID;							//�����ʶ	
	TCHAR							szAgentName[32];					//��������
	WORD							wAgentKind;							//��������
	WORD							wMaxConnect;						//�������	
	WORD							wMappedCount;						//ӳ����Ŀ
	tagMappedItem					MappedItem[16];						//ӳ������Ŀ
};
//////////////////////////////////////////////////////////////////////////
//�ʼ�����

//�ʼ�����
struct tagUserMailItem
{
	//�ʼ�ͷ��
	DWORD							dwToUserID;							//���ձ�ʶ
	WORD							wFromModule;						//����ģ��	
	TCHAR							szMailTitle[32];					//�ʼ�����		

	//�ʼ�����
	WORD							wMailKind;							//�ʼ�����
	WORD							wDataSize;							//���ݴ�С
	BYTE							cbDataBuffer[256];					//���ݻ���
};

//�ʼ���Ϣ
struct tagMailInfo
{
	DWORD							dwMailID;						//�ʼ���ʶ
	BYTE							cbMailKind;						//�ʼ�����	
	BYTE							cbMailType;						//�ʼ�����
	BYTE							cbMailState;					//�ʼ�״̬
	DWORD							dwSendTime;						//����ʱ��
	DWORD							dwExpireTime;					//����ʱ��	
	TCHAR							szMailTitle[LEN_MAIL_TITLE];	//�ʼ�����
	TCHAR							szMailContent[LEN_MAIL_CONTENT];//�ʼ�����
	TCHAR							szAttachGoods[LEN_MAIL_ATTACH];	//������Ϣ	
};

//////////////////////////////////////////////////////////////////////////
//���߶���

//��������
struct tagPropertyItem
{
	DWORD							dwPropKind;							//��������
	LONG							lPropPrice;							//���߼۸�
	BYTE							cbUseRange;							//ʹ�÷�Χ	
	TCHAR							szPropName[LEN_PROPERTY_NAME];		//��������
};

//////////////////////////////////////////////////////////////////////////

//�����Ϣ
struct tagStockItem
{
	WORD							wKindID;						//��Ϸ����
	WORD							wStockID;						//�����ʶ
	WORD							wStockKind;						//�������
	WORD							wStockSubtype;					//�������		
	WORD							wSafetyRatio;					//��������
	WORD							wShrinkRatio;					//��ˮ���� ǧ�ֱ�
	SCORE							lInitialScore;					//��ʼ���
	SCORE							lCurrentScore;					//��ǰ���
	SCORE							lAlarmScore;					//Ԥ�����
	SCORE							lStopLossScore;					//ֹ����
	SCORE							lVariationLimit;				//�������
	SCORE							lVariationScore;				//�仯���
	SCORE							lRecordScore;					//��¼���
	TCHAR							szStockName[32];				//�������
	CHAR							szCustomOption[256];			//�Զ�������
};

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif