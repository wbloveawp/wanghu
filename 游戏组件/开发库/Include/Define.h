#ifndef DEFINE_HEAD_FILE
#define DEFINE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//数值定义

//头像大小
#define FACE_CX						96									//头像宽度
#define FACE_CY						96									//头像高度

#define MAX_CHAIR					200									//最大椅子
#define MAX_TABLE					512									//最大桌子
#define MAX_BATCH					48									//最大批次
#define MAX_ROBOT					2048								//最大机器
#define MAX_STATION_COUNT			100									//最多站点

#define MAX_KIND					128									//最大类型
#define MAX_SERVER					1024								//最大房间

#define INVALID_CHAIR				0xFFFF								//无效椅子
#define INVALID_TABLE				0xFFFF								//无效桌子

#define LEN_USER_CHAT				128									//聊天长度
#define MAX_NORMAL_FACE				123									//普通头像
#define MAX_CONVERSATION_USER		16									//会话人数
#define MAX_BUGLE_LEN				200									//喇叭长度
#define BUGLE_LIMIT_TIME			30L									//限制时间	

#define MAX_DAY						31									//最大天数
#define MAX_MONTH					12									//最大月份


//////////////////////////////////////////////////////////////////////////
//常量定义

#define MAX_ROBOT_CHIP_COUNT		5									//最大筹码(携带相关)

//////////////////////////////////////////////////////////////////////////
//系统参数

//游戏状态
#define GAME_STATUS_FREE			0									//空闲状态
#define GAME_STATUS_PLAY			100									//游戏状态
#define GAME_STATUS_WAIT			200									//等待状态

//系统参数
#define TIME_CHAT_INTERVAL			1L									//聊天间隔

//////////////////////////////////////////////////////////////////////////
//索引质数

//列表质数
#define PRIME_TYPE					11L									//种类数目
#define PRIME_KIND					53L									//类型数目
#define PRIME_NODE					101L								//节点数目
#define PRIME_SERVER				1009L								//房间数目
#define PRIME_MATCH					53L									//比赛数目
#define PRIME_CUSTOM				53L									//自定数目
#define PRIME_GATE					23L									//接入数目
#define PRIME_BATTLE				11L									//约战数目
#define PRIME_LOGIC					11L									//逻辑数目
#define PRIME_GROUP					11L									//群组数目
#define PRIME_STATUS				11L									//状态数目
#define PRIME_MESSAGE				3L									//消息数目
#define PRIME_STATION				11L									//站点数目

//人数质数
#define PRIME_SERVER_USER			1007L								//房间人数
#define PRIME_ROBOT_USER			2003L								//机器人数
#define PRIME_PLATFORM_USER			100003L								//平台人数

//////////////////////////////////////////////////////////////////////////
//数据长度

//资料数据
#define LEN_MD5						33									//加密密码
#define LEN_AREA					16									//地区长度
#define LEN_CITY					16									//城市长度
#define LEN_PROVINCE				16									//省份长度
#define LEN_ACCOUNTS				32									//帐号长度
#define LEN_NICKNAME				32									//昵称长度
#define LEN_PASSWORD				33									//密码长度
#define LEN_GROUP_NAME				32									//社团名字
#define LEN_UNDER_WRITE				64									//签名长度
#define LEN_PROTECT_QUESTION		32									//密保问题
#define LEN_PASSPORT_ID				32									//证件号码

//数据长度
#define LEN_QQ						16									//Q Q 号码
#define LEN_EMAIL					64									//电子邮件
#define LEN_UNIQUEID				33									//UID长度
#define LEN_USER_NOTE				256									//用户备注
#define LEN_SEAT_PHONE				33									//固定电话
#define LEN_MOBILE_PHONE			22									//移动电话
#define LEN_COMPELLATION			16									//真实名字
#define LEN_DWELLING_PLACE			128									//联系地址
#define LEN_AUTHCODE				7									//验证长度
#define LEN_VALIDATE_CODE			6									//验证长度
#define LEN_USER_DATUM				128									//用户资料
#define LEN_USER_FACE				256									//头像地址

//其他数据
#define LEN_DOMAIN					63									//域名长度	
#define LEN_ADDRESS					16									//地址长度
#define LEN_VALIDATE				33									//验证地址
#define LEN_ACCREDIT_ID				33									//授权号码
#define LEN_MACHINE_ID				33									//序列长度
#define	LEN_NETWORK_ID				13									//序列长度
#define LEN_PROPERTY_NAME			33									//道具名称
#define LEN_GROUP_INTRODUCE			128									//群组介绍
#define LEN_LINK_EXTEND				16									//链接扩展
#define LEN_INVITE_CODE				7									//邀请码
#define LEN_LOGON_TOKEN				256									//登录令牌
#define LEN_MACHINE_OSID			64									//序列长度

//视频定义
#define LEN_VIDEO_KEY				33									//秘钥长度
#define LEN_VIDEO_CHANNEL			22									//频道长度

//列表数据
#define LEN_TYPE					32									//种类长度
#define LEN_KIND					32									//类型长度
#define LEN_NODE					32									//节点长度
#define LEN_SERVER					32									//房间长度
#define LEN_CUSTOM					32									//定制长度
#define LEN_PROCESS					32									//进程长度

//库存定义
#define MAX_CARD_REPERTORY			152									//最大库存

//物品信息
#define LEN_GOODS_NAME				64									//物品长度
#define LEN_GOODS_OPTION			2048								//物品选项

//邮件信息
#define LEN_MAIL_TITLE				32									//邮件标题
#define LEN_MAIL_CONTENT			256									//邮件内容
#define LEN_MAIL_ATTACH				512									//邮件附带
#define LEN_MAX_RECEIVER			1000								//最大接收

//////////////////////////////////////////////////////////////////////////

//用户关系
#define	CP_NORMAL					0									//未知关系
#define	CP_FRIEND					1									//好友关系
#define	CP_DETEST					2									//厌恶关系

//////////////////////////////////////////////////////////////////////////

//性别定义
#define GENDER_UNKNOW				0									//未知性别
#define GENDER_MANKIND				1									//男性性别
#define GENDER_FEMALE				2									//女性性别

//////////////////////////////////////////////////////////////////////////

//游戏模式
#define GAME_GENRE_GOLD				0x0001								//金币类型
#define GAME_GENRE_SCORE			0x0002								//点值类型
#define GAME_GENRE_MATCH			0x0004								//比赛类型
#define GAME_GENRE_BATTLE			0x0008                              //约战类型
#define GAME_GENRE_RANKING			0x0010                              //排名类型

//分数模式
#define SCORE_GENRE_NORMAL			0x0100								//普通模式
#define SCORE_GENRE_POSITIVE		0x0200								//非负模式

//桌子模式
#define TABLE_GENRE_DISTRIBUTE		0x1000								//分配模式
#define TABLE_GENRE_NORMAL			0x2000								//普通模式

//////////////////////////////////////////////////////////////////////////
// 
//服务状态
#define SERVICE_STATUS_STOP			0									//停止状态
#define SERVICE_STATUS_CONFIG		1									//配置状态
#define SERVICE_STATUS_SERVICE		2									//服务状态
#define SERVICE_STATUS_WILLSTOP		3									//即停状态

//////////////////////////////////////////////////////////////////////////

//分数定义
#define SCORE						LONGLONG							//积分类型
#define SCORE_MULTIPLE				1000ll								//分数倍数
#define SCORE_STRING				TEXT("%I64d")						//积分占位符

//比例定义
#define RATIO_VALUE					1000								//比例值

//分数类型
#define SCORE_KIND_GOLD				1									//金币类型
#define SCORE_KIND_SCORE			2									//点值类型
#define SCORE_KIND_MATCH			4									//比赛分类型

//////////////////////////////////////////////////////////////////////////

//用户状态
#define US_NULL						0x00								//没有状态
#define US_FREE						0x01								//站立状态
#define US_SIT						0x02								//坐下状态
#define US_READY					0x03								//同意状态
#define US_LOOKON					0x04								//旁观状态
#define US_PLAYING					0x05								//游戏状态
#define US_OFFLINE					0x06								//断线状态

//用户状态
#define US_IM_OFFLINE				0x00								//离线状态	
#define US_IM_ONLINE				0x01								//在线状态	

//////////////////////////////////////////////////////////////////////////

//房间规则
#define SRL_LOOKON					0x00000001							//旁观标志
#define SRL_OFFLINE					0x00000002							//断线标志
#define SRL_SAME_IP					0x00000004							//同网标志
#define SRL_USER_RULE				0x00000008							//规则标志
#define SRL_ROOM_CHAT				0x00000100							//聊天标志
#define SRL_GAME_CHAT				0x00000200							//聊天标志
#define SRL_WISPER_CHAT				0x00000400							//私聊标志
#define SRL_HIDE_USER_INFO			0x00000800							//隐藏标志

//规则掩码
#define SRL_LOOKON_MASK				0x00000010							//旁观掩码
#define SRL_OFFLINE_MASK			0x00000020							//断线掩码
#define SRL_SAME_IP_MASK			0x00000040							//同网掩码
#define SRL_USER_RULE_MASK			0x00000080							//规则掩码
#define SRL_ROOM_CHAT_MASK			0x00001000							//聊天掩码
#define SRL_GAME_CHAT_MASK			0x00002000							//聊天掩码
#define SRL_WISPER_CHAT_MASK		0x00004000							//私聊掩码
#define SRL_HIDE_USER_INFO_MASK		0x00008000							//隐藏掩码

//////////////////////////////////////////////////////////////////////////
//列表数据

//无效属性
#define UD_NULL						0									//无效子项
#define UD_IMAGE					100									//图形子项

//基本属性
#define UD_GAME_ID					1									//游戏标识
#define UD_USER_ID					2									//用户标识
#define	UD_ACCOUNTS					3									//用户帐号

//扩展属性
#define UD_GENDER					10									//用户性别
#define UD_GROUP_NAME				11									//社团名字
#define UD_UNDER_WRITE				12									//个性签名

//状态信息
#define UD_TABLE					20									//游戏桌号
#define UD_CHAIR					21									//椅子号码

//积分信息
#define UD_GOLD						30									//用户金币
#define UD_SCORE					31									//用户分数
#define UD_SEGMENT					32									//用户段位
#define UD_EXPERIENCE				33									//用户经验
#define UD_WIN_COUNT				34									//胜局盘数
#define UD_LOST_COUNT				35									//输局盘数
#define UD_DRAW_COUNT				36									//和局盘数
#define UD_FLEE_COUNT				37									//逃局盘数
#define UD_PLAY_COUNT				38									//总局盘数

//积分比率
#define UD_WIN_RATE					40									//用户胜率
#define UD_LOST_RATE				41									//用户输率
#define UD_DRAW_RATE				42									//用户和率
#define UD_FLEE_RATE				43									//用户逃率
#define UD_GAME_LEVEL				44									//游戏等级

//地区信息
#define UD_AREA						50									//用户区域
#define UD_CITY						51									//用户城市 
#define UD_PROVINCE					52									//用户省份

//扩展信息
#define UD_NOTE_INFO				60									//用户备注
#define UD_LOOKON_USER				61									//旁观用户

//玩家位置
#define UD_LOCATION					70									//玩家位置

//图像列表
#define UD_FLAG						(UD_IMAGE+1)						//用户标志
#define UD_STATUS					(UD_IMAGE+2)						//用户状态

//////////////////////////////////////////////////////////////////////////
//数据库定义

#define DB_ERROR 					-1  								//处理失败
#define DB_SUCCESS 					0  									//处理成功

//////////////////////////////////////////////////////////////////////////
//组件标识

//全局组件
#define MODULE_FACE_CONTROL			1000								//头像组件
#define MODULE_LEVEL_PARSER			1001								//等级描述
#define MODULE_MEMBER_PARSER		1002								//会员描述
#define MODULE_USER_INFORMATION		1003								//用户信息
#define MODULE_PLATFORM_RESOURCE	1004								//平台资源

//大厅组件
#define MODULE_DOWNLOAD_SERVICE		2000								//下载服务

//游戏组件
#define MODULE_GAME_FRAME_WND		3000								//游戏框架
#define MODULE_GAME_FRAME_VIEW		3001								//游戏视图
#define MODULE_GAME_FRAME_ENGINE	3002								//驱动引擎
#define MODULE_GAME_FRAME_CONTROL	3003								//控制视图
#define MODULE_GAME_STRING_MESSAGE	3004								//消息接口

//内核组件
#define MODULE_CLIENT_KERNEL		4000								//游戏内核
#define MODULE_PROCESS_TRADE		4001								//进程通讯
#define MODULE_GAME_USER_MANAGER	4002								//用户管理

//外部组件
#define MODULE_CHAIR_SWITCHER		5000								//座位转换

//////////////////////////////////////////////////////////////////////////////////
//设备掩码

//设备掩码
#define DEVICE_MASK					0xFF								//设备掩码
#define DEVICE_MASK_PC				0x0F								//电脑设备
#define DEVICE_MASK_MB				0xF0								//移动设备

//电脑设备
#define DEVICE_KIND_PC				0x01								//电脑类型
#define DEVICE_KIND_ROBOT			0x02								//机器类型

//移动设备
#define DEVICE_KIND_WEB				0x10								//网页类型
#define DEVICE_KIND_IPAD            0x20								//平板电脑
#define DEVICE_KIND_IPHONE          0x40								//苹果手机
#define DEVICE_KIND_ANDROID         0x80								//安卓类型

//终端类型
#define CLIENT_KIND_MVC				0x0100								//C++ 类型
#define CLIENT_KIND_U3D				0x0200								//U3D 类型
#define CLIENT_KIND_LUA				0x0400								//LUA 类型
#define CLIENT_KIND_H5				0x0800								//H5  类型

//////////////////////////////////////////////////////////////////////////////////

//可视掩码
#define MASK_VISIBLE_COMPUTER		0x01								//电脑可视
#define MASK_VISIBLE_MOBILE			0x02								//手机可视
#define MASK_VISIBLE_FLASH			0x04								//网页可视

/////////////////////////////////////////////////////////////////////////////////
//手机定义

//视图模式
#define	VIEW_MODE_ALL				0x0001								//全部可视
#define	VIEW_MODE_PART				0x0002								//部分可视

//信息模式
#define VIEW_INFO_LEVEL_1			0x0010								//部分信息
#define VIEW_INFO_LEVEL_2			0x0020								//部分信息
#define VIEW_INFO_LEVEL_3			0x0040								//部分信息
#define VIEW_INFO_LEVEL_4			0x0080								//部分信息

//其他配置
#define RECVICE_GAME_CHAT			0x0100								//接收聊天
#define RECVICE_ROOM_CHAT			0x0200								//接收聊天
#define RECVICE_ROOM_WHISPER		0x0400								//接收私聊

//行为标识
#define BEHAVIOR_LOGON_NORMAL       0x0000                              //普通登录
#define BEHAVIOR_LOGON_IMMEDIATELY  0x1000                              //立即登录

//////////////////////////////////////////////////////////////////////////

//机器类型
#define ROBOT_SIMULATE				0x01								//相互模拟
#define ROBOT_PASSIVITY				0x02								//被动陪打
#define ROBOT_INITIATIVE			0x04								//主动陪打

//////////////////////////////////////////////////////////////////////////////////
//物品定义

//财富类型
#define GOODS_KIND_INGOT			1									//钻石类型
#define GOODS_KIND_GOLD				2									//金币类型
#define GOODS_KIND_TICKET			3									//赛券类型
#define GOODS_KIND_LOVEHEART		4									//爱心类型

//物品类型
#define GOODS_KIND_GAME_PROPS		10									//游戏道具
#define GOODS_KIND_MATCH_PROPS		11									//比赛道具
#define GOODS_KIND_MEMBER_POINT		12									//会员点数
#define GOODS_KIND_MEMBER_TIME		13									//会员时长
#define GOODS_KIND_ACTOR			14									//角色类型
#define GOODS_KIND_FACE_FRAME		15									//头像框类型
#define GOODS_KIND_CHAT_FRAME		16									//聊天框类型
#define GOODS_KIND_CARD_BACK		17									//牌背类型
#define GOODS_KIND_TABLE_BACK		18									//桌背类型

//货币类型
#define	GOODS_ID_INGOT				1									//钻石标识
#define GOODS_ID_GOLD				2									//金币标识
#define GOODS_ID_TICKET				3									//赛券标识
#define GOODS_ID_LOVEHEART			4									//爱心标识
#define GOODS_ID_COMMISSION			5									//佣金标识

//物品类型
#define GOODS_ID_LOVELINESS         5                                   //魅力值

//存放位置
#define STORE_PLACE_PACK			0x01								//背包
#define STORE_PLACE_SHOP			0x02								//商城

//交易类型
#define TRADE_TYPE_BUY				0x01								//购买类型
#define TRADE_TYPE_RECYCLE			0x02								//回收类型
#define TRADE_TYPE_EXCHANGE			0x04								//兑换类型

//变更原因
#define GOODS_REASON_SYNC			    0								//同步物品
#define GOODS_REASON_REGISTER			1								//注册赠送
#define GOODS_REASON_GAMELEVEL_UPGRADE  2								//升级奖励
#define GOODS_REASON_USERLEVEL_UPGRADE  3								//升级奖励
#define GOODS_REASON_RANKLEVEL_UPGRADE  4								//段位升级
#define GOODS_REASON_RANKGAME_FEE	    5								//排位费用
#define GOODS_REASON_RANKGAME_LOST	    6								//排位失败
#define GOODS_REASON_EXCHANGE_MALLGOODS 7								//兑换商品
#define GOODS_REASON_BUY_MATCH_TICKET	8								//购买比赛券
#define GOODS_REASON_DEDUCT_MATCH_FEE	9								//扣除比赛费用
#define GOODS_REASON_RETURN_MATCH_FEE	10								//退还比赛费用
#define GOODS_REASON_DEDUCT_REVIVE_FEE	11								//扣除复活费用
#define GOODS_REASON_RETURN_REVIVE_FEE	12								//退还复活费用
#define GOODS_REASON_EXCHANGE_GIFTBAGCODE 13							//兑换礼包码
#define GOODS_REASON_BATTLE_FEE			14								//好友房入场费
#define GOODS_REASON_LARGESS_SUBSIDY	15								//领取低保
#define GOODS_REASON_SEND_GIFT			16								//购买礼物
#define GOODS_REASON_MODIFY_NICKNAME	17								//修改昵称
#define GOODS_REASON_MODIFY_USERFACE	18								//修改头像
#define GOODS_REASON_TURNTABLE_LOTTERY  19								//转盘抽奖
#define GOODS_REASON_WEALTH_TRANSFER	20								//货币转账
#define GOODS_REASON_DRAW_COMMISSION	21								//取出佣金
#define GOODS_REASON_CLAIM_MAIL_ATTACH	80								//领取邮件附件（80-99）

//////////////////////////////////////////////////////////////////////////
//比赛定义

//比赛用户状态
#define MUS_NULL					0x00								//没有状态
#define MUS_SIGNUP	    			0x01								//报名状态
#define MUS_PLAYING					0x02								//比赛进行
#define MUS_ELIMINATE				0x04								//淘汰状态
#define MUS_COMPLETED				0x80								//完成状态

//比赛状态
#define MS_FREE						0x00								//空闲状态
#define MS_SIGNUP					0x01								//报名状态
#define MS_WAITPLAY					0x02								//等待开始
#define MS_MATCHING					0x04								//比赛进行
#define MS_WAITEND					0x08								//等待结束
#define MS_MATCHEND					0x10								//比赛结束

//配置状态
#define OPTION_STATUS_NONE			0									//配置状态
#define OPTION_STATUS_SERVICE		1									//服务状态
#define OPTION_STATUS_WAIT			2									//等待状态
#define OPTION_STATUS_FINISH		3									//结束状态

//比赛人数
#define MAX_MATCH_USER				1000								//比赛人数	
#define MAX_MATCH_TABLE				400									//比赛桌子	

//缴费区域
#define DEDUCT_AREA_SERVER			0									//房间缴费
#define DEDUCT_AREA_WEBSITE			1									//网站缴费

//费用类型
#define FEE_TYPE_GOLD				0									//金币类型
#define FEE_TYPE_MEDAL				1									//奖牌类型
#define FEE_TYPE_INGOT				2									//钻石类型

//参赛条件
#define MATCH_JOINCD_MEMBER_ORDER	0x01								//会员等级	
#define MATCH_JOINCD_EXPERIENCE		0x02								//经验等级

//筛选方式
#define FILTER_TYPE_SINGLE_TURN		0									//单轮排名
#define FILTER_TYPE_TOTAL_RANKING	1									//总排名次

//报名模式
#define SIGNUP_MODE_SIGNUP_FEE		0x01								//报名模式
#define SIGNUP_MODE_MATCH_TICKET	0x02								//报名模式

//排名方式
#define RANKING_MODE_TOTAL_GRADES   0									//排名方式
#define RANKING_MODE_SPECIAL_GRADES 1									//排名方式 

//筛选成绩
#define FILTER_GRADES_MODE_BEST		0									//最优成绩
#define FILTER_GRADES_MODE_AVERAGE	1									//平均成绩
#define FILTER_GRADES_MODE_AVERAGEEX 2									//平均成绩

//晋级方式
#define PROMOTE_MODE_OUT_GAME		0									//打立出局
#define PROMOTE_MODE_FIXED_GAME		1									//定局积分
#define PROMOTE_MODE_FIXED_TABLE	2									//按桌积分

//赛分规则
#define MSCORE_SET_TYPE_RESET		0									//重新设置
#define MSCORE_SET_TYPE_RESERVE		1									//保留上轮

//增长模式
#define INCREASE_MODE_TIME			0									//增长模式
#define INCREASE_MODE_INNING		1									//增长模式

//等待掩码
#define MASk_WAIT_RANKING			1									//等待排名
#define MASK_WAIT_DISTRIBUTE		2									//等待搓桌
#define MASK_WAIT_SWITCHTABLE		3									//等待换桌

//开赛模式
#define MATCH_MODE_LOCKTIME			0x00								//定时开赛
#define MATCH_MODE_ROUND			0x01								//循环开赛

//开赛周期
#define MATCH_CYCLE_PER_DAY			1									//每天
#define MATCH_CYCLE_PER_WEEK		2									//每周
#define MATCH_CYCLE_PER_MONTH		3									//每月

//比赛类型
#define MATCH_TYPE_NONE				0x00								//
#define MATCH_TYPE_LOCKTIME			0x01								//定时类型
#define MATCH_TYPE_IMMEDIATE		0x02								//即时类型
#define MATCH_TYPE_TOURNAMENT		0x04								//锦标赛类型

//展示类型
#define MATCH_SHOW_TTYPE_NORMAL		0									//普通类型
#define MATCH_SHOW_TTYPE_DAYLY		1									//日赛类型
#define MATCH_SHOW_TTYPE_WEEKLY		2									//周赛类型
#define MATCH_SHOW_TTYPE_PLAYOFF	3									//线下赛类型

//选项掩码
#define MATCH_BTOP_RESET_SCORE		0x01								//重置分数

//////////////////////////////////////////////////////////////////////////
//功能选项

#define FUNCTION_OPTION_BENSURE		0x01								//低保功能
#define FUNCTION_OPTION_HQPLAYER	0x08								//优质玩家

//////////////////////////////////////////////////////////////////////////
//游戏平衡

//控制方式
#define CONTROL_KIND_RATE			0									//胜率控制
#define CONTROL_KIND_TIME			1									//时间控制
#define CONTROL_KIND_COUNT			2									//局数控制

//控制模式
#define CONTROL_MODE_AUTO			0x01								//自动控制
#define CONTROL_MODE_MANUAL			0x02								//手动控制

//控制状态
#define CONTROL_STATUS_NORMAL		0									//普通
#define CONTROL_STATUS_WHITELIST	1									//白名单
#define CONTROL_STATUS_BLACKLIST	2									//黑名单

//////////////////////////////////////////////////////////////////////////////////

//邮件定义

//时间定义
#define WEEK_DAYS					7									//天数
#define MONTH_DAYS					30									//天数

//时间定义
#define ONE_DAY						24*3600								//一天		
#define ONE_WEEK					ONE_DAY*WEEK_DAYS					//一周		
#define ONE_MONTH					ONE_DAY*MONTH_DAYS					//一月

//有效期定义
#define INDATE_ONE_DAY				ONE_DAY								//一天		
#define INDATE_ONE_WEEK				ONE_WEEK							//一周		
#define INDATE_ONE_MONTH			ONE_MONTH							//一月

//邮件种类
#define MAIL_KIND_NOTICE			1									//通知类型
#define MAIL_KIND_ATTACH			2									//附件类型	

//邮件内容类型
#define MAIL_TYPE_PROMOTION			1									//用户推广
#define MAIL_TYPE_TRANSFER			2									//货币转账
#define MAIL_TYPE_PRESENT_GIFT		3									//赠送礼物
#define MAIL_TYPE_MATCH_REWARD		4									//比赛奖励
#define MAIL_TYPE_RANKING_REWARD	5									//排位奖励
#define MAIL_TYPE_SYSTEM_PRESENT	11									//系统赠送

//邮件状态
#define MAIL_STATE_UNTREATED		1									//待处理
#define MAIL_STATE_HAVETOSEE		2									//已查看
#define MAIL_STATE_HAVECLAIM		3									//已领取
#define MAIL_STATE_ONDELETED		4									//已删除

//////////////////////////////////////////////////////////////////////////////////
//筹码定义

#define MIN_CELL					1									//最小筹码

//////////////////////////////////////////////////////////////////////////////////
//排位定义

#define MIN_MAIN_ORDER				1									//最小段位（最高）
#define MAX_MAIN_ORDER				6									//最大段位（最低）
#define MAIN_ORDER_COUNT			(MAX_MAIN_ORDER-MIN_MAIN_ORDER)		//段位个数

//////////////////////////////////////////////////////////////////////////
//约战定义

#define TABLE_USER_COUNT			8									//用户数量

//查询类型
#define QUERY_KIND_NUMBER			0									//编号类型
#define QUERY_KIND_USERID			1									//标识类型
#define QUERY_KIND_GROUPID			2									//群组类型

//结算方式
#define SETTLE_KIND_TIME			0									//时间结算
#define SETTLE_KIND_COUNT			1									//局数结算
#define SETTLE_KIND_NONE			2									//

//配置掩码
#define OPTION_MASK_VOICE			0x01								//语音配置
#define OPTION_MASK_LOOKON			0x02								//旁观配置
#define OPTION_MASK_PASSWORD		0x04								//密码配置
#define OPTION_MASK_CARD_BACK		0x08								//牌背配置
#define OPTION_MASK_TABLE_BACK		0x10								//桌背配置

//货币类型
#define CURRENCY_KIND_INGOT			0									//字符货币
#define CURRENCY_KIND_ROOMCARD		1									//支付货币

//通知掩码
#define NOTIFY_MASK_TABLE_PARAM		0x01								//桌子参数
#define NOTIFY_MASK_TABLE_STATUS	0x02								//桌子状态
#define NOTIFY_MASK_TABLE_USER		0x04								//桌子用户

//展示模式
#define SHOW_SEAT_MODE_RAND			0									//随机模式
#define SHOW_SEAT_MODE_FIXED		1									//固定模式

//财富掩码
#define WEALTH_MASK_INGOT			0x01								//钻石掩码
#define WEALTH_MASK_GOLD			0x02								//金币掩码

//支付类型
#define PAY_TYPE_OWNER				0x01								//房主支付
#define PAY_TYPE_USER				0x02								//玩家支付

//税收模式
#define REVENUE_MODE_0				0									//税收模式
#define REVENUE_MODE_1				1									//税收模式
#define REVENUE_MODE_2				2									//税收模式

//视频模式
#define VIDEO_MODE_CLOSE			0									//关闭模式
#define VIDEO_MODE_FREE				1									//自由模式
#define VIDEO_MODE_OPEN				2									//打开模式

//////////////////////////////////////////////////////////////////////////
//管理定义

#define SUB_CMD_MODIFY_RULE			1									//修改命令
#define SUB_CMD_EXECUTE_CMD			2									//执行命令

//命令定义
#define CMD_LOAD_GAME_VERSION		0x0001								//加载版本
#define CMD_LOAD_PLAZA_VERSION		0x0002								//加载版本
#define CMD_LOAD_CREATE_OPTION		0x0004								//加载配置
#define CMD_LOAD_ALLOW_ADDRESS		0x0008								//加载地址

//约战命令
#define CMD_STOP_BATTLE_SERVICE     0x0010								//约战服务
#define CMD_START_BATTLE_SERVICE	0x0020								//约战服务

//茶馆命令
#define CMD_STOP_GROUP_SERVICE		0x0040								//茶馆服务	
#define CMD_START_GROUP_SERVICE		0x0080								//茶馆服务

//命令定义
#define CMD_LOAD_FORBID_WORDS		0x0100								//禁止词汇
#define CMD_LOAD_GROUP_OPTION		0x0200								//茶馆配置
#define CMD_OUTPUT_DEBUG_INFO		0x0400								//打印调试

//比赛命令
#define CMD_STOP_MATCH_SERVICE		0x0800								//停止服务	
#define CMD_START_MATCH_SERVICE		0x1000								//比赛服务	
#define CMD_LOAD_MATCH_OPTION		0x2000								//加载配置	

//排位命令
#define CMD_STOP_RANKING_SERVICE    0x4000								//排位服务
#define CMD_START_RANKING_SERVICE	0x8000								//排位服务

//////////////////////////////////////////////////////////////////////////
//录像定义

//常量定义
#define LEN_VIDEO_NUMBER			21									//录像编号

//压缩类型
#define COMPRESS_KIND_NONE			0x00								//压缩类型
#define COMPRESS_KIND_ZIP			0x01								//压缩类型
#define COMPRESS_KIND_RAR			0x02								//压缩类型

//////////////////////////////////////////////////////////////////////////
//代理定义

//代理类型
#define AGENT_KIND_GATE				1									//网关代理
#define AGENT_KIND_NORMAL			2									//普通代理

#define COMPRESS_KIND_RAR			0x02								//压缩类型

//////////////////////////////////////////////////////////////////////////
//消息定义

//消息类型
#define MSG_TYPE_NORMAL				1									//普通消息
#define MSG_TYPE_SYSTEM				2									//系统消息

//////////////////////////////////////////////////////////////////////////
//道具定义

//道具类型
#define PROP_KIND_RIFFLE			1									//洗牌道具
#define PROP_KIND_TRANSPOSAL		2									//换位道具

//使用范围
#define PROP_USERANGE_GAME			0x01								//游戏中使用
#define PROP_USERANGE_PLAZA			0x02								//大厅中使用

//////////////////////////////////////////////////////////////////////////
//IM定义

//信息掩码
#define INFO_MASK_GAMEID			0x0001								//游戏标识
#define INFO_MASK_NICKNAME			0x0002								//用户昵称
#define INFO_MASK_RANKSCORE			0x0004								//排位积分
#define INFO_MASK_EXPERIENCE		0x0008								//游戏经验
#define INFO_MASK_MEMBER_POINT		0x0010								//会员点数
#define INFO_MASK_UNDER_WRITE		0x0020								//个性签名
#define INFO_MASK_MASTER_ORDER		0x0040								//管理等级
#define INFO_MASK_LOVELINESS		0x0080								//魅力值

//群组定义
#define GS_GROUP_COUNT				20000								//群组数目
#define GS_GROUP_INIT				15000								//群组初始值

//馆分模式
#define GSCORE_MODE_FREEDOM			0									//免充模式
#define GSCORE_MODE_RECHARGE		1									//充值模式

//更新茶馆
#define UPGROUP_KIND_RULES			0x01								//茶馆规则
#define UPGROUP_KIND_WEALTH			0x02								//茶馆财富
#define UPGROUP_KIND_PROPERTY		0x04								//茶馆熟悉

//更新成员
#define UPMEMBER_KIND_TYPE			1									//馆员类型
#define UPMEMBER_KIND_RATE			2									//分成比例
#define UPMEMBER_KIND_PARENT		3									//馆员上级
#define UPMEMBER_KIND_STATUS		4									//馆员状态
#define UPMEMBER_KIND_BATTLE		5									//馆员约战

//成员类型
#define MEMBER_TYPE_MEMBER			0									//馆员类型
#define MEMBER_TYPE_HELPER			1									//助手类型
#define MEMBER_TYPE_PARTNER			2									//合伙人类型
#define MEMBER_TYPE_MANAGER			3									//管理员类型
#define MEMBER_TYPE_EMPLOYER		4									//老板类型

//成员状态
#define MEMBER_STATUS_NORMAL		0									//正常状态
#define MEMBER_STATUS_FREEZE		1									//冻结状态

//开关选项
#define SWITCH_OPTION_AUDIT			0x01								//审核开关
#define SWITCH_OPTION_PRIVATE		0x02								//隐私开关
#define SWITCH_OPTION_RANKLIST		0x04								//排行开关
#define SWITCH_OPTION_SHUTUP		0x08								//打烊开关
#define SWITCH_OPTION_GAMELIST		0x10								//列表开关
#define SWITCH_OPTION_GROUPCOWRY	0x20								//贝壳开关

//消息类型
#define APPLY_MSG_TYPE_REQUEST		1									//请求消息
#define APPLY_MSG_TYPE_RESPOND		2									//响应消息

//茶馆级别
#define GROUP_LEVEL_NORMAL			0									//普通茶馆
#define GROUP_LEVEL_MATCH			1									//比赛茶馆		 

//群组状态
#define GROUP_STATUS_ENABLE			1									//可用状态
#define GROUP_STATUS_DISABLE		2									//停用状态

//约战掩码
#define	GROUP_BATTLE_MASK_GYKF		0x01								//馆员开房

//申请状态
#define APPLY_STATUS_NONE			0									//审核状态
#define APPLY_STATUS_AGREE			1									//同意状态
#define APPLY_STATUS_REFUSE			2									//拒绝状态
#define APPLY_STATUS_REPEAL			3									//撤销状态	

//响应状态
#define RESPOND_CODE_WAIT			0									//等待状态
#define RESPOND_CODE_AGREE			1									//同意状态
#define RESPOND_CODE_REFUSE			2									//拒绝状态

//错误代码
#define LOGON_ECODE_FAILED			1									//失败代码
#define LOGON_ECODE_REENTRY			2									//重入代码

//积分权限
#define SCORE_RIGHT_MANAGER			0									//管理员
#define SCORE_RIGHT_PARTNER			1									//合伙人
#define SCORE_RIGHT_ALLIANCE		2									//大联盟

//积分调整
#define SCORE_ADJUST_GAMEUSER		1									//游戏中玩家
#define SCORE_ADJUST_UNGAMEUSER		0									//非游戏中玩家

//清除对象
#define CLEANUP_OBJECT_ONE_MEMBER	0									//一个成员
#define CLEANUP_OBJECT_ALL_MEMBER	1									//所有成员
#define CLEANUP_OBJECT_WHOLE_GROUP	2									//整个茶馆

//积分变更类型
#define SCORE_CHANGE_TYPE_MODIFY	1									//修改类型
#define SCORE_CHANGE_TYPE_PRESENT	2									//赠送类型

//贝壳变更类型
#define COWRY_CHANGE_TYPE_MODIFY	1									//修改类型	

//////////////////////////////////////////////////////////////////////////
//跑马灯定义
#define MARQUEE_NOTICY_LOGON		1									//玩家登陆
#define MARQUEE_NOTICY_BIGWIN		2									//赢得大额
#define MARQUEE_NOTICY_AWARD_POOL	3									//摇中奖池

//显示位置
#define VIEW_PLACE_GAME				0x01								//游戏界面
#define VIEW_PLACE_PLAZA			0x02								//广场界面

//公告类型
#define NOTICE_KIND_SYSTEM			0									//系统类型
#define NOTICE_KIND_LOGON			1									//登录类型
#define NOTICE_KIND_BIG_WINNER		2									//赢家类型	
#define NOTICE_KIND_PRIZE_POOL		3									//奖池类型	

//////////////////////////////////////////////////////////////////////////
//排行榜定义

//榜单类型
#define LIST_TYPE_PAY				1									//充值榜单
#define LIST_TYPE_WEALTH			2									//财富榜单
#define LIST_TYPE_SPREAD			3									//推广榜单
#define LIST_TYPE_BIGWINNER			4									//赢家榜单
#define LIST_TYPE_COMMISSION		5									//佣金榜单

//////////////////////////////////////////////////////////////////////////
//短信验证

//验证码类型
#define CODE_KIND_MP				1									//手机类型
#define CODE_KIND_EMAIL				2									//邮件类型

//验证类型
#define AUTH_KIND_BIND_MP			1									//绑定手机
#define AUTH_KIND_UNBIND_MP			2									//解绑手机
#define AUTH_KIND_BIND_EMAIL		3									//绑定邮件
#define AUTH_KIND_UNBIND_EMAIL		4									//解绑邮件
#define AUTH_KIND_RESETPASSWD		5									//重置密码
#define AUTH_KIND_CODE_LOGON		6									//验证码登陆
#define AUTH_KIND_MB_REGISTER		7									//手机注册
#define AUTH_KIND_EMAIL_REGISTER	8									//邮箱注册
#define AUTH_KIND_WEALTH_TRANSFER	10									//货币转账

//错误代码
#define AUTHECODE_AT_BINDED			1									//账号已绑定
#define AUTHECODE_MP_BINDED			2									//手机号已绑定
#define AUTHECODE_AC_HAVESENT		3									//验证码已发送
#define AUTHECODE_DB_EXCEPTION		99									//数据库异常

//////////////////////////////////////////////////////////////////////////

//服务数量
#define SERVICE_COUNT				15									//服务数量

//服务掩码
#define SERVICE_MASK_GATE			0x0001								//服务掩码
#define SERVICE_MASK_GAME			0x0002								//服务掩码
#define SERVICE_MASK_LOGON			0x0004								//服务掩码
#define SERVICE_MASK_MATCH			0x0008								//服务掩码
#define SERVICE_MASK_STORE			0x0010								//服务掩码
#define SERVICE_MASK_BATTLE			0x0020								//服务掩码
#define SERVICE_MASK_MESSAGE		0x0040								//服务掩码
#define SERVICE_MASK_LOGGER			0x0080								//服务掩码
#define SERVICE_MASK_MONITOR		0x0100								//服务掩码
#define SERVICE_MASK_RANKING		0x0200								//服务掩码
#define SERVICE_MASK_CHAT			0x0400								//服务掩码
#define SERVICE_MASK_SYNC			0x0800								//服务掩码
#define SERVICE_MASK_QUERY			0x1000								//服务掩码

//服务模块
#define SERVICE_MODULE_NONE			0									//服务模块
#define SERVICE_MODULE_GATE			1									//服务模块
#define SERVICE_MODULE_GAME			2									//服务模块
#define SERVICE_MODULE_LOGON		3									//服务模块
#define SERVICE_MODULE_CHAT			4									//服务模块
#define SERVICE_MODULE_SYNC			5									//服务模块
#define SERVICE_MODULE_MATCH		6									//服务模块
#define SERVICE_MODULE_STORE		7									//服务模块
#define SERVICE_MODULE_AGENT		8									//服务模块
#define SERVICE_MODULE_BATTLE		9									//服务模块
#define SERVICE_MODULE_MESSAGE		10									//服务模块
#define SERVICE_MODULE_RANKING		11									//服务模块
#define SERVICE_MODULE_QUERY		12									//服务模块
#define SERVICE_MODULE_LOGGER		13									//服务模块
#define SERVICE_MODULE_MONITOR		14									//服务模块
#define SERVICE_MODULE_MASTER		15									//服务模块

//////////////////////////////////////////////////////////////////////////
//Redis

//常量定义
#define	PRODUCT_NAME				"zz"								//产品名称
#define MSG_PAGE_SIZE				20									//页面大小

//索引
#define RKEY_DDI_TOKEN				0									//数据索引
#define RKEY_DDI_STATE				1									//数据索引
#define RKEY_DDI_GROUP				2									//数据索引
#define RKEY_DDI_GMEMBER			3									//数据索引
#define RKEY_DDI_USER				4									//数据索引
#define RKEY_DDI_UGROUP				5									//数据索引	
#define RKEY_DDI_UFRIEND			6									//数据索引
#define RKEY_DDI_MESSAGE			7									//数据索引
#define RKEY_DDI_MQUEUE			8									//数据索引
#define RKEY_DDI_MASTER			0									//数据索引

//前缀定义
#define RKEY_PREFIX_GATELIST		PRODUCT_NAME "_gate_list"			//网关列表
#define RKEY_PREFIX_USER			PRODUCT_NAME "_user_"				//用户信息
#define RKEY_PREFIX_TOKEN			PRODUCT_NAME "_token_"				//令牌信息		
#define RKEY_PREFIX_STATE			PRODUCT_NAME "_state_"				//状态信息
#define RKEY_PREFIX_GROUP			PRODUCT_NAME "_group_"				//群组信息
#define RKEY_PREFIX_GMEMBER		PRODUCT_NAME "_gmember_"			//群成员信息
#define RKEY_PREFIX_UGROUP			PRODUCT_NAME "_ugroup_"			//用户群组
#define RKEY_PREFIX_UFRIEND			PRODUCT_NAME "_ufriend_"			//用户好友
#define RKEY_PREFIX_UBLACKLIST		PRODUCT_NAME "_ublacklist_"		//用户黑名单
#define RKEY_PREFIX_MSG			PRODUCT_NAME "_msg_"				//消息前缀
#define RKEY_PREFIX_MQUEUE			PRODUCT_NAME "_mqueue_"			//消息队列
#define RKEY_PREFIX_SERVERLIST		PRODUCT_NAME "_server_list"		//房间列表
#define RKEY_PREFIX_RANKINGLIST		PRODUCT_NAME "_ranking_list_"		//排行榜单

//过期时间
#define TTL_WORLD_MESSAGE			43200000L							//过期时间
#define TTL_GROUP_MESSAGE			1296000000							//过期时间
#define TTL_PRIVATE_MESSAGE			1296000000							//过期时间

//////////////////////////////////////////////////////////////////////////

#endif