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
	_out_num = 0;
	_peng_num = 0;
	_gang_num = 0;
	memset(_pm, 0, sizeof(_pm));
	memset(_baoting, 0, sizeof(_baoting));
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
			else {
				_opt[i] = OPT_NULL;
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
						_table->KillGameTimer(TM_IDE_BAOTING);
					}
					user_take_card(_banker);
				};

				_timeout_fun = [this,i]() {
					user_take_card(_banker);
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

void CTableLogic::user_take_card(WORD chair, bool is_gang ,carder cd)
{
	assert(_logic.cards() > 0);

	clean_opt();

	cd = _logic.take_card(cd);

	_opt[chair] = make_user_opt(_pm[chair],cd,1);

	if (_baoting[chair]) {
		DWORD tm_out = OPT_TIME_OUT;
		if (_opt[chair]>= OPT_GANG) {
			auto opt = _opt[chair];
			_opt[chair] = OPT_PASS;
			
			if (OPT_OK(opt, OPT_GANG)) {
				auto mj = _pm[chair];
				_logic.add_card(mj, cd);
				_logic.gang(mj, chair, cd);
				if (_logic.is_hu(mj,C_WILD)) {
					_opt[chair] |= OPT_GANG;
				}
			}

			if (OPT_OK(opt, OPT_HU)) {
				_opt[chair] |= OPT_HU;
			}

			_pass_fun = [this,chair,cd](WORD wchair, carder cder, bool is_tmout) {
				_table->KillGameTimer(TM_IDE_DAIDA);
				_opt[chair] = OPT_OUT;
				operator_out(chair, cd, false, true);
			};
		}
		else
		{
			tm_out = 1000;
			_opt[chair] = OPT_NULL;
		}
		_table->SetGameTimer(TM_IDE_DAIDA, tm_out, 1, 0);
		_timeout_fun = [this, chair, cd]() {
			_opt[chair] = OPT_OUT;
			operator_out(chair, cd, false, true);
		};
	}
	
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		s_take_card_t tc = { cd,i,_opt[i] };
		if (i != chair) {
			tc.cd = C_INVALID;
		}
		send(i, SUB_S_TAKE_CARD, &tc, sizeof(tc));
	}

	_logic.add_card(_pm[chair], cd);
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

	if (is_taker) {
		for (int i = 0; i < 3 && mj.door_info[i][0]>0; i++) {
			if (mj.door_info[i][0] == DOOR_TYPE_PENG &&
				mj.door_info[i][2] == cd) {
				opt |= OPT_GANG;
				break;
			}
		}
	}
	else {
		if (mj.hand_cards[COLOR(cd)][VALUE(cd)] == 2) {
			opt |= OPT_PENG;
		}
	}

	return  opt;
}

void CTableLogic::clean_opt()
{
	memset(_opt, 0, sizeof(_opt));
	_pgh_card = C_INVALID;
	_pgh_card_owner = INVALID_CHAIR;
	_pass_fun = [](WORD, carder, bool) {};
	_timeout_fun = [](){};
}

void CTableLogic::operator_pass(WORD chair, carder cd, bool is_tmout)
{
	_pass_fun(chair, cd, is_tmout);
}

bool CTableLogic::operator_out(WORD chair, carder cd, bool baoting, bool is_tmout)
{
	assert(chair == _banker);
	if (chair != _banker) {
		return false;
	}

	if (!_logic.del_card(_pm[chair], cd)) {
		return false;
	}

	if (baoting) {
		assert(_out_num + _peng_num + _gang_num == 0);
		
		if (_out_num + _peng_num + _gang_num == 0 && _logic.is_hu(_pm[chair], C_WILD)) {

			_baoting[chair] = 1;
			s_user_baoting_t ub = { chair };
			for (WORD i = 0; i < GAME_PLAYER; i++) {
				send(i, SUB_S_BAOTING, &ub, sizeof(ub));
			}
		}
	}

	clean_opt();
	++_out_num;

	s_out_card_t oc = {cd,chair ,OPT_NULL};

	for (WORD i = 0; i < GAME_PLAYER; i++){
		if (i == chair) {
			continue;
		}
		_opt[i] = make_user_opt(_pm[i], cd, 0);
		if (_opt[i] > OPT_NULL) {
			oc.my_opt = _opt[i];
			_table->SetGameTimer(TM_IDE_PGH, OPT_TIME_OUT, 1, 0);
			_pass_fun = [this,i, chair](WORD chair_id, carder cder, bool is_timeout) {
				assert(i == chair_id);
				if (chair_id != i) {
					return;
				}
				if (!is_timeout) {
					_table->KillGameTimer(TM_IDE_PGH);
				}
				_gang_status[chair] = 0;
				change_next_banker();
				user_take_card(_banker);
			};

			_timeout_fun = [this, i,cd]() {
				operator_pass(i, cd, true);
			};

			_pgh_card = cd;
			_pgh_card_owner = chair;
		}
		send(i, SUB_S_OUT_CARD, &oc, sizeof(oc));
	}

	if (oc.my_opt > OPT_NULL) {
		return true;
	}
	_gang_status[chair] = 0;
	change_next_banker();
	user_take_card(_banker);

	return true;
}

bool CTableLogic::operator_peng(WORD chair, carder cd)
{
	assert(OPT_OK(_opt[chair], OPT_PENG));
	assert(_pgh_card == cd);
	assert(chair == _pgh_card_owner);
	if (!OPT_OK(_opt[chair], OPT_PENG)|| 
		_pgh_card != cd || 
		chair != _pgh_card_owner) {
		return false;
	}
	++_peng_num;

	_logic.peng(_pm[chair], _pgh_card_owner, cd);

	change_next_banker(chair); 

	clean_opt();

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
	++_gang_num;
	gang_type = _logic.gang(_pm[chair], _pgh_card == C_INVALID? _banker : chair, cd);
	assert(gang_type);
	clean_opt();
	s_gang_t gang = { cd,chair,0,OPT_NULL };
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		if (i == chair) {
			continue;
		}
		
		if (gang_type == GANG_PENG) {
			auto opt = make_user_opt(_pm[i], cd);
			if (OPT_OK(opt, OPT_HU)) {
				_opt[i] = OPT_HU;
				_opt[i] |= OPT_PASS;
				_table->SetGameTimer(TM_IDE_PGH,OPT_TIME_OUT,1,0);
				_pass_fun = [this, i, chair,cd](WORD wchair, carder cder, bool is_timeout) {

					assert(i == wchair);
					assert(cder==cd);
					if (!is_timeout) {
						_table->KillGameTimer(TM_IDE_PGH);
					}
					change_next_banker(chair);
					user_take_card(_banker);
				};
				_timeout_fun = [this,i,cd]() {
					operator_pass(i,cd,true);
				};
				gang.my_opt = _opt[i];
			}
		}
		send(i, SUB_S_GANG_CARD,&gang,sizeof(gang));
	}
	_gang_status[chair] = 1;

	if (!OPT_OK(gang.my_opt, OPT_HU)) {
		change_next_banker(chair);
		user_take_card(_banker);
	}

	return true;

}

bool CTableLogic::operator_hu(WORD chair, carder cd, bool is_qinghu)
{
	assert(OPT_OK(_opt[chair], OPT_HU));
	if (!OPT_OK(_opt[chair], OPT_HU)) {
		return false;
	}

	SCORE final_score = 0;
	WORD fans = 1;
	int idx = 0;
	e_hu_type hu_types[e_hu_type::H_TYPE_MAX] = { e_hu_type::H_NULL };
	if (chair != _banker) {
		//放炮
		bool ok = _logic.is_hu(_pm[chair], cd);
		if (!ok || is_qinghu || _pm[chair].hand_num % 3 != 1) {
			assert(0);
			return false;
		}
		//地胡判断
		if (_out_num == 1 && _peng_num == 0 && _gang_num == 0) {
			fans = MAX_FANS;
			hu_types[0] = H_DIHU;
		}
		else {
			if (_gang_status[_banker]) {
				if (_pgh_card == cd) {
					//抢杠
					hu_types[idx++] = H_GANG_QIANG;
				}
				else {
					//杠上炮
					hu_types[idx++] = H_GANG_PAO;
				}
			}
		}

	}
	else {
		//自摸
		final_score += ( HU_ZIMO_MUL * base_score);
		_logic.del_card(_pm[chair], cd);
		bool ok = _logic.is_hu(_pm[chair], cd);
		if (!ok || _pm[chair].hand_num % 3 != 2) {
			assert(0);
			return false;
		}
		//天胡判断
		if (_out_num == 0 && _peng_num == 0 && _gang_num == 0) {
			fans = MAX_FANS;
			hu_types[0] = H_TIANHU;
		}
		else {
			if (_gang_status[_banker]) {
				//杠上花
				hu_types[idx++] = H_GANG_HUA;
			}
		}
	}

	do {
		if (fans >= MAX_FANS) {
			break;
		}
		memcpy(hu_types, _pm[chair].final_type, _pm[chair].final_type_num * sizeof(e_hu_type));
		idx += _pm[chair].final_type_num;

		if (_logic.cards() == 0) {
			//海底
			hu_types[idx++] = H_HAIDI;
		}

		if (is_qinghu) {
			//请胡
			hu_types[idx++] = H_QINGHU;
		}

		for (int i = 0; i < idx; i++){
			fans += hu_type_fans[hu_types[i]];
			if (fans >= MAX_FANS) {
				break;
			}
		}

		if (_baoting[chair]) {
			//报听
			if (fans > MAX_FANS) {
				break;
			}
		}

		//带根
		
	} while (0);

	if (fans > MAX_FANS) {
		fans = MAX_FANS;
	}
	if (fans <= 0) {
		fans = 1;
	}

	final_score += base_score * pow(2, fans - 1);
	//计算下雨分
	
	//下发

	_score[chair] += final_score;
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		if (i != chair) {
			_score[i] -= final_score;
		}
	}
	conclude_table(chair);
	return true;
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
		if (i != chair) {
			auto opt = make_user_opt(_pm[i], cd);
			if (OPT_OK(opt, OPT_HU)) {
				have_qinghu = true;
				_opt[i] = OPT_PASS;
				_opt[i] |= OPT_ZHUAQINGHU;
				_table->SetGameTimer(TM_IDE_QINGHU,OPT_TIME_OUT,1,0);
				_pass_fun = [this, i, cd, chair](WORD wchair, carder cder, bool is_tmout) {
					assert(i == wchair);
					if (i != wchair) {
						return;
					}
					if (!is_tmout) {
						_table->KillGameTimer(TM_IDE_QINGHU);
					}
					operator_hu(i, cd);
				};
				_timeout_fun = [this,i,cd]() {
					operator_pass(i, cd, true);
				};
				s_qinghu_t qh = { cd,chair };
				//发送请胡
				send(i, SUB_S_QINGHU, &qh, sizeof(qh));
			}
		}
	}

	if (!have_qinghu) {

		operator_hu(chair,cd);
	}
	return true;
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