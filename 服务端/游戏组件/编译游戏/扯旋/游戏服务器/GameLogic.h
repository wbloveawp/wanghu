#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////
//宏定义
#define MAX_COUNT 4
//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//扑克类型
#define D2H_VALUE0					0									//无效牌型
#define D2H_D2H						199									//丁二皇型
#define D2H_DOBLUE_Q				198									//天对牌型
#define D2H_DOBLUE_2				197									//地对牌型
#define D2H_DOBLUE_8				196									//仁对牌型
#define D2H_DOBLUE_4				195									//和对牌型
#define D2H_DOBLUE_46A				194									//中对牌型
#define D2H_DOBLUE_67AJ				193									//下对牌型
#define D2H_DOBLUE_9875				192									//花对牌型
#define D2H_TH						191									//天皇牌型
#define D2H_DH						190									//地皇牌型
#define D2H_TG						189									//天杠牌型
#define D2H_DG						188									//地杠牌型

//////////////////////////////////////////////////////////////////////////
static CString GetFileDialogPath()
{
	CString strFileDlgPath;
	TCHAR szModuleDirectory[MAX_PATH];	//模块目录
	GetModuleFileName(AfxGetInstanceHandle(),szModuleDirectory,sizeof(szModuleDirectory));
	int nModuleLen=lstrlen(szModuleDirectory);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE")) + 1;
	if (nModuleLen<=nProcessLen) 
		return TEXT("");
	szModuleDirectory[nModuleLen-nProcessLen]=0;
	strFileDlgPath = szModuleDirectory;
	return strFileDlgPath;
}

static void NcaTextOut(CString strInfo)
{
	CString strName = GetFileDialogPath()+	TEXT("\\CheXuan.log");
	CTime time = CTime::GetCurrentTime() ;
	CString strTime ;
	strTime.Format( TEXT( "%d-%d-%d %d:%d:%d" ) ,
		time.GetYear() ,
		time.GetMonth() ,
		time.GetDay() ,
		time.GetHour() ,
		time.GetMinute() ,
		time.GetSecond()
		);
	CString strMsg;
	strMsg.Format(TEXT("%s,%s\r\n"),strTime,strInfo);

	CFile fLog;
	if(fLog.Open( strName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite ))
	{

		fLog.SeekToEnd(); 	
		int strLength=strMsg.GetLength();
#ifdef _UNICODE
		BYTE bom[2] = {0xff, 0xfe};
		fLog.Write(bom,sizeof(BYTE)*2);
		strLength*=2;
#endif
		fLog.Write((LPCTSTR)strMsg,strLength);
		fLog.Close();
	}

}
//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
private:
	BYTE							m_cbRuleOption;						//规则配置
	static BYTE						m_cbCardListData[32];				//扑克定义

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//配置函数
public:
	//规则配置
	VOID SetRuleOption(BYTE cbRuleOption) { m_cbRuleOption=cbRuleOption; }

	//类型函数
public:
	//获取类型
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//功能函数
public:	
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//特殊换牌
	void ChangeSpeCard( BYTE cbCardData[], BYTE cbCardCount);
	//特殊数值
	BYTE GetSpeCardValue(BYTE cbCardData[], BYTE cbCardCount);
	//持有特牌
	bool IsHoldSpecialCard(BYTE cbCardData[], BYTE cbCardCount);	
	//对比扑克
	BYTE CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount);
	//特殊处理王5和38
	BYTE DealSpeCard(BYTE cbFirstData[], BYTE cbNextData[]);
};

//////////////////////////////////////////////////////////////////////////

#endif
