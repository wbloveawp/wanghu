#ifndef BATTLE_RULE_HEAD_FILE
#define BATTLE_RULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//约战规则
#define	TR_FORBID_BIND_USER			0x00000001							//禁止绑定用户
#define	TR_FORBID_SEND_GRADE		0x00000002							//禁止发送成绩
#define	TR_FORBID_WRITE_REWARD		0x00000004							//禁止写入奖励
#define TR_FORBID_BATTLE_RECORD		0x00000008							//禁止约战记录
#define TR_FORBID_SAMEIP_REMIND		0x00000010							//禁止同IP提醒

//约战规则
#define TR_FORBID_DYNAMIC_JOIN		0x00010000							//禁止动态加入

//////////////////////////////////////////////////////////////////////////

//约战规则
class CBattleRule
{
	//查询函数
public:
	//禁止绑定用户
	static bool IsForbidBindUser(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_BIND_USER)!=0; }
	//禁止发送成绩
	static bool IsForbidSendGrade(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_SEND_GRADE)!=0; }
	//禁止写入奖励
	static bool IsForbidBattleReward(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_WRITE_REWARD)!=0; }
	//禁止约战记录
	static bool IsForbidBattleRecord(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_BATTLE_RECORD)!=0; }
	//禁止同IP提醒
	static bool IsForbidSameIPRemind(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_SAMEIP_REMIND)!=0; }
	//禁止动态加入
	static bool IsForbidDynamicJoin(DWORD dwBattleRule) { return (dwBattleRule&TR_FORBID_DYNAMIC_JOIN) != 0; }

	//设置函数
public:
	//禁止绑定用户
	static VOID SetForbidBindUser(DWORD & dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_BIND_USER:dwBattleRule&=~TR_FORBID_BIND_USER; }
	//禁止发送成绩
	static VOID SetForbidSendGrade(DWORD &dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_SEND_GRADE:dwBattleRule&=~TR_FORBID_SEND_GRADE; }
	//禁止写入奖励
	static VOID SetForbidWriteReward(DWORD &dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_WRITE_REWARD:dwBattleRule&=~TR_FORBID_WRITE_REWARD; }
	//禁止约战记录
	static VOID SetForbidBattleRecord(DWORD &dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_BATTLE_RECORD:dwBattleRule&=~TR_FORBID_BATTLE_RECORD; }	
	//禁止同IP提醒
	static VOID SetForbidSameIPRemind(DWORD &dwBattleRule, bool bEnable) { (bEnable==true)?dwBattleRule|=TR_FORBID_SAMEIP_REMIND:dwBattleRule&=~TR_FORBID_SAMEIP_REMIND; }
	//禁止动态加入
	static VOID SetForbidDynamicJoin(DWORD &dwBattleRule, bool bEnable) { (bEnable == true) ? dwBattleRule |= TR_FORBID_DYNAMIC_JOIN : dwBattleRule &= ~TR_FORBID_DYNAMIC_JOIN; }
};

//////////////////////////////////////////////////////////////////////////


#endif