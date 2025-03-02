#ifndef BATTLE_RULE_HEAD_FILE
#define BATTLE_RULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//Լս����
#define	TR_FORBID_BIND_USER			0x00000001							//��ֹ���û�
#define	TR_FORBID_SEND_GRADE		0x00000002							//��ֹ���ͳɼ�
#define	TR_FORBID_WRITE_REWARD		0x00000004							//��ֹд�뽱��
#define TR_FORBID_BATTLE_RECORD		0x00000008							//��ֹԼս��¼
#define TR_FORBID_SAMEIP_REMIND		0x00000010							//��ֹͬIP����

//Լս����
#define TR_FORBID_DYNAMIC_JOIN		0x00010000							//��ֹ��̬����

//////////////////////////////////////////////////////////////////////////

//Լս����
class CBattleRule
{
	//��ѯ����
public:
	//��ֹ���û�
	static bool IsForbidBindUser(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_BIND_USER)!=0; }
	//��ֹ���ͳɼ�
	static bool IsForbidSendGrade(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_SEND_GRADE)!=0; }
	//��ֹд�뽱��
	static bool IsForbidBattleReward(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_WRITE_REWARD)!=0; }
	//��ֹԼս��¼
	static bool IsForbidBattleRecord(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_BATTLE_RECORD)!=0; }
	//��ֹͬIP����
	static bool IsForbidSameIPRemind(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_SAMEIP_REMIND)!=0; }
	//��ֹ��̬����
	static bool IsForbidDynamicJoin(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_DYNAMIC_JOIN) != 0; }

	//���ú���
public:
	//��ֹ���û�
	static VOID SetForbidBindUser(DWORD & dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_BIND_USER:dwBattleRule&=~TR_FORBID_BIND_USER; }
	//��ֹ���ͳɼ�
	static VOID SetForbidSendGrade(DWORD &dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_SEND_GRADE:dwBattleRule&=~TR_FORBID_SEND_GRADE; }
	//��ֹд�뽱��
	static VOID SetForbidWriteReward(DWORD &dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_WRITE_REWARD:dwBattleRule&=~TR_FORBID_WRITE_REWARD; }
	//��ֹԼս��¼
	static VOID SetForbidBattleRecord(DWORD &dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_BATTLE_RECORD:dwBattleRule&=~TR_FORBID_BATTLE_RECORD; }	
	//��ֹͬIP����
	static VOID SetForbidSameIPRemind(DWORD &dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_SAMEIP_REMIND:dwBattleRule&=~TR_FORBID_SAMEIP_REMIND; }
	//��ֹ��̬����
	static VOID SetForbidDynamicJoin(DWORD &dwBattleRule, bool bEnable) { (bEnable == true) ? dwBattleRule |= TR_FORBID_DYNAMIC_JOIN : dwBattleRule &= ~TR_FORBID_DYNAMIC_JOIN; }
};

//////////////////////////////////////////////////////////////////////////


#endif