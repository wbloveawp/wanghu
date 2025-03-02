#ifndef CLIENT_CONTROLS_HEAD_HEAD_FILE
#define CLIENT_CONTROLS_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//系统头文件
#include "AfxHtml.h"

//平台头文件
#include "..\..\全局定义\Platform.h"
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\客户端组件\界面控件\SkinControlsHead.h"
#include "..\..\客户端组件\服务组件\ClientServiceHead.h"

//游戏服务消息
#include "..\..\消息定义\CMD_GameServer.h"

//////////////////////////////////////////////////////////////////////////
//数据结构

//短语控件结构
struct tagPhraseVoiceInfo
{
	//资源标识
	LPCTSTR						pszImageFrame;						//框架资源
	LPCTSTR						pszImageBtUp;						//按钮资源
	LPCTSTR						pszImageBtDown;						//按钮资源
	LPCTSTR						pszPngFocus;						//焦点资源

	//位置变量
	INT							cxBorderWidth;						//边框宽度
	INT							cyBorderHeight;						//边框高度
	INT							cyBottomHeight;						//底部高度

	//配置变量
	INT							nPageCount;							//每页显示条数
	INT							nTransPercent;						//透明百分比
	COLORREF					crTextColor;						//文字颜色
};

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef CLIENT_CONTROLS_CLASS
	#ifdef  CLIENT_CONTROLS_DLL
		#define CLIENT_CONTROLS_CLASS _declspec(dllexport)
	#else
		#define CLIENT_CONTROLS_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define CLIENT_CONTROLS_DLL_NAME	TEXT("ClientControls.dll")			//组件 DLL 名字
#else
	#define CLIENT_CONTROLS_DLL_NAME	TEXT("ClientControlsD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IRichEditMessageSink INTERFACE_VERSION(1,1)
	static const GUID IID_IRichEditMessageSink={0x41579168,0x39fe,0x4862,0x00a7,0x0a,0xc2,0x93,0xb9,0xd3,0xb2,0x6e};
#else
	#define VER_IRichEditMessageSink INTERFACE_VERSION(1,1)
	static const GUID IID_IRichEditMessageSink={0xd4b0e8b0,0x05d5,0x4fa2,0x0095,0xb6,0x00,0xc5,0xc6,0x7a,0xde,0x5b};
#endif

//消息接口
interface IRichEditMessageSink : public IUnknownEx
{
	//点击接口
	virtual bool __cdecl OnEventStringLink(LPCTSTR pszString, UINT uMessage)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IStringMessage INTERFACE_VERSION(1,1)
	static const GUID IID_IStringMessage={0x3b21dbbe,0xebdf,0x46b2,0x00be,0x8e,0x8d,0xcc,0x8b,0x03,0xde,0x11};
#else
	#define VER_IStringMessage INTERFACE_VERSION(1,1)
	static const GUID IID_IStringMessage={0x2cda3e22,0xc8ca,0x46e4,0x00af,0x35,0x5e,0xd7,0xc5,0xbc,0x88,0x50};
#endif

//信息接口
interface IStringMessage : public IUnknownEx
{
	//事件消息
public:
	//进入事件
	virtual bool __cdecl InsertUserEnter(LPCTSTR pszUserName)=NULL;
	//离开事件
	virtual bool __cdecl InsertUserLeave(LPCTSTR pszUserName)=NULL;
	//断线事件
	virtual bool __cdecl InsertUserOffLine(LPCTSTR pszUserName)=NULL;

	//字符消息
public:
	//普通消息
	virtual bool __cdecl InsertNormalString(LPCTSTR pszString)=NULL;
	//系统消息
	virtual bool __cdecl InsertSystemString(LPCTSTR pszString)=NULL;
	//提示消息
	virtual bool __cdecl InsertPromptString(LPCTSTR pszString)=NULL;
	//公告消息
	virtual bool __cdecl InsertAfficheString(LPCTSTR pszString)=NULL;
	//喇叭消息
	virtual bool __cdecl InserBugleString(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, BYTE cbBugleType)=NULL;

	//定制消息
public:
	//定制消息
	virtual bool __cdecl InsertCustomString(LPCTSTR pszString, COLORREF crColor)=NULL;
	//定制消息
	virtual bool __cdecl InsertCustomString(LPCTSTR pszString, COLORREF crColor, COLORREF crBackColor)=NULL;

	//聊天消息
public:
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor)=NULL;
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszString, COLORREF crColor,bool bSoundNotify)=NULL;
	//用户私聊
	virtual bool __cdecl InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString)=NULL;
	//用户私聊
	virtual bool __cdecl InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString, const SYSTEMTIME &time)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//短语语音回调
interface IPhraseVoiceSink
{
	//选择事件
	virtual VOID __cdecl OnEventPhraseSelect(INT nPhraseIndex)=NULL;
};

//声音事件
interface IDirectSoundEvent
{
	//开始播放
	virtual VOID __cdecl OnEventSoundPlay( DWORD dwSoundID )=NULL;
	//播放结束
	virtual VOID __cdecl OnEventSoundStop( DWORD dwSoundID )=NULL; 
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IPhraseVoice INTERFACE_VERSION(1,1)
	static const GUID IID_IPhraseVoice={0x9d3c7688,0x492f,0x4117,0x008c,0x75,0xee,0xdd,0xa4,0x53,0x3f,0x85};
#else
	#define VER_IPhraseVoice INTERFACE_VERSION(1,1)
	static const GUID IID_IPhraseVoice={0x8944ac61,0xf067,0x441e,0x0093,0xf7,0x66,0xbb,0xd3,0x54,0xfe,0xcb};
#endif

//短语语音
interface IPhraseVoice: public IUnknownEx
{
	//设置函数
public:
	//初始函数
	virtual VOID __cdecl InitPhraseVoice( const tagPhraseVoiceInfo &PhraseVoiceInfo, CWnd *pParentWnd=NULL )=NULL;
	//基准位置
	virtual VOID __cdecl SetBenchmarkPos( INT nXPos, INT nYPos )=NULL;
	//基准位置
	virtual VOID __cdecl SetBenchmarkPos( CPoint ptBenchmarkPos )=NULL;
	//设置回调
	virtual VOID __cdecl SetPhraseVoiceSink( IPhraseVoiceSink *pIPhraseVoiceSink )=NULL;

	//功能函数
public:
	//显示短语
	virtual VOID __cdecl ShowPhraseWindow()=NULL;
	//获取短语
	virtual LPCTSTR __cdecl GetPhraseString( INT nPhraseIndex, TCHAR szResult[], INT nMaxLength )=NULL;
	//播放短语
	virtual VOID __cdecl PlayPhraseVoice( WORD wChairID, INT nPhraseIndex, INT nGender )=NULL;
};

//////////////////////////////////////////////////////////////////////////

//导出文件
#ifndef CLIENT_CONTROLS_DLL
	#include "ChatPhrase.h"
	#include "Expression.h"
	#include "Information.h"
	#include "BrowserControl.h"
	#include "SystemTrayIcon.h"
	#include "UserListControl.h"
	#include "RichEditMessage.h"
	//#include "DirectSound.h"
	#include "WebPublicize.h"
	//#include "BugleCtrl.h"
#endif

//////////////////////////////////////////////////////////////////////////
//组件辅助类

DECLARE_MODULE_HELPER(PhraseVoice, PhraseVoice, CLIENT_CONTROLS_DLL_NAME,"CreatePhraseVoice")

//////////////////////////////////////////////////////////////////////////

#endif