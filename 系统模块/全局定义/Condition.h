#ifndef CONDITION_HEAD_HEAD_FILE
#define CONDITION_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//����ѡ��

#define	FUNCTION_OPTION_CHAT		//����ģ��	
#define	FUNCTION_OPTION_VIDEO		//¼��ģ��
#define	FUNCTION_OPTION_BATTLE		//Լսģ��

//////////////////////////////////////////////////////////////////////////
//����ѡ��

//����ѡ��
#ifdef FUNCTION_OPTION_CHAT
	#define CHAT_SERVICE_MODULE
#else
	#undef	CHAT_SERVICE_MODULE
#endif

//¼��ѡ��
#ifdef FUNCTION_OPTION_VIDEO
	#define VIDEO_SERVICE_MODULE
#else
	#undef	VIDEO_SERVICE_MODULE
#endif

//����ѡ��
#ifdef FUNCTION_OPTION_MACTH
	#define MATCH_SERVICE_MODULE
#else
	#undef	MATCH_SERVICE_MODULE
#endif

//Լսѡ��
#ifdef FUNCTION_OPTION_BATTLE
	#define BATTLE_SERVICE_MODULE
#else
	#undef	BATTLE_SERVICE_MODULE
#endif

//��λѡ��
#ifdef FUNCTION_OPTION_RANKING
	#define RANKING_SERVICE_MODULE
#else
	#undef	RANKING_SERVICE_MODULE
#endif

//////////////////////////////////////////////////////////////////////////

#endif