#ifndef DEFINE_HEAD_FILE
#define DEFINE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//��ֵ����

//ͷ���С
#define FACE_CX						96									//ͷ����
#define FACE_CY						96									//ͷ��߶�

#define MAX_CHAIR					200									//�������
#define MAX_TABLE					512									//�������
#define MAX_BATCH					48									//�������
#define MAX_ROBOT					2048								//������
#define MAX_STATION_COUNT			100									//���վ��

#define MAX_KIND					128									//�������
#define MAX_SERVER					1024								//��󷿼�

#define INVALID_CHAIR				0xFFFF								//��Ч����
#define INVALID_TABLE				0xFFFF								//��Ч����

#define LEN_USER_CHAT				128									//���쳤��
#define MAX_NORMAL_FACE				123									//��ͨͷ��
#define MAX_CONVERSATION_USER		16									//�Ự����
#define MAX_BUGLE_LEN				200									//���ȳ���
#define BUGLE_LIMIT_TIME			30L									//����ʱ��	

#define MAX_DAY						31									//�������
#define MAX_MONTH					12									//����·�


//////////////////////////////////////////////////////////////////////////
//ϵͳ����

//��Ϸ״̬
#define GAME_STATUS_FREE			0									//����״̬
#define GAME_STATUS_PLAY			100									//��Ϸ״̬
#define GAME_STATUS_WAIT			200									//�ȴ�״̬

//ϵͳ����
#define TIME_CHAT_INTERVAL			1L									//������

//////////////////////////////////////////////////////////////////////////
//��������

//�б�����
#define PRIME_TYPE					11L									//������Ŀ
#define PRIME_KIND					53L									//������Ŀ
#define PRIME_NODE					101L								//�ڵ���Ŀ
#define PRIME_SERVER				1009L								//������Ŀ
#define PRIME_MATCH					53L									//������Ŀ
#define PRIME_CUSTOM				53L									//�Զ���Ŀ
#define PRIME_GATE					23L									//������Ŀ
#define PRIME_BATTLE				11L									//Լս��Ŀ
#define PRIME_LOGIC					11L									//�߼���Ŀ
#define PRIME_GROUP					11L									//Ⱥ����Ŀ
#define PRIME_STATUS				11L									//״̬��Ŀ
#define PRIME_MESSAGE				3L									//��Ϣ��Ŀ
#define PRIME_STATION				11L									//վ����Ŀ

//��������
#define PRIME_SERVER_USER			1007L								//��������
#define PRIME_ROBOT_USER			2003L								//��������
#define PRIME_PLATFORM_USER			100003L								//ƽ̨����

//////////////////////////////////////////////////////////////////////////
//���ݳ���

//��������
#define LEN_MD5						33									//��������
#define LEN_AREA					16									//��������
#define LEN_CITY					16									//���г���
#define LEN_PROVINCE				16									//ʡ�ݳ���
#define LEN_ACCOUNTS				32									//�ʺų���
#define LEN_NICKNAME				32									//�ǳƳ���
#define LEN_PASSWORD				33									//���볤��
#define LEN_GROUP_NAME				32									//��������
#define LEN_UNDER_WRITE				64									//ǩ������
#define LEN_PROTECT_QUESTION		32									//�ܱ�����
#define LEN_PASSPORT_ID				32									//֤������

//���ݳ���
#define LEN_QQ						16									//Q Q ����
#define LEN_EMAIL					64									//�����ʼ�
#define LEN_UNIQUEID				33									//UID����
#define LEN_USER_NOTE				256									//�û���ע
#define LEN_SEAT_PHONE				33									//�̶��绰
#define LEN_MOBILE_PHONE			22									//�ƶ��绰
#define LEN_COMPELLATION			16									//��ʵ����
#define LEN_DWELLING_PLACE			128									//��ϵ��ַ
#define LEN_AUTHCODE				7									//��֤����
//#define LEN_VALIDATE_CODE			6									//��֤����
#define LEN_USER_DATUM				128									//�û�����
#define LEN_USER_FACE				256									//ͷ���ַ

//��������
#define LEN_DOMAIN					63									//��������	
#define LEN_ADDRESS					16									//��ַ����
#define LEN_VALIDATE				33									//��֤��ַ
#define LEN_ACCREDIT_ID				33									//��Ȩ����
#define LEN_MACHINE_ID				33									//���г���
#define	LEN_NETWORK_ID				13									//���г���
#define LEN_CLUB_NAME				64									//��������
#define LEN_CLUB_NOTICE				128									//���繫��
#define LEN_CLUB_INTRODUCE			128									//�������
#define LEN_LINK_EXTEND				16									//������չ
#define LEN_INVITE_CODE				7									//������
#define LEN_LOGON_TOKEN				256									//��¼����
#define LEN_MACHINE_OSID			64									//���г���
#define LEN_PACKAGE_NAME			64									//��������
#define LEN_PASS_PORT_ID			19									//֤������
#define LEN_COMPELLATION			16									//��ʵ����
#define LEN_PLACE_NAME				64									//��ַ����

//��Ƶ����
#define LEN_VIDEO_KEY				33									//��Կ����
#define LEN_VIDEO_CHANNEL			22									//Ƶ������

//�б�����
#define LEN_TYPE					32									//���೤��
#define LEN_KIND					32									//���ͳ���
#define LEN_NODE					32									//�ڵ㳤��
#define LEN_SERVER					32									//���䳤��
#define LEN_CUSTOM					32									//���Ƴ���
#define LEN_PROCESS					32									//���̳���

//��涨��
#define MAX_CARD_REPERTORY			152									//�����

//��Ʒ��Ϣ
#define LEN_GOODS_NAME				64									//��Ʒ����
#define LEN_GOODS_OPTION			2048								//��Ʒѡ��

//�ʼ���Ϣ
#define LEN_MAIL_TITLE				32									//�ʼ�����
#define LEN_MAIL_CONTENT			256									//�ʼ�����
#define LEN_MAIL_ATTACH				512									//�ʼ�����
#define LEN_MAX_RECEIVER			1000								//������

//////////////////////////////////////////////////////////////////////////

//�û���ϵ
#define	CP_NORMAL					0									//δ֪��ϵ
#define	CP_FRIEND					1									//���ѹ�ϵ
#define	CP_DETEST					2									//����ϵ

//////////////////////////////////////////////////////////////////////////

//�Ա���
#define GENDER_UNKNOW				0									//δ֪�Ա�
#define GENDER_MANKIND				1									//�����Ա�
#define GENDER_FEMALE				2									//Ů���Ա�

//////////////////////////////////////////////////////////////////////////

//��Ϸģʽ
#define GAME_GENRE_GOLD				0x0001								//�������
#define GAME_GENRE_SCORE			0x0002								//��ֵ����
#define GAME_GENRE_MATCH			0x0004								//��������
#define GAME_GENRE_BATTLE			0x0008                              //Լս����
#define GAME_GENRE_RANKING			0x0010                              //��������

//����ģʽ
#define SCORE_GENRE_NORMAL			0x0100								//��ͨģʽ
#define SCORE_GENRE_POSITIVE		0x0200								//�Ǹ�ģʽ

//����ģʽ
#define TABLE_GENRE_DISTRIBUTE		0x1000								//����ģʽ
#define TABLE_GENRE_NORMAL			0x2000								//��ͨģʽ

//////////////////////////////////////////////////////////////////////////
// 
//����״̬
#define SERVICE_STATUS_STOP			0									//ֹͣ״̬
#define SERVICE_STATUS_CONFIG		1									//����״̬
#define SERVICE_STATUS_SERVICE		2									//����״̬
#define SERVICE_STATUS_WILLSTOP		3									//��ͣ״̬

//////////////////////////////////////////////////////////////////////////

//��������
#define SCORE						LONGLONG							//��������
#define SCORE_MULTIPLE				1ll									//��������
#define SCORE_STRING				TEXT("%I64d")						//����ռλ��

//��������
#define RATIO_VALUE					1000								//����ֵ

//��������
#define SCORE_KIND_GOLD				1									//�������
#define SCORE_KIND_CLUB				2									//��������
#define SCORE_KIND_BATTLE			3									//Լս����
#define SCORE_KIND_MATCH			4									//��������
#define SCORE_KIND_RANKING			5									//��λ����

//////////////////////////////////////////////////////////////////////////

//�û�״̬
#define US_NULL						0x00								//û��״̬
#define US_FREE						0x01								//վ��״̬
#define US_SIT						0x02								//����״̬
#define US_READY					0x03								//ͬ��״̬
#define US_LOOKON					0x04								//�Թ�״̬
#define US_PLAYING					0x05								//��Ϸ״̬
#define US_OFFLINE					0x06								//����״̬

//�û�״̬
#define US_IM_OFFLINE				0x00								//����״̬	
#define US_IM_ONLINE				0x01								//����״̬	

//�û�״̬
#define US_CB_ONLINE				0x01								//����״̬
#define US_CB_OFFLINE				0x00								//����״̬

//�û���ǩ
#define UL_SUPPER_PLAYER			0x0001								//��R���

//////////////////////////////////////////////////////////////////////////

//�������
#define SRL_LOOKON					0x00000001							//�Թ۱�־
#define SRL_OFFLINE					0x00000002							//���߱�־
#define SRL_SAME_IP					0x00000004							//ͬ����־
#define SRL_USER_RULE				0x00000008							//�����־
#define SRL_ROOM_CHAT				0x00000100							//�����־
#define SRL_GAME_CHAT				0x00000200							//�����־
#define SRL_WISPER_CHAT				0x00000400							//˽�ı�־
#define SRL_HIDE_USER_INFO			0x00000800							//���ر�־

//��������
#define SRL_LOOKON_MASK				0x00000010							//�Թ�����
#define SRL_OFFLINE_MASK			0x00000020							//��������
#define SRL_SAME_IP_MASK			0x00000040							//ͬ������
#define SRL_USER_RULE_MASK			0x00000080							//��������
#define SRL_ROOM_CHAT_MASK			0x00001000							//��������
#define SRL_GAME_CHAT_MASK			0x00002000							//��������
#define SRL_WISPER_CHAT_MASK		0x00004000							//˽������
#define SRL_HIDE_USER_INFO_MASK		0x00008000							//��������

//////////////////////////////////////////////////////////////////////////
//�б�����

//��Ч����
#define UD_NULL						0									//��Ч����
#define UD_IMAGE					100									//ͼ������

//��������
#define UD_GAME_ID					1									//��Ϸ��ʶ
#define UD_USER_ID					2									//�û���ʶ
#define	UD_ACCOUNTS					3									//�û��ʺ�

//��չ����
#define UD_GENDER					10									//�û��Ա�
#define UD_GROUP_NAME				11									//��������
#define UD_UNDER_WRITE				12									//����ǩ��

//״̬��Ϣ
#define UD_TABLE					20									//��Ϸ����
#define UD_CHAIR					21									//���Ӻ���

//������Ϣ
#define UD_GOLD						30									//�û����
#define UD_SCORE					31									//�û�����
#define UD_SEGMENT					32									//�û���λ
#define UD_EXPERIENCE				33									//�û�����
#define UD_WIN_COUNT				34									//ʤ������
#define UD_LOST_COUNT				35									//�������
#define UD_DRAW_COUNT				36									//�;�����
#define UD_FLEE_COUNT				37									//�Ӿ�����
#define UD_PLAY_COUNT				38									//�ܾ�����

//���ֱ���
#define UD_WIN_RATE					40									//�û�ʤ��
#define UD_LOST_RATE				41									//�û�����
#define UD_DRAW_RATE				42									//�û�����
#define UD_FLEE_RATE				43									//�û�����
#define UD_GAME_LEVEL				44									//��Ϸ�ȼ�

//������Ϣ
#define UD_AREA						50									//�û�����
#define UD_CITY						51									//�û����� 
#define UD_PROVINCE					52									//�û�ʡ��

//��չ��Ϣ
#define UD_NOTE_INFO				60									//�û���ע
#define UD_LOOKON_USER				61									//�Թ��û�

//���λ��
#define UD_LOCATION					70									//���λ��

//ͼ���б�
#define UD_FLAG						(UD_IMAGE+1)						//�û���־
#define UD_STATUS					(UD_IMAGE+2)						//�û�״̬

//////////////////////////////////////////////////////////////////////////
//���ݿⶨ��

#define DB_ERROR 					-1  								//����ʧ��
#define DB_SUCCESS 					0  									//����ɹ�

//////////////////////////////////////////////////////////////////////////
//�����ʶ

//ȫ�����
#define MODULE_FACE_CONTROL			1000								//ͷ�����
#define MODULE_LEVEL_PARSER			1001								//�ȼ�����
#define MODULE_MEMBER_PARSER		1002								//��Ա����
#define MODULE_USER_INFORMATION		1003								//�û���Ϣ
#define MODULE_PLATFORM_RESOURCE	1004								//ƽ̨��Դ

//�������
#define MODULE_DOWNLOAD_SERVICE		2000								//���ط���

//��Ϸ���
#define MODULE_GAME_FRAME_WND		3000								//��Ϸ���
#define MODULE_GAME_FRAME_VIEW		3001								//��Ϸ��ͼ
#define MODULE_GAME_FRAME_ENGINE	3002								//��������
#define MODULE_GAME_FRAME_CONTROL	3003								//������ͼ
#define MODULE_GAME_STRING_MESSAGE	3004								//��Ϣ�ӿ�

//�ں����
#define MODULE_CLIENT_KERNEL		4000								//��Ϸ�ں�
#define MODULE_PROCESS_TRADE		4001								//����ͨѶ
#define MODULE_GAME_USER_MANAGER	4002								//�û�����

//�ⲿ���
#define MODULE_CHAIR_SWITCHER		5000								//��λת��

//////////////////////////////////////////////////////////////////////////////////
//�豸����

//�豸����
#define DEVICE_MASK					0xFF								//�豸����
#define DEVICE_MASK_PC				0x0F								//�����豸
#define DEVICE_MASK_MB				0xF0								//�ƶ��豸

//�����豸
#define DEVICE_KIND_PC				0x01								//��������
#define DEVICE_KIND_ROBOT			0x02								//��������

//�ƶ��豸
#define DEVICE_KIND_WEB				0x10								//��ҳ����
#define DEVICE_KIND_IPAD            0x20								//ƽ�����
#define DEVICE_KIND_IPHONE          0x40								//ƻ���ֻ�
#define DEVICE_KIND_ANDROID         0x80								//��׿����

//�ն�����
#define CLIENT_KIND_MVC				0x0100								//C++ ����
#define CLIENT_KIND_U3D				0x0200								//U3D ����
#define CLIENT_KIND_LUA				0x0400								//LUA ����
#define CLIENT_KIND_H5				0x0800								//H5  ����

//////////////////////////////////////////////////////////////////////////////////

//��������
#define MASK_VISIBLE_COMPUTER		0x01								//���Կ���
#define MASK_VISIBLE_MOBILE			0x02								//�ֻ�����
#define MASK_VISIBLE_FLASH			0x04								//��ҳ����

//////////////////////////////////////////////////////////////////////////

//��������
#define ROBOT_SIMULATE				0x01								//�໥ģ��
#define ROBOT_PASSIVITY				0x02								//�������
#define ROBOT_INITIATIVE			0x04								//�������

//////////////////////////////////////////////////////////////////////////////////
//��Ʒ����

//�Ƹ�����
#define GOODS_KIND_GOLD				1									//�������
#define GOODS_KIND_CARD				2									//��������
#define GOODS_KIND_TICKET			3									//��ȯ����
#define GOODS_KIND_LOVEHEART		4									//��������

//��������
#define GOODS_ID_GOLD				1									//��ұ�ʶ
#define	GOODS_ID_CARD				2									//������ʶ

//���λ��
#define STORE_PLACE_PACK			0x01								//����
#define STORE_PLACE_SHOP			0x02								//�̳�

//��������
#define TRADE_TYPE_BUY				0x01								//��������
#define TRADE_TYPE_RECYCLE			0x02								//��������
#define TRADE_TYPE_EXCHANGE			0x04								//�һ�����

//���ԭ��
#define GOODS_REASON_SYNC			    0								//ͬ����Ʒ
#define GOODS_REASON_REGISTER			1								//ע������
#define GOODS_REASON_GAMELEVEL_UPGRADE  2								//��������
#define GOODS_REASON_USERLEVEL_UPGRADE  3								//��������
#define GOODS_REASON_RANKLEVEL_UPGRADE  4								//��λ����
#define GOODS_REASON_RANKGAME_FEE	    5								//��λ����
#define GOODS_REASON_RANKGAME_LOST	    6								//��λʧ��
#define GOODS_REASON_EXCHANGE_MALLGOODS 7								//�һ���Ʒ
#define GOODS_REASON_BUY_MATCH_TICKET	8								//�������ȯ
#define GOODS_REASON_DEDUCT_MATCH_FEE	9								//�۳���������
#define GOODS_REASON_RETURN_MATCH_FEE	10								//�˻���������
#define GOODS_REASON_DEDUCT_REVIVE_FEE	11								//�۳��������
#define GOODS_REASON_RETURN_REVIVE_FEE	12								//�˻��������
#define GOODS_REASON_EXCHANGE_GIFTBAGCODE 13							//�һ������
#define GOODS_REASON_DEDUCT_BATTLE_COST	14								//Լս�۷�
#define GOODS_REASON_RETURN_BATTLE_COST	15								//Լս�˷�
#define GOODS_REASON_BATTLE_REWARD		16								//Լս����
#define GOODS_REASON_MODIFY_NICKNAME	17								//�޸��ǳ�
#define GOODS_REASON_MODIFY_USERFACE	18								//�޸�ͷ��
#define GOODS_REASON_TURNTABLE_LOTTERY  19								//ת�̳齱
#define GOODS_REASON_WEALTH_TRANSFER	20								//����ת��
#define GOODS_REASON_DRAW_COMMISSION	21								//ȡ��Ӷ��
#define GOODS_REASON_LARGESS_SUBSIDY	22								//��ȡ�ͱ�
#define GOODS_REASON_SEND_GIFT			23								//��������
#define GOODS_REASON_INSURE				24								//���в���
#define GOODS_REASON_CHECKIN			25								//ÿ��ǩ��
#define GOODS_REASON_CLUB_REVENUE		77								//����˰��
#define GOODS_REASON_DEDUCT_GAME_SCORE	78								//�۳���������
#define GOODS_REASON_CLEAN_GAME_SCORE	79								//�������
#define GOODS_REASON_CLAIM_MAIL_ATTACH	80								//��ȡ�ʼ�������80-99��

//////////////////////////////////////////////////////////////////////////
//��������

//�����û�״̬
#define MUS_NULL					0x00								//û��״̬
#define MUS_SIGNUP	    			0x01								//����״̬
#define MUS_PLAYING					0x02								//��������
#define MUS_ELIMINATE				0x04								//��̭״̬
#define MUS_COMPLETED				0x80								//���״̬

//����״̬
#define MS_FREE						0x00								//����״̬
#define MS_SIGNUP					0x01								//����״̬
#define MS_WAITPLAY					0x02								//�ȴ���ʼ
#define MS_MATCHING					0x04								//��������
#define MS_WAITEND					0x08								//�ȴ�����
#define MS_MATCHEND					0x10								//��������

//����״̬
#define OPTION_STATUS_NONE			0									//����״̬
#define OPTION_STATUS_SERVICE		1									//����״̬
#define OPTION_STATUS_WAIT			2									//�ȴ�״̬
#define OPTION_STATUS_FINISH		3									//����״̬

//��������
#define MAX_MATCH_USER				1000								//��������	
#define MAX_MATCH_TABLE				400									//��������	

//�ɷ�����
#define DEDUCT_AREA_SERVER			0									//����ɷ�
#define DEDUCT_AREA_WEBSITE			1									//��վ�ɷ�

//��������
#define FEE_TYPE_GOLD				0									//�������
#define FEE_TYPE_MEDAL				1									//��������
#define FEE_TYPE_INGOT				2									//��ʯ����

//��������
#define MATCH_JOINCD_MEMBER_ORDER	0x01								//��Ա�ȼ�	
#define MATCH_JOINCD_EXPERIENCE		0x02								//����ȼ�

//ɸѡ��ʽ
#define FILTER_TYPE_SINGLE_TURN		0									//��������
#define FILTER_TYPE_TOTAL_RANKING	1									//��������

//����ģʽ
#define SIGNUP_MODE_SIGNUP_FEE		0x01								//����ģʽ
#define SIGNUP_MODE_MATCH_TICKET	0x02								//����ģʽ

//������ʽ
#define RANKING_MODE_TOTAL_GRADES   0									//������ʽ
#define RANKING_MODE_SPECIAL_GRADES 1									//������ʽ 

//ɸѡ�ɼ�
#define FILTER_GRADES_MODE_BEST		0									//���ųɼ�
#define FILTER_GRADES_MODE_AVERAGE	1									//ƽ���ɼ�
#define FILTER_GRADES_MODE_AVERAGEEX 2									//ƽ���ɼ�

//������ʽ
#define PROMOTE_MODE_OUT_GAME		0									//��������
#define PROMOTE_MODE_FIXED_GAME		1									//���ֻ���
#define PROMOTE_MODE_FIXED_TABLE	2									//��������

//���ֹ���
#define MSCORE_SET_TYPE_RESET		0									//��������
#define MSCORE_SET_TYPE_RESERVE		1									//��������

//����ģʽ
#define INCREASE_MODE_TIME			0									//����ģʽ
#define INCREASE_MODE_INNING		1									//����ģʽ

//�ȴ�����
#define MASk_WAIT_RANKING			1									//�ȴ�����
#define MASK_WAIT_DISTRIBUTE		2									//�ȴ�����
#define MASK_WAIT_SWITCHTABLE		3									//�ȴ�����

//����ģʽ
#define MATCH_MODE_LOCKTIME			0x00								//��ʱ����
#define MATCH_MODE_ROUND			0x01								//ѭ������

//��������
#define MATCH_CYCLE_PER_DAY			1									//ÿ��
#define MATCH_CYCLE_PER_WEEK		2									//ÿ��
#define MATCH_CYCLE_PER_MONTH		3									//ÿ��

//��������
#define MATCH_TYPE_NONE				0x00								//
#define MATCH_TYPE_LOCKTIME			0x01								//��ʱ����
#define MATCH_TYPE_IMMEDIATE		0x02								//��ʱ����
#define MATCH_TYPE_TOURNAMENT		0x04								//����������

//չʾ����
#define MATCH_SHOW_TTYPE_NORMAL		0									//��ͨ����
#define MATCH_SHOW_TTYPE_DAYLY		1									//��������
#define MATCH_SHOW_TTYPE_WEEKLY		2									//��������
#define MATCH_SHOW_TTYPE_PLAYOFF	3									//����������

//ѡ������
#define MATCH_BTOP_RESET_SCORE		0x01								//���÷���

//////////////////////////////////////////////////////////////////////////
//����ѡ��

#define FUNCTION_OPTION_BENSURE		0x01								//�ͱ�����
#define FUNCTION_OPTION_HQPLAYER	0x08								//�������

//////////////////////////////////////////////////////////////////////////
//��Ϸƽ��

//���Ʒ�ʽ
#define CONTROL_KIND_RATE			0									//ʤ�ʿ���
#define CONTROL_KIND_TIME			1									//ʱ�����
#define CONTROL_KIND_COUNT			2									//��������
#define CONTROL_KIND_CUSTOM			3									//�Զ������

//����ģʽ
#define CONTROL_MODE_AUTO			0x01								//�Զ�����
#define CONTROL_MODE_MANUAL			0x02								//�ֶ�����

//����״̬
#define CONTROL_STATUS_NORMAL		0									//��ͨ
#define CONTROL_STATUS_WHITELIST	1									//������
#define CONTROL_STATUS_BLACKLIST	2									//������

//////////////////////////////////////////////////////////////////////////////////

//�ʼ�����

//ʱ�䶨��
#define WEEK_DAYS					7									//����
#define MONTH_DAYS					30									//����

//ʱ�䶨��
#define ONE_DAY						24*3600								//һ��		
#define ONE_WEEK					ONE_DAY*WEEK_DAYS					//һ��		
#define ONE_MONTH					ONE_DAY*MONTH_DAYS					//һ��

//��Ч�ڶ���
#define INDATE_ONE_DAY				ONE_DAY								//һ��		
#define INDATE_ONE_WEEK				ONE_WEEK							//һ��		
#define INDATE_ONE_MONTH			ONE_MONTH							//һ��

//�ʼ�����
#define MAIL_KIND_NOTICE			1									//֪ͨ����
#define MAIL_KIND_ATTACH			2									//��������	

//�ʼ���������
#define MAIL_TYPE_PROMOTION			1									//�û��ƹ�
#define MAIL_TYPE_TRANSFER			2									//����ת��
#define MAIL_TYPE_PRESENT_GIFT		3									//��������
#define MAIL_TYPE_MATCH_REWARD		4									//��������
#define MAIL_TYPE_RANKING_REWARD	5									//��λ����
#define MAIL_TYPE_SYSTEM_PRESENT	11									//ϵͳ����

//�ʼ�״̬
#define MAIL_STATE_UNTREATED		1									//������
#define MAIL_STATE_HAVETOSEE		2									//�Ѳ鿴
#define MAIL_STATE_HAVECLAIM		3									//����ȡ
#define MAIL_STATE_ONDELETED		4									//��ɾ��

//////////////////////////////////////////////////////////////////////////////////
//���붨��

#define MIN_CELL					1									//��С����

//////////////////////////////////////////////////////////////////////////////////
//��λ����

#define MIN_MAIN_ORDER				1									//��С��λ����ߣ�
#define MAX_MAIN_ORDER				6									//����λ����ͣ�
#define MAIN_ORDER_COUNT			(MAX_MAIN_ORDER-MIN_MAIN_ORDER)		//��λ����

//////////////////////////////////////////////////////////////////////////
//Լս����

#define TABLE_USER_COUNT			8									//�û�����

//��ѯ����
#define QUERY_KIND_USERID			1									//��ʶ����
#define QUERY_KIND_CLUBID			2									//��������

//���㷽ʽ
#define SETTLE_KIND_COUNT			0									//��������
#define SETTLE_KIND_CUSTOM			2									//�Զ������

//��������
#define OPTION_MASK_VOICE			0x0001								//��������
#define OPTION_MASK_LOOKON			0x0002								//�Թ�����
#define OPTION_MASK_PUBLIC			0x0004								//��������
#define OPTION_MASK_SAMEIP			0x0008								//ͬIP����
#define OPTION_MASK_GOLD			0x0010								//�������
#define OPTION_MASK_CARD			0x0020								//��������
#define OPTION_MASK_COUNT			0x0100								//��������
#define OPTION_MASK_CUSTOM			0x0200								//�Զ������

//��������
#define CURRENCY_KIND_GOLD			0x01								//֧������
#define CURRENCY_KIND_CARD			0x02								//֧������

//֪ͨ����
#define NOTIFY_MASK_TABLE_PARAM		0x01								//���Ӳ���
#define NOTIFY_MASK_TABLE_STATUS	0x02								//����״̬
#define NOTIFY_MASK_TABLE_USER		0x04								//�����û�

//չʾģʽ
#define SHOW_SEAT_MODE_RAND			0									//���ģʽ
#define SHOW_SEAT_MODE_FIXED		1									//�̶�ģʽ

//�Ƹ�����
#define WEALTH_MASK_GOLD			0x01								//�������
#define WEALTH_MASK_CARD			0x02								//��������
#define WEALTH_MASK_CLUBSCORE		0x04								//�ݷ�����

//֧������
#define PAY_TYPE_OWNER				0x01								//����֧��
#define PAY_TYPE_USER				0x02								//���֧��
#define PAY_TYPE_WINNER				0x04								//Ӯ��֧��

//��Ƶģʽ
#define VIDEO_MODE_CLOSE			0									//�ر�ģʽ
#define VIDEO_MODE_FREE				1									//����ģʽ
#define VIDEO_MODE_OPEN				2									//��ģʽ

//�Թ�ģʽ
#define LOOKON_MODE_FORBID			0									//��ֹģʽ
#define LOOKON_MODE_HIDE_CARD		1									//����ģʽ
#define LOOKON_MODE_SHOW_CARD		2									//����ģʽ

//Ӧ�ó���
#define EMPLOY_SCENE_CLUB_OUTSIDE	100									//�����ⲿ

//////////////////////////////////////////////////////////////////////////
//������

//�����
#define CMD_LOAD_CONFIG_DATA		0x0001								//��������

//Լս����
#define CMD_STOP_BATTLE_SERVICE     0x0002								//Լս����
#define CMD_START_BATTLE_SERVICE	0x0004								//Լս����

//��������
#define CMD_STOP_CLUB_SERVICE		0x0010								//ֹͣ����	
#define CMD_START_CLUB_SERVICE		0x0020								//��������
#define CMD_RELOAD_CLUB_DATA		0x0040								//���ز���
#define CMD_PRINT_CLUB_INFO			0x0080								//��ӡ��Ϣ

//��������
#define CMD_STOP_MATCH_SERVICE		0x0100								//ֹͣ����	
#define CMD_START_MATCH_SERVICE		0x0200								//��������	

//��λ����
#define CMD_STOP_RANKING_SERVICE    0x1000								//��λ����
#define CMD_START_RANKING_SERVICE	0x2000								//��λ����

//////////////////////////////////////////////////////////////////////////
//¼����

//��������
#define LEN_VIDEO_NUMBER			21									//¼����

//ѹ������
#define COMPRESS_KIND_NONE			0x00								//ѹ������
#define COMPRESS_KIND_ZIP			0x01								//ѹ������
#define COMPRESS_KIND_RAR			0x02								//ѹ������

//////////////////////////////////////////////////////////////////////////
//������

//��������
#define AGENT_KIND_GATE				1									//���ش���
#define AGENT_KIND_NORMAL			2									//��ͨ����

#define COMPRESS_KIND_RAR			0x02								//ѹ������

//////////////////////////////////////////////////////////////////////////
//��Ϣ����

//��Ϣ����
#define MSG_TYPE_NORMAL				1									//��ͨ��Ϣ
#define MSG_TYPE_SYSTEM				2									//ϵͳ��Ϣ

//////////////////////////////////////////////////////////////////////////
//���߶���

//��������
#define PROP_KIND_RIFFLE			1									//ϴ�Ƶ���
#define PROP_KIND_TRANSPOSAL		2									//��λ����

//ʹ�÷�Χ
#define PROP_USERANGE_GAME			0x01								//��Ϸ��ʹ��
#define PROP_USERANGE_PLAZA			0x02								//������ʹ��

//////////////////////////////////////////////////////////////////////////
//���綨��

//��Ϣ����
#define INFO_MASK_GAMEID			0x0001								//��Ϸ��ʶ
#define INFO_MASK_FACEID			0x0002								//ͷ���ǳ�
#define INFO_MASK_NICKNAME			0x0004								//�û��ǳ�

//�ݷ�ģʽ
#define SCORE_MODE_FREEDOM			0									//���ģʽ
#define SCORE_MODE_RECHARGE			1									//��ֵģʽ

//��������
#define CLUB_KIND_GOLD				0									//�������
#define CLUB_KIND_NORMAL			1									//��ͨ����
#define CLUB_KIND_ADVANCED			2									//�߼�����

//��������
#define CLUB_KIND_COUNT				3									//��������

//���²��
#define UPCLUB_KIND_RULES			0x01								//��ݹ���
#define UPCLUB_KIND_NOTICE			0x02								//��ݹ���
#define UPCLUB_KIND_PROPERTY		0x04								//�������

//���³�Ա
#define UPMEMBER_KIND_TYPE			1									//��Ա����
#define UPMEMBER_KIND_STATUS		2									//��Ա״̬
#define UPMEMBER_KIND_BATTLE		3									//��ԱԼս
#define UPMEMBER_KIND_CLUBSCORE		5									//��Ա����

//��Ա����
#define MEMBER_TYPE_MEMBER			0									//��Ա����
#define MEMBER_TYPE_MANAGER			3									//����Ա����
#define MEMBER_TYPE_EMPLOYER		4									//�ϰ�����

//��Ա״̬
#define MEMBER_STATUS_NORMAL		0									//����״̬
#define MEMBER_STATUS_FREEZE		1									//����״̬

//����Ȩ��
#define MANAGE_RIGHT_MODIFY_CLUB	0x01								//�༭����
#define MANAGE_RIGHT_MODIFY_CONFIG	0x02								//�༭�淨
#define MANAGE_RIGHT_AUDIT_APPLY 	0x04								//�������
#define MANAGE_RIGHT_FREEZE_MEMBER  0x08								//�����Ա
#define MANAGE_RIGHT_KICKOUT_MEMBER	0x10								//�߳���Ա
#define MANAGE_RIGHT_ADJUST_SCORE	0x20								//��������
#define MANAGE_RIGHT_WINNER_SCORE	0x40								//��Ӯ�һ���

//����ѡ��
#define SWITCH_OPTION_AUDIT			0x01								//��˿���
#define SWITCH_OPTION_PRIVATE		0x02								//��˽����
#define SWITCH_OPTION_SHUTUP		0x04								//���ȿ���

//��Ϣ����
#define APPLY_MSG_TYPE_REQUEST		1									//������Ϣ
#define APPLY_MSG_TYPE_RESPOND		2									//��Ӧ��Ϣ

//����״̬
#define CLUB_STATUS_ENABLE			1									//����״̬
#define CLUB_STATUS_DISABLE			2									//ͣ��״̬

//Լս����
#define	CLUB_BATTLE_MASK_GYKF		0x01								//��Ա����

//����״̬
#define APPLY_STATUS_NONE			0									//���״̬
#define APPLY_STATUS_AGREE			1									//ͬ��״̬
#define APPLY_STATUS_REFUSE			2									//�ܾ�״̬
#define APPLY_STATUS_REPEAL			3									//����״̬	

//��Ӧ״̬
#define RESPOND_CODE_WAIT			0									//�ȴ�״̬
#define RESPOND_CODE_AGREE			1									//ͬ��״̬
#define RESPOND_CODE_REFUSE			2									//�ܾ�״̬

//�������
#define LOGON_ECODE_FAILED			1									//ʧ�ܴ���
#define LOGON_ECODE_REENTRY			2									//�������

//�������
#define CLEANUP_OBJECT_ONE_MEMBER	0									//һ����Ա
#define CLEANUP_OBJECT_ALL_MEMBER	1									//���г�Ա
#define CLEANUP_OBJECT_WHOLE_CLUB	2									//��������

//���ֱ������
#define SCORE_CHANGE_TYPE_MODIFY	1									//�޸�����
#define SCORE_CHANGE_TYPE_PRESENT	2									//��������

//////////////////////////////////////////////////////////////////////////
//����ƶ���
#define MARQUEE_NOTICY_LOGON		1									//��ҵ�½
#define MARQUEE_NOTICY_BIGWIN		2									//Ӯ�ô��
#define MARQUEE_NOTICY_AWARD_POOL	3									//ҡ�н���

//��ʾλ��
#define VIEW_PLACE_GAME				0x01								//��Ϸ����
#define VIEW_PLACE_PLAZA			0x02								//�㳡����

//��������
#define NOTICE_KIND_SYSTEM			0									//ϵͳ����
#define NOTICE_KIND_LOGON			1									//��¼����
#define NOTICE_KIND_BIG_WINNER		2									//Ӯ������	
#define NOTICE_KIND_PRIZE_POOL		3									//��������	

//////////////////////////////////////////////////////////////////////////
//���а���

//������
#define LIST_TYPE_PAY				1									//��ֵ��
#define LIST_TYPE_WEALTH			2									//�Ƹ���
#define LIST_TYPE_SPREAD			3									//�ƹ��
#define LIST_TYPE_BIGWINNER			4									//Ӯ�Ұ�
#define LIST_TYPE_COMMISSION		5									//Ӷ���

//////////////////////////////////////////////////////////////////////////////////
//���ö���

//���붨��
#define GAME_CONFIG_MASK			0xFF000000								//��Ϸ����
#define GLOBAL_OPTION_MASK			0x00FF0000								//ȫ������	

//��������
#define CONFIG_MASK_GAME_LIST		0x00000001								//��Ϸ�б�
#define CONFIG_MASK_GAME_LEVEL		0x00000002								//��Ϸ�ȼ�
#define CONFIG_MASK_GAME_GOODS		0x00000004								//��Ϸ��Ʒ
#define CONFIG_MASK_ROBOT_PARAM		0x00000008								//��������
#define CONFIG_MASK_SUBSIDY_PARAM	0x00000010								//�ͱ�����
#define CONFIG_MASK_CONTROL_PARAM	0x00000020								//���Ʋ���
#define CONFIG_MASK_BATTLE_OPTION	0x00000040								//Լս����
#define CONFIG_MASK_GAME_VERSION	0x00000080								//��Ϸ�汾
#define CONFIG_MASK_PLAZA_VERSION	0x00000100								//�����汾
#define CONFIG_MASK_FILTER_WORDS	0x00000200								//���дʻ�
#define CONFIG_MASK_TRANSFER_PARAM	0x00000400								//ת�˲���
#define CONFIG_MASK_GAME_STOCK		0x00000800								//��Ϸ���
#define CONFIG_MASK_SPLAYER_CONTROL	0x00001000								//��R����
#define CONFIG_MASK_NICKANME_OPTION	0x00002000								//�ǳ�����
#define CONFIG_MASK_CLUB_OPTION		0x00004000								//��������

//ȫ��ѡ��
#define OPTION_MASK_CHECKIN			0x00010000								//ǩ��ѡ��
#define OPTION_MASK_TURNTABLE		0x00020000								//ת��ѡ��
#define OPTION_MASK_MARQUEE_NOTICE	0x00040000								//����ѡ��

//��Ϸ����
#define CONFIG_MASK_BASIC_CONFIG	0x01000000								//��������
#define CONFIG_MASK_WEIGHT_CONFIG	0x02000000								//Ȩ������

//////////////////////////////////////////////////////////////////////////
//��涨��

//�������
#define STOCK_KIND_GOLD				1									//��ҿ��

//���״̬
#define STOCK_STATE_LOCKED			2									//����״̬
#define STOCK_STATE_ENABLE			1									//����״̬
#define STOCK_STATE_DISABLE			0									//ͣ��״̬

//����ģʽ
#define STOCK_ENABLE_MODE_AUTO		0									//����ģʽ
#define STOCK_ENABLE_MODE_MANUAL	1									//�ֶ�ģʽ

//��漶��
#define STOCK_LEVEL_BET				1									//������
#define STOCK_LEVEL_GAME			2									//��Ϸ���
#define STOCK_LEVEL_GROUP			3									//������
#define STOCK_LEVEL_ACTIVITY		4									//����

//�ʳض���
#define MAX_JACKPOT_COUNT			5									//�ʳ�����

//////////////////////////////////////////////////////////////////////////
//������֤

//��֤������
#define CODE_KIND_MP				1									//�ֻ�����
#define CODE_KIND_EMAIL				2									//�ʼ�����

//��֤����
#define AUTH_KIND_BIND_MP			1									//���ֻ�
#define AUTH_KIND_UNBIND_MP			2									//����ֻ�
#define AUTH_KIND_BIND_EMAIL		3									//���ʼ�
#define AUTH_KIND_UNBIND_EMAIL		4									//����ʼ�
#define AUTH_KIND_RESETPASSWD		5									//��������
#define AUTH_KIND_CODE_LOGON		6									//��֤���½
#define AUTH_KIND_MB_REGISTER		7									//�ֻ�ע��
#define AUTH_KIND_EMAIL_REGISTER	8									//����ע��
#define AUTH_KIND_WEALTH_TRANSFER	10									//����ת��
#define AUTH_KIND_BIND_BANK_ACCOUNT	11									//���˺�

//�������
#define AUTHECODE_AT_BINDED			1									//�˺��Ѱ�
#define AUTHECODE_MP_BINDED			2									//�ֻ����Ѱ�
#define AUTHECODE_AC_HAVESENT		3									//��֤���ѷ���
#define AUTHECODE_DB_EXCEPTION		99									//���ݿ��쳣

//////////////////////////////////////////////////////////////////////////

//�����ʶ
#define NONE_SERVICE_ID				0									//�����ʶ
#define MAX_SERVICE_ID				(INVALID_WORD-1)					//�����ʶ
#define INVALID_SERVICE_ID			INVALID_WORD						//�����ʶ

//��������
#define SERVICE_COUNT				18									//��������

//��������
#define SERVICE_MASK_GATE			0x0001								//��������
#define SERVICE_MASK_GAME			0x0002								//��������
#define SERVICE_MASK_LOGON			0x0004								//��������
#define SERVICE_MASK_MATCH			0x0008								//��������
#define SERVICE_MASK_STORE			0x0010								//��������
#define SERVICE_MASK_BATTLE			0x0020								//��������
#define SERVICE_MASK_MESSAGE		0x0040								//��������
#define SERVICE_MASK_LOGGER			0x0080								//��������
#define SERVICE_MASK_MONITOR		0x0100								//��������
#define SERVICE_MASK_RANKING		0x0200								//��������
#define SERVICE_MASK_CHAT			0x0400								//��������
#define SERVICE_MASK_SYNC			0x0800								//��������
#define SERVICE_MASK_QUERY			0x1000								//��������
#define SERVICE_MASK_CLUB			0x2000								//��������
#define SERVICE_MASK_CLUBPLAZA		0x4000								//��������

//����ģ��
#define SERVICE_MODULE_NONE			0									//����ģ��
#define SERVICE_MODULE_GATE			1									//����ģ��
#define SERVICE_MODULE_GAME			2									//����ģ��
#define SERVICE_MODULE_LOGON		3									//����ģ��
#define SERVICE_MODULE_CHAT			4									//����ģ��
#define SERVICE_MODULE_SYNC			5									//����ģ��
#define SERVICE_MODULE_MATCH		6									//����ģ��
#define SERVICE_MODULE_STORE		7									//����ģ��
#define SERVICE_MODULE_AGENT		8									//����ģ��
#define SERVICE_MODULE_BATTLE		9									//����ģ��
#define SERVICE_MODULE_MESSAGE		10									//����ģ��
#define SERVICE_MODULE_RANKING		11									//����ģ��
#define SERVICE_MODULE_QUERY		12									//����ģ��
#define SERVICE_MODULE_LOGGER		13									//����ģ��
#define SERVICE_MODULE_MONITOR		14									//����ģ��
#define SERVICE_MODULE_MASTER		15									//����ģ��
#define SERVICE_MODULE_CLUB			16									//����ģ��
#define SERVICE_MODULE_CLUBPLAZA	17									//����ģ��

//ӳ�䷽ʽ
#define MAP_KIND_HASH				1									//��ϣӳ��
#define MAP_KIND_ROUTE				2									//·��ӳ��

//·��״̬
#define ROUTE_STATE_NULL			0									//��ʼ״̬
#define ROUTE_STATE_ACTIVE			1									//����״̬
#define ROUTE_STATE_SERVICE			2									//����״̬

//////////////////////////////////////////////////////////////////////////
//Redis

//��������
#define	PRODUCT_NAME				"lh"								//��Ʒ����
#define MSG_PAGE_SIZE				20									//ҳ���С

//����
#define RKEY_DDI_TOKEN				0									//��������
#define RKEY_DDI_STATE				1									//��������
#define RKEY_DDI_GROUP				2									//��������
#define RKEY_DDI_GMEMBER			3									//��������
#define RKEY_DDI_USER				4									//��������
#define RKEY_DDI_UGROUP				5									//��������	
#define RKEY_DDI_UFRIEND			6									//��������
#define RKEY_DDI_MESSAGE			7									//��������
#define RKEY_DDI_MQUEUE				8									//��������
#define RKEY_DDI_MASTER				0									//��������

//ǰ׺����
#define RKEY_PREFIX_GATELIST		PRODUCT_NAME "_gate_list"			//�����б�
#define RKEY_PREFIX_USER			PRODUCT_NAME "_user_"				//�û���Ϣ
#define RKEY_PREFIX_TOKEN			PRODUCT_NAME "_token_"				//������Ϣ		
#define RKEY_PREFIX_STATE			PRODUCT_NAME "_state_"				//״̬��Ϣ
#define RKEY_PREFIX_GROUP			PRODUCT_NAME "_group_"				//������Ϣ
#define RKEY_PREFIX_GMEMBER			PRODUCT_NAME "_gmember_"			//Ⱥ��Ա��Ϣ
#define RKEY_PREFIX_UGROUP			PRODUCT_NAME "_ugroup_"			//�û�����
#define RKEY_PREFIX_UFRIEND			PRODUCT_NAME "_ufriend_"			//�û�����
#define RKEY_PREFIX_UBLACKLIST		PRODUCT_NAME "_ublacklist_"		//�û�������
#define RKEY_PREFIX_MSG				PRODUCT_NAME "_msg_"				//��Ϣǰ׺
#define RKEY_PREFIX_MQUEUE			PRODUCT_NAME "_mqueue_"			//��Ϣ����
#define RKEY_PREFIX_SERVERLIST		PRODUCT_NAME "_server_list"		//�����б�
#define RKEY_PREFIX_RANKINGLIST		PRODUCT_NAME "_ranking_list_"		//���а�

//����ʱ��
#define TTL_WORLD_MESSAGE			43200000L							//����ʱ��
#define TTL_GROUP_MESSAGE			1296000000							//����ʱ��
#define TTL_PRIVATE_MESSAGE			1296000000							//����ʱ��

//////////////////////////////////////////////////////////////////////////

#endif