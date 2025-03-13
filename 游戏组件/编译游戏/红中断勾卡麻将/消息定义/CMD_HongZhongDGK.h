#pragma once

#define KIND_ID						150									//游戏 I D 血战麻将302
#define GAME_NAME					TEXT("红中断卡勾二人麻将")					//游戏名字

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

#define COLOR(c) ((c>>4)&0x0F) 
#define VALUE(c) (c&0x0F)

#define IS_WILD(c) (c==C_WILD)

#define KA5SHUNZI	0x04
#define KEZI		0x03
#define DUIZI		0x02
#define SHUNZI		0x01

#define INIT_CARD_NUM	10	

typedef unsigned char carder;

constexpr int __all_cards_num = 4 * 9 * COLOR_NUM + 4;

#define HU_TYPE_NULL
#define HU_TYPE_5DUI
#define HU_TYPE_LONGDUI1
#define HU_TYPE_LONGDUI2
#define HU_TYPE_DUIDUI
#define HU_TYPE_JIANGDUI
#define HU_TYPE_NO19
#define HU_TYPE_ALL19
#define HU_TYPE_KA5
#define HU_TYPE_TONGSE

#define HU_TYPE_TIANHU
#define HU_TYPE_DIHU
		
#define HU_TYPE_GANG_QIANG
#define HU_TYPE_GANG_PAO
#define HU_TYPE_GANG_HUA



enum e_hu_type {
	H_NULL = 0,	//平胡
	H_5DUI = 1,
	H_LONGDUI1,
	H_LONGDUI2,
	H_DUIDUI,
	H_JDUIDUI,

	H_NO19,
	H_KA5,
	H_ALL19,
	H_TONGSE,

	H_GANG_QIANG,
	H_GANG_PAO,
	H_GANG_HUA,

	H_BAOTING,
	H_QINGHU,
	H_HAIDI,

	H_TIANHU,
	H_DIHU,

	H_GEN,

	H_TYPE_MAX
};

#define OPT_TIME_OUT	30000

#define OPT_NULL		0x00
#define OPT_PASS		0x01
#define OPT_OUT			0x02
#define OPT_PENG		0x04
#define OPT_GANG		0x08
#define OPT_HU			0x10
#define OPT_QINGHU		0x20
#define OPT_ZHUAQINGHU	0x40
#define OPT_BAOTING		0x80

#define GANG_PAO		0x01 
#define GANG_PENG		0x02
#define GANG_AN			0x03

#define OPT_OK(o,x)		((o&x)==x)


//服务器命令
#define SUB_S_GAME_START			100									//游戏开始	
#define SUB_S_TAKE_CARD				101									//摸牌
#define SUB_S_PASS_CARD				102									//过牌
#define SUB_S_OUT_CARD				103									//出牌

#define SUB_S_PENG_CARD				104									//碰
#define SUB_S_GANG_CARD				105									//杠
#define SUB_S_HU_CARD				106									//胡
#define SUB_S_QINGHU				107									//请胡
#define SUB_S_ZHUAQINGHU			108									//抓请胡
#define SUB_S_BAOTING				109									//报听

#define SUB_S_CONCLUD_NOW			110									//结算本局
#define SUB_S_CONCLUD_OVER			111									//结算本场


#pragma   pack(1) 

typedef struct {
	//我的信息
	unsigned short		my_chair;
	char				wilds;//红中数量
	carder				cards[COLOR_NUM][10];//[c][v] c代表颜色(0:筒，1：条)，v代表牌值，[c][0]表示c颜色总牌数
	char				opt;
	

	//桌面信息
	int					hand_num[GAME_PLAYER];			//手上牌数
	carder				door_info[GAME_PLAYER][3][4];	//碰杠信息,[0][0][i] i值解释 0:type(1 peng、2 gang) 1:chairid 2:card 3:(1 an,2 paogang 3penggang)
	char				_baoting[GAME_PLAYER];
	carder				_table_cards[GAME_PLAYER][__all_cards_num];
	unsigned short		banker;
		
	int					left_cards;//剩余未摸的牌数
}s_sence_data_t;

typedef struct {	//游戏开始

	unsigned short		banker;
	unsigned short		my_chair;
	char				wilds;//红中数量
	carder				cards[COLOR_NUM][10];//[c][v] c代表颜色(0:筒，1：条)，v代表牌值，[c][0]表示c颜色总牌数
	char				opt;//可以的操作,对于庄家是否可以报听，客户端决定(玩家打出牌的服务器无法预测)，服务器只校验
	
	//char				wait_baoting;
}s_game_start_t;


typedef struct {

	unsigned short		chair;	//

}s_user_baoting_t;

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

	int					hu_types_num;
	e_hu_type			hu_types[e_hu_type::H_TYPE_MAX];

	int					score;
}s_hu_t;

typedef struct {

	carder				cd;
	unsigned short		chair;

}s_qinghu_t;


typedef struct {

	unsigned short		chair;

}s_baoting_t;

typedef struct {

	unsigned short		winer;	//winder

	int					final_type_num;	//牌型数量
	int					final_type[e_hu_type::H_TYPE_MAX];//类型数组
	int					gens;			//根数
	int					ka5;			//卡数
	int					gang_score;		//杠获得的分
	int					final_score;	//最终得分

	carder				hand_cards[GAME_PLAYER][COLOR_NUM][10];//所有人手牌信息
						//hand_cards[chair][c][0] 表示颜色为c的总牌数，[chair][c][v] 表示颜色为c，值为v的牌的总数
	int					hongzhongs[GAME_PLAYER];//手中红中数量
}s_conclude_now_t;

//客户端命令
#define SUB_C_PASS				1001								//过
#define SUB_C_OUT				1002								//出
#define SUB_C_PENG				1003								//碰
#define SUB_C_GANG				1004								//杠
#define SUB_C_HU				1005								//胡
#define SUB_C_QINGHU			1006								//请胡
#define SUB_C_BAOTING			1006								//报听
#define SUB_C_ZHUAQINGHU		1007								//抓请胡


#define SUB_C_CONTROL_CARD		999									//控制摸牌
//出牌
typedef struct {
	carder		cd;
	char		is_baoting;	//是否报听,首轮庄家有用
}c_out_card_t;

//其他操作均使用此数据结构
typedef struct {
	carder		cd;	//如果是炮胡，则把炮胡的牌上传，如果是自摸也自摸的那张牌上传
}c_opt_card_t;

#pragma   pack() 