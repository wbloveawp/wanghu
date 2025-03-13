#pragma once
#pragma warning(disable:6385)

#include "../消息定义/CMD_HongZhongDGK.h"


#define DOOR_TYPE_PENG	1
#define DOOR_TYPE_GANG	2

const char str_hu_type_name[e_hu_type::H_TYPE_MAX][16] = {
	"无",
	"五对",
	"龙对1",
	"龙对2",
	"对对胡",
	"将对对",
	"断幺九",
	"卡五",
	"带幺九",
	"同色",
};

struct combine_t {
	char		cidx;
	char		combine[4][4];
};

typedef struct {

	char		hand_num;	//含wild
	char		wild;
	carder		hand_cards[COLOR_NUM][10];
	
	char		door_num;
	carder		door_cards[COLOR_NUM][10];

	carder		door_info[3][4];//最多三个,0:type(1 peng、2 gang) 1:chairid 2:card 3:(1 an,2 paogang 3penggang)       

	char		double_num;
	char		triple_num;
	char		quadra_num;

	int			final_type_num;
	int			final_type[e_hu_type::H_TYPE_MAX];

	void*		data;
}mj_cards;



class CLogic
{
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
public:
	void add_card(mj_cards& mj, const carder& cd);
	bool del_card(mj_cards& mj, const carder& cd);
public:
	bool is_hu(mj_cards& mj, const carder& cd,bool is_taker);

	e_hu_type is_5dui(const mj_cards& mj,const carder & cd);

	bool peng(mj_cards& mj,char chair, const carder& cd);

	char gang(mj_cards& mj, char chair, const carder& cd,bool is_taker);

protected:

	int make_hu(const mj_cards& mj, const carder& cd);
	bool make_combine(const mj_cards& mj, const carder& cd);
	bool make_combine19(const mj_cards& mj);
protected:
	void		update_hand_data(mj_cards& mj);

	e_hu_type	qidui_check(const mj_cards& mj);
	e_hu_type	duidui_check(const mj_cards& mj);

	e_hu_type	tongse_check(const mj_cards& mj);
	e_hu_type	no19_check(const mj_cards& mj);
	e_hu_type	ka5_check(const mj_cards& mj,const carder& cd=C_INVALID);



	bool		make_19(const mj_cards& mj);
	e_hu_type	aal19_check(const mj_cards& mj);
	
protected:

	table_info_t tinfo;

	//for control
	carder	cards_info[COLOR_NUM][10];
	int		wilds;
};


/*
	2025-03-13
	遗留点：
	1.抓请胡的结算
	2.卡5 的卡数
	3.带幺九判定算法还可以继续改进，提高效率
*/