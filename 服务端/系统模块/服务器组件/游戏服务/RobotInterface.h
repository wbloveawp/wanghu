#ifndef ANDROID_INTERFACE_HEAD_FILE
#define ANDROID_INTERFACE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//接口声明
interface IAndroidService;
interface IAndroidUserItem;
interface IAndroidUserItemSink;

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IAndroidService INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidService = { 0x5468bfb,0xeb56,0x480c,0xa9,0x82,0x62,0x1a,0xc2,0x20,0xf9,0xa2 };
#else
#define VER_IAndroidService INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidService = { 0xcf77eef2,0xbe9,0x49d8,0xb4,0x50,0xcb,0x27,0xe5,0x5f,0xc8,0x44 };
#endif

//机器人接口
interface IAndroidService : public IUnknownEx
{
	//功能接口
public:
	//创建机器人
	virtual VOID* __cdecl CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer) = NULL;
};

DECLARE_MODULE_DYNAMIC(AndroidService);

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAndroidUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItem={0x014b4ef4,0xa9d5,0x45cb,0x0092,0xa1,0x01,0xcf,0xe9,0xe7,0x0f,0x3c};
#else
	#define VER_IAndroidUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItem={0xebb22c87,0x7612,0x4707,0x00b3,0xf8,0x95,0xdd,0x51,0xad,0x93,0x98};
#endif

//机器人接口
interface IAndroidUserItem : public IUnknownEx
{
	//状态函数
public:
	//获取状态
	virtual BYTE __cdecl GetGameStatus()=NULL;
	//设置状态
	virtual VOID __cdecl SetGameStatus(BYTE cbGameStatus)=NULL;

	//信息接口
public:
	//获取 I D
	virtual DWORD __cdecl GetUserID()=NULL;
	//桌子号码
	virtual WORD __cdecl GetTableID()=NULL;
	//椅子号码
	virtual WORD __cdecl GetChairID()=NULL;
	//用户状态
	virtual BYTE __cdecl GetUserStatus()=NULL;
	//用户名字
	virtual LPCWSTR __cdecl GetAccounts()=NULL;	

	//积分信息
public:
	//分数类型
	virtual BYTE __cdecl GetScoreKind()=NULL;
	//用户积分
	virtual LONGLONG __cdecl GetUserScore()=NULL;

	//网络接口
public:
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wSubCmdID)=NULL;
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	
	//功能接口
public:
	//删除时间
	virtual bool __cdecl KillGameTimer(UINT nTimerID)=NULL;
	//设置时间
	virtual bool __cdecl SetGameTimer(UINT nTimerID, UINT nElapse)=NULL;
	//发送准备
	virtual bool __cdecl SendUserReady(VOID * pData, WORD wDataSize)=NULL;
	//发送聊天
	virtual bool __cdecl SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)=NULL;

	//配置函数
public:
	//设置版本
	virtual VOID __cdecl SetAdroidVersion( DWORD dwVersion )=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAndroidUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItemSink={0x366aba89,0xcca2,0x40d6,0x0087,0x95,0xb5,0xb2,0xb6,0x1d,0x29,0x80};
#else
	#define VER_IAndroidUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItemSink={0x4cbe7840,0x5068,0x42fb,0x0083,0x38,0x8a,0xb6,0x58,0x65,0xa5,0xdd};
#endif

//机器人接口
interface IAndroidUserItemSink : public IUnknownEx
{
	//控制接口
public:
	//初始接口
	virtual bool __cdecl InitUserItemSink(IUnknownEx * pIUnknownEx)=NULL;
	//重置接口
	virtual bool __cdecl RepositUserItemSink()=NULL;

	//游戏事件
public:
	//时间消息
	virtual bool __cdecl OnEventTimer(UINT nTimerID)=NULL;
	//游戏消息
	virtual bool __cdecl OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//游戏消息
	virtual bool __cdecl OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//场景消息
	virtual bool __cdecl OnEventGameScene(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)=NULL;

	//用户事件
public:
	//用户进入
	virtual VOID __cdecl OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//用户离开
	virtual VOID __cdecl OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//用户积分
	virtual VOID __cdecl OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//用户状态
	virtual VOID __cdecl OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//用户段位
	virtual VOID __cdecl OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#endif