#include "logic.h"
#include <assert.h>
#include <iostream>
#include <vector>
#define NEW_F

#ifdef _DEBUG
#define DLOG(...) printf(__VA_ARGS__);
#else
#define DLOG(...)
#endif

const int CLogic::__check_array[9] = { 5,2,3,4,6,7,8,1,9 };
									

const carder _base_cards[] = {
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
	C_WILD,C_WILD,C_WILD,C_WILD,
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19
};

const carder _init_cards_num[COLOR_NUM][10] = {
	{ 4 * 9, 4, 4, 4, 4, 4, 4, 4, 4, 4 },
	{ 4 * 9, 4, 4, 4, 4, 4, 4, 4, 4, 4 }
};


CLogic::CLogic()
	:wilds(0)
{
	memset(&tinfo ,0, sizeof(tinfo));
	memset(&cards_info, 0, sizeof(cards_info));
}

CLogic::~CLogic()
{

}

void CLogic::rand_cards()
{
	memset(&tinfo,0, sizeof(tinfo));
	std::vector<carder> vec(_base_cards, _base_cards +sizeof(_base_cards)/sizeof(carder));
	int n = 0;
	for (auto i = 0; i < vec.size(); i++) {
		n = rand() * vec.size() / RAND_MAX;
		tinfo.all_cards[i] = vec[n];
		vec.erase(vec.begin() + n);
	}
	wilds = 4;
	memcpy(cards_info, _init_cards_num, sizeof(cards_info));
}

void CLogic::fill_hand_cards(mj_cards& mj)
{
	carder c = C_INVALID;
	int n = INIT_CARD_NUM;
	while (n--) {
		c = tinfo.all_cards[tinfo.idx++];

		if (IS_WILD(c)) {
			--wilds;
			++mj.wild;
		}
		else {
			--cards_info[COLOR(c)][0];
			--cards_info[COLOR(c)][VALUE(c)];
			++mj.hand_cards[COLOR(c)][0];
			++mj.hand_cards[COLOR(c)][VALUE(c)];
		}
	}
	mj.hand_num = INIT_CARD_NUM;
	update_hand_data(mj);
}

void CLogic::take_control_card(carder cd)
{
	carder first = tinfo.all_cards[tinfo.idx];

	if (C_INVALID == cd || cd == first) {
		return ;
	}

	if (IS_WILD(cd) && wilds <= 0) {
		return;
	}

	if (cards_info[COLOR(cd)][VALUE(cd)] <= 0) {
		return;
	}
	
	for (int i = tinfo.idx + 1; i < __all_cards_num; i++) {
		//find then swap
		if (tinfo.all_cards[i] == cd) {
			tinfo.all_cards[i] = first;
			tinfo.all_cards[tinfo.idx] = cd;
			break;
		}
	}

}

carder CLogic::take_card(carder cd)
{
	if (tinfo.idx >= __all_cards_num) {
		assert(0);
		return C_INVALID;
	}

	take_control_card(cd);

	carder c = tinfo.all_cards[tinfo.idx++];
	if (IS_WILD(c)) {
		--wilds;
	}
	else {
		--cards_info[COLOR(c)][0];
		--cards_info[COLOR(c)][VALUE(c)];
	}
	return c;
}

int CLogic::cards()
{
	return __all_cards_num - tinfo.idx;
}

bool CLogic::make_combine19(const mj_cards& mj)
{
	auto _mj = mj;
	auto& cds = _mj.hand_cards;
	combine_t& cb = *((combine_t*)_mj.data);
	combine_t cb_bak = cb;
	for (char i = 0; i < COLOR_NUM; i++) //color
	{
		if (cds[i][0] % 3 == 1 || cds[i][0] <= 0) {//改进一，数量判断，数量为1 4 7 10 13 ，不可能直接返回
			return false;
		}
		
		for (char j = 1; j < 10; j++) { //value

			if (cds[i][j] <= 0) {
				continue;
			}

			if (j > 3 && j < 7) {
				continue;
			}

			if (_mj.hand_num == 2) {
				if (cds[i][0] == 2 && cds[i][j] == 2) {

					if (1 != j && 9 != j) {	//最后2张必须是19
						return false;
					}
					return true;
				}
				return false;
			}

			if (cds[i][j] >= 3 && (j==1 || j==9)) {//坎子必须是19
				cds[i][j] -= 3;
				cds[i][0] -= 3;
				_mj.hand_num -= 3;
				//assert(cb.cidx < 4);
				if (make_combine19(_mj)) {
					return true;
				}
				else {
					_mj = mj;
					cb = cb_bak;
				}
			}

			if (j >= 8) {
				continue;
			}

			if (cds[i][j] && cds[i][j + 1] && cds[i][j + 2]) {
				cds[i][j]--;
				cds[i][j + 1]--;
				cds[i][j + 2]--;

				cds[i][0] -= 3;
				_mj.hand_num -= 3;
				//assert(cb.cidx < 4);
				if (make_combine19(_mj)) {
					return true;
				}
				else {
					_mj = mj;
					cb = cb_bak;
				}
			}
		}
	}
	return false;
}

bool CLogic::make_combine(const mj_cards& mj, const carder& cd)
{
	auto _mj = mj;
	auto& cds = _mj.hand_cards;
	combine_t& cb = *((combine_t*)_mj.data);
	combine_t cb_bak = cb;
	for (char i = 0; i < COLOR_NUM; i++) //color
	{
#ifdef NEW_F
		if (cds[i][0] % 3 == 1) {//改进一，数量判断，数量为1 4 7 10 13 ，不可能直接返回
			return false;
		}
#endif
		if (cds[i][0] <= 0) {
			continue;
		}

		for (char j = 1; j < 10; j++) { //value

			if (cds[i][j] <= 0) {
				continue;
			}

			if (_mj.hand_num == 2) {
				if (cds[i][0] == 2 && cds[i][j] == 2) {
					cb.combine[cb.cidx][0] = DUIZI;
					cb.combine[cb.cidx++][1] = MK_CARD(i, j);
					return true;
				}
				return false;
			}
			//ka5
			if (cds[i][4] > 0 && cds[i][5] > 0 && cds[i][6] > 0) {
				--cds[i][4];
				--cds[i][5];
				--cds[i][6];
				cds[i][0] -= 3;
				_mj.hand_num -= 3;
				cb.combine[cb.cidx][0] = KA5SHUNZI;
				cb.combine[cb.cidx][1] = MK_CARD(i, 4);
				cb.combine[cb.cidx][2] = MK_CARD(i, 5);
				cb.combine[cb.cidx++][3] = MK_CARD(i,6);
				if (make_combine(_mj, cd)) {
					return true;
				}
				else {
					_mj = mj;
					cb = cb_bak;
				}
			}
			

			if (cds[i][j] >= 3) {
				cds[i][j] -= 3;
				cds[i][0] -= 3;
				_mj.hand_num -= 3;
				cb.combine[cb.cidx][0] = KEZI;
				cb.combine[cb.cidx++][1] = MK_CARD(i,j);
				assert(cb.cidx < 4);
				if (make_combine(_mj,cd)) {
					return true;
				}
				else {
					_mj = mj;
					cb = cb_bak;
				}
			}

			if (j >= 8) {
				continue;
			}

			if (cds[i][j] && cds[i][j + 1] && cds[i][j + 2]) {
				cds[i][j]--;
				cds[i][j + 1]--;
				cds[i][j + 2]--;

				cds[i][0] -= 3;
				_mj.hand_num -= 3;
				cb.combine[cb.cidx][0] = SHUNZI;
				cb.combine[cb.cidx][1] = MK_CARD(i, j);
				cb.combine[cb.cidx][2] = MK_CARD(i, j+1);
				cb.combine[cb.cidx++][3] = MK_CARD(i, j+2);
				assert(cb.cidx < 4);
				if (make_combine(_mj,cd)) {
					return true;
				}
				else {
					_mj = mj;
					cb = cb_bak;
				}
			}
		}
	}
	return false;
}

int CLogic::make_hu(const mj_cards& mj, const carder& cd)
{
	auto _mj = mj;
	char card_choos_flag[10] = { 0 };
	char card_wild_sort[10] = { 0 };
	int choose_n = 0;
	if (_mj.wild > 0) {
		--_mj.wild;
		for (auto c = 0; c < COLOR_NUM; c++) {
			if (_mj.hand_cards[c][0] <= 0 && _mj.door_cards[c][0]<=0) {
				//此色不匹配
				continue;
			}
			//重新设计排序牌
			for (auto v = 0; v < 9; v++) {
				if (_mj.hand_cards[c][__check_array[v]] > 3)
					continue;
				++_mj.hand_cards[c][0];
				++_mj.hand_cards[c][__check_array[v]];
				if (make_hu(_mj, cd)) {
					return true;
				}
				//数据先恢复
				memcpy(_mj.hand_cards, mj.hand_cards, sizeof(mj.hand_cards));
			}
			
		}
	}
	if (!make_combine(_mj,cd)) {
		memset(_mj.data, 0, sizeof(combine_t));
		return false;
	}
	return true;
}

void CLogic::add_card(mj_cards& mj, const carder& cd)
{
	if (IS_WILD(cd)) {
		++mj.wild;
	}
	else {
		assert(mj.hand_cards[COLOR(cd)][VALUE(cd)] < 4);
		++mj.hand_cards[COLOR(cd)][0];
		++mj.hand_cards[COLOR(cd)][VALUE(cd)];
	}
	++mj.hand_num;
	update_hand_data(mj);
}

bool CLogic::del_card(mj_cards& mj, const carder& cd)
{
	if (IS_WILD(cd)) {
		if (mj.hand_num > 0) {
			return  false;
		}
		--mj.wild;
	}
	else {
		--mj.hand_cards[COLOR(cd)][0];
		--mj.hand_cards[COLOR(cd)][VALUE(cd)];
	}
	--mj.hand_num;
	update_hand_data(mj);
	return true;
}

bool CLogic::is_hu(mj_cards &_mj, const carder& cd)
{
	memset(_mj.final_type,0, sizeof(_mj.final_type));
	mj_cards mj = _mj;
	combine_t cb = {};
	bool check_ok = false;
	carder cd_combine[4][4] = {};
	carder cd_wild_val = cd;
	int final_idx = 0;
	char qidui_flag = 0;
	mj.data = &cb;
	
	add_card(mj, cd);
	
	//special check
	DLOG("\n%x  ",cd);
	auto hu_type = e_hu_type::H_NULL;
	do {
		
		hu_type = aal19_check(mj);
		if (hu_type != e_hu_type::H_NULL) {
			DLOG("%s  ", str_hu_type_name[hu_type]);
		}
		hu_type = qidui_check(mj);
		if (hu_type != e_hu_type::H_NULL) {
			printf("%s  ", str_hu_type_name[hu_type]);
			break;
		}
		hu_type = duidui_check(mj);
		if (hu_type != e_hu_type::H_NULL) {
			printf("%s  ", str_hu_type_name[hu_type]);
			break;
		}
	} while (0);

	do {
		if (hu_type == e_hu_type::H_NULL) {
			break;
		}

		mj.final_type[final_idx++] = hu_type;

		if (hu_type == e_hu_type::H_JDUIDUI) {
			break;
		}

		if (tongse_check(mj) == e_hu_type::H_TONGSE) {
			printf(" %s  ", str_hu_type_name[e_hu_type::H_TONGSE]);
			mj.final_type[final_idx++] = e_hu_type::H_TONGSE;
		}

		if (no19_check(mj) == e_hu_type::H_NO19) {
			mj.final_type[final_idx++] = e_hu_type::H_NO19;
			printf(" %s  ", str_hu_type_name[e_hu_type::H_NO19]);
		}
		else {
			if (aal19_check(mj) == e_hu_type::H_ALL19) {
				mj.final_type[final_idx++] = e_hu_type::H_ALL19;
			}
		}

	} while (0); 
	
	if (hu_type != e_hu_type::H_NULL) {
		return true;
	}
	if (!make_hu(mj,cd)) {
		return false;
	}

	//1.清一色
	if (tongse_check(mj)) {
		mj.final_type[final_idx++] = e_hu_type::H_TONGSE;
	}
	//2.卡5
	if (IS_WILD(cd) || VALUE(cd) == 0x05) {
		if (ka5_check(mj)) {
			mj.final_type[final_idx++] = e_hu_type::H_KA5;
		}
	}
	//3.带幺和幺九
	if (aal19_check(mj)) {
		mj.final_type[final_idx++] = e_hu_type::H_ALL19 ;
	}
	else if(no19_check(mj)){
		mj.final_type[final_idx++] = e_hu_type::H_NO19;
	}

	return true;
}

void CLogic::update_hand_data(mj_cards& mj)
{
	mj.double_num = 0;
	mj.triple_num = 0;
	mj.quadra_num = 0;
	for (auto c = 0; c < COLOR_NUM; c++) {
		if (mj.hand_cards[c][0] <= 0) {
			continue;
		}
		for (auto v = 1; v < 10; v++) {
			switch (mj.hand_cards[c][v])
			{
			case 2:++mj.double_num; break;
			case 3:++mj.triple_num; break;
			case 4:++mj.quadra_num; break;
			default:break;
			}
		}
	}
}

e_hu_type CLogic::qidui_check(const mj_cards& mj)
{
	int t_num[] = { mj.quadra_num , mj.triple_num ,mj.double_num, mj.wild };

	if (mj.door_num > 0) {
		return e_hu_type::H_NULL;
	}

	if (mj.double_num + mj.triple_num + mj.quadra_num * 2 + mj.wild >= 5) {
		assert(mj.door_num == 0);
		assert(mj.door_cards[C_TONG][0] == 0);
		assert(mj.door_cards[C_TIAO][0] == 0);

		while (t_num[3]>0) {
			
			if (t_num[1] > 0) {
				--t_num[1];
				--t_num[3];
				++t_num[0];
				continue;
			}
			if (t_num[2] > 0 && t_num[3]>1) {
				--t_num[2];
				t_num[3] -= 2;
				++t_num[0];
				continue;
			}
			break;
		}
		if (t_num[0] == 2) {
			return e_hu_type::H_LONGDUI2;
		}
		if (t_num[0] == 1) {
			return e_hu_type::H_LONGDUI1;
		}
		return e_hu_type::H_5DUI;
	}
	return e_hu_type::H_NULL;
}


e_hu_type CLogic::tongse_check(const mj_cards& mj)
{
	if (mj.door_cards[C_TONG] == 0 && mj.hand_cards[C_TONG][0] == 0)
		return e_hu_type::H_TONGSE;
	if (mj.door_cards[C_TIAO] == 0 && mj.hand_cards[C_TIAO][0] == 0)
		return e_hu_type::H_TONGSE;
	return e_hu_type::H_NULL;
}

e_hu_type CLogic::duidui_check(const mj_cards& mj)
{
	auto _mj = mj;
	while (_mj.double_num > 0 && _mj.wild > 0) {
		--_mj.double_num;
		--_mj.wild;
		++_mj.triple_num;
	}
	
	while (_mj.wild > 1) {
		++_mj.triple_num;
		_mj.wild -= 2;
	}

	if (_mj.hand_num - _mj.triple_num * 3 == 2) {
		if (_mj.double_num == 1 || _mj.wild >= 1) {

			bool jiangduidui = true;
			for (char c = 0; c < COLOR_NUM; c++) { //1.check hand cards
				if (mj.hand_cards[c][0] <= 0)
					continue;
				for (char v = 1; v < 10; v++){
					if (v == 2 || v == 5 || v == 8) {
						continue;
					}	
					else{
						if (mj.hand_cards[c][v] > 0) {
							jiangduidui = false;
							break;
						}
					}
				}
				if (!jiangduidui) {
					break;
				}
			}

			if (!jiangduidui) {
				return e_hu_type::H_DUIDUI;
			}

			//2.check door cards
			if (mj.door_num <= 0) {
				return e_hu_type::H_JDUIDUI;
			}
			
			for (int i = 0; i < 3 && mj.door_info[i][0]>0; i++) {
				if (VALUE(mj.door_info[i][2]) % 3 != 2) {
					jiangduidui = false;
					break;
				}
			}
			
			return jiangduidui? e_hu_type::H_JDUIDUI : e_hu_type::H_DUIDUI;
		}
	}
	return e_hu_type::H_NULL;
}

e_hu_type CLogic::no19_check(const mj_cards& _mj)
{
	int n19 = _mj.hand_cards[0][1] + _mj.hand_cards[0][9] + _mj.hand_cards[1][1] + _mj.hand_cards[1][1] ;
	if (n19 > 0) {
		return e_hu_type::H_NULL;
	}

	if (_mj.door_num > 0) {
		for (int i = 0; i < 3 && _mj.door_info[i][0]>0; i++) {
			if (VALUE(_mj.door_info[i][2]) == 1 || VALUE(_mj.door_info[i][2]) == 9) {
				return  e_hu_type::H_NULL;
			}
		}
	}

	combine_t* cb = (combine_t*)_mj.data;
	for (int i = 0; i < cb->cidx; i++) {

		switch (cb->combine[i][0])
		{
		case KA5SHUNZI:return e_hu_type::H_NULL;
		case KEZI:
		case DUIZI:
			if (VALUE(cb->combine[i][1]) == 0x01)
				return e_hu_type::H_NULL;
			break;
		case SHUNZI:
			if (VALUE(cb->combine[i][1]) == 0x01 || VALUE(cb->combine[i][3])==0x09)
				return e_hu_type::H_NULL;
			break;
		default:
			break;
		}
	}

	return e_hu_type::H_NO19;
}

e_hu_type CLogic::aal19_check(const mj_cards& _mj)
{
	//hand
	if (_mj.hand_cards[C_TONG][4] + _mj.hand_cards[C_TONG][5] + _mj.hand_cards[C_TONG][6] > 0) {
		return e_hu_type::H_NULL;
	}
	if (_mj.hand_cards[C_TIAO][4] + _mj.hand_cards[C_TIAO][5] + _mj.hand_cards[C_TIAO][6] > 0) {
		return e_hu_type::H_NULL;
	}
	//door
	if (_mj.door_num > 0) {
		if (_mj.hand_cards[C_TIAO][4] + _mj.hand_cards[C_TIAO][5] + _mj.hand_cards[C_TIAO][6] > 0) {
			return e_hu_type::H_NULL;
		}

		if (_mj.hand_cards[C_TIAO][4] + _mj.hand_cards[C_TIAO][5] + _mj.hand_cards[C_TIAO][6] > 0) {
			return e_hu_type::H_NULL;
		}
	}

	return make_19(_mj)? e_hu_type::H_ALL19: e_hu_type::H_NULL ;
	/*
	combine_t* cb = (combine_t * )_mj.data;
	for (int i = 0; i < cb->cidx; i++) {
		switch (cb->combine[i][0]) 
		{
		case SHUNZI19:break;
		case KEZI:
		case DUIZI:
			if (VALUE(cb->combine[i][1]) != 0x09 && VALUE(cb->combine[i][1]) != 0x01) {
					return e_hu_type::H_NULL;
			}
			break;
		default:return e_hu_type::H_NULL;
		}
	}
	return e_hu_type::H_ALL19;
	*/
}
bool CLogic::make_19(const mj_cards& mj)
{
	auto _mj = mj;
	if (_mj.wild > 0) {
		--_mj.wild;
		for (auto c = 0; c < COLOR_NUM; c++) {

			if (_mj.hand_cards[c][0] <= 0 && _mj.door_cards[c][0] <= 0) {
				//此色不匹配
				continue;
			}
			char wc[] = { 1,9,2,3,7,8 };//带幺 7，8，2，3 不可能出现4张的
			for (auto i = 0x00; i < sizeof(wc) / sizeof(char); i++) {

				++_mj.hand_cards[c][0];
				++_mj.hand_cards[c][wc[i]];
				if (make_19(_mj)) {
					return true;
				}
				//匹配不成功，牌数据恢复
				memcpy(_mj.hand_cards, mj.hand_cards, sizeof(mj.hand_cards));
			}
		}
	}

	return make_combine19(mj);
}

e_hu_type CLogic::ka5_check(const mj_cards& mj)
{
	combine_t* cb = (combine_t*)mj.data;
	assert(cb);
	for (int i = 0; i < cb->cidx; i++) {
		if (cb->combine[i][0] == SHUNZI && VALUE(cb->combine[i][2])==0x05) {
			return e_hu_type::H_KA5;
		}
	}
	return e_hu_type::H_NULL;
}


bool CLogic::peng(mj_cards& mj, char chair, const carder& cd)
{
	mj.hand_cards[COLOR(cd)][0] -= 2;
	mj.hand_cards[COLOR(cd)][VALUE(cd)] -= 2;
	mj.hand_num -= 2;
	mj.door_num += 3;
	mj.door_cards[COLOR(cd)][0] += 3;
	for (int i = 0; i < 3; i++) {
		if (mj.door_info[i][0]<=0) {
			mj.door_info[i][0] = DOOR_TYPE_PENG;
			mj.door_info[i][1] = chair;
			mj.door_info[i][2] = cd;
		}
	}
	update_hand_data(mj);
	return  true;
}

char CLogic::gang(mj_cards& mj, char chair, const carder& cd)
{
	if (mj.hand_cards[COLOR(cd)][VALUE(cd)] == 3) {
	
		mj.hand_cards[COLOR(cd)][0] -= 3;
		mj.hand_cards[COLOR(cd)][VALUE(cd)] -= 3;
		mj.hand_num -= 3;
		mj.door_num += 4;
		mj.door_cards[COLOR(cd)][0] += 4;

		for (int i = 0; i < 3; i++) {
			if (mj.door_info[i][0] <= 0) {
				mj.door_info[i][0] = DOOR_TYPE_GANG;
				mj.door_info[i][1] = chair;
				mj.door_info[i][2] = cd;
				mj.door_info[i][3] = GANG_PAO;
			}
		}
		update_hand_data(mj);
		return GANG_PAO;
	}
	if(mj.hand_cards[COLOR(cd)][VALUE(cd)] == 4){
		mj.hand_cards[COLOR(cd)][0] -= 4;
		mj.hand_cards[COLOR(cd)][VALUE(cd)] -= 4;
		mj.hand_num -= 4;
		mj.door_num += 4;
		mj.door_cards[COLOR(cd)][0] += 4;

		for (int i = 0; i < 3; i++) {
			if (mj.door_info[i][0] <= 0) {
				mj.door_info[i][0] = DOOR_TYPE_GANG;
				mj.door_info[i][1] = chair;
				mj.door_info[i][2] = cd;
				mj.door_info[i][3] = GANG_AN;
			}
		}
		update_hand_data(mj);
		return GANG_AN;
	}

	char type = 0x00;
	for (int i = 0; i < 3; i++) {
		if (mj.door_info[i][0] == 1) {
			mj.door_info[i][0] = DOOR_TYPE_GANG;
			//mj.door_info[i][1] = chair;
			mj.door_info[i][2] = cd;
			mj.door_info[i][3] = GANG_PENG;
			type = GANG_PENG;
			update_hand_data(mj);
		}
	}

	return type;
}
