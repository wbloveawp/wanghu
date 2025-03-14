#include "operator_logic_u2.h"

#ifdef WB_DEBUG
#define	WASSERT(x)	do{					\
						if(!(x)){		\
							CTraceService::TraceString(TEXT("assert failed :%s %d (%s)"), \
									TraceLevel_Warning , __FILE__,__LINE__,#x);			\
						}				\
					}while(0) 
#else
#define	WASSERT(x)
#endif

CTableLogic::CTableLogic(WORD max_cycles, ITableFrameItem* pTableFrameSink)
	:_banker(INVALID_CHAIR)
	, _last_cycle_banker(INVALID_CHAIR)
	, _last_banker(INVALID_CHAIR)
	, _table(pTableFrameSink)
	, _base_score(1)
	, _control_card(C_INVALID)
	, _control_chair(INVALID_CHAIR)
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
	memset(_gang_status, 0, sizeof(_gang_status));
	memset(_table_card_num, 0, sizeof(_table_card_num));
	memset(_table_cards, 0, sizeof(_table_cards));


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

			_opt[i] = make_user_opt(_pm[i], C_WILD,false);
			if (OPT_OK(_opt[i], OPT_HU)) {
				_opt[i] = OPT_BAOTING;
				_opt[i] |= OPT_PASS;
			}
			else {
				_opt[i] = OPT_NULL;
			}
			gs[i].opt = _opt[i];
			if (gs[i].opt>OPT_NULL) {
				WASSERT(OPT_OK(_opt[i], OPT_BAOTING));
				baoting_chair = i;
				_table->SetGameTimer(TM_IDE_BAOTING, OPT_TIME_OUT, 1, 0);
				_pass_fun = [this, i](WORD chair, carder cd , bool is_timeout) {
					WASSERT(i == chair);
					if (i != chair) {
						return;
					}
					if (!is_timeout) {
						_table->KillGameTimer(TM_IDE_BAOTING);
					}
					user_take_card(_banker);
				};

				_timeout_fun = [this]() {
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
	WASSERT(_logic.cards() > 0);

	clean_opt();

	cd = _logic.take_card(cd);

	_opt[chair] = make_user_opt(_pm[chair],cd,true);

	if (_baoting[chair]) {
		DWORD tm_out = OPT_TIME_OUT;
		if (_opt[chair]>= OPT_GANG) {
			auto opt = _opt[chair];
			_opt[chair] = OPT_PASS;
			
			if (OPT_OK(opt, OPT_GANG)) {
				auto mj = _pm[chair];
				_logic.add_card(mj, cd);
				_logic.gang(mj, chair, cd,true);
				if (_logic.is_hu(mj,C_WILD,true)) {
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

char CTableLogic::make_user_opt(mj_cards& mj,carder cd, bool is_taker)
{
	//qing hu,hu,peng,gang,chu
	char opt = OPT_NULL;
	opt|=is_taker? OPT_OUT : OPT_PASS;
	if (_logic.is_hu(mj, cd, is_taker)) {
		opt |= OPT_HU;
		if (is_taker && _logic.is_5dui(mj, cd)>e_hu_type::H_NULL) {
			opt |= OPT_QINGHU;
			//后续生成可请的card list
		}
	}

	if (mj.hand_cards[COLOR(cd)][VALUE(cd)] == 3){
		opt |= OPT_GANG;
	}

	if (!is_taker && mj.hand_cards[COLOR(cd)][VALUE(cd)] == 2) {
		opt |= OPT_PENG;
	}

	if (is_taker && mj.door_cards[COLOR(cd)][VALUE(cd)] == 3) {
		opt |= OPT_GANG;
	}

	if (!OPT_OK(opt, OPT_GANG)) {
		for (auto c = 0; c < COLOR_NUM; c++) {
			if (mj.hand_cards[c][0] <= 0) {
				continue;
			}
			for (auto v = 1; v < 10; v++) {
				if (mj.hand_cards[c][v] > 0 && mj.hand_cards[c][v] + mj.door_cards[c][v] == 4) {
					opt |= OPT_GANG;
					return opt;
				}
			}
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

bool CTableLogic::operator_baoting(WORD chair, carder cd)
{
	WASSERT(OPT_OK(_opt[chair],OPT_BAOTING));
	if (!OPT_OK(_opt[chair], OPT_BAOTING)) {
		return false;
	}

	WASSERT(chair != _banker);
	WASSERT(_out_num + _peng_num + _gang_num == 0);

	//WASSERT(_table_card_num[0] ==0 && _table_card_num[1] == 0);

	_baoting[chair] = 1;
	//下发
	s_baoting_t bt = { chair };
	for (WORD i = 0; i > GAME_PLAYER; i++) {
		send(i, SUB_S_BAOTING, &bt, sizeof(bt));
	}
	user_take_card(_banker);
}

void CTableLogic::operator_pass(WORD chair, carder cd, bool is_tmout)
{
	_pass_fun(chair, cd, is_tmout);
}

bool CTableLogic::operator_out(WORD chair, carder cd, bool baoting, bool is_tmout)
{
	WASSERT(chair == _banker);
	if (chair != _banker) {
		return false;
	}

	if (!_logic.del_card(_pm[chair], cd)) {
		return false;
	}

	if (baoting) {
		WASSERT(_out_num + _peng_num + _gang_num == 0);
		
		if (_out_num + _peng_num + _gang_num == 0 && _logic.is_hu(_pm[chair], C_WILD,true)) {

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
		_opt[i] = make_user_opt(_pm[i], cd, false);
		if (_opt[i] > OPT_NULL) {
			oc.my_opt = _opt[i];
			_table->SetGameTimer(TM_IDE_PGH, OPT_TIME_OUT, 1, 0);
			_pass_fun = [this,i, chair,cd](WORD chair_id, carder cder, bool is_timeout) {
				WASSERT(i == chair_id);
				if (chair_id != i) {
					return;
				}
				if (!is_timeout) {
					_table->KillGameTimer(TM_IDE_PGH);
				}
				_gang_status[chair] = 0;
				_table_cards[chair][_table_card_num[chair]++] = cd;
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
	_table_cards[chair][_table_card_num[chair]++] = cd;
	change_next_banker();
	user_take_card(_banker);

	return true;
}

bool CTableLogic::operator_peng(WORD chair, carder cd)
{
	WASSERT(OPT_OK(_opt[chair], OPT_PENG));
	WASSERT(_pgh_card == cd);
	WASSERT(chair == _pgh_card_owner);
	WASSERT(chair != _banker);

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
	WASSERT(OPT_OK(_opt[chair], OPT_GANG));
	//WASSERT(_pgh_card == cd);
	//WASSERT(chair == _pgh_card_owner);
	if (!OPT_OK(_opt[chair], OPT_GANG)) {
		return false;
	}
	++_gang_num;
	gang_type = _logic.gang(_pm[chair], _pgh_card == C_INVALID? _banker : chair, cd, _banker == chair);
	WASSERT(gang_type);
	clean_opt();
	s_gang_t gang = { cd,chair,0,OPT_NULL };
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		if (i == chair) {
			continue;
		}
		
		if (gang_type == GANG_PENG) {
			WASSERT(_banker== chair);
			auto opt = make_user_opt(_pm[i], cd , false);
			if (OPT_OK(opt, OPT_HU)) {
				_opt[i] = OPT_HU;
				_opt[i] |= OPT_PASS;
				_table->SetGameTimer(TM_IDE_PGH,OPT_TIME_OUT,1,0);
				_pgh_card = cd;
				_pgh_card_owner = chair;
				_pass_fun = [this, i, chair,cd](WORD wchair, carder cder, bool is_timeout) {

					WASSERT(i == wchair);
					WASSERT(cder==cd);
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
	//is_qinghu true :if banker 则是qinghu 否则是zhuaqinghu
	WASSERT(OPT_OK(_opt[chair], OPT_HU));
	if (!OPT_OK(_opt[chair], OPT_HU)) {
		return false;
	}

	int final_score = 0;
	WORD fans = 1;
	int idx = 0;
	int gens = 0;
	int gang_score = 0;
	e_hu_type qing_hu_type = e_hu_type::H_NULL;
	int hu_types[e_hu_type::H_TYPE_MAX] = { e_hu_type::H_NULL };

	if (chair == _banker) {
		//自摸
		final_score += (HU_ZIMO_MUL * _base_score);
		if (is_qinghu) {
			qing_hu_type = _logic.is_5dui(_pm[chair], cd);
			WASSERT(qing_hu_type > e_hu_type::H_NULL);
			if (qing_hu_type == e_hu_type::H_NULL) {
				return false;
			}
			hu_types[idx++] = qing_hu_type;
		}
		else {
			//再校验一次，同时获取类型
			WASSERT(_pm[chair].hand_num % 3 == 2);
			_logic.del_card(_pm[chair], cd);
			if (!_logic.is_hu(_pm[chair], cd, true)) {
				WASSERT(0);
				return false;
			}
		}

		//天胡判断
		if (_out_num == 0 && _peng_num == 0 && _gang_num == 0) {
			fans = MAX_FANS;
			idx = 1;
			hu_types[0] = e_hu_type::H_TIANHU;
		}
		else {

			if (_gang_status[_banker]) {
				//杠上花
				hu_types[idx++] = e_hu_type::H_GANG_HUA;
			}
			for (WORD i = 0; i < GAME_PLAYER; i++){
				if (i != chair) {
					if (_baoting[i]) {
						hu_types[idx++] = e_hu_type::H_BAOTING;
						break;
					}
				}
			}
		}
	}
	else {
		//放炮
		bool ok = _logic.is_hu(_pm[chair], cd, false);
		if (!ok || is_qinghu || _pm[chair].hand_num % 3 != 1) {
			WASSERT(0);
			return false;
		}
		//地胡判断
		if (_out_num == 1 && _peng_num == 0 && _gang_num == 0 && _baoting[chair] == 1) {
			fans = MAX_FANS;
			hu_types[0] = e_hu_type::H_DIHU;
		}
		else {
			if (_gang_status[_banker]) {
				if (_pgh_card == cd) {
					//抢杠
					hu_types[idx++] = e_hu_type::H_GANG_QIANG;
				}
				else {
					//杠上炮
					hu_types[idx++] = e_hu_type::H_GANG_PAO;
				}
			}
			for (WORD i = 0; i < GAME_PLAYER; i++) {
				if (i != chair) {
					if (_baoting[i]) {
						hu_types[idx++] = e_hu_type::H_BAOTING;
						break;
					}
				}
			}
		}
	}

	//处理海底，带根
	do {
		if (fans >= MAX_FANS) {
			break;
		}
		memcpy(hu_types, _pm[chair].final_type, _pm[chair].final_type_num * sizeof(e_hu_type));
		idx += _pm[chair].final_type_num;

		if (_logic.cards() == 0) {
			//海底
			hu_types[idx++] = e_hu_type::H_HAIDI;
		}

		if (_baoting[chair]) {
			//报听
			hu_types[idx++] = e_hu_type::H_BAOTING;
		}

		//带根
		for (auto i = 0; i < 3; i++) {
			if (_pm[chair].door_info[i][0] == 0) {
				break;
			}
			//统计杠
			if (_pm[chair].door_info[i][0] == DOOR_TYPE_GANG) {
				gens++;
				gang_score += (_pm[chair].door_info[i][3] == GANG_PENG ?1:2)* _base_score;
			}
		}

		//统计最终牌型四张数

		//conclue fans
		for (int i = 0; i < idx; i++){
			fans += hu_type_fans[hu_types[i]];
		}
		fans += gens;
	} while (0);

	if (fans > MAX_FANS) {
		fans = MAX_FANS;
	}
	if (fans <= 0) {
		fans = 1;
	}

	final_score += _base_score * pow(2, fans - 1) + gang_score;
	//下发
	s_conclude_now_t cn = {};
	cn.winer = chair;
	cn.final_type_num = idx;
	memcpy(cn.final_type, hu_types, sizeof(hu_types));
	cn.gens = gens;
	cn.gang_score = gang_score;
	cn.final_score = final_score;

	for (WORD i = 0; i < GAME_PLAYER; i++){

		memcpy(cn.hand_cards[i], _pm[i].hand_cards, sizeof(_pm[i].hand_cards));
		cn.hongzhongs[i] = _pm[i].wild;
	}

	_table->GetTableUserItem(chair)->WriteUserScore(final_score,0,0,0, ScoreType_Win);
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		if (i != chair) {
			_table->GetTableUserItem(i)->WriteUserScore(final_score,0,0,0, ScoreType_Lose);
		}
		send(i, SUB_S_CONCLUD_NOW, &cn, sizeof(cn));
	}
	_table->ConcludeGame(TEXT("一局结束"));
	return true;
}

bool CTableLogic::operator_qinghu(WORD chair, carder cd)
{
	WASSERT(OPT_OK(_opt[chair], OPT_QINGHU));
	WASSERT(_banker == chair);
	if (!OPT_OK(_opt[chair], OPT_QINGHU)|| _banker != chair) {
		return false;
	}
	auto is_5dui = _logic.is_5dui(_pm[chair],cd);
	WASSERT(is_5dui>e_hu_type::H_NULL);
	if (is_5dui == e_hu_type::H_NULL) {
		
		return false;
	}

	bool have_qinghu = false;
	for (WORD i = 0; i < GAME_PLAYER; i++) {
		if (i != chair) {
			auto opt = make_user_opt(_pm[i], cd,false);
			if (OPT_OK(opt, OPT_HU)) {
				have_qinghu = true;
				_opt[i] = OPT_PASS;
				_opt[i] |= OPT_ZHUAQINGHU;
				_table->SetGameTimer(TM_IDE_QINGHU,OPT_TIME_OUT,1,0);
				_pass_fun = [this, i, cd, chair](WORD wchair, carder cder, bool is_tmout) {
					WASSERT(i == wchair);
					if (i != wchair) {
						return;
					}
					if (!is_tmout) {
						_table->KillGameTimer(TM_IDE_QINGHU);
					}
					operator_hu(chair, cd,true);
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

		operator_hu(chair,cd,true);
	}
	return true;
}


void CTableLogic::conclude_game()
{
	_table->ConcludeGame(TEXT("ok"));
}

void CTableLogic::send_free_scence(IServerUserItem* su)
{
	s_sence_data_free_t df = {};
	df.cellscore = _base_score;
	df.cbTimeOutCard = OPT_TIME_OUT/1000;
	df.cbTimeOperateCard = df.cbTimeOutCard;
	df.cbTimeStartGame = 2;
	df.cbTimeStartGame = 12;

	_table->OnEventSendGameScene(su, &df, sizeof(df));
}

void CTableLogic::send_playing_scence(IServerUserItem* su)
{
	s_sence_data_playing_t sd = {};
	WORD chair = 
	sd.my_chair = su->GetChairID();
	auto& mj = _pm[sd.my_chair];
	sd.wilds = mj.wild;
	memcpy(sd.cards, mj.hand_cards, sizeof(mj.hand_cards));
	sd.opt = _opt[sd.my_chair];
	sd.banker = _banker;
	sd.left_cards = _logic.cards();
	for (WORD i = 0; i < GAME_PLAYER; i++) {

		sd.hand_num[i] = _pm[i].hand_num;
		memcpy(sd.door_info[i], _pm[i].door_info, sizeof(_pm[i].door_info));
		sd._baoting[i] = _baoting[i];
		memcpy(sd._table_cards[i], _table_cards[i], sizeof(_table_cards[i]));
	}
	
	_table->OnEventSendGameScene(su, &sd, sizeof(sd));

}

void CTableLogic::change_next_banker(WORD chair)
{
	if (INVALID_CHAIR != chair) {
		_banker = chair;
		return;
	}

	_banker = ++_banker % GAME_PLAYER;
}