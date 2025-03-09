#include "operator_logic_u2.h"

CTableLogic::CTableLogic(WORD max_cycles, ITableFrameItem* pTableFrameSink)
	:_banker(INVALID_CHAIR)
	, _over_num(0)
	, _total_cycles(0)
	, _max_cycles(max_cycles)
	, _last_cycle_banker(INVALID_CHAIR)
	, _last_banker(INVALID_CHAIR)
	, _table(pTableFrameSink)
{
	_pass_fun = [](WORD, carder, bool) {};
}

CTableLogic::~CTableLogic()
{

}

void CTableLogic::send(WORD chair,WORD cmd , void* msg, WORD len)
{
	_table->SendTableData(chair, cmd, msg, len);
}

void CTableLogic::new_cycle()
{
	_logic.rand_cards();
	clean_opt();
	memset(_pm, 0, sizeof(_pm));
	for (WORD i = 0; i < GAME_PLAYER; i++){
		_logic.fill_hand_cards(_pm[i]);
	}
	
	if (_last_cycle_banker !=INVALID_CHAIR) {
		_banker = _last_cycle_banker;
	}
	else
	{
		_banker = rand() * GAME_PLAYER / RAND_MAX;
	}
	_last_banker = INVALID_CHAIR;
	//下发
	WORD baoting_chair = INVALID_CHAIR;
	s_game_start_t gs[GAME_PLAYER] = {};
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		gs[i].banker = _banker;
		gs[i].my_chair = i;
		gs[i].wilds = _pm[i].wild;
		if (i != _banker) {

			_opt[i] = make_user_opt(_pm[i], C_WILD, 0);
			if (OPT_OK(_opt[i], OPT_HU)) {
				_opt[i] = OPT_BAOTING;
				_opt[i] |= OPT_PASS;
			}
			gs[i].opt = _opt[i];
			if (gs[i].opt>OPT_NULL) {
				assert(OPT_OK(_opt[i], OPT_BAOTING));
				baoting_chair = i;
				_table->SetGameTimer(TM_IDE_BAOTING, OPT_TIME_OUT, 1, 0);
				_pass_fun = [this, i](WORD chair, carder cd , bool is_timeout) {
					assert(i == chair);
					if (i != chair) {
						return;
					}
					if (!is_timeout) {
						this->_table->KillGameTimer(TM_IDE_BAOTING);
					}
					this->user_take_card(this->_banker);
				};

				_timeout_fun = [this,i]() {
					this->operator_pass(i, C_INVALID, true);
				};
			}
		}
		memcpy(gs[i].cards, _pm[i].hand_cards, sizeof(gs[i].cards));
		send(i, SUB_S_GAME_START,&gs[i], sizeof(gs[i]));
	}	
	if (baoting_chair==INVALID_CHAIR) {
		user_take_card(_banker);
	}
}

void CTableLogic::user_take_card(WORD chair, carder cd)
{
	assert(_logic.cards() > 0);
	clean_opt();
	cd = _logic.take_card(cd);
	_opt[chair] = make_user_opt(_pm[chair],cd,1);
	_logic.add_card(_pm[chair], cd);
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		s_take_card_t tc = { cd,i,_opt[i] };
		if (i != chair) {
			tc.cd = C_INVALID;
		}
		send(i, SUB_S_TAKE_CARD, &tc, sizeof(tc));
	}
	//超时代打
}

char CTableLogic::make_user_opt(mj_cards& mj,carder cd, char is_taker)
{
	//qing hu,hu,peng,gang,chu
	char opt = OPT_NULL;
	opt|=is_taker? OPT_OUT : OPT_PASS;
	if (_logic.is_hu(mj, cd)) {
		opt |= OPT_HU;
		//请胡判断

	}

	if (mj.hand_cards[COLOR(cd)][VALUE(cd)] == 3){
		opt |= OPT_GANG;
	}
	for (int i = 0; i < 3 && mj.door_info[i][0]>0; i++) {
		if (mj.door_info[i][0] == DOOR_TYPE_PENG &&
			mj.door_info[i][2] == cd) {
			opt |= OPT_GANG;
			break;
		}
	}
	if (mj.hand_cards[COLOR(cd)][VALUE(cd)] == 2) {
		opt |= OPT_PENG;
	}
	return  opt;
}

void CTableLogic::clean_opt()
{
	memset(_opt, 0, sizeof(_opt));
	memset(_status, 0, sizeof(_status));
	_pgh_card = C_INVALID;
	_pgh_card_owner = INVALID_CHAIR;
	_pass_fun = [](WORD, carder, bool) {};
	_timeout_fun = [](){};
}

void CTableLogic::operator_pass(WORD chair, carder cd, bool is_tmout)
{
	_pass_fun(chair, cd, is_tmout);
}

bool CTableLogic::operator_out(WORD chair, carder cd, bool is_tmout)
{
	assert(chair == _banker);
	if (chair != _banker) {
		return false;
	}

	if (!_logic.del_card(_pm[chair], cd)) {
		return false;
	}
	clean_opt();
	s_out_card_t oc = {cd,chair ,OPT_NULL};

	for (WORD i = 0; i < GAME_PLAYER; i++){
		if (i == chair) {
			continue;
		}
		_opt[i] = make_user_opt(_pm[i], cd, 0);
		if (_opt[i] > OPT_NULL) {
			oc.my_opt = _opt[i];
			_table->SetGameTimer(TM_IDE_PGH, OPT_TIME_OUT, 1, 0);
			_pass_fun = [this,i](WORD chair_id, carder cder, bool is_timeout) {
				assert(i == chair_id);
				if (chair_id != i) {
					return;
				}
				if (!is_timeout) {
					this->_table->KillGameTimer(TM_IDE_PGH);
				}
				this->change_next_banker(i);
				this->user_take_card(this->_banker);
			};
			_timeout_fun = [this, i,cd]() {
				this->operator_pass(i, cd, true);
			};
			_pgh_card = cd;
			_pgh_card_owner = chair;
		}
		send(i, SUB_S_OUT_CARD, &oc, sizeof(oc));
	}

	if (oc.chair > OPT_NULL) {
		return true;
	}

	change_next_banker();
	user_take_card(_banker);

	return true;
}

bool CTableLogic::operator_peng(WORD chair, carder cd)
{
	assert(OPT_OK(_opt[chair], OPT_PENG));
	assert(_pgh_card == cd);
	if (!OPT_OK(_opt[chair], OPT_PENG)|| _pgh_card != cd) {
		return false;
	}

	clean_opt();

	_logic.peng(_pm[chair], _pgh_card_owner, cd);
	change_next_banker(chair); 
	_opt[chair] = OPT_OUT;

	for (WORD i = 0; i < GAME_PLAYER; i++){
		s_peng_t peng = { cd,i };
		send(i, SUB_S_PENG_CARD, &peng, sizeof(peng));
	}

	return true;
}

bool CTableLogic::operator_gang(WORD chair , carder cd )
{
	char gang_type = 0x00;
	assert(OPT_OK(_opt[chair], OPT_GANG));
	if (!OPT_OK(_opt[chair], OPT_GANG)) {
		return false;
	}

	clean_opt();

	gang_type = _logic.gang(_pm[chair], _pgh_card == C_INVALID? _banker : chair, cd);
	assert(gang_type);
	s_gang_t gang = { cd,chair,0,OPT_NULL };
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		if (i == chair) {
			continue;
		}
		
		if (gang_type == GANG_PENG) {
			auto opt = make_user_opt(_pm[i], cd);
			if (OPT_OK(opt, OPT_HU)) {
				_opt[i] = opt;
				_opt[i] |= OPT_PASS;
				_table->SetGameTimer(TM_IDE_PGH,OPT_TIME_OUT,1,0);
				_pass_fun = [this, i, chair,cd](WORD wchair, carder cder, bool is_timeout) {

					assert(i == wchair);
					assert(cder==cd);
					if (!is_timeout) {
						_table->KillGameTimer(TM_IDE_PGH);
					}
					this->_status[chair] = OPT_GANG;
					this->change_next_banker(chair);
					this->user_take_card(this->_banker);
				};
				_timeout_fun = [this,i,cd]() {
					this->operator_pass(i,cd,true);
				};
			}
		}
		send(i, SUB_S_GANG_CARD,&gang,sizeof(gang));
	}

	_status[chair] = OPT_GANG;
	change_next_banker(chair);
	user_take_card(_banker);
	return true;

}

bool CTableLogic::operator_hu(WORD chair, carder cd)
{
	assert(OPT_OK(_opt[chair], OPT_HU));

	if (!OPT_OK(_opt[chair], OPT_HU)) {
		return false;
	}
	conclude_table(chair);
}

bool CTableLogic::operator_qinghu(WORD chair, carder cd)
{
	assert(OPT_OK(_opt[chair], OPT_QINGHU));
	assert(_banker == chair);
	if (!OPT_OK(_opt[chair], OPT_QINGHU)|| _banker != chair) {
		return false;
	}

	bool have_qinghu = false;
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		if (i == chair) {
			_opt[i] = make_user_opt(_pm[i], cd);
			if (OPT_OK(_opt[i], OPT_HU)) {
				have_qinghu = true;
				_opt[i] = OPT_PASS;
				_opt[i] |= OPT_QINGHU;
				_table->SetGameTimer(TM_IDE_QINGHU,OPT_TIME_OUT,1,0);
				_pass_fun = [this, i, cd, chair](WORD wchair, carder cder, bool is_tmout) {
					assert(i == wchair);
					if (i != wchair) {
						return;
					}
					if (!is_tmout) {
						this->_table->KillGameTimer(TM_IDE_QINGHU);
					}
					this->conclude_table(chair);
				};
				_timeout_fun = [this,i,cd]() {
					this->operator_pass(i, cd, true);
				};
				s_qinghu_t qh = { cd,chair };
				//发送请胡
				send(i, SUB_S_QINGHU, &qh, sizeof(qh));
			}
		}
	}

	if (!have_qinghu) {
		conclude_table(chair);
	}
}

void CTableLogic::conclude_table(WORD winer)
{

	if (++_total_cycles >= _max_cycles) {
		conclude_game();
	}
}

void CTableLogic::conclude_game()
{
	_table->ConcludeGame(TEXT("ok"));
}