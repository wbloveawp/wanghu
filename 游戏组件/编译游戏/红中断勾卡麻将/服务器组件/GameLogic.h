#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <vector>
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#define C_ALL_NUM	72
#define C_MAX	11		//最大牌数
#define C_INIT	10		//初始牌数
#define C_WILD	0xF0	//癞子牌
#define C_TONG 0
#define C_TIAO 1
const char mj_color[][4] = { "筒","条" };

#define COLOR_NUM 2		//花色数量
#define MK_CARD(c,v) (((c<<4)&0xF0) | (v&0x0F))	//生成一张花色c，值为v的牌
#define COLOR(c) ((c>>4)&0x0F)	//获取牌的花色
#define VALUE(c) (c&0x0F)		//获取牌的值
#define IS_WILD(c) (c==C_WILD)	//是否是癞子牌

typedef unsigned char carder;	//牌类型

typedef struct {

	char		hand_num;		//当前手牌数
	char		wild;			//癞子牌数
	carder		hand_cards[COLOR_NUM][10];//手牌信息 [][0]表示对应花色总牌数

	char		door_num;		//门前牌数
	carder		door_cards[COLOR_NUM][10];//门前牌信息 [][0]对应花色总牌数

	char		double_num;		//当前手中对子数

	char		final_type[6];
}user_cards_t;

//////////////////////////////////////////////////////////////////////////
//动作定义

//动作标志
enum class e_user_action {
	e_chupai = 0x01
};


class CGameLogic
{
	using card_vector = std::vector<carder> ;

	card_vector c_vec;

public:
	CGameLogic();
	~CGameLogic() {};

	//生成牌
	void rand_cards(user_cards_t* uc, carder* table_cards, char type=0,WORD good_chairid=0);

};

//////////////////////////////////////////////////////////////////////////

#endif
