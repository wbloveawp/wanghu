#pragma once
#pragma warning(disable:6385)

#include "../消息定义/CMD_HongZhongDGK.h"


#define DOOR_TYPE_PENG	1
#define DOOR_TYPE_GANG	2

typedef struct {

	char		hand_num;
	char		wild;
	carder		hand_cards[COLOR_NUM][10];

	char		door_num;
	carder		door_color_num[COLOR_NUM];

	carder		door_info[3][4];//最多三个,0:type(1 peng、2 gang) 1:chairid 2:card 3:(1 an,2 paogang 3penggang)       

	char		double_num;
	char		triple_num;
	char		quadra_num;

	int			final_type_num;
	e_hu_type	final_type[e_hu_type::H_TYPE_MAX];

	void*		data;
}mj_cards;

constexpr int __all_cards_num = 4 * 9 * COLOR_NUM + 4;

class CLogic
{
	struct combine_t {
		char		cidx;
		char		combine[4][4];
	};
	
	struct table_info_t {
		int idx;
		carder all_cards[__all_cards_num];
	};

public:
	CLogic();
	virtual ~CLogic();

public:
	void rand_cards();
	carder take_card(carder cd=C_INVALID);
	void take_control_card(carder cd);
	void fill_hand_cards(mj_cards &mj);

	int cards();
	bool is_init();
public:
	void add_card(mj_cards& mj, const carder& cd);
	bool del_card(mj_cards& mj, const carder& cd);
public:
	bool is_hu(mj_cards& mj, const carder& cd);

	bool peng(mj_cards& mj,char chair, const carder& cd);

	char gang(mj_cards& mj, char chair, const carder& cd);

protected:

	int make_hu(const mj_cards& mj, const carder& cd);
	bool make_combine(const mj_cards& mj, const carder& cd);
protected:
	void update_hand_data(mj_cards& mj);
	e_hu_type qidui_check(const mj_cards& mj);
	e_hu_type duidui_check(const mj_cards& mj);

	e_hu_type tongse_check(const mj_cards& mj);
	e_hu_type no19_check(const mj_cards& mj);
	e_hu_type aal19_check(const mj_cards& mj);
	e_hu_type ka5_check(const mj_cards& mj);
protected:
	static const int __check_array[2][9]; 

	table_info_t tinfo;

	//for control
	carder	cards_info[COLOR_NUM][10];
	int		wilds;
};

