#pragma once
#include "logic.h"
#include "Stdafx.h"
#include <functional>


#define TM_IDE_BASE_ID	10
#define TM_IDE_BAOTING	TM_IDE_BASE_ID+1
#define TM_IDE_PGH		TM_IDE_BASE_ID+2
#define TM_IDE_QINGHU	TM_IDE_BASE_ID+3
#define TM_IDE_DAIDA	TM_IDE_BASE_ID+4

#define STATUS_NULL		0x00 
#define STATUS_PGH		0x01 
#define STATUS_BAOTING	0x02
#define STATUS_QINGHU	0x03

#define STATUS_TIMEOUT_ID(s)	(TM_IDE_BAOTING+s)

#define MAX_FANS	6

const int hu_type_fans[e_hu_type::H_TYPE_MAX] = {
	1 ,		//平胡
	2 ,		//5对
	3 ,		//龙1
	4 ,		//龙2
	1 ,		//对对
	5 ,		//将对对

	1 ,		//断幺九
	1 ,		//卡5
	4 ,		//带幺九
	2 ,		//清一色

	6 ,		//天胡
	6 ,		//地胡

	1 ,		//抢杠
	1 ,		//杠上炮
	1 ,		//杠上花

	2 ,		//报听
	2 ,		//请胡
	1 		//海底
};

#define HU_ZIMO_MUL	2

class CTableLogic
{
	//WORD				_mbanker;
	WORD				_banker;
	WORD				_last_cycle_banker;
	WORD				_last_banker;
	WORD				_over_num;
	WORD				_total_cycles;
	WORD				_max_cycles;
	CLogic				_logic;

	mj_cards			_pm[GAME_PLAYER];
	char				_opt[GAME_PLAYER];
	char				_baoting[GAME_PLAYER];

	char				_gang_status[GAME_PLAYER];

	int					_score[GAME_PLAYER];

	WORD				_out_num;
	WORD				_peng_num;
	WORD				_gang_num;

	carder				_pgh_card;
	WORD				_pgh_card_owner;
	ITableFrameItem*	_table;

	std::function<void(WORD, carder, bool)> _pass_fun;// _pass_fun[GAME_PLAYER];
	//std::function<bool(WORD, carder, bool)> _hu_fun;
	std::function<void(void)> _timeout_fun;
	//控制
	carder				_control_card;
	WORD				_control_chair;

	int					base_score;
protected:
	void		clean_opt();

	void		hu_qiang_gang();
	void		hu_fang_pao();
	void		hu_gang_pao();
public:
	CTableLogic(WORD max_cycles, ITableFrameItem* pTableFrameSink);
	virtual ~CTableLogic();


	void new_cycle();

	void user_take_card(WORD chair, bool is_gang = false , carder cd = C_INVALID);

	char make_user_opt(mj_cards& mj,carder cd, char is_taker=0);
	//char make_user_baoting(mj_cards& mj, carder cd);

	void change_next_banker(WORD chair = INVALID_CHAIR) {};

	void operator_pass(WORD chair , carder cd, bool is_tmoutt = false);
	bool operator_out(WORD chair, carder cd,bool baoting = false , bool is_tmoutt = false);
	bool operator_peng(WORD chair ,carder cd);
	bool operator_gang(WORD chair, carder cd);
	bool operator_hu(WORD chair, carder cd,bool is_qinghu=false);
	bool operator_qinghu(WORD chair, carder cd);
	bool operator_zhuaqinghu(WORD chair, carder cd);
	//bool conclude_check() { return true; };
	//void conclude_user(WORD bk) {};
	void conclude_table(WORD winer);
	void conclude_game();

public:
	void send(WORD chair, WORD cmd, void* msg, WORD len);
};
