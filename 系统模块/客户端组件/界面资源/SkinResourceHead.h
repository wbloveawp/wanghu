#ifndef SKIN_RESOURCE_HEAD_HEAD_FILE
#define SKIN_RESOURCE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//游戏资源

//游戏控制
struct tagGameBarImage
{
	//背景资源
	UINT							uGameBar;							//资源标识

	//控制按钮
	UINT							uBtHideControl;						//资源标识
	UINT							uBtShowControl;						//资源标识

	//功能按钮
	UINT							uBtMore;							//资源标识
	UINT							uBtInvite;							//资源标识
	UINT							uBtOption;							//资源标识
	UINT							uBtLimitSound;						//资源标识
	UINT							uBtAllowSound;						//资源标识
};

//游戏广告
struct tagGameBillImage
{
	//广告资源
	UINT							uBillL;								//资源标识
	UINT							uBillM;								//资源标识
	UINT							uBillR;								//资源标识

	//广告按钮
	UINT							uBtShowBill;						//资源标识
	UINT							uBtHideBill;						//资源标识
};

//游戏聊天
struct tagGameChatImage
{
	//聊天资源
	UINT							uChatML;							//资源标识
	UINT							uChatMR;							//资源标识
	UINT							uChatTL;							//资源标识
	UINT							uChatTM;							//资源标识
	UINT							uChatTR;							//资源标识
	UINT							uChatBL;							//资源标识
	UINT							uChatBM;							//资源标识
	UINT							uChatBR;							//资源标识

	//聊天资源
	UINT							uChatHML;							//资源标识
	UINT							uChatHMR;							//资源标识
	UINT							uChatHTL;							//资源标识
	UINT							uChatHTM;							//资源标识
	UINT							uChatHTR;							//资源标识
	UINT							uChatHBL;							//资源标识
	UINT							uChatHBM;							//资源标识
	UINT							uChatHBR;							//资源标识

	//聊天按钮
	UINT							uBtPhrase;							//资源标识
	UINT							uBtStopChat;						//资源标识
	UINT							uBtRecvChat;						//资源标识
	UINT							uBtColorSet;						//资源标识
	UINT							uBtSaveChat;						//资源标识
	UINT							uBtSendChat;						//资源标识
	UINT							uBtChatShort;						//资源标识
	UINT							uBtExpression;						//资源标识
	UINT							uBtCleanScreen;						//资源标识
	UINT							uBtMoreFunction;					//资源标识
	UINT							uBtBugle;							//资源标识

	//聊天控制
	UINT							uBtChatClose;						//资源标识
	UINT							uBtChatControl;						//资源标识
	UINT							uChatControlBack;					//资源标识

	//颜色定义
	COLORREF						crChatBK;							//颜色定义
};

//游戏框架
struct tagGameFrameImage
{
	//颜色定义
	COLORREF						crTitleTX;							//颜色定义

	//框架资源
	UINT							uFrameML;							//资源标识
	UINT							uFrameMR;							//资源标识
	UINT							uFrameTL;							//资源标识
	UINT							uFrameTM;							//资源标识
	UINT							uFrameTR;							//资源标识
	UINT							uFrameBL;							//资源标识
	UINT							uFrameBM;							//资源标识
	UINT							uFrameBR;							//资源标识

	//框架按钮
	UINT							uBtFrameMin;						//资源标识
	UINT							uBtFrameMax;						//资源标识
	UINT							uBtFrameClose;						//资源标识
	UINT							uBtFrameResore;						//资源标识
};

//游戏用户
struct tagGameUserInfoImage
{
	//用户资源
	UINT							uUserInfoML;						//资源标识
	UINT							uUserInfoMR;						//资源标识
	UINT							uUserInfoTL;						//资源标识
	UINT							uUserInfoTM;						//资源标识
	UINT							uUserInfoTR;						//资源标识
	UINT							uUserInfoBL;						//资源标识
	UINT							uUserInfoBM;						//资源标识
	UINT							uUserInfoBR;						//资源标识
};

//游戏列表
struct tagGameUserListImage
{
	//用户资源
	UINT							uUserListML;						//资源标识
	UINT							uUserListMR;						//资源标识
	UINT							uUserListTL;						//资源标识
	UINT							uUserListTM;						//资源标识
	UINT							uUserListTR;						//资源标识
	UINT							uUserListBL;						//资源标识
	UINT							uUserListBM;						//资源标识
	UINT							uUserListBR;						//资源标识

	//用户资源
	UINT							uUserListHML;						//资源标识
	UINT							uUserListHMR;						//资源标识
	UINT							uUserListHTL;						//资源标识
	UINT							uUserListHTM;						//资源标识
	UINT							uUserListHTR;						//资源标识
	UINT							uUserListHBL;						//资源标识
	UINT							uUserListHBM;						//资源标识
	UINT							uUserListHBR;						//资源标识

	//颜色定义
	COLORREF						crUserListSelectTX;					//字体颜色
	COLORREF						crUserListSelectBK;					//背景颜色
	COLORREF						crUserListNormalTX;					//字体颜色
	COLORREF						crUserListNormalBK;					//背景颜色

	//颜色定义
	COLORREF						crUserListMyselfTX;					//字体颜色
	COLORREF						crUserListMyselfBK;					//背景颜色
	COLORREF						crUserListMasterTX;					//字体颜色
	COLORREF						crUserListMasterBK;					//背景颜色
	COLORREF						crUserListMemberTX;					//字体颜色
	COLORREF						crUserListMemberBK;					//背景颜色
};

//////////////////////////////////////////////////////////////////////////
//大厅资源

//大厅框架
struct tagPlazaFrameImage
{
	//颜色定义
	COLORREF						crSplitter;							//颜色定义

	//框架资源
	UINT							uFrameML;							//资源标识
	UINT							uFrameMR;							//资源标识
	UINT							uFrameTL;							//资源标识
	UINT							uFrameTM;							//资源标识
	UINT							uFrameTR;							//资源标识
	UINT							uFrameBL;							//资源标识
	UINT							uFrameBM;							//资源标识
	UINT							uFrameBR;							//资源标识

	//框架资源
	UINT							uItemFrameML;						//资源标识
	UINT							uItemFrameMR;						//资源标识
	UINT							uItemFrameTL;						//资源标识
	UINT							uItemFrameTM;						//资源标识
	UINT							uItemFrameTR;						//资源标识
	UINT							uItemFrameBL;						//资源标识
	UINT							uItemFrameBM;						//资源标识
	UINT							uItemFrameBR;						//资源标识

	//框架按钮
	UINT							uBtFrameMin;						//资源标识
	UINT							uBtFrameMax;						//资源标识
	UINT							uBtFrameClose;						//资源标识
	UINT							uBtFrameResore;						//资源标识

	//框架按钮
	UINT							uBtFrameOption;						//资源标识
	UINT							uBtFrameCompanion;					//资源标识
	UINT							uBtFrameBugReport;					//资源标识
	UINT							uBtFrameSystemAffiche;				//资源标识
	UINT							uBtFrameBank;						//资源标识
	UINT							uBtFrameBaseEnsure;					//资源标识

	//切换按钮
	UINT							uBtShowGameList;					//资源标识
	UINT							uBtHideGameList;					//资源标识
	UINT							uBtFrameItemNormal;					//资源标识
	UINT							uBtFrameItemActive;					//资源标识

	//导航按钮
	UINT							uFrameNavigation[5];				//资源标识
};

//大厅列表
struct tagPlazaServerImage
{
	//列表资源
	UINT							uServerML;							//资源标识
	UINT							uServerMR;							//资源标识
	UINT							uServerTL;							//资源标识
	UINT							uServerTM;							//资源标识
	UINT							uServerTR;							//资源标识
	UINT							uServerBL;							//资源标识
	UINT							uServerBM;							//资源标识
	UINT							uServerBR;							//资源标识

	//颜色定义
	COLORREF						crServerTX;							//颜色定义
	COLORREF						crServerBK;							//颜色定义
};

//广场浏览
struct tagPlazaBrowserImage
{
	//按钮资源
	UINT							uBtLast;							//资源标识
	UINT							uBtNext;							//资源标识
	UINT							uBtStop;							//资源标识
	UINT							uBtHome;							//资源标识
	UINT							uBtReload;							//资源标识

	//浏览资源
	UINT							uBrowserML;							//资源标识
	UINT							uBrowserMR;							//资源标识
	UINT							uBrowserTL;							//资源标识
	UINT							uBrowserTM;							//资源标识
	UINT							uBrowserTR;							//资源标识
	UINT							uBrowserBL;							//资源标识
	UINT							uBrowserBM;							//资源标识
	UINT							uBrowserBR;							//资源标识
};

//大厅桌子
struct tagPlazaTableImage
{
	//桌子资源
	UINT							uTableML;							//资源标识
	UINT							uTableMR;							//资源标识
	UINT							uTableTL;							//资源标识
	UINT							uTableTM;							//资源标识
	UINT							uTableTR;							//资源标识
	UINT							uTableBL;							//资源标识
	UINT							uTableBM;							//资源标识
	UINT							uTableBR;							//资源标识

	//桌子资源
	UINT							uTableNumber;						//资源标识
	UINT							uTableGround;						//资源标识
	UINT							uTableInvalid;						//资源标识

	//桌子按钮
	UINT							uBtAutoJoin;						//资源标识
	UINT							uBtJoinMenu;						//资源标识
	UINT							uBtViewMode;						//资源标识
	UINT							uBtQuitGame;						//资源标识
	UINT							uBtCallMaster;						//资源标识
	UINT							uBtSearchUser;						//资源标识
	UINT							uBtSecretTable;						//资源标识
	UINT							uBtUnSecretTable;					//资源标识
	UINT							uBtServerManage;					//资源标识
};

//大厅聊天
struct tagPlazaChatImage
{
	//颜色定义
	COLORREF						crChatBK;							//颜色定义
	COLORREF						crSplitter;							//颜色定义

	//聊天资源
	UINT							uChatML;							//资源标识
	UINT							uChatMR;							//资源标识
	UINT							uChatTL;							//资源标识
	UINT							uChatTM;							//资源标识
	UINT							uChatTR;							//资源标识
	UINT							uChatBL;							//资源标识
	UINT							uChatBM;							//资源标识
	UINT							uChatBR;							//资源标识

	//聊天按钮
	UINT							uBtPhrase;							//资源标识
	UINT							uBtStopChat;						//资源标识
	UINT							uBtRecvChat;						//资源标识
	UINT							uBtColorSet;						//资源标识
	UINT							uBtSaveChat;						//资源标识
	UINT							uBtSendChat;						//资源标识
	UINT							uBtChatShort;						//资源标识
	UINT							uBtExpression;						//资源标识
	UINT							uBtCleanScreen;						//资源标识
	UINT							uBtMoreFunction;					//资源标识
	UINT							uBtBugle;							//资源标识

	//聊天控制
	UINT							uBtChatClose;						//资源标识
	UINT							uBtChatControl;						//资源标识
	UINT							uChatControlBack;					//资源标识
};

//大厅用户
struct tagPlazaUserListImage
{
	//列表资源
	UINT							uUserListML;						//资源标识
	UINT							uUserListMR;						//资源标识
	UINT							uUserListTL;						//资源标识
	UINT							uUserListTM;						//资源标识
	UINT							uUserListTR;						//资源标识
	UINT							uUserListBL;						//资源标识
	UINT							uUserListBM;						//资源标识
	UINT							uUserListBR;						//资源标识

	//切换按钮
	UINT							uBtShowUserList;					//资源标识
	UINT							uBtHideUserList;					//资源标识
	UINT							uBtUserCompetition;					//资源标识
	UINT							uBtNormalUserNormal;				//资源标识
	UINT							uBtNormalUserActive;				//资源标识
	UINT							uBtCustomUserNormal;				//资源标识
	UINT							uBtCustomUserActive;				//资源标识

	//颜色定义
	COLORREF						crUserListSelectTX;					//字体颜色
	COLORREF						crUserListSelectBK;					//背景颜色
	COLORREF						crUserListNormalTX;					//字体颜色
	COLORREF						crUserListNormalBK;					//背景颜色

	//颜色定义
	COLORREF						crUserListMyselfTX;					//字体颜色
	COLORREF						crUserListMyselfBK;					//背景颜色
	COLORREF						crUserListMasterTX;					//字体颜色
	COLORREF						crUserListMasterBK;					//背景颜色
	COLORREF						crUserListMemberTX;					//字体颜色
	COLORREF						crUserListMemberBK;					//背景颜色
};

//大厅私聊
struct tagPlazaWhisperImage
{
	//聊天资源
	UINT							uChatML;							//资源标识
	UINT							uChatMR;							//资源标识
	UINT							uChatTL;							//资源标识
	UINT							uChatTM;							//资源标识
	UINT							uChatTR;							//资源标识
	UINT							uChatBL;							//资源标识
	UINT							uChatBM;							//资源标识
	UINT							uChatBR;							//资源标识

	//用户资源
	UINT							uUserML;							//资源标识
	UINT							uUserMR;							//资源标识
	UINT							uUserTL;							//资源标识
	UINT							uUserTM;							//资源标识
	UINT							uUserTR;							//资源标识
	UINT							uUserBL;							//资源标识
	UINT							uUserBM;							//资源标识
	UINT							uUserBR;							//资源标识

	//输入资源
	UINT							uInputML;							//资源标识
	UINT							uInputMR;							//资源标识
	UINT							uInputTL;							//资源标识
	UINT							uInputTM;							//资源标识
	UINT							uInputTR;							//资源标识
	UINT							uInputBL;							//资源标识
	UINT							uInputBM;							//资源标识
	UINT							uInputBR;							//资源标识

	//按钮资源
	UINT							uBtAddUser;							//资源标识
	UINT							uBtColorSet;						//资源标识
	UINT							uBtLeaveChat;						//资源标识
	UINT							uBtExpression;						//资源标识
	UINT							uBtCleanScreen;						//资源标识
	UINT							uBtMoreFunction;					//资源标识

	//控制按钮
	UINT							uBtSendControl;						//资源标识
};

//用户信息
struct tagPlazaUserInfoImage
{
	//颜色信息
	COLORREF						crHyperBack;						//颜色定义
	COLORREF						crHyperText;						//颜色定义
	COLORREF						crUserInfoText;						//颜色定义

	//用户信息
	UINT							uUserInfoL;							//资源标识
	UINT							uUserInfoM;							//资源标识
	UINT							uUserInfoR;							//资源标识
	UINT							uBtUserInfoMenu;					//资源标识
};

//大厅配置
struct tagPlazaCollocateImage
{
	//设置标题
	UINT							uTitleML;							//资源标识
	UINT							uTitleMR;							//资源标识
	UINT							uTitleTL;							//资源标识
	UINT							uTitleTM;							//资源标识
	UINT							uTitleTR;							//资源标识
	UINT							uTitleBL;							//资源标识
	UINT							uTitleBM;							//资源标识
	UINT							uTitleBR;							//资源标识

	//设置资源
	UINT							uCollocateBar;						//资源标识
	LPCTSTR							pszBtCollocateBar;					//资源标识

	//颜色定义
	COLORREF						crItemTitleTX;						//颜色定义
	COLORREF						crItemHoverTX;						//颜色定义
	COLORREF						crItemActiveTX;						//颜色定义
	COLORREF						crItemNormalTX;						//颜色定义
};

//服务专区
struct tagPlazaServiceImage
{
	//服务标题
	UINT							uTitleML;							//资源标识
	UINT							uTitleMR;							//资源标识
	UINT							uTitleTL;							//资源标识
	UINT							uTitleTM;							//资源标识
	UINT							uTitleTR;							//资源标识
	UINT							uTitleBL;							//资源标识
	UINT							uTitleBM;							//资源标识
	UINT							uTitleBR;							//资源标识

	//服务资源
	UINT							uUserServiceBar;					//资源标识
	UINT							uMemberServiceBar;					//资源标识

	//颜色定义
	COLORREF						crServiceItemTX;					//颜色定义
	COLORREF						crServiceItemBK;					//颜色定义
};

//安全提醒
struct tagPlazaSafePromptImage
{
	UINT							uBtProtect;							//资源标识
	UINT							uBtBind;							//资源标识
};

//喇叭资源
struct tagBugleImage
{
	UINT							uBtColorSet;						//资源标识
	UINT							uBtExpression;						//资源标识
};

//下载资源
struct tagDownLoadImage
{
	UINT							uBtLoadLast;						//资源标识
	UINT							uBtLoadNext;						//资源标识
};
//////////////////////////////////////////////////////////////////////////

//编辑资源
struct tagSkinEditResource
{
	COLORREF						crFocusTX;							//字体颜色
	COLORREF						crFocusBK;							//背景颜色
	COLORREF						crNoFocusTX;						//字体颜色
	COLORREF						crNoFocusBK;						//背景颜色
	COLORREF						crDisFocusTX;						//字体颜色
	COLORREF						crDisFocusBK;						//背景颜色
	COLORREF						crBoradNormal;						//边框颜色
};

//菜单资源
struct tagSkinMenuResource
{
	//资源信息
	UINT							uFlags;								//加载信息

	//字体颜色
	COLORREF						crNormalText;						//菜单颜色
	COLORREF						crSelectText;						//菜单颜色

	//边框颜色
	COLORREF						crMenuBar;							//菜单颜色
	COLORREF						crSeparator;						//菜单颜色
	COLORREF						crNormalBack;						//菜单颜色
	COLORREF						crSelectBack;						//菜单颜色
	COLORREF						crSelectBorder;						//菜单颜色

	//位置信息
	UINT							uItemHeight;						//菜单高度
	UINT							uMenuBarWidth;						//图像宽度
};

//按钮资源
struct tagSkinButtonResource
{
	UINT							uButton;							//加载信息
	COLORREF						crButtonText;						//字体颜色
};

//选择资源
struct tagSkinComboBoxResource
{
	UINT							uThumb;								//加载信息
	COLORREF						crBoradHover;						//边框颜色
	COLORREF						crBoradNormal;						//边框颜色
};

//窗口资源
struct tagSkinDialogResource
{
	//控制信息
	bool							bShowIcon;							//显示图标
	bool							bShowTitle;							//显示标题
	HICON							hDefaultIcon;						//默认图标

	//偏移信息
	INT								nXIconExcursion;					//图标偏移
	INT								nYIconExcursion;					//图标偏移
	INT								nXButtonExcursion;					//按钮偏移
	INT								nYButtonExcursion;					//按钮偏移
	INT								nXTitleTextExcursion;				//标题偏移
	INT								nYTitleTextExcursion;				//标题偏移

	//配色信息
	COLORREF						crTitleText;						//标题颜色
	COLORREF						crBackGround;						//背景颜色
	COLORREF						crControlText;						//控制颜色

	//加载信息
	UINT							uImageTL;							//加载信息
	UINT							uImageTM;							//加载信息
	UINT							uImageTR;							//加载信息
	UINT							uImageMR;							//加载信息
	UINT							uImageML;							//加载信息
	UINT							uImageBL;							//加载信息
	UINT							uImageBM;							//加载信息
	UINT							uImageBR;							//加载信息

	//加载信息
	UINT							uButtonMin;							//加载信息
	UINT							uButtonMax;							//加载信息
	UINT							uButtonRes;							//加载信息
	UINT							uButtonClose;						//加载信息
};

//列表资源
struct tagSkinListCtrlResource
{
	//颜色信息
	COLORREF						crTitle;							//标题颜色
	COLORREF						crListTX;							//列表文字
	COLORREF						crListBK;							//列表底色

	//资源信息
	UINT							uImageL;							//加载信息
	UINT							uImageM;							//加载信息
	UINT							uImageR;							//加载信息
};

//滚动资源
struct tagSkinScrollResource 
{
	UINT							uScrollImage;						//加载信息
};

//拆分资源
struct tagSkinSplitterResource
{
	COLORREF						crBorad;							//边框颜色
	COLORREF						crBackGround;						//背景颜色
};

//热点资源
struct tagSkinHyperLinkResource
{
	COLORREF						crBackColor;						//背景颜色
	COLORREF						crHoverText;						//字体颜色
	COLORREF						crNormalText;						//字体颜色
	COLORREF						crVisitedText;						//字体颜色
};

//页面资源
struct tagSkinTabControlResource
{
	COLORREF						crNormalTX;							//字体颜色
	COLORREF						crSelectTX;							//字体颜色
	COLORREF						crControlBack;						//背景颜色
	COLORREF						crControlBorad;						//边框颜色
};

//环绕资源
struct tagSkinEncircleResource
{
	UINT							uImageML;							//资源标识
	UINT							uImageMR;							//资源标识
	UINT							uImageTL;							//资源标识
	UINT							uImageTM;							//资源标识
	UINT							uImageTR;							//资源标识
	UINT							uImageBL;							//资源标识
	UINT							uImageBM;							//资源标识
	UINT							uImageBR;							//资源标识
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IPlazaResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlazaResource={0xb532bc36,0xb955,0x4bdb,0x9f,0xb0,0x78,0x81,0x36,0x1b,0x2d,0x2d};
#else
	#define VER_IPlazaResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlazaResource={0x27eaccdd,0x9076,0x4c10,0x89,0xde,0x11,0xf7,0x5e,0x88,0x9f,0x3a};
#endif

//大厅资源
interface IPlazaResource : public IUnknownEx
{
	//内核接口
public:
	//资源句柄
	virtual HINSTANCE __cdecl GetResInstance()=NULL;

	//资源信息
public:
	//大厅聊天
	virtual VOID __cdecl GetPlazaChatImage(tagPlazaChatImage & ResourceInfo)=NULL;
	//大厅桌子
	virtual VOID __cdecl GetPlazaTableImage(tagPlazaTableImage & ResourceInfo)=NULL;
	//大厅框架
	virtual VOID __cdecl GetPlazaFrameImage(tagPlazaFrameImage & ResourceInfo)=NULL;
	//大厅列表
	virtual VOID __cdecl GetPlazaServerImage(tagPlazaServerImage & ResourceInfo)=NULL;
	//大厅浏览
	virtual VOID __cdecl GetPlazaBrowserImage(tagPlazaBrowserImage & ResourceInfo)=NULL;
	//大厅服务
	virtual VOID __cdecl GetPlazaServiceImage(tagPlazaServiceImage & ResourceInfo)=NULL;
	//大厅私聊
	virtual VOID __cdecl GetPlazaWhisperImage(tagPlazaWhisperImage & ResourceInfo)=NULL;
	//大厅用户
	virtual VOID __cdecl GetPlazaUserListImage(tagPlazaUserListImage & ResourceInfo)=NULL;
	//大厅用户
	virtual VOID __cdecl GetPlazaUserInfoImage(tagPlazaUserInfoImage & ResourceInfo)=NULL;
	//大厅设置
	virtual VOID __cdecl GetPlazaCollocateImage(tagPlazaCollocateImage & ResourceInfo)=NULL;
	//安全设置
	virtual VOID __cdecl GetPlazaSafePromptImage(tagPlazaSafePromptImage & ResourceInfo)=NULL;
	//喇叭资源
	virtual VOID __cdecl GetBugleImage(tagBugleImage &ResourceInfo)=NULL;
	//下载资源
	virtual VOID __cdecl GetDownloadImage(tagDownLoadImage &ResourceInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IFrameResource INTERFACE_VERSION(1,1)
	static const GUID IID_IFrameResource={0xbb80ea74,0x4259,0x4180,0x9f,0xb4,0xd9,0x37,0x3a,0x5f,0xab,0xb9};
#else
	#define VER_IFrameResource INTERFACE_VERSION(1,1)
	static const GUID IID_IFrameResource={0x8512b9a8,0x40f6,0x426a,0xa7,0xe7,0x25,0x59,0xfe,0x88,0xb5,0x3f};
#endif

//框架资源
interface IFrameResource : public IUnknownEx
{
	//内核接口
public:
	//资源句柄
	virtual HINSTANCE __cdecl GetResInstance()=NULL;

	//资源信息
public:
	//游戏控制
	virtual VOID __cdecl GetGameBarImage(tagGameBarImage & ResourceInfo)=NULL;
	//游戏广告
	virtual VOID __cdecl GetGameBillImage(tagGameBillImage & ResourceInfo)=NULL;
	//游戏聊天
	virtual VOID __cdecl GetGameChatImage(tagGameChatImage & ResourceInfo)=NULL;
	//游戏框架
	virtual VOID __cdecl GetGameFrameImage(tagGameFrameImage & ResourceInfo)=NULL;
	//游戏用户
	virtual VOID __cdecl GetGameUserInfoImage(tagGameUserInfoImage & ResourceInfo)=NULL;
	//游戏列表
	virtual VOID __cdecl GetGameUserListImage(tagGameUserListImage & ResourceInfo)=NULL;
	//喇叭资源
	virtual VOID __cdecl GetBugleImage(tagBugleImage &ResourceInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ISkinResource INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinResource={0x24775ee6,0x729c,0x4b60,0x0098,0x54,0xd8,0xb7,0x4c,0x27,0x25,0x8e};
#else
	#define VER_ISkinResource INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinResource={0xcaf877cf,0x37ca,0x44f0,0x00af,0x14,0x85,0xf5,0x67,0xd2,0x55,0x14};
#endif

//界面资源
interface ISkinResource : public IUnknownEx
{
	//内核接口
public:
	//资源句柄
	virtual HINSTANCE __cdecl GetResInstance()=NULL;

	//特殊资源
public:
	//创建字体
	virtual bool __cdecl GetFontResource(CFont & Font)=NULL;
	//获取信息
	virtual bool __cdecl GetEncircleResource(tagSkinEncircleResource & SkinEncircleResource)=NULL;

	//界面资源
public:
	//获取信息
	virtual bool __cdecl GetMenuResource(tagSkinMenuResource & ResourceInfo)=NULL;
	//获取信息
	virtual bool __cdecl GetEditResource(tagSkinEditResource & ResourceInfo)=NULL;
	//获取信息
	virtual bool __cdecl GetDialogResource(tagSkinDialogResource & ResourceInfo)=NULL;
	//获取信息
	virtual bool __cdecl GetButtonResource(tagSkinButtonResource & ResourceInfo)=NULL;
	//获取信息
	virtual bool __cdecl GetComboBoxResource(tagSkinComboBoxResource & ResourceInfo)=NULL;
	//获取信息
	virtual bool __cdecl GetSplitterResource(tagSkinSplitterResource & ResourceInfo)=NULL;
	//获取信息
	virtual bool __cdecl GetListCtrlResource(tagSkinListCtrlResource & ResourceInfo)=NULL;
	//获取信息
	virtual bool __cdecl GetHyperLinkResource(tagSkinHyperLinkResource & ResourceInfo)=NULL;
	//获取信息
	virtual bool __cdecl GetTabControlResource(tagSkinTabControlResource & ResourceInfo)=NULL;
	//获取信息
	virtual bool __cdecl GetScrollResource(tagSkinScrollResource & ResourceInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IPlatformResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlatformResource={0x82ad737b,0xc01e,0x481f,0x0082,0x60,0x8e,0x45,0x07,0xef,0xaf,0x8e};
#else
	#define VER_IPlatformResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlatformResource={0x7fbe972b,0x898f,0x4429,0x00ab,0xfb,0x91,0xec,0x89,0x99,0x25,0x24};
#endif

//资源管理
interface IPlatformResource : public IUnknownEx
{
	//内核接口
public:
	//资源句柄
	virtual HINSTANCE __cdecl GetResInstance()=NULL;
	//获取预览
	virtual VOID __cdecl GetSkinPreview(CImage & ImagePreview)=NULL;

	//界面资源
public:
	//界面资源
	virtual VOID * __cdecl GetSkinResource(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//大厅资源
	virtual VOID * __cdecl GetPlazaResource(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//框架资源
	virtual VOID * __cdecl GetFrameResource(REFGUID Guid, DWORD dwQueryVer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//组件辅助类
DECLARE_MODULE_DYNAMIC(SkinResource, SkinResource)
DECLARE_MODULE_DYNAMIC(PlazaResource, PlazaResource)
DECLARE_MODULE_DYNAMIC(FrameResource, FrameResource)
DECLARE_MODULE_DYNAMIC(PlatformResource, PlatformResource)

//界面模块
#ifndef _DEBUG
	#define RESOURCE_MODELE			TEXT("BuleResource.dll")			//模块名字
#else
	#define RESOURCE_MODELE			TEXT("BuleResourceD.dll")			//模块名字
#endif

//资源目录
#define SKIN_RESOURCE				TEXT("SkinResource\\蓝色激情")		//资源目录		

//函数名字
#define MODULE_CREATE_FUNCTION		"CreatePlatformResource"			//创建函数

//////////////////////////////////////////////////////////////////////////

#endif