#include "StdAfx.h"
#include "Resource.h"
#include "FrameResource.h"
#include "PlatformResource.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CFrameResource::CFrameResource()
{
}

//��������
CFrameResource::~CFrameResource()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CFrameResource::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IFrameResource,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IFrameResource,Guid,dwQueryVer);
	return NULL;
}

//ʵ�����
HINSTANCE __cdecl CFrameResource::GetResInstance()
{
	return g_PlatformResource.GetResInstance();
}

//��Ϸ����
VOID __cdecl CFrameResource::GetGameBarImage(tagGameBarImage & ResourceInfo)
{
	//������Դ
	ResourceInfo.uGameBar=IDB_GAME_BAR;

	//���ư�ť
	ResourceInfo.uBtHideControl=IDB_BT_GAME_BAR_HIDE_CONTROL;
	ResourceInfo.uBtShowControl=IDB_BT_GAME_BAR_SHOW_CONTROL;

	//���ܰ�ť
	ResourceInfo.uBtMore=IDB_BT_GAME_BAR_MORE;
	ResourceInfo.uBtInvite=IDB_BT_GAME_BAR_INVITE;
	ResourceInfo.uBtOption=IDB_BT_GAME_BAR_OPTION;
	ResourceInfo.uBtLimitSound=IDB_BT_GAME_BAR_LIMIT_SOUND;
	ResourceInfo.uBtAllowSound=IDB_BT_GAME_BAR_ALLOW_SOUND;

	return;
}

//��Ϸ���
VOID __cdecl CFrameResource::GetGameBillImage(tagGameBillImage & ResourceInfo)
{
	//�����Դ
	ResourceInfo.uBillL=IDB_GAME_BILL_L;
	ResourceInfo.uBillM=IDB_GAME_BILL_M;
	ResourceInfo.uBillR=IDB_GAME_BILL_R;

	//���ư�ť
	ResourceInfo.uBtShowBill=IDB_BT_SHOW_GAME_BILL;
	ResourceInfo.uBtHideBill=IDB_BT_HIDE_GAME_BILL;

	return;
}

//��Ϸ����
VOID __cdecl CFrameResource::GetGameChatImage(tagGameChatImage & ResourceInfo)
{
	//������Դ
	ResourceInfo.uChatML=IDB_GAME_CHAT_ML;
	ResourceInfo.uChatMR=IDB_GAME_CHAT_MR;
	ResourceInfo.uChatTL=IDB_GAME_CHAT_TL;
	ResourceInfo.uChatTM=IDB_GAME_CHAT_TM;
	ResourceInfo.uChatTR=IDB_GAME_CHAT_TR;
	ResourceInfo.uChatBL=IDB_GAME_CHAT_BL;
	ResourceInfo.uChatBM=IDB_GAME_CHAT_BM;
	ResourceInfo.uChatBR=IDB_GAME_CHAT_BR;

	//������Դ
	ResourceInfo.uChatHML=IDB_GAME_CHAT_H_ML;
	ResourceInfo.uChatHMR=IDB_GAME_CHAT_H_MR;
	ResourceInfo.uChatHTL=IDB_GAME_CHAT_H_TL;
	ResourceInfo.uChatHTM=IDB_GAME_CHAT_H_TM;
	ResourceInfo.uChatHTR=IDB_GAME_CHAT_H_TR;
	ResourceInfo.uChatHBL=IDB_GAME_CHAT_H_BL;
	ResourceInfo.uChatHBM=IDB_GAME_CHAT_H_BM;
	ResourceInfo.uChatHBR=IDB_GAME_CHAT_H_BR;

	//�������
	ResourceInfo.uBtChatClose=IDB_BT_GAME_CHAT_CLOSE;
	ResourceInfo.uBtChatControl=IDB_BT_GAME_CHAT_CONTROL;
	ResourceInfo.uChatControlBack=IDB_GAME_CHAT_CONTROL_BACK;

	//���찴ť
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


	//��ɫ����
	ResourceInfo.crChatBK=COLOR_CHAT_BK;

	return;
}

//��Ϸ���
VOID __cdecl CFrameResource::GetGameFrameImage(tagGameFrameImage & ResourceInfo)
{
	//��ɫ����
	ResourceInfo.crTitleTX=RGB(239,239,239);

	//�����Դ
	ResourceInfo.uFrameML=IDB_GAME_FRAME_ML;
	ResourceInfo.uFrameMR=IDB_GAME_FRAME_MR;
	ResourceInfo.uFrameTL=IDB_GAME_FRAME_TL;
	ResourceInfo.uFrameTM=IDB_GAME_FRAME_TM;
	ResourceInfo.uFrameTR=IDB_GAME_FRAME_TR;
	ResourceInfo.uFrameBL=IDB_GAME_FRAME_BL;
	ResourceInfo.uFrameBM=IDB_GAME_FRAME_BM;
	ResourceInfo.uFrameBR=IDB_GAME_FRAME_BR;

	//��ܰ�ť
	ResourceInfo.uBtFrameMin=IDB_GAME_FRAME_BT_MIN;
	ResourceInfo.uBtFrameMax=IDB_GAME_FRAME_BT_MAX;
	ResourceInfo.uBtFrameClose=IDB_GAME_FRAME_BT_CLOSE;
	ResourceInfo.uBtFrameResore=IDB_GAME_FRAME_BT_RESORE;

	return;
}

//��Ϸ�û�
VOID __cdecl CFrameResource::GetGameUserInfoImage(tagGameUserInfoImage & ResourceInfo)
{
	//�û���Դ
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

//��Ϸ�б�
VOID __cdecl CFrameResource::GetGameUserListImage(tagGameUserListImage & ResourceInfo)
{
	//�û���Դ
	ResourceInfo.uUserListML=IDB_GAME_USER_LIST_ML;
	ResourceInfo.uUserListMR=IDB_GAME_USER_LIST_MR;
	ResourceInfo.uUserListTL=IDB_GAME_USER_LIST_TL;
	ResourceInfo.uUserListTM=IDB_GAME_USER_LIST_TM;
	ResourceInfo.uUserListTR=IDB_GAME_USER_LIST_TR;
	ResourceInfo.uUserListBL=IDB_GAME_USER_LIST_BL;
	ResourceInfo.uUserListBM=IDB_GAME_USER_LIST_BM;
	ResourceInfo.uUserListBR=IDB_GAME_USER_LIST_BR;

	//�û���Դ
	ResourceInfo.uUserListHML=IDB_GAME_USER_LIST_H_ML;
	ResourceInfo.uUserListHMR=IDB_GAME_USER_LIST_H_MR;
	ResourceInfo.uUserListHTL=IDB_GAME_USER_LIST_H_TL;
	ResourceInfo.uUserListHTM=IDB_GAME_USER_LIST_H_TM;
	ResourceInfo.uUserListHTR=IDB_GAME_USER_LIST_H_TR;
	ResourceInfo.uUserListHBL=IDB_GAME_USER_LIST_H_BL;
	ResourceInfo.uUserListHBM=IDB_GAME_USER_LIST_H_BM;
	ResourceInfo.uUserListHBR=IDB_GAME_USER_LIST_H_BR;

	//��ɫ����
	ResourceInfo.crUserListSelectTX=COLOR_SELECT_TX;
	ResourceInfo.crUserListSelectBK=COLOR_SELECT_BK;
	ResourceInfo.crUserListNormalTX=COLOR_NORMAL_TX;
	ResourceInfo.crUserListNormalBK=COLOR_NORMAL_BK;

	//��ɫ����
	ResourceInfo.crUserListMyselfTX=COLOR_MYSELF_TX;
	ResourceInfo.crUserListMyselfBK=COLOR_MYSELF_BK;
	ResourceInfo.crUserListMasterTX=COLOR_MASTER_TX;
	ResourceInfo.crUserListMasterBK=COLOR_MASTER_BK;
	ResourceInfo.crUserListMemberTX=COLOR_MEMBER_TX;
	ResourceInfo.crUserListMemberBK=COLOR_MEMBER_BK;

	return;
}

//������Դ
VOID __cdecl CFrameResource::GetBugleImage(tagBugleImage &ResourceInfo)
{
	ResourceInfo.uBtColorSet=IDB_BUGLE_COLOR_SET;
	ResourceInfo.uBtExpression=IDB_BUGLE_EXPRESSION;
}
//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(FrameResource);

//////////////////////////////////////////////////////////////////////////
