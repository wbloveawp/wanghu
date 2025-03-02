#ifndef SERVER_RULE_HEAD_FILE
#define SERVER_RULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//�������
#define SR_RECORD_GAME_SCORE		0x00000001							//��¼����
#define SR_IMMEDIATE_WRITE_SCORE	0x00000002							//��ʱд��
#define SR_RECORD_GAME_VIDEO		0x00000004							//��¼��Ϸ���� 
#define SR_FORBID_ROOM_CHAT			0x00000008							//�����ֹ����
#define SR_FORBID_ROOM_WISPER		0x00000010							//�����ֹ˽��
#define SR_FORBIT_GAME_CHAT			0x00000020							//��Ϸ��ֹ����
#define SR_ALLOW_VOICE_CHAT			0x00000040							//������������
#define SR_TABLE_END_IMMEDIATE		0x00000080							//��������Ϸ״̬�ı�
#define SR_DYNAMIC_JOIN_GAME		0x00000100							//��̬������Ϸ
#define SR_ALLOW_LARGESS_SUBSIDY	0x00000200							//�������͵ͱ�
#define SR_CHARGE_MODE_FEE			0x00000400							//�̶������ģʽ
#define SR_FORBID_PLAY_CHAT			0x00000800							//ͬ����Ϸ��ֹ˽��
#define SR_SIT_FOR_LOOKON			0x00001000							//���¼��Թ�
#define SR_FORBID_QUERY_USER		0x00002000							//��ֹԶ������
#define SR_MEMBER_CAN_CHAT			0x00004000							//��Ա������
#define SR_FORBID_LOOKON			0x00008000							//��ֹ�Թ�
#define SR_HIDE_USER_INFO			0x00010000							//�����û���Ϣ
#define SR_MATCH_USER_BY_SCORE		0x00020000							//���ݷ���ƥ���û�
#define SR_FORBID_BUGLE				0x00040000							//��ֹ������
#define SR_ALLOW_AGREEMENT_DISMISS	0x00080000							//����Э���ɢ
#define SR_FORBIT_OFFLINE_STANDUP   0x00100000							//���߳�ʱ��ֹ����
#define SR_ALLOW_ROBOT_ATTEND		0x00200000							//��������
#define SR_ALLOW_ROBOT_SIMULATE		0x00400000							//����ռλ
#define SR_FORBIT_FORCE_STANDUP		0x00800000							//ǿ�˽�ֹ����

//���ӹ���
#define TR_FORBID_WINRATE_SET		0x01000000							//��ֹʤ������
#define	TR_FORBID_FLEERATE_SET		0x02000000							//��ֹ��������
#define	TR_FORBID_SCORE_LIMIT		0x04000000							//��ֹ�ɼ�����
#define TR_FORBID_BLACKLIST_SET		0x08000000							//��ֹ�������ܻ�ӭ�����Ϸ������
#define TR_FORBID_IP_LIMIT			0x10000000							//��ֹIP����
#define TR_FORBID_PASSWORD_SET		0x20000000							//��ֹ��������
#define TR_FORBID_SHOW_BASESCORE	0x40000000							//��ֹ��ʾ�׷�

//�������
#define MR_FORBID_ENTER_ROOM		0x00000001							//��ֹ������

//////////////////////////////////////////////////////////////////////////

//�������
class CServerRule
{
	//��ѯ����
public:
	//��¼����
	static bool IsRecordGameScore(DWORD dwServerRule) { return (dwServerRule&SR_RECORD_GAME_SCORE)!=0; }
	//��ʱд��
	static bool IsImmediateWriteScore(DWORD dwServerRule) { return (dwServerRule&SR_IMMEDIATE_WRITE_SCORE)!=0; }
	//��¼��Ϸ¼��
	static bool IsRecordGameVideo( DWORD dwServerRule ) { return (dwServerRule&SR_RECORD_GAME_VIDEO) != 0; }
	//�����ֹ����
	static bool IsForbidRoomChat( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_ROOM_CHAT) != 0; }
	//�����ֹ˽��
	static bool IsForbidRoomWisper( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_ROOM_WISPER) != 0; }
	//��Ϸ��ֹ����
	static bool IsForbidGameChat( DWORD dwServerRule ) { return (dwServerRule&SR_FORBIT_GAME_CHAT) != 0; }
	//��������
	static bool IsAllowRobotAttend(DWORD dwServerRule) { return (dwServerRule&SR_ALLOW_ROBOT_ATTEND)!=0; }
	//����ռλ
	static bool IsAllowRobotSimulate(DWORD dwServerRule) { return (dwServerRule&SR_ALLOW_ROBOT_SIMULATE)!=0; }
	//��������Ϸ״̬�ı�
	static bool IsTableEndImmediate( DWORD dwServerRule ) { return (dwServerRule&SR_TABLE_END_IMMEDIATE) != 0; }
	//��̬������Ϸ
	static bool IsDynamicJoinGame( DWORD dwServerRule ) { return (dwServerRule&SR_DYNAMIC_JOIN_GAME) != 0; }
	//�̶������ģʽ
	static bool IsChargeModeFee( DWORD dwServerRule ) { return (dwServerRule&SR_CHARGE_MODE_FEE) != 0; }
	//ͬ����ֹ˽��
	static bool IsForbidPlayChat( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_PLAY_CHAT) != 0; }
	//���¼��Թ�
	static bool IsSitForLookon( DWORD dwServerRule ) { return (dwServerRule&SR_SIT_FOR_LOOKON) != 0; }
	//��ֹԶ������
	static bool IsForbidQueryUser( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_QUERY_USER) != 0; }
	//��Ա������
	static bool IsMemberCanChat( DWORD dwServerRule ) { return (dwServerRule&SR_MEMBER_CAN_CHAT) != 0; }
	//��ֹ�Թ�
	static bool IsForbidLookon( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_LOOKON) != 0; }
	//�����û���Ϣ
	static bool IsHideUserInfo(DWORD dwServerRule) {return (dwServerRule&SR_HIDE_USER_INFO) != 0;}
	//���ݷ���ƥ���û�
	static bool IsMatchUserByScore(DWORD dwServerRule) {return (dwServerRule&SR_MATCH_USER_BY_SCORE) != 0;}
	//��ֹ������
	static bool IsForbidBugle(DWORD dwServerRule) {return (dwServerRule&SR_FORBID_BUGLE) != 0;}
	//����Э���ɢ
	static bool IsAllowAgreementDismiss(DWORD dwServerRule) {return (dwServerRule&SR_ALLOW_AGREEMENT_DISMISS) != 0;}
	//ǿ�˽�ֹ����
	static bool IsForbidForceStandup(DWORD dwServerRule) {return (dwServerRule&SR_FORBIT_FORCE_STANDUP)!=0; }
	//���߳�ʱ��ֹ����
	static bool IsForbidOfflineStandup(DWORD dwServerRule) {return (dwServerRule&SR_FORBIT_OFFLINE_STANDUP)!=0; }
	//������������
	static bool IsAllowVoiceChat(DWORD dwServerRule) {return (dwServerRule&SR_ALLOW_VOICE_CHAT)!=0; }
	//�������͵ͱ�
	static bool IsAllowLargessSubsidy(DWORD dwServerRule) {return (dwServerRule&SR_ALLOW_LARGESS_SUBSIDY)!=0; }

	//���ú���
public:
	//��¼����
	static VOID SetRecordGameScore(DWORD & dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=SR_RECORD_GAME_SCORE:dwServerRule&=~SR_RECORD_GAME_SCORE; }
	//��ʱд��
	static VOID SetImmediateWriteScore(DWORD & dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=SR_IMMEDIATE_WRITE_SCORE:dwServerRule&=~SR_IMMEDIATE_WRITE_SCORE; }
	//��¼��Ϸ¼��
	static VOID SetRecordGameVideo( DWORD &dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_RECORD_GAME_VIDEO:dwServerRule&=~SR_RECORD_GAME_VIDEO;
	}
	//�����ֹ����
	static VOID SetForbidRoomChat( DWORD &dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_ROOM_CHAT:dwServerRule&=~SR_FORBID_ROOM_CHAT;
	}
	//�����ֹ˽��
	static VOID SetForbidRoomWisper( DWORD &dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_ROOM_WISPER:dwServerRule&=~SR_FORBID_ROOM_WISPER;
	}
	//��Ϸ��ֹ����
	static VOID SetForbidGameChat( DWORD &dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBIT_GAME_CHAT:dwServerRule&=~SR_FORBIT_GAME_CHAT;
	}
	//��������
	static VOID SetAllowRobotAttend(DWORD & dwServerRule, bool bEnable) 
	{ 
		(bEnable==true)?dwServerRule|=SR_ALLOW_ROBOT_ATTEND:dwServerRule&=~SR_ALLOW_ROBOT_ATTEND; 
	}	
	//����ռλ
	static VOID SetAllowRobotSimulate(DWORD & dwServerRule, bool bEnable) 
	{ 
		(bEnable==true)?dwServerRule|=SR_ALLOW_ROBOT_SIMULATE:dwServerRule&=~SR_ALLOW_ROBOT_SIMULATE;
	}
	//��������Ϸ״̬�ı�
	static VOID SetTableEndImmediate( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_TABLE_END_IMMEDIATE:dwServerRule&=~SR_TABLE_END_IMMEDIATE;
	}
	//��̬������Ϸ
	static VOID SetDynamicJoinGame( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_DYNAMIC_JOIN_GAME:dwServerRule&=~SR_DYNAMIC_JOIN_GAME;
	}
	//�̶������ģʽ
	static VOID SetChargeModeFee( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_CHARGE_MODE_FEE:dwServerRule&=~SR_CHARGE_MODE_FEE;
	}
	//ͬ����ֹ˽��
	static VOID SetForbidPlayChat( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_PLAY_CHAT:dwServerRule&=~SR_FORBID_PLAY_CHAT;
	}
	//���¼��Թ�
	static VOID SetSitForLookon( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_SIT_FOR_LOOKON:dwServerRule&=~SR_SIT_FOR_LOOKON;
	}
	//��ֹԶ������
	static VOID SetForbidQueryUser( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_QUERY_USER:dwServerRule&=~SR_FORBID_QUERY_USER;
	}
	//��Ա������
	static VOID SetMemberCanChat( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_MEMBER_CAN_CHAT:dwServerRule&=~SR_MEMBER_CAN_CHAT;
	}
	//��ֹ�Թ�
	static VOID SetForbidLookon( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_LOOKON:dwServerRule&=~SR_FORBID_LOOKON;
	}
	//�����û���Ϣ
	static VOID SetHideUserInfo(DWORD & dwServerRule, bool bEnable)
	{
		bEnable?dwServerRule|=SR_HIDE_USER_INFO:dwServerRule&=~SR_HIDE_USER_INFO;
	}
	//���ø��ݷ���ƥ���û�
	static VOID SetMatchUserByScore(DWORD & dwServerRule, bool bEnable)
	{
		bEnable?dwServerRule|=SR_MATCH_USER_BY_SCORE:dwServerRule&=~SR_MATCH_USER_BY_SCORE;
	}
	//��ֹ������
	static VOID SetForbidBugle(DWORD &	dwServerRule, bool bEnable)
	{
		bEnable?dwServerRule|=SR_FORBID_BUGLE:dwServerRule&=~SR_FORBID_BUGLE;
	}

	//����Э���ɢ
	static VOID SetAllowAgreementDismiss(DWORD & dwServerRule, bool bEnable)
	{
		bEnable?dwServerRule|=SR_ALLOW_AGREEMENT_DISMISS:dwServerRule&=~SR_ALLOW_AGREEMENT_DISMISS;
	}

	//ǿ�˽�ֹ����
	static VOID SetForbidForceStandup(DWORD & dwServerRule,bool bEnable)
	{ 
		bEnable?dwServerRule|=SR_FORBIT_FORCE_STANDUP:dwServerRule&=~SR_FORBIT_FORCE_STANDUP;
	}

	//���߳�ʱ��ֹ����
	static VOID SetForbidOfflineStandup(DWORD & dwServerRule,bool bEnable)
	{ 
		bEnable?dwServerRule|=SR_FORBIT_OFFLINE_STANDUP:dwServerRule&=~SR_FORBIT_OFFLINE_STANDUP;
	}

	//������������
	static VOID SetAllowVoiceChat(DWORD & dwServerRule,bool bEnable)
	{ 
		bEnable?dwServerRule|=SR_ALLOW_VOICE_CHAT:dwServerRule&=~SR_ALLOW_VOICE_CHAT;
	}

	//�������͵ͱ�
	static VOID SetAllowLargessSubsidy(DWORD & dwServerRule,bool bEnable)
	{ 
		bEnable?dwServerRule|=SR_ALLOW_LARGESS_SUBSIDY:dwServerRule&=~SR_ALLOW_LARGESS_SUBSIDY;
	}
	
	//��ѯ����
public:
	//��ֹʤ������
	static bool IsForbidWinRateSet(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_WINRATE_SET)!=0; }
	//��ֹ��������
	static bool IsForbidFleeRateSet(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_FLEERATE_SET)!=0; }
	//��ֹ�ɼ�����
	static bool IsForbidScoreLimit(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_SCORE_LIMIT)!=0; }
	//��ֹ�������ܻ�ӭ�����Ϸ������
	static bool IsForbidBlackListSet(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_BLACKLIST_SET)!=0; }
	//��ֹIP����
	static bool IsForbidIPLimit(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_IP_LIMIT)!=0; }
	//��ֹ��������
	static bool IsForbidPasswordSet(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_PASSWORD_SET)!=0; }
	//��ֹ��ʾ�׷�
	static bool IsForbidShowBaseScore(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_SHOW_BASESCORE)!=0; }

	//���ú���
public:
	//��ֹʤ������
	static VOID SetForbidWinRateSet(DWORD & dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_WINRATE_SET:dwServerRule&=~TR_FORBID_WINRATE_SET; }
	//��ֹ��������
	static VOID SetForbidFleeRateSet(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_FLEERATE_SET:dwServerRule&=~TR_FORBID_FLEERATE_SET; }
	//��ֹ�ɼ�����
	static VOID SetForbidScoreLimit(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_SCORE_LIMIT:dwServerRule&=~TR_FORBID_SCORE_LIMIT; }
	//��ֹ�������ܻ�ӭ�����Ϸ������
	static VOID SetForbidBlackListSet(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_BLACKLIST_SET:dwServerRule&=~TR_FORBID_BLACKLIST_SET; }
	//��ֹIP����
	static VOID SetForbidIPLimit(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_IP_LIMIT:dwServerRule&=~TR_FORBID_IP_LIMIT; }
	//��ֹ��������
	static VOID SetForbidPasswordSet(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_PASSWORD_SET:dwServerRule&=~TR_FORBID_PASSWORD_SET; }
	//��ֹ��ʾ�׷�
	static VOID SetForbidShowBaseScore(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_SHOW_BASESCORE:dwServerRule&=~TR_FORBID_SHOW_BASESCORE; }

	//��Ϣ����
public:
	//֧������
	static WORD GetSupportType()
	{
		WORD wSupportType = 0xFFFF;

		//�Ƴ�����
		wSupportType &= ~GAME_GENRE_SCORE;

#ifndef MATCH_SERVICE_MODULE		
		wSupportType &= ~GAME_GENRE_MATCH;
#endif	
#ifndef BATTLE_SERVICE_MODULE
		wSupportType &= ~GAME_GENRE_BATTLE;
#endif	
#ifndef RANKING_SERVICE_MODULE
		wSupportType &= ~GAME_GENRE_RANKING;
#endif

		return wSupportType;
	};
};

//////////////////////////////////////////////////////////////////////////


#endif