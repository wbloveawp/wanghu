
// DataDefine.h : 头文件
//

#pragma once

//#pragma pack(1)
//////////////////////////////////////////////////////////////////////////

#define VESION							3				//当前版本
#define MAX_ALLOT_COUNT					54				//分配牌数
#define MAX_CARDS_NUMBER					4				//扑克副数
#define MAX_PLAYER_COUNT					8				//座位数目
#define MAX_DIPAI_COUNT					5				//底牌数目
#define DIPAI_CHAIRID						0xFFFF			//底牌座位
#define DIPAI_CTRL_INDEX					8				//控件索引
#define MAX_GAME_PLAYER					8					//最大人数
#define MAX_GAME_HAND_CARD				20					//最大手牌

//////////////////////////////////////////////////////////////////////////
//文件结构
struct tagFileHead
{
	BYTE cbFlags1;
	BYTE cbFlags2;
	BYTE cbFlags3;
	BYTE cbFlags4;
	DWORD dwVesion;	
};

//配牌信息
struct tagAllotCardInfo
{
	BYTE cbDipaiCard;
	BYTE cbLaiziCard;
	DWORD dwCardsNum;
	DWORD dwChairCount;
};

//座位数据
struct tagChairCardData
{
	WORD wChairID;
	WORD wCardCount;
	BYTE cbCardData[MAX_ALLOT_COUNT];
};


//发送配牌
struct tagConfigCard
{
	BOOL									bAllotFlag;						//配牌标识
	BYTE									cbCardData[200];				//扑克数据
};