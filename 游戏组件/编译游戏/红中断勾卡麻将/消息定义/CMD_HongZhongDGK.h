#pragma once

#define KIND_ID						150									//游戏 I D 血战麻将302
#define GAME_NAME					TEXT("血战麻将")					//游戏名字

//组件属性
#define GAME_PLAYER					2									//游戏人数
#define VERSION_SERVER					PROCESS_VERSION(8,0,1)			//程序版本
#define VERSION_CLIENT					PROCESS_VERSION(8,0,1)			//程序版本

//牌类定义
#define C_WILD		0xF0	//红中牌
#define C_INVALID	0x00	//未知牌
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
	"无",
	"五对",
	"龙对1",
	"龙对2",
	"对对胡",
	"将对对",
	"断幺九",
	"卡五",
	"带幺九",
};

/*
*{
	"清一色",
	"勾",
	"天胡",
	"地胡",
	"海底"
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


//服务器命令
#define SUB_S_GAME_START			100									//游戏开始	
#define SUB_S_TAKE_CARD				101									//摸牌
#define SUB_S_PASS_CARD				102									//过牌
#define SUB_S_OUT_CARD				103									//出牌

#define SUB_S_PENG_CARD				104									//碰
#define SUB_S_GANG_CARD				105									//杠
#define SUB_S_HU_CARD				106									//胡
#define SUB_S_QINGHU				107									//请胡
#define SUB_S_BAOTING				108									//报听

#define SUB_S_CONTROL_CARD			999									//控制摸牌
typedef struct {

}s_sence_data_t;

typedef struct {	//游戏开始

	unsigned short		banker;
	unsigned short		my_chair;
	char				wilds;//红中数量
	carder				cards[COLOR_NUM][10];//[c][v] c代表颜色(0:筒，1：条)，v代表牌值，[c][0]表示c颜色总牌数
	char				opt;//可以的操作,对于庄家是否可以报听，客户端决定(玩家打出牌的服务器无法预测)，服务器只校验
	
	//char				wait_baoting;
}s_game_start_t;

typedef struct {	//摸牌

	carder				cd;		//摸的牌
	unsigned short		chair;	//谁摸，也是庄家
	char				opt;	//我可以的操作

}s_take_card_t;	

typedef struct {	//出牌

	carder				cd;
	unsigned short		chair;//出牌者
	char				my_opt;

}s_out_card_t;


typedef struct {	//过

	carder				cd;
	unsigned short		chair;
	unsigned short		banker;//过牌后的庄家
	char				type;//0.主动 1.超时
	char				status;//0.无 1.碰杠胡 1.请胡 2.报听
}s_pass_t;

typedef struct {	//碰
	carder				cd;
	unsigned short		chair;//
}s_peng_t;

typedef struct {

	carder				cd;	
	unsigned short		chair;//
	char				type;//1:炮杠 2：碰杠 3：暗杠
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

//客户端命令
#define SUB_C_PASS				1001								//过
#define SUB_C_OUT				1002								//出
#define SUB_C_PENG				1003								//碰
#define SUB_C_GANG				1004								//杠
#define SUB_C_HU				1005								//胡
#define SUB_C_QINGHU			1006								//请胡
#define SUB_C_BAOTING			1006								//报听

typedef struct {
	carder		cd;
	char		is_baoting;	//是否报听
}c_out_card_t;

typedef struct {
	carder		cd;
}c_opt_card_t;
