#ifndef CMD_OX_HEAD_FILE
#define CMD_OX_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义


#define KIND_ID							60									//游戏 I D
#define GAME_PLAYER						8									//游戏人数
#define GAME_NAME						TEXT("扯旋")						//游戏名字
#define ENGLISH_NAME					TEXT("CheXuan")                        //斗地主
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型
#define MAX_COUNT						4									//最大数目
#define LEN_NICKNAME					32
#define MIN_MUTIPLIE					10									//最小倍数

//结束原因						
#define GER_NO_PLAYER					0x10							    //没有玩家

//游戏状态						
#define GS_TK_FREE						GAME_STATUS_FREE					//等待开始
#define GS_TK_INVEST					GAME_STATUS_PLAY					//下本钱注
#define GS_TK_SCORE						GAME_STATUS_PLAY+1					//下注状态
#define GS_TK_OPEN_CARD					GAME_STATUS_PLAY+2				    //游戏进行

#define VERSION_SERVER					PROCESS_VERSION(8,0,1)				//程序版本
#define VERSION_CLIENT					PROCESS_VERSION(8,0,1)				//程序版本

//用户状态
#define GUS_NULL						0									
#define GUS_PLAYING						1									//游戏状态
#define GUS_LOOKON						2									//观看状态
#define GUS_DYNAMIC						3									//动态加入

//结束原因						
#define END_REASON_EXIT					1									//强退结束
#define END_REASON_PASS					2									//让牌结束
#define END_REASON_NORMAL				3									//正常结束
#define END_REASON_GIVEUP				4									//放弃结束

//代码定义
#define CODE_ADD						1									//加注代码
#define CODE_PASS						2									//休牌代码
#define CODE_FOLLOW						3									//跟注代码
#define CODE_SHOWHAND					4									//梭哈代码

//////////////////////////////////////////////////////////////////////////

#define JETTON_COUNT					8								//筹码个数
#define LAYER_HEIGHT					4								//层叠高度
#define JETTON_SPECE					3								//筹码间距
#define CONTROL_COUNT					6								//控件个数


//规则定义
#define GR_DJW_BIG_CARD					0x01							//地九王大牌
#define GR_SHS_AND_SHL					0x02							//双花十和三花六
#define GR_ALLOW_BET_GGF				0x04							//下注滚滚翻
#define GR_ALLOW_FIRSTER				0x08							//允许先后手
#define GR_ALLOW_CUSTOM_SCORE			0x10							//自定义底分

//////////////////////////////////////////////////////////////////////////

//服务端配置
struct tagCustomConfig
{
	BYTE							cbRuleOption;						//规则配置
	LONGLONG						lMaxBoboMultiple;					//簸簸倍数
	LONG							lMinBetAmount;						//最低下注	

	//构造函数
	tagCustomConfig()
	{
		cbRuleOption=0;
		lMinBetAmount=1;
		lMaxBoboMultiple=0;		
	}
	
	void PrimaryDefaultCustomRule()
	{
		cbRuleOption = 0;
		lMinBetAmount = 1;
		lMaxBoboMultiple = 0;
	}
};

//////////////////////////////////////////////////////////////////////////

//服务器命令结构
#define SUB_S_GAME_START				101									//游戏开始
#define SUB_S_USER_INVEST				107									//用户下本
#define SUB_S_ADD_SCORE					102									//加注结果
#define SUB_S_SEND_CARD					103									//发牌消息
#define SUB_S_GAME_END					104									//游戏结束
#define SUB_S_OPEN_START				115									//开始分牌
#define SUB_S_OPEN_CARD					105									//用户分牌
#define SUB_S_GIVE_UP					106									//用户放弃
#define SUB_S_REQUEST_RULE				108									//请求设置
#define SUB_S_USER_AGREE_SCORE			111									//询问底分
#define SUB_S_UPDATE_SCORE				112									//更新分数

#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#endif


//用户同意
struct CMD_S_UserAgreeScore
{

	LONGLONG                            lBaseScore;							//坐庄庄注
	WORD								wSetRuleUser;						//庄家用户
};

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;							//基础积分
	BYTE								cbGameRule;							//规则标志
	WORD								wSetRuleUser;						//规则玩家
	BYTE								cbRuleOption;						//规则配置
};

//游戏状态
struct CMD_S_StatusInvest
{
	WORD								wBankerUser;						//庄家用户
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	bool								bInvestFinish[GAME_PLAYER];			//完成标志
	LONGLONG							lCellScore;							//基础积分
	LONGLONG							lUserScore[GAME_PLAYER];			//用户积分
	BYTE								cbGameRule;							//规则标志
	BYTE								cbRuleOption;						//规则配置
};

//游戏状态
struct CMD_S_StatusScore
{
	WORD								wCurrentUser;						//当前用户
	WORD								wBankerUser;						//庄家用户

	BYTE								cbHandCard[2];						//用户扑克
	BYTE								cbMingCard[GAME_PLAYER][2];			//用户扑克
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态

	LONGLONG							lCellScore;							//基础积分
	LONGLONG							lUserScore[GAME_PLAYER];			//用户积分
	LONGLONG							lTotalScore[GAME_PLAYER];			//总注数目
	LONGLONG							lMinAddScore;						//最小加注
	LONGLONG							lTurnMaxScore;						//最大下注
	LONGLONG							lTurnMinScore;						//最小下注
	BYTE								cbGameRule;							//规则标志
	BYTE								cbRuleOption;						//规则配置
};

//游戏状态
struct CMD_S_StatusOpenCard
{
	WORD								wBankerUser;						//庄家用户

	BYTE								cbOpenFinish[GAME_PLAYER];			//完成标志
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态

	BYTE								cbHandCard[2];						//用户扑克
	BYTE								cbMingCard[GAME_PLAYER][2];			//用户扑克

	LONGLONG							lCellScore;							//基础积分
	LONGLONG							lUserScore[GAME_PLAYER];			//用户积分
	LONGLONG							lTotalScore[GAME_PLAYER];			//总注数目
	BYTE								cbGameRule;							//规则标志
	BYTE								cbRuleOption;						//规则配置
};

//更新分数
struct CMD_S_UpdateScore
{
	WORD								wChairID;							//椅子标识
	LONGLONG							lUserScore;							//用户分数
};

//////////////////////////////////////////////////////////////////////////

//游戏开始
struct CMD_S_GameStart
{
	WORD								wBankerUser;						//庄家用户
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	LONGLONG							lUserScore[GAME_PLAYER];			//用户积分
};

//用户下本
struct CMD_S_UserInvest
{
	WORD								wChairID;							//用户玩家
	LONGLONG							lScore;								//用户筹码	
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wGameStatus;						//游戏状态
	WORD								wGiveUpUser;						//放弃用户
	LONGLONG							lLostScore;							//输掉金币	
};

//用户下注
struct CMD_S_AddScore
{
	WORD								wAddScoreUser;						//加注用户
	WORD								wCurrentUser;						//当前用户
	LONGLONG							lAddScoreCount;						//加注数目
	LONGLONG							lMinAddScore;						//最小加注
	LONGLONG							lTurnMaxScore;						//最大下注
	LONGLONG							lTurnMinScore;						//最小下注	
};

//游戏结束
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax[GAME_PLAYER];				//游戏税收
	LONGLONG							lGameScore[GAME_PLAYER];			//游戏得分
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//用户扑克
	BYTE								cbEndReason;						//结束原因
	WORD                                wMaxUser;							//最大用户
};

//发牌数据包
struct CMD_S_SendCard
{
	BYTE								cbCardCount;						//扑克数目
	WORD								wCurrentUser;						//当前用户
	LONGLONG							lMinAddScore;						//最小加注
	BYTE								cbHandCard[2];						//用户扑克
	BYTE								cbMingCard[GAME_PLAYER][2];			//用户扑克	
};


//用户摊牌
struct CMD_S_Open_Card
{
	WORD								wChairID;							//摊牌用户
};
//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_USER_INVEST				1									//用户下本
#define SUB_C_ADD_SCORE					2									//用户加注
#define SUB_C_OPEN_CARD					3									//用户摊牌
#define SUB_C_GIVE_UP					4									//用户放弃
#define SUB_C_SEND_CARD				5								//发送配牌


//发送配牌
struct CMD_C_SendCard
{
	BOOL									bAllotFlag;															//配牌标识
	//BYTE									cbControlGameCount;										//控制次数
	//WORD								wBankerUser;													//控制庄家
	BYTE									cbCardData[GAME_PLAYER][MAX_COUNT];			//扑克数据
};

//用户叫庄
struct CMD_C_UserInvest
{
	LONGLONG							lInvestGold;						//下本数目
};

//用户加注
struct CMD_C_AddScore
{
	BYTE								cbCode;								//加注代码
	LONGLONG							lScore;								//加注数目
};

//用户开牌
struct CMD_C_OpenCard
{
	BYTE								cbCardData[MAX_COUNT];				//用户扑克
};

//用户叫庄
struct CMD_C_BankerBet
{
	LONGLONG                            lBaseScore;							//底注分数
};

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif
