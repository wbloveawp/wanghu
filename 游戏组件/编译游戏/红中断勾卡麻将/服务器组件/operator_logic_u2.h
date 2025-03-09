#pragma once
#include "logic.h"
#include "Stdafx.h"
#include <functional>
#ifndef SCORE 
#define SCORE long long
#endif

#define TM_IDE_BASE_ID	10
#define TM_IDE_BAOTING	TM_IDE_BASE_ID+1
#define TM_IDE_PGH		TM_IDE_BASE_ID+2
#define TM_IDE_QINGHU	TM_IDE_BASE_ID+3

#define STATUS_NULL		0x00 
#define STATUS_PGH		0x01 
#define STATUS_BAOTING	0x02
#define STATUS_QINGHU	0x03

#define STATUS_TIMEOUT_ID(s)	(TM_IDE_BAOTING+s)

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
	char				_status[GAME_PLAYER];
	char				_baotinhg[GAME_PLAYER];

	carder				_pgh_card;
	WORD				_pgh_card_owner;
	ITableFrameItem*	_table;

	std::function<void(WORD, carder, bool)> _pass_fun;// _pass_fun[GAME_PLAYER];
	std::function<void(void)> _timeout_fun;
	//¿ØÖÆ
	carder				_control_card;
	WORD				_control_chair;

protected:
	void		clean_opt();
public:
	CTableLogic(WORD max_cycles, ITableFrameItem* pTableFrameSink);
	virtual ~CTableLogic();


	void new_cycle();

	void user_take_card(WORD chair, carder cd=C_INVALID);

	char make_user_opt(mj_cards& mj,carder cd, char is_taker=0);
	//char make_user_baoting(mj_cards& mj, carder cd);

	void change_next_banker(WORD chair = INVALID_CHAIR) {};

	void operator_pass(WORD chair , carder cd, bool is_tmoutt = false);
	bool operator_out(WORD chair, carder cd, bool is_tmoutt = false);
	bool operator_peng(WORD chair ,carder cd);
	bool operator_gang(WORD chair, carder cd);
	bool operator_hu(WORD chair, carder cd);
	bool operator_qinghu(WORD chair, carder cd);
	//bool conclude_check() { return true; };
	//void conclude_user(WORD bk) {};
	void conclude_table(WORD winer);
	void conclude_game();

public:
	void send(WORD chair, WORD cmd, void* msg, WORD len);
};
