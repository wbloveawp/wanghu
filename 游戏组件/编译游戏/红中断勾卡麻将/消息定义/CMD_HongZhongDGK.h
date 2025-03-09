#pragma once

#define KIND_ID						150									//��Ϸ I D Ѫս�齫302
#define GAME_NAME					TEXT("Ѫս�齫")					//��Ϸ����

//�������
#define GAME_PLAYER					2									//��Ϸ����
#define VERSION_SERVER					PROCESS_VERSION(8,0,1)			//����汾
#define VERSION_CLIENT					PROCESS_VERSION(8,0,1)			//����汾

//���ඨ��
#define C_WILD		0xF0	//������
#define C_INVALID	0x00	//δ֪��
#define C_TONG 0
#define C_TIAO 1

#define COLOR_NUM 2
#define MK_CARD(c,v) (((c<<4)&0xF0) | (v&0x0F))

#define COLOR(c) ((c>>4)&C_WILD) 
#define VALUE(c) (c&C_WILD)
#define IS_WILD(c) (c==C_WILD)

#define SHUNZI19	0x04
#define KEZI		0x03
#define DUIZI		0x02
#define SHUNZI		0x01

#define INIT_CARD_NUM	10	

typedef unsigned char carder;

enum e_hu_type {
	H_NULL = 0,
	H_5DUI = 1,
	H_LONGDUI1,
	H_LONGDUI2,
	H_DUIDUI,
	H_JDUIDUI,
	H_NO19,
	H_KA5,
	H_ALL19,
	H_TIANHU,
	H_DIHU ,
	H_TONGSE,
	H_TYPE_MAX
};

const char str_hu_type_name[][16] = {
	"��",
	"���",
	"����1",
	"����2",
	"�ԶԺ�",
	"���Զ�",
	"���۾�",
	"����",
	"���۾�",
};

/*
*{
	"��һɫ",
	"��",
	"���",
	"�غ�",
	"����"
}
*/

#define OPT_TIME_OUT	30000

#define OPT_NULL		0x00
#define OPT_PASS		0x01
#define OPT_OUT			0x02
#define OPT_PENG		0x04
#define OPT_GANG		0x08
#define OPT_HU			0x10
#define OPT_QINGHU		0x20
#define OPT_BAOTING		0x40

#define GANG_PAO		0x01 
#define GANG_PENG		0x02
#define GANG_AN			0x03

#define OPT_OK(o,x)		(o&x==x)


//����������
#define SUB_S_GAME_START			100									//��Ϸ��ʼ	
#define SUB_S_TAKE_CARD				101									//����
#define SUB_S_PASS_CARD				102									//����
#define SUB_S_OUT_CARD				103									//����

#define SUB_S_PENG_CARD				104									//��
#define SUB_S_GANG_CARD				105									//��
#define SUB_S_HU_CARD				106									//��
#define SUB_S_QINGHU				107									//���
#define SUB_S_BAOTING				108									//����

#define SUB_S_CONTROL_CARD			999									//��������
typedef struct {

}s_sence_data_t;

typedef struct {	//��Ϸ��ʼ

	unsigned short		banker;
	unsigned short		my_chair;
	char				wilds;//��������
	carder				cards[COLOR_NUM][10];//[c][v] c������ɫ(0:Ͳ��1����)��v������ֵ��[c][0]��ʾc��ɫ������
	char				opt;//���ԵĲ���,����ׯ���Ƿ���Ա������ͻ��˾���(��Ҵ���Ƶķ������޷�Ԥ��)��������ֻУ��
	
	//char				wait_baoting;
}s_game_start_t;

typedef struct {	//����

	carder				cd;		//������
	unsigned short		chair;	//˭����Ҳ��ׯ��
	char				opt;	//�ҿ��ԵĲ���

}s_take_card_t;	

typedef struct {	//����

	carder				cd;
	unsigned short		chair;//������
	char				my_opt;

}s_out_card_t;


typedef struct {	//��

	carder				cd;
	unsigned short		chair;
	unsigned short		banker;//���ƺ��ׯ��
	char				type;//0.���� 1.��ʱ
	char				status;//0.�� 1.���ܺ� 1.��� 2.����
}s_pass_t;

typedef struct {	//��
	carder				cd;
	unsigned short		chair;//
}s_peng_t;

typedef struct {

	carder				cd;	
	unsigned short		chair;//
	char				type;//1:�ڸ� 2������ 3������
	char				my_opt;

}s_gang_t;

typedef struct {
	carder				cd;
	unsigned short		chair;

}s_hu_t;

typedef struct {

	carder				cd;
	unsigned short		chair;

}s_qinghu_t;

//�ͻ�������
#define SUB_C_PASS				1001								//��
#define SUB_C_OUT				1002								//��
#define SUB_C_PENG				1003								//��
#define SUB_C_GANG				1004								//��
#define SUB_C_HU				1005								//��
#define SUB_C_QINGHU			1006								//���
#define SUB_C_BAOTING			1006								//����

typedef struct {
	carder		cd;
	char		is_baoting;	//�Ƿ���
}c_out_card_t;

typedef struct {
	carder		cd;
}c_opt_card_t;
