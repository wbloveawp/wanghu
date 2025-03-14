#pragma once
#include "logic.h"
#include "Stdafx.h"
#include <functional>


#define TM_IDE_BASE_ID		10
#define TM_IDE_BAOTING		TM_IDE_BASE_ID+1
#define TM_IDE_PGH			TM_IDE_BASE_ID+2
#define TM_IDE_QINGHU		TM_IDE_BASE_ID+3
#define TM_IDE_DAIDA		TM_IDE_BASE_ID+4

#define TM_IDE_NEW_CYCLE	TM_IDE_BASE_ID+10

#define STATUS_NULL		0x00 
#define STATUS_PGH		0x01 
#define STATUS_BAOTING	0x02
#define STATUS_QINGHU	0x03

#define STATUS_TIMEOUT_ID(s)	(TM_IDE_BAOTING+s)



#define HU_ZIMO_MUL	2

class CTableLogic
{
	//WORD				_mbanker;
	WORD				_banker;
	WORD				_last_cycle_banker;
	WORD				_last_banker;
	WORD				_over_num;
	CLogic				_logic;

	
	mj_cards			_pm[GAME_PLAYER];
	char				_opt[GAME_PLAYER];
	char				_baoting[GAME_PLAYER];

	char				_gang_status[GAME_PLAYER];

	WORD				_table_card_num[GAME_PLAYER];
	carder				_table_cards[GAME_PLAYER][__all_cards_num];

	WORD				_out_num;
	WORD				_peng_num;
	WORD				_gang_num;

	carder				_pgh_card;
	WORD				_pgh_card_owner;
	ITableFrameItem*	_table;

	std::function<void(WORD, carder, bool)> _pass_fun;// _pass_fun[GAME_PLAYER];
	//std::function<bool(WORD, carder, bool)> _baoting;
	std::function<void(void)> _timeout_fun;
	//¿ØÖÆ
	carder				_control_card;
	WORD				_control_chair;

	SCORE				_base_score;
	SCORE				_min_win;
	SCORE				_revenue;

protected:
	void		clean_opt();

public:
	CTableLogic(WORD max_cycles, ITableFrameItem* pTableFrameSink);
	virtual ~CTableLogic();


	void new_cycle();

	void user_take_card(WORD chair, bool is_gang = false , carder cd = C_INVALID);

	char make_user_opt(mj_cards& mj,carder cd, bool is_taker);
	//char make_user_baoting(mj_cards& mj, carder cd);

	void change_next_banker(WORD chair = INVALID_CHAIR);

	void operator_pass(WORD chair , carder cd, bool is_tmoutt = false);
	bool operator_out(WORD chair, carder cd,bool baoting = false , bool is_tmoutt = false);
	bool operator_peng(WORD chair ,carder cd);
	bool operator_gang(WORD chair, carder cd);
	bool operator_hu(WORD chair, carder cd,bool is_qinghu=false);
	bool operator_qinghu(WORD chair, carder cd);
	bool operator_zhuaqinghu(WORD chair, carder cd) { return  true; };
	bool operator_baoting(WORD chair, carder cd);
	//bool conclude_check() { return true; };
	//void conclude_user(WORD bk) {};
	void conclude_table(WORD winer, int types, e_hu_type hu_type[],int fans,int s);
	void conclude_game();

	void send_playing_scence(IServerUserItem* su); 
	void send_free_scence(IServerUserItem* su);
protected:
	void send(WORD chair, WORD cmd, void* msg, WORD len);

public:
	void set_base_score(SCORE lbase_score) { _base_score = lbase_score; }
	void set_min_win(SCORE lmin_win) { _min_win = lmin_win; }
	void set_revenue(SCORE lrevenue) { _revenue = lrevenue; }
};
