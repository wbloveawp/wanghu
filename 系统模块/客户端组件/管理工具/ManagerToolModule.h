#ifndef GAME_PLAZA_MODULE_HEAD_FILE
#define GAME_PLAZA_MODULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//类型说明

//类说明
class CTableResource;

//接口说明
interface ITableView;
interface ITableFrameView;
interface IServerViewItem;

//结构说明
struct tagTableMirrorAttrib;

//////////////////////////////////////////////////////////////////////////
//结构定义

//查找桌子
struct tagSearchTable
{
	bool							bOneNull;							//一个空位
	bool							bTwoNull;							//两个空位
	bool							bAllNull;							//全空位置
	bool							bNotFull;							//不全满位
	bool							bFilterPass;						//过滤密码
	WORD							wStartTableID;						//起始桌子
	WORD							wResultTableID;						//结果桌子
	WORD							wResultChairID;						//结果椅子
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGlobalUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IGlobalUnits={0xe5090ee7,0x0df4,0x4b96,0x00ac,0x51,0x3d,0x34,0x9c,0x16,0xe8,0xb6};
#else
	#define VER_IGlobalUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IGlobalUnits={0x7438c9bb,0x672d,0x4d04,0x00b7,0xd2,0x52,0xb7,0x7f,0xaa,0x8d,0xd1};
#endif

//全局单元
interface IGlobalUnits : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableView INTERFACE_VERSION(1,1)
	static const GUID IID_ITableView={0x3257c494,0x8c75,0x4b5a,0x00b2,0x77,0xbd,0xfd,0xb9,0x3c,0xf7,0x62};
#else
	#define VER_ITableView INTERFACE_VERSION(1,1)
	static const GUID IID_ITableView={0x98298df9,0x8eb6,0x4618,0x0089,0xa3,0xe6,0x37,0xec,0xc6,0xe7,0x44};
#endif

//桌子视图
interface ITableView : public IUnknownEx
{
	//功能接口
public:
	//空椅子数
	virtual WORD __cdecl GetNullChairCount(WORD & wNullChairID)=NULL;
	//配置函数
	virtual VOID __cdecl InitTableView(WORD wTableID, WORD wChairCount, ITableFrameView * pITableFrameView)=NULL;

	//用户接口
public:
	//获取用户
	virtual IClientUserItem * __cdecl GetUserInfo(WORD wChairID)=NULL;
	//设置信息
	virtual bool __cdecl SetUserInfo(WORD wChairID, IClientUserItem * pIClientUserItem)=NULL;

	//设置接口
public:
	//鼠标按下
	virtual VOID __cdecl SetMouseDown(bool bMouseDown)=NULL;
	//焦点框架
	virtual VOID __cdecl SetFocusFrame(bool bFocusFrame)=NULL;
	//盘旋位置
	virtual VOID __cdecl SetHoverChairID(WORD wHoverChairID)=NULL;
	//桌子状态 
	virtual VOID __cdecl SetTableStatus(bool bPlaying, bool bLocker)=NULL;

	//底分接口
public:
	//设置底分
	virtual VOID __cdecl SetTableBaseScore(LONGLONG lBaseScore)=NULL;

	//查询接口
public:
	//游戏标志
	virtual bool __cdecl GetPlayFlag()=NULL;
	//密码标志
	virtual bool __cdecl GetLockerFlag()=NULL;
	//焦点框架
	virtual bool __cdecl GetFocusFrame()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableViewMirror INTERFACE_VERSION(1,1)
	static const GUID IID_ITableViewMirror={0x31a64dfd,0xe5c2,0x4975,0x00a6,0x25,0xc8,0xc1,0xc7,0x6d,0x0e,0x5d};
#else
	#define VER_ITableViewMirror INTERFACE_VERSION(1,1)
	static const GUID IID_ITableViewMirror={0x52198850,0x071c,0x49c6,0x00a6,0x4f,0x20,0x80,0x13,0xec,0x45,0x70};
#endif

//镜像接口
interface ITableViewMirror : public IUnknownEx
{
	//管理接口
public:
	//更新视图
	virtual bool __cdecl UpdateMirrorView()=NULL;
	//镜像号码
	virtual WORD __cdecl GetMirrorTableID()=NULL;

	//配置接口
public:
	//设置桌子
	virtual VOID __cdecl SetTableFrameView(ITableFrameView * pITableFrameView)=NULL;
	//设置房间
	virtual VOID __cdecl SetServerViewItem(IServerViewItem * pIServerViewItem)=NULL;
	//设置参数
	virtual bool __cdecl SetTableMirrorAttrib(tagTableMirrorAttrib & TableMirrorAttrib)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameView INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameView={0xac262468,0x4312,0x468f,0x0095,0xe7,0xae,0x32,0xa9,0x50,0xe5,0x7d};
#else
	#define VER_ITableFrameView INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameView={0xd53c2971,0xb7b1,0x48ed,0x008f,0xb0,0x9e,0x35,0x07,0x82,0xeb,0xaa};
#endif

//桌子框架
interface ITableFrameView : public IUnknownEx
{
	//配置接口
public:
	//创建桌子
	virtual bool __cdecl CreateTableFrame(CWnd * pParentWnd, UINT uWndID)=NULL;
	//配置桌子
	virtual bool __cdecl InitTableFrame(WORD wTableCount, WORD wChairCount, LPCTSTR pszResDirectory, IUnknownEx * pIUnknownEx,bool bHideUserInfo,bool bDrawBaseScore)=NULL;

	//信息接口
public:
	//桌子数目
	virtual WORD __cdecl GetTableCount()=NULL;
	//椅子数目
	virtual WORD __cdecl GetChairCount()=NULL;
	//获取资源
	virtual CTableResource * __cdecl GetTableResource()=NULL;

	//视图接口
public:
	//视图窗口
	virtual CWnd * GetViewWnd()=NULL;
	//视图数据
	virtual bool OnEventViewData(WORD wCmdID, VOID * pData, WORD wDataSize)=NULL; 

	//用户接口
public:
	//获取用户
	virtual IClientUserItem * __cdecl GetUserInfo(WORD wTableID, WORD wChairID)=NULL;
	//设置信息
	virtual bool __cdecl SetUserInfo(WORD wTableID, WORD wChairID, IClientUserItem * pIClientUserItem)=NULL;

	//状态接口
public:
	//游戏标志
	virtual bool __cdecl GetPlayFlag(WORD wTableID)=NULL;
	//密码标志
	virtual bool __cdecl GetLockerFlag(WORD wTableID)=NULL;	
	//焦点框架
	virtual bool __cdecl GetFocusFrame(WORD wTableID)=NULL;
	//载入标识
	virtual VOID __cdecl SetGameLoadFlag(bool bGameload)=NULL;
	//焦点框架
	virtual VOID __cdecl SetFocusFrame(WORD wTableID, bool bFocusFrame)=NULL;
	//桌子状态 
	virtual VOID __cdecl SetTableStatus(WORD wTableID, bool bPlaying, bool bLocker)=NULL;

	//底分接口
public:
	//设置底分
	virtual VOID __cdecl SetTableBaseScore(WORD wTableID,LONGLONG lBaseScore)=NULL;

	//视图控制
public:
	//桌子可视
	virtual bool __cdecl EnsureVisibleTable(WORD wTableID)=NULL;
	//闪动桌子
	virtual bool __cdecl FlashGameTable(WORD wTableID)=NULL;
	//闪动椅子
	virtual bool __cdecl FlashGameChair(WORD wTableID, WORD wChairID)=NULL;

	//功能接口
public:
	//更新桌子
	virtual bool __cdecl UpdateTableView(WORD wTableID)=NULL;
	//获取桌子
	virtual ITableView * __cdecl GetTableArrayPtr(WORD wTableID)=NULL;
	//空椅子数
	virtual WORD __cdecl GetNullChairCount(WORD wTableID, WORD & wNullChairID)=NULL;

	//镜像接口
public:
	//注销镜像
	virtual bool __cdecl UnRegisterTableMirror(ITableViewMirror * pITableViewMirror)=NULL;
	//更新镜像
	virtual bool __cdecl UpdateTableMirror(ITableViewMirror * pITableViewMirror, WORD wTableID)=NULL;
	//注册镜像
	virtual bool __cdecl RegisterTableMirror(WORD wTableID, ITableViewMirror * pITableViewMirror)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameViewSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameViewSink={0x94bf9ba0,0x48dc,0x4834,0x00a1,0x52,0xf4,0x67,0xae,0x34,0xe2,0xf8};
#else
	#define VER_ITableFrameViewSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameViewSink={0xf6b4805d,0x2792,0x4795,0x0087,0x01,0x57,0xf4,0x2b,0x6a,0x65,0x0b};
#endif

//桌子通知
interface ITableFrameViewSink : public IUnknownEx
{
	//桌子按钮
	virtual VOID __cdecl OnHitTableButton(BYTE cbButton)=NULL;
	//鼠标双击
	virtual VOID __cdecl OnDoubleHitTable(WORD wTableID, bool bMirror)=NULL;
	//左键按下
	virtual VOID __cdecl OnLButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror)=NULL;
	//右键按下
	virtual VOID __cdecl OnRButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IMessageHandler INTERFACE_VERSION(1,1)
static const GUID IID_IMessageHandler={0xe28ce454,0xf7eb,0x49c2,0x0087,0x32,0x52,0xda,0xf3,0xb9,0x20,0x92};
#else
#define VER_IMessageHandler INTERFACE_VERSION(1,1)
static const GUID IID_IMessageHandler={0xfdd17562,0xf0e1,0x419c,0x0083,0x9c,0x53,0xbf,0xc0,0x3,0x2a,0x56};
#endif

//房间接口
interface IMessageHandler : public IUnknownEx
{
	//功能接口
public:
	//网络事件
	virtual bool __cdecl OnEventNetworkLink(WORD wSocketID, INT nErrorCode)=NULL;
	//网络事件
	virtual bool __cdecl OnEventNetworkMessage(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;
	//网络事件
	virtual bool __cdecl OnEventNetworkShut(WORD wSocketID, BYTE cbShutReason)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerViewItem INTERFACE_VERSION(1,2)
	static const GUID IID_IServerViewItem={0x4cf09fc5,0x7a5e,0x4e41,0x00b4,0xe5,0xfd,0x40,0x1b,0xb0,0xf2,0xd9};
#else
	#define VER_IServerViewItem INTERFACE_VERSION(1,2)
	static const GUID IID_IServerViewItem={0xfbc533f9,0x9961,0x4ad2,0x008d,0x29,0xa2,0xf1,0x80,0x69,0x37,0x59};
#endif

//房间接口
interface IServerViewItem : public IUnknownEx
{
	//功能接口
public:
	//查找桌子
	virtual bool __cdecl SearchGameTable(tagSearchTable & SearchTable)=NULL;
	//聊天对象
	virtual VOID __cdecl SetChatTargetUser(IClientUserItem * pIClientUserItem)=NULL;
	//私聊会话
	virtual bool __cdecl WhisperConversation(IClientUserItem * pIClientUserItem[], WORD wUserCount)=NULL;
	//显示表情
	virtual VOID __cdecl ShowExpressionControl(CWnd * pParentWnd, IExpressionSink * pIExpressionSink, INT nXPos, INT nYPos)=NULL;

	//用户菜单
public:
	//显示菜单
	virtual VOID __cdecl TrackSelectChatMenu()=NULL;
	//显示菜单
	virtual VOID __cdecl TrackUserItemMenu(LPCWSTR pszUserAccounts)=NULL;
	//显示菜单
	virtual VOID __cdecl TrackUserItemMenu(CUserListControl * pUserListControl, INT nItemIndex)=NULL;
	//显示菜单
	virtual VOID __cdecl TrackUserItemMenu(IGlobalUserItem *pIGlobalUserItem[], INT nItemCount)=NULL;

	//消息处理
public:
	//添加处理接口
	virtual bool __cdecl AddMessageHandler(IUnknownEx *pIMessageHandler)=NULL;
	//删除处理接口
	virtual bool __cdecl RemoveMessageHandler(IUnknownEx *pIMessageHandler)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IChatMsgItem INTERFACE_VERSION(1,1)
static const GUID IID_IChatMsgItem={0x3a168ac6,0xd09c,0x4d26,0x00aa,0x45,0x1b,0xa,0x62,0x6,0x29,0x66};
#else
#define VER_IChatMsgItem INTERFACE_VERSION(1,1)
static const GUID IID_IChatMsgItem={0x8f04a9a0,0x5737,0x43e5,0x009a,0x1b,0xea,0x81,0x79,0x89,0xb0,0xa0};
#endif

//消息接口
interface IChatMsgItem : public IUnknownEx
{
	//功能接口
public:
	//重置项目
	virtual VOID __cdecl ResetChatMsg()=NULL;
	//获取玩家
	virtual LPCTSTR __cdecl GetSendUserAccounts()=NULL;
	//获取发送消息
	virtual LPCTSTR __cdecl GetMsg()=NULL;
	//获取字体颜色
	virtual COLORREF __cdecl GetChatColor()=NULL;
	//是否自己消息
	virtual bool __cdecl IsSelfMsg()=NULL;
	//获取时间
	virtual const SYSTEMTIME& GetMsgTime()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IChatHistory INTERFACE_VERSION(1,1)
static const GUID IID_IChatHistory={0xb0a24da4,0x7e14,0x47ef,0x00ab,0xdc,0xd0,0xc3,0x65,0x48,0xc8,0xf9};
#else
#define VER_IChatHistory INTERFACE_VERSION(1,1)
static const GUID IID_IChatHistory={0xf00991f3,0xd009,0x4ca9,0x008b,0xd1,0x78,0x24,0x95,0xe,0xbf,0x8};
#endif

//聊天记录
interface IChatHistory : public IUnknownEx
{
	//查询函数
public:
	//获取记录ID
	virtual DWORD __cdecl GetChatID()=NULL;
	//获取消息数
	virtual INT_PTR __cdecl GetMsgItemCount()=NULL;
	//枚举消息
	virtual IChatMsgItem* __cdecl EnumChatMsgItem(INT_PTR nEnumIndex)=NULL;

	//功能函数
public:
	//重置记录
	virtual VOID __cdecl ResetChatHistory()=NULL;
	//添加消息
	virtual bool __cdecl AddMsgItem(LPCTSTR lpszSendUser,LPCTSTR lpszMsg,COLORREF crColor,bool bMySelfString)=NULL;
	//清除记录
	virtual bool __cdecl ClearHistory()=NULL;

	//文件操作
public:
	//保存记录
	virtual bool __cdecl Save()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IChatHistoryManager INTERFACE_VERSION(1,1)
static const GUID IID_IChatHistoryManager={0x94afc65f,0x2c55,0x46d3,0x00a0,0x84,0x6c,0x17,0x84,0x66,0xf0,0x15};
#else
#define VER_IChatHistoryManager INTERFACE_VERSION(1,1)
static const GUID IID_IChatHistoryManager={0x4e39ada4,0x239,0x4209,0x00a0,0x9b,0xc3,0xeb,0xd2,0x77,0xae,0xb4};
#endif

//聊天记录管理接口
interface IChatHistoryManager : public IUnknownEx
{
	//配置接口
public:
	//初始化
	virtual VOID SetMyselftUserItem(IClientUserItem *pIMyselfUserItem)=NULL;

	//查询接口
public:
	//获取玩家接口
	virtual IClientUserItem * __cdecl GetMyselftUserItem()=NULL;

	//功能接口
public:
	//激活记录
	virtual IChatHistory* __cdecl ActiveChatHistory(DWORD dwChatID)=NULL;
	//关闭记录
	virtual bool __cdecl CloseChatHistory(DWORD dwChatID)=NULL;
	//保存记录
	virtual bool __cdecl SaveChatHistory(DWORD dwChatID)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#endif