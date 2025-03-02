#ifndef CONDITION_HEAD_HEAD_FILE
#define CONDITION_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//功能选项

#define	FUNCTION_OPTION_CHAT		//聊天模块	
#define	FUNCTION_OPTION_VIDEO		//录像模块
#define	FUNCTION_OPTION_BATTLE		//约战模块

//////////////////////////////////////////////////////////////////////////
//功能选项

//聊天选项
#ifdef FUNCTION_OPTION_CHAT
	#define CHAT_SERVICE_MODULE
#else
	#undef	CHAT_SERVICE_MODULE
#endif

//录像选项
#ifdef FUNCTION_OPTION_VIDEO
	#define VIDEO_SERVICE_MODULE
#else
	#undef	VIDEO_SERVICE_MODULE
#endif

//比赛选项
#ifdef FUNCTION_OPTION_MACTH
	#define MATCH_SERVICE_MODULE
#else
	#undef	MATCH_SERVICE_MODULE
#endif

//约战选项
#ifdef FUNCTION_OPTION_BATTLE
	#define BATTLE_SERVICE_MODULE
#else
	#undef	BATTLE_SERVICE_MODULE
#endif

//排位选项
#ifdef FUNCTION_OPTION_RANKING
	#define RANKING_SERVICE_MODULE
#else
	#undef	RANKING_SERVICE_MODULE
#endif

//////////////////////////////////////////////////////////////////////////

#endif