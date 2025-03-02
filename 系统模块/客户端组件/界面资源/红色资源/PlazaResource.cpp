#include "StdAfx.h"
#include "Resource.h"
#include "PlazaResource.h"
#include "PlatformResource.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CPlazaResource::CPlazaResource()
{
}

//��������
CPlazaResource::~CPlazaResource()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CPlazaResource::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPlazaResource,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPlazaResource,Guid,dwQueryVer);
	return NULL;
}

//ʵ�����
HINSTANCE __cdecl CPlazaResource::GetResInstance()
{
	return g_PlatformResource.GetResInstance();
}

//��������
VOID __cdecl CPlazaResource::GetPlazaChatImage(tagPlazaChatImage & ResourceInfo)
{
	//��ɫ����
	ResourceInfo.crChatBK=COLOR_CHAT_BK;
	ResourceInfo.crSplitter=RGB(250,146,36);

	//������Դ
	ResourceInfo.uChatML=IDB_PLAZA_CHAT_ML;
	ResourceInfo.uChatMR=IDB_PLAZA_CHAT_MR;
	ResourceInfo.uChatTL=IDB_PLAZA_CHAT_TL;
	ResourceInfo.uChatTM=IDB_PLAZA_CHAT_TM;
	ResourceInfo.uChatTR=IDB_PLAZA_CHAT_TR;
	ResourceInfo.uChatBL=IDB_PLAZA_CHAT_BL;
	ResourceInfo.uChatBM=IDB_PLAZA_CHAT_BM;
	ResourceInfo.uChatBR=IDB_PLAZA_CHAT_BR;

	//�������
	ResourceInfo.uBtChatClose=IDB_BT_PLAZA_CHAT_CLOSE;
	ResourceInfo.uBtChatControl=IDB_BT_PLAZA_CHAT_CONTROL;
	ResourceInfo.uChatControlBack=IDB_PLAZA_CHAT_CONTROL_BACK;
	
	//���찴ť
	ResourceInfo.uBtPhrase=IDB_BT_PLAZA_CHAT_PHRASE;
	ResourceInfo.uBtStopChat=IDB_BT_PLAZA_CHAT_STOP_CHAT;
	ResourceInfo.uBtRecvChat=IDB_BT_PLAZA_CHAT_RECV_CHAT;
	ResourceInfo.uBtColorSet=IDB_BT_PLAZA_CHAT_COLOR_SET;
	ResourceInfo.uBtSaveChat=IDB_BT_PLAZA_CHAT_SAVE_CHAT;
	ResourceInfo.uBtSendChat=IDB_BT_PLAZA_CHAT_SEND_CHAT;
	ResourceInfo.uBtChatShort=IDB_BT_PLAZA_CHAT_CHAT_SHORT;
	ResourceInfo.uBtExpression=IDB_BT_PLAZA_CHAT_EXPESSION;
	ResourceInfo.uBtCleanScreen=IDB_BT_PLAZA_CHAT_CLEAN_SCREEN;
	ResourceInfo.uBtMoreFunction=IDB_BT_PLAZA_CHAT_MORE_FUNCTION;
	ResourceInfo.uBtBugle=IDB_BT_PLAZA_CHAT_BUGLE;

	return;
}

//��������
VOID __cdecl CPlazaResource::GetPlazaTableImage(tagPlazaTableImage & ResourceInfo)
{
	//������Դ
	ResourceInfo.uTableML=IDB_PLAZA_TABLE_ML;
	ResourceInfo.uTableMR=IDB_PLAZA_TABLE_MR;
	ResourceInfo.uTableTL=IDB_PLAZA_TABLE_TL;
	ResourceInfo.uTableTM=IDB_PLAZA_TABLE_TM;
	ResourceInfo.uTableTR=IDB_PLAZA_TABLE_TR;
	ResourceInfo.uTableBL=IDB_PLAZA_TABLE_BL;
	ResourceInfo.uTableBM=IDB_PLAZA_TABLE_BM;
	ResourceInfo.uTableBR=IDB_PLAZA_TABLE_BR;

	//������Դ
	ResourceInfo.uTableNumber=IDB_PLAZA_TABLE_NUMBER;
	ResourceInfo.uTableGround=IDB_PLAZA_TABLE_GROUND;
	ResourceInfo.uTableInvalid=IDB_PLAZA_TABLE_INVALID;

	//���Ӱ�ť
	ResourceInfo.uBtAutoJoin=IDB_BT_TABLE_FRAME_AUTO_JOIN;
	ResourceInfo.uBtJoinMenu=IDB_BT_TABLE_FRAME_JOIN_MENU;
	ResourceInfo.uBtViewMode=IDB_BT_TABLE_FRAME_VIEW_MODE;
	ResourceInfo.uBtQuitGame=IDB_BT_TABLE_FRAME_QUIT_GAME;
	ResourceInfo.uBtCallMaster=IDB_BT_TABLE_FRAME_CALL_MASTER;
	ResourceInfo.uBtSearchUser=IDB_BT_TABLE_FRAME_SEARCH_USER;
	ResourceInfo.uBtSecretTable=IDB_BT_TABLE_FRAME_SECRET_TABLE;
	ResourceInfo.uBtUnSecretTable=IDB_BT_TABLE_FRAME_UNSECRET_TABLE;
	ResourceInfo.uBtServerManage=IDB_BT_TABLE_FRAME_SERVER_MANAGE;
	
	return;
}

//�������
VOID __cdecl CPlazaResource::GetPlazaFrameImage(tagPlazaFrameImage & ResourceInfo)
{
	//��ɫ����
	ResourceInfo.crSplitter=RGB(153,4,0);

	//�����Դ
	ResourceInfo.uFrameML=IDB_PLAZA_FRAME_ML;
	ResourceInfo.uFrameMR=IDB_PLAZA_FRAME_MR;
	ResourceInfo.uFrameTL=IDB_PLAZA_FRAME_TL;
	ResourceInfo.uFrameTM=IDB_PLAZA_FRAME_TM;
	ResourceInfo.uFrameTR=IDB_PLAZA_FRAME_TR;
	ResourceInfo.uFrameBL=IDB_PLAZA_FRAME_BL;
	ResourceInfo.uFrameBM=IDB_PLAZA_FRAME_BM;
	ResourceInfo.uFrameBR=IDB_PLAZA_FRAME_BR;

	//�б���Դ
	ResourceInfo.uItemFrameML=IDB_ITEM_FRAME_ML;
	ResourceInfo.uItemFrameMR=IDB_ITEM_FRAME_MR;
	ResourceInfo.uItemFrameTL=IDB_ITEM_FRAME_TL;
	ResourceInfo.uItemFrameTM=IDB_ITEM_FRAME_TM;
	ResourceInfo.uItemFrameTR=IDB_ITEM_FRAME_TR;
	ResourceInfo.uItemFrameBL=IDB_ITEM_FRAME_BL;
	ResourceInfo.uItemFrameBM=IDB_ITEM_FRAME_BM;
	ResourceInfo.uItemFrameBR=IDB_ITEM_FRAME_BR;

	//��ܰ�ť
	ResourceInfo.uBtFrameMin=IDB_BT_PLAZA_FRAME_MIN;
	ResourceInfo.uBtFrameMax=IDB_BT_PLAZA_FRAME_MAX;
	ResourceInfo.uBtFrameClose=IDB_BT_PLAZA_FRAME_CLOSE;
	ResourceInfo.uBtFrameResore=IDB_BT_PLAZA_FRAME_RESORE;
	ResourceInfo.uBtFrameOption=IDB_BT_PLAZA_FRAME_OPTION;
	ResourceInfo.uBtFrameCompanion=IDB_BT_PLAZA_FRAME_COMPANION;
	ResourceInfo.uBtFrameBugReport=IDB_BT_PLAZA_FRAME_BUG_REPORT;
	ResourceInfo.uBtFrameSystemAffiche=IDB_BT_PLAZA_FRAME_SYSTEM_AFFICHE;
	ResourceInfo.uBtFrameBank=IDB_BT_PLAZA_FRAME_BANK;
	ResourceInfo.uBtFrameBaseEnsure=IDB_BT_PLAZA_FRAME_BASE_ENSURE;

	//�л���ť
	ResourceInfo.uBtShowGameList=IDB_BT_SHOW_GAME_LIST;
	ResourceInfo.uBtHideGameList=IDB_BT_HIDE_GAME_LIST;
	ResourceInfo.uBtFrameItemNormal=IDB_BT_FRAME_ITEM_NORMAL;
	ResourceInfo.uBtFrameItemActive=IDB_BT_FRAME_ITEM_AVTIVE;

	//������ť
	ResourceInfo.uFrameNavigation[0]=IDB_BT_PLAZA_FRAME_NAVIGATION_1;
	ResourceInfo.uFrameNavigation[1]=IDB_BT_PLAZA_FRAME_NAVIGATION_2;
	ResourceInfo.uFrameNavigation[2]=IDB_BT_PLAZA_FRAME_NAVIGATION_3;
	ResourceInfo.uFrameNavigation[3]=IDB_BT_PLAZA_FRAME_NAVIGATION_4;
	ResourceInfo.uFrameNavigation[4]=IDB_BT_PLAZA_FRAME_NAVIGATION_5;

	return;
}

//�����б�
VOID __cdecl CPlazaResource::GetPlazaServerImage(tagPlazaServerImage & ResourceInfo)
{
	//�б���Դ
	ResourceInfo.uServerML=IDB_PLAZA_SERVER_ML;
	ResourceInfo.uServerMR=IDB_PLAZA_SERVER_MR;
	ResourceInfo.uServerTL=IDB_PLAZA_SERVER_TL;
	ResourceInfo.uServerTM=IDB_PLAZA_SERVER_TM;
	ResourceInfo.uServerTR=IDB_PLAZA_SERVER_TR;
	ResourceInfo.uServerBL=IDB_PLAZA_SERVER_BL;
	ResourceInfo.uServerBM=IDB_PLAZA_SERVER_BM;
	ResourceInfo.uServerBR=IDB_PLAZA_SERVER_BR;

	//������ɫ
	ResourceInfo.crServerTX=COLOR_SERVER_TX;
	ResourceInfo.crServerBK=COLOR_SERVER_BK;

	return;
}

//�������
VOID __cdecl CPlazaResource::GetPlazaBrowserImage(tagPlazaBrowserImage & ResourceInfo)
{
	//�����Դ
	ResourceInfo.uBrowserML=IDB_PLAZA_BROWSER_ML;
	ResourceInfo.uBrowserMR=IDB_PLAZA_BROWSER_MR;
	ResourceInfo.uBrowserTL=IDB_PLAZA_BROWSER_TL;
	ResourceInfo.uBrowserTM=IDB_PLAZA_BROWSER_TM;
	ResourceInfo.uBrowserTR=IDB_PLAZA_BROWSER_TR;
	ResourceInfo.uBrowserBL=IDB_PLAZA_BROWSER_BL;
	ResourceInfo.uBrowserBM=IDB_PLAZA_BROWSER_BM;
	ResourceInfo.uBrowserBR=IDB_PLAZA_BROWSER_BR;

	//��ť��Դ
	ResourceInfo.uBtLast=IDB_BT_PLAZA_BROWSER_LAST;
	ResourceInfo.uBtNext=IDB_BT_PLAZA_BROWSER_NEXT;
	ResourceInfo.uBtStop=IDB_BT_PLAZA_BROWSER_STOP;
	ResourceInfo.uBtHome=IDB_BT_PLAZA_BROWSER_HOME;
	ResourceInfo.uBtReload=IDB_BT_PLAZA_BROWSER_RELOAD;

	return;
}

//��������
VOID __cdecl CPlazaResource::GetPlazaServiceImage(tagPlazaServiceImage & ResourceInfo)
{
	//�������
	ResourceInfo.uTitleML=IDB_ENCIRCLE_ML;
	ResourceInfo.uTitleMR=IDB_ENCIRCLE_MR;
	ResourceInfo.uTitleTL=IDB_ENCIRCLE_TL;
	ResourceInfo.uTitleTM=IDB_ENCIRCLE_TM;
	ResourceInfo.uTitleTR=IDB_ENCIRCLE_TR;
	ResourceInfo.uTitleBL=IDB_ENCIRCLE_BL;
	ResourceInfo.uTitleBM=IDB_ENCIRCLE_BM;
	ResourceInfo.uTitleBR=IDB_ENCIRCLE_BR;

	//����ר��
	ResourceInfo.uUserServiceBar=IDB_USER_SERVICE_BAR;
	ResourceInfo.uMemberServiceBar=IDB_MEMBER_SERVICE_BAR;

	//��ɫ����
	ResourceInfo.crServiceItemTX=RGB(10,10,10);
	ResourceInfo.crServiceItemBK=RGB(254,250,217);

	return;
}

//����˽��
VOID __cdecl CPlazaResource::GetPlazaWhisperImage(tagPlazaWhisperImage & ResourceInfo)
{
	//������Դ
	ResourceInfo.uChatML=IDB_CHAT_WHISPER_ML;
	ResourceInfo.uChatMR=IDB_CHAT_WHISPER_MR;
	ResourceInfo.uChatTL=IDB_CHAT_WHISPER_TL;
	ResourceInfo.uChatTM=IDB_CHAT_WHISPER_TM;
	ResourceInfo.uChatTR=IDB_CHAT_WHISPER_TR;
	ResourceInfo.uChatBL=IDB_CHAT_WHISPER_BL;
	ResourceInfo.uChatBM=IDB_CHAT_WHISPER_BM;
	ResourceInfo.uChatBR=IDB_CHAT_WHISPER_BR;

	//�û���Դ
	ResourceInfo.uUserML=IDB_USER_WHISPER_ML;
	ResourceInfo.uUserMR=IDB_USER_WHISPER_MR;
	ResourceInfo.uUserTL=IDB_USER_WHISPER_TL;
	ResourceInfo.uUserTM=IDB_USER_WHISPER_TM;
	ResourceInfo.uUserTR=IDB_USER_WHISPER_TR;
	ResourceInfo.uUserBL=IDB_USER_WHISPER_BL;
	ResourceInfo.uUserBM=IDB_USER_WHISPER_BM;
	ResourceInfo.uUserBR=IDB_USER_WHISPER_BR;

	//������Դ
	ResourceInfo.uInputML=IDB_INPUT_WHISPER_ML;
	ResourceInfo.uInputMR=IDB_INPUT_WHISPER_MR;
	ResourceInfo.uInputTL=IDB_INPUT_WHISPER_TL;
	ResourceInfo.uInputTM=IDB_INPUT_WHISPER_TM;
	ResourceInfo.uInputTR=IDB_INPUT_WHISPER_TR;
	ResourceInfo.uInputBL=IDB_INPUT_WHISPER_BL;
	ResourceInfo.uInputBM=IDB_INPUT_WHISPER_BM;
	ResourceInfo.uInputBR=IDB_INPUT_WHISPER_BR;

	//��ť��Դ
	ResourceInfo.uBtAddUser=IDB_BT_WHISPER_ADD_USER;
	ResourceInfo.uBtColorSet=IDB_BT_WHISPER_COLOR_SET;
	ResourceInfo.uBtLeaveChat=IDB_BT_WHISPER_LEAVE_CHAT;
	ResourceInfo.uBtExpression=IDB_BT_WHISPER_EXPESSION;
	ResourceInfo.uBtCleanScreen=IDB_BT_WHISPER_CLEAN_SCREEN;
	ResourceInfo.uBtMoreFunction=IDB_BT_WHISPER_MORE_FUNCTION;

	//���ư�ť
	ResourceInfo.uBtSendControl=IDB_BT_WHISPER_SEND_CONTROL;

	return;
}

//�����û�
VOID __cdecl CPlazaResource::GetPlazaUserListImage(tagPlazaUserListImage & ResourceInfo)
{
	//�б���Դ
	ResourceInfo.uUserListML=IDB_PLAZA_USER_LIST_ML;
	ResourceInfo.uUserListMR=IDB_PLAZA_USER_LIST_MR;
	ResourceInfo.uUserListTL=IDB_PLAZA_USER_LIST_TL;
	ResourceInfo.uUserListTM=IDB_PLAZA_USER_LIST_TM;
	ResourceInfo.uUserListTR=IDB_PLAZA_USER_LIST_TR;
	ResourceInfo.uUserListBL=IDB_PLAZA_USER_LIST_BL;
	ResourceInfo.uUserListBM=IDB_PLAZA_USER_LIST_BM;
	ResourceInfo.uUserListBR=IDB_PLAZA_USER_LIST_BR;

	//�л���ť
	ResourceInfo.uBtShowUserList=IDB_BT_SHOW_USER_LIST;
	ResourceInfo.uBtHideUserList=IDB_BT_HIDE_USER_LIST;
	ResourceInfo.uBtUserCompetition=IDB_BT_USER_COMPETITION;
	ResourceInfo.uBtNormalUserNormal=IDB_BT_NORMAL_USER_NORMAL;
	ResourceInfo.uBtNormalUserActive=IDB_BT_NORMAL_USER_ACTIVE;
	ResourceInfo.uBtCustomUserNormal=IDB_BT_CUSTOM_USER_NORMAL;
	ResourceInfo.uBtCustomUserActive=IDB_BT_CUSTOM_USER_ACTIVE;

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

//�����û�
VOID __cdecl CPlazaResource::GetPlazaUserInfoImage(tagPlazaUserInfoImage & ResourceInfo)
{
	//��ɫ��Ϣ
	ResourceInfo.crHyperText=RGB(41,11,198);
	ResourceInfo.crHyperBack=RGB(254,168,56);
	ResourceInfo.crUserInfoText=RGB(64,0,0);

	//�û���Ϣ
	ResourceInfo.uUserInfoL=IDB_PLAZA_USER_INFO_L;
	ResourceInfo.uUserInfoM=IDB_PLAZA_USER_INFO_M;
	ResourceInfo.uUserInfoR=IDB_PLAZA_USER_INFO_R;
	ResourceInfo.uBtUserInfoMenu=IDB_BT_PLAZA_USER_INFO_MENU;

	return;
}

//��������
VOID __cdecl CPlazaResource::GetPlazaCollocateImage(tagPlazaCollocateImage & ResourceInfo)
{
	//���ñ���
	ResourceInfo.uTitleML=IDB_ENCIRCLE_ML;
	ResourceInfo.uTitleMR=IDB_ENCIRCLE_MR;
	ResourceInfo.uTitleTL=IDB_ENCIRCLE_TL;
	ResourceInfo.uTitleTM=IDB_ENCIRCLE_TM;
	ResourceInfo.uTitleTR=IDB_ENCIRCLE_TR;
	ResourceInfo.uTitleBL=IDB_ENCIRCLE_BL;
	ResourceInfo.uTitleBM=IDB_ENCIRCLE_BM;
	ResourceInfo.uTitleBR=IDB_ENCIRCLE_BR;
					 
	//������Դ
	ResourceInfo.uCollocateBar=IDB_COLLOCATE_BAR;
	ResourceInfo.pszBtCollocateBar=TEXT("COLLOCATE_BAR_BT");

	//��ɫ����
	ResourceInfo.crItemTitleTX=RGB(10,10,10);
	ResourceInfo.crItemHoverTX=RGB(10,10,10);
	ResourceInfo.crItemActiveTX=RGB(129,32,1);
	ResourceInfo.crItemNormalTX=RGB(10,10,10);

	return;
}

//��ȫ����
VOID __cdecl CPlazaResource::GetPlazaSafePromptImage(tagPlazaSafePromptImage & ResourceInfo)
{
	ResourceInfo.uBtProtect=IDB_SAFE_PROMPT;
	ResourceInfo.uBtBind=IDB_SAFE_BIND;
}

//������Դ
VOID __cdecl CPlazaResource::GetBugleImage(tagBugleImage &ResourceInfo)
{
	ResourceInfo.uBtColorSet=IDB_BUGLE_COLOR_SET;
	ResourceInfo.uBtExpression=IDB_BUGLE_EXPRESSION;
}

//������Դ
VOID __cdecl CPlazaResource::GetDownloadImage(tagDownLoadImage &ResourceInfo)
{
	ResourceInfo.uBtLoadLast=IDB_DOWNLOAD_LOAD_LAST;
	ResourceInfo.uBtLoadNext=IDB_DOWNLOAD_LOAD_NEXT;
}
//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(PlazaResource);

//////////////////////////////////////////////////////////////////////////
