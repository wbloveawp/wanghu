#include "StdAfx.h"
#include "Resource.h"
#include "FrameResource.h"
#include "PlatformResource.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CFrameResource::CFrameResource()
{
}

//析构函数
CFrameResource::~CFrameResource()
{
}

//接口查询
VOID * __cdecl CFrameResource::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IFrameResource,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IFrameResource,Guid,dwQueryVer);
	return NULL;
}

//实例句柄
HINSTANCE __cdecl CFrameResource::GetResInstance()
{
	return g_PlatformResource.GetResInstance();
}

//游戏控制
VOID __cdecl CFrameResource::GetGameBarImage(tagGameBarImage & ResourceInfo)
{
	//背景资源
	ResourceInfo.uGameBar=IDB_GAME_BAR;

	//控制按钮
	ResourceInfo.uBtHideControl=IDB_BT_GAME_BAR_HIDE_CONTROL;
	ResourceInfo.uBtShowControl=IDB_BT_GAME_BAR_SHOW_CONTROL;

	//功能按钮
	ResourceInfo.uBtMore=IDB_BT_GAME_BAR_MORE;
	ResourceInfo.uBtInvite=IDB_BT_GAME_BAR_INVITE;
	ResourceInfo.uBtOption=IDB_BT_GAME_BAR_OPTION;
	ResourceInfo.uBtLimitSound=IDB_BT_GAME_BAR_LIMIT_SOUND;
	ResourceInfo.uBtAllowSound=IDB_BT_GAME_BAR_ALLOW_SOUND;

	return;
}

//游戏广告
VOID __cdecl CFrameResource::GetGameBillImage(tagGameBillImage & ResourceInfo)
{
	//广告资源
	ResourceInfo.uBillL=IDB_GAME_BILL_L;
	ResourceInfo.uBillM=IDB_GAME_BILL_M;
	ResourceInfo.uBillR=IDB_GAME_BILL_R;

	//控制按钮
	ResourceInfo.uBtShowBill=IDB_BT_SHOW_GAME_BILL;
	ResourceInfo.uBtHideBill=IDB_BT_HIDE_GAME_BILL;

	return;
}

//游戏聊天
VOID __cdecl CFrameResource::GetGameChatImage(tagGameChatImage & ResourceInfo)
{
	//聊天资源
	ResourceInfo.uChatML=IDB_GAME_CHAT_ML;
	ResourceInfo.uChatMR=IDB_GAME_CHAT_MR;
	ResourceInfo.uChatTL=IDB_GAME_CHAT_TL;
	ResourceInfo.uChatTM=IDB_GAME_CHAT_TM;
	ResourceInfo.uChatTR=IDB_GAME_CHAT_TR;
	ResourceInfo.uChatBL=IDB_GAME_CHAT_BL;
	ResourceInfo.uChatBM=IDB_GAME_CHAT_BM;
	ResourceInfo.uChatBR=IDB_GAME_CHAT_BR;

	//聊天资源
	ResourceInfo.uChatHML=IDB_GAME_CHAT_H_ML;
	ResourceInfo.uChatHMR=IDB_GAME_CHAT_H_MR;
	ResourceInfo.uChatHTL=IDB_GAME_CHAT_H_TL;
	ResourceInfo.uChatHTM=IDB_GAME_CHAT_H_TM;
	ResourceInfo.uChatHTR=IDB_GAME_CHAT_H_TR;
	ResourceInfo.uChatHBL=IDB_GAME_CHAT_H_BL;
	ResourceInfo.uChatHBM=IDB_GAME_CHAT_H_BM;
	ResourceInfo.uChatHBR=IDB_GAME_CHAT_H_BR;

	//聊天控制
	ResourceInfo.uBtChatClose=IDB_BT_GAME_CHAT_CLOSE;
	ResourceInfo.uBtChatControl=IDB_BT_GAME_CHAT_CONTROL;
	ResourceInfo.uChatControlBack=IDB_GAME_CHAT_CONTROL_BACK;

	//聊天按钮
	ResourceInfo.uBtPhrase=IDB_BT_GAME_CHAT_PHRASE;
	ResourceInfo.uBtStopChat=IDB_BT_GAME_CHAT_STOP_CHAT;
	ResourceInfo.uBtRecvChat=IDB_BT_GAME_CHAT_RECV_CHAT;
	ResourceInfo.uBtColorSet=IDB_BT_GAME_CHAT_COLOR_SET;
	ResourceInfo.uBtSaveChat=IDB_BT_GAME_CHAT_SAVE_CHAT;
	ResourceInfo.uBtSendChat=IDB_BT_GAME_CHAT_SEND_CHAT;
	ResourceInfo.uBtChatShort=IDB_BT_GAME_CHAT_CHAT_SHORT;
	ResourceInfo.uBtExpression=IDB_BT_GAME_CHAT_EXPESSION;
	ResourceInfo.uBtCleanScreen=IDB_BT_GAME_CHAT_CLEAN_SCREEN;
	ResourceInfo.uBtMoreFunction=IDB_BT_GAME_CHAT_MORE_FUNCTION;
	ResourceInfo.uBtBugle=IDB_BT_GAME_CHAT_BUGLE;


	//颜色定义
	ResourceInfo.crChatBK=COLOR_CHAT_BK;

	return;
}

//游戏框架
VOID __cdecl CFrameResource::GetGameFrameImage(tagGameFrameImage & ResourceInfo)
{
	//颜色定义
	ResourceInfo.crTitleTX=RGB(239,239,239);

	//框架资源
	ResourceInfo.uFrameML=IDB_GAME_FRAME_ML;
	ResourceInfo.uFrameMR=IDB_GAME_FRAME_MR;
	ResourceInfo.uFrameTL=IDB_GAME_FRAME_TL;
	ResourceInfo.uFrameTM=IDB_GAME_FRAME_TM;
	ResourceInfo.uFrameTR=IDB_GAME_FRAME_TR;
	ResourceInfo.uFrameBL=IDB_GAME_FRAME_BL;
	ResourceInfo.uFrameBM=IDB_GAME_FRAME_BM;
	ResourceInfo.uFrameBR=IDB_GAME_FRAME_BR;

	//框架按钮
	ResourceInfo.uBtFrameMin=IDB_GAME_FRAME_BT_MIN;
	ResourceInfo.uBtFrameMax=IDB_GAME_FRAME_BT_MAX;
	ResourceInfo.uBtFrameClose=IDB_GAME_FRAME_BT_CLOSE;
	ResourceInfo.uBtFrameResore=IDB_GAME_FRAME_BT_RESORE;

	return;
}

//游戏用户
VOID __cdecl CFrameResource::GetGameUserInfoImage(tagGameUserInfoImage & ResourceInfo)
{
	//用户资源
	ResourceInfo.uUserInfoML=IDB_GAME_USER_INFO_ML;
	ResourceInfo.uUserInfoMR=IDB_GAME_USER_INFO_MR;
	ResourceInfo.uUserInfoTL=IDB_GAME_USER_INFO_TL;
	ResourceInfo.uUserInfoTM=IDB_GAME_USER_INFO_TM;
	ResourceInfo.uUserInfoTR=IDB_GAME_USER_INFO_TR;
	ResourceInfo.uUserInfoBL=IDB_GAME_USER_INFO_BL;
	ResourceInfo.uUserInfoBM=IDB_GAME_USER_INFO_BM;
	ResourceInfo.uUserInfoBR=IDB_GAME_USER_INFO_BR;

	return;
}

//游戏列表
VOID __cdecl CFrameResource::GetGameUserListImage(tagGameUserListImage & ResourceInfo)
{
	//用户资源
	ResourceInfo.uUserListML=IDB_GAME_USER_LIST_ML;
	ResourceInfo.uUserListMR=IDB_GAME_USER_LIST_MR;
	ResourceInfo.uUserListTL=IDB_GAME_USER_LIST_TL;
	ResourceInfo.uUserListTM=IDB_GAME_USER_LIST_TM;
	ResourceInfo.uUserListTR=IDB_GAME_USER_LIST_TR;
	ResourceInfo.uUserListBL=IDB_GAME_USER_LIST_BL;
	ResourceInfo.uUserListBM=IDB_GAME_USER_LIST_BM;
	ResourceInfo.uUserListBR=IDB_GAME_USER_LIST_BR;

	//用户资源
	ResourceInfo.uUserListHML=IDB_GAME_USER_LIST_H_ML;
	ResourceInfo.uUserListHMR=IDB_GAME_USER_LIST_H_MR;
	ResourceInfo.uUserListHTL=IDB_GAME_USER_LIST_H_TL;
	ResourceInfo.uUserListHTM=IDB_GAME_USER_LIST_H_TM;
	ResourceInfo.uUserListHTR=IDB_GAME_USER_LIST_H_TR;
	ResourceInfo.uUserListHBL=IDB_GAME_USER_LIST_H_BL;
	ResourceInfo.uUserListHBM=IDB_GAME_USER_LIST_H_BM;
	ResourceInfo.uUserListHBR=IDB_GAME_USER_LIST_H_BR;

	//颜色定义
	ResourceInfo.crUserListSelectTX=COLOR_SELECT_TX;
	ResourceInfo.crUserListSelectBK=COLOR_SELECT_BK;
	ResourceInfo.crUserListNormalTX=COLOR_NORMAL_TX;
	ResourceInfo.crUserListNormalBK=COLOR_NORMAL_BK;

	//颜色定义
	ResourceInfo.crUserListMyselfTX=COLOR_MYSELF_TX;
	ResourceInfo.crUserListMyselfBK=COLOR_MYSELF_BK;
	ResourceInfo.crUserListMasterTX=COLOR_MASTER_TX;
	ResourceInfo.crUserListMasterBK=COLOR_MASTER_BK;
	ResourceInfo.crUserListMemberTX=COLOR_MEMBER_TX;
	ResourceInfo.crUserListMemberBK=COLOR_MEMBER_BK;

	return;
}

//喇叭资源
VOID __cdecl CFrameResource::GetBugleImage(tagBugleImage &ResourceInfo)
{
	ResourceInfo.uBtColorSet=IDB_BUGLE_COLOR_SET;
	ResourceInfo.uBtExpression=IDB_BUGLE_EXPRESSION;
}
//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(FrameResource);

//////////////////////////////////////////////////////////////////////////
