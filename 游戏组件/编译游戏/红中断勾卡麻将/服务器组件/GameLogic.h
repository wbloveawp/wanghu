#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <vector>
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#define C_ALL_NUM	72
#define C_MAX	11		//�������
#define C_INIT	10		//��ʼ����
#define C_WILD	0xF0	//�����
#define C_TONG 0
#define C_TIAO 1
const char mj_color[][4] = { "Ͳ","��" };

#define COLOR_NUM 2		//��ɫ����
#define MK_CARD(c,v) (((c<<4)&0xF0) | (v&0x0F))	//����һ�Ż�ɫc��ֵΪv����
#define COLOR(c) ((c>>4)&0x0F)	//��ȡ�ƵĻ�ɫ
#define VALUE(c) (c&0x0F)		//��ȡ�Ƶ�ֵ
#define IS_WILD(c) (c==C_WILD)	//�Ƿ��������

typedef unsigned char carder;	//������

typedef struct {

	char		hand_num;		//��ǰ������
	char		wild;			//�������
	carder		hand_cards[COLOR_NUM][10];//������Ϣ [][0]��ʾ��Ӧ��ɫ������

	char		door_num;		//��ǰ����
	carder		door_cards[COLOR_NUM][10];//��ǰ����Ϣ [][0]��Ӧ��ɫ������

	char		double_num;		//��ǰ���ж�����

	char		final_type[6];
}user_cards_t;

//////////////////////////////////////////////////////////////////////////
//��������

//������־
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

	//������
	void rand_cards(user_cards_t* uc, carder* table_cards, char type=0,WORD good_chairid=0);

};

//////////////////////////////////////////////////////////////////////////

#endif
