#ifndef RICH_EDIT_MESSAGE_HEAD_FILE
#define RICH_EDIT_MESSAGE_HEAD_FILE

#pragma once

#include "Expression.h"
#include "ClientControlsHead.h"
//#include "DirectSound.h"
//////////////////////////////////////////////////////////////////////////

//颜色定义
#define COLOR_DEBUG					RGB(0,128,128)						//调试颜色
#define COLOR_EXCEPTION				RGB(200,0,0)						//异常颜色

//颜色定义
#define COLOR_WARN					RGB(255,128,0)						//警告颜色
#define COLOR_EVENT					RGB(125,125,125)					//事件消息
#define COLOR_NORMAL				RGB(0,0,0)							//普通消息
#define COLOR_SYSTEM_STRING			RGB(154,0,2)						//系统消息

//私聊颜色
#define COLOR_USER					RGB(0,0,255)						//对方颜色
#define COLOR_MYSELF				RGB(0,128,64)						//自己颜色
 
//位置定义
#define STRING_OFFSET				90									//上移位置

//////////////////////////////////////////////////////////////////////////

//字符消息
class CLIENT_CONTROLS_CLASS CRichEditMessage : public CSkinRichEdit, public IStringMessage
{
	//图片资源
protected:
	COLORREF						m_crFillColor;						//填充颜色
	CImageDataObject				m_ImageDataSystem;					//资源变量
	CImageDataObject				m_ImageDataPrompt;					//资源变量
	CImageDataObject				m_ImageDataAffiche;					//资源变量
	CImageDataObject				m_ImageBigBugle;					//资源变量
	CImageDataObject				m_ImageSmallBugle;					//资源变量

	//组件变量
protected:
	CSkinScrollBar					m_SkinScrollBar;					//滚动条类

	//组件变量
protected:
	CExpressionManager *			m_pExpressionManager;				//表情管理
	IRichEditMessageSink *			m_pIRichEditMessageSink;			//接口变量
	//CDirectSound					m_DirectSound;						//声音播放
	DWORD							m_dwMsgSoundID;						//声音ID

	//函数定义
public:
	//构造函数
	CRichEditMessage();
	//析构函数
	virtual ~CRichEditMessage();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { return; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//事件消息
public:
	//进入事件
	virtual bool __cdecl InsertUserEnter(LPCTSTR pszUserName);
	//离开事件
	virtual bool __cdecl InsertUserLeave(LPCTSTR pszUserName);
	//断线事件
	virtual bool __cdecl InsertUserOffLine(LPCTSTR pszUserName);

	//字符消息
public:
	//普通消息
	virtual bool __cdecl InsertNormalString(LPCTSTR pszString);
	//系统消息
	virtual bool __cdecl InsertSystemString(LPCTSTR pszString);
	//提示消息
	virtual bool __cdecl InsertPromptString(LPCTSTR pszString);
	//公告消息
	virtual bool __cdecl InsertAfficheString(LPCTSTR pszString);
	//喇叭消息
	virtual bool __cdecl InserBugleString(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, BYTE cbBugleType);

	//定制消息
public:
	//定制消息
	virtual bool __cdecl InsertCustomString(LPCTSTR pszString, COLORREF crColor);
	//定制消息
	virtual bool __cdecl InsertCustomString(LPCTSTR pszString, COLORREF crColor, COLORREF crBackColor);

	//聊天消息
public:
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor);
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszString, COLORREF crColor,bool bSoundNotify);
	//用户私聊
	virtual bool __cdecl InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString);
	//用户私聊
	virtual bool __cdecl InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString, const SYSTEMTIME &time);

	//重载函数
protected:
	//控件绑定
	virtual VOID PreSubclassWindow();

	//配置函数
public:
	//设置接口
	bool SetRichEditMessageSink(IUnknownEx * pIUnknownEx);
	//设置组件
	bool SetExpressionManager(CExpressionManager * pExpressionManager, COLORREF crFillColor);

	//功能函数
public:
	//插入时间
	bool InsertSystemTime(COLORREF crColor);
	//用户名字
	bool InsertUserAccounts(LPCTSTR pszAccounts);
	//翻译字符
	bool TranslateInsertString(LPCTSTR pszString, COLORREF crColor);
	//翻译字符
	bool TranslateInsertString(LPCTSTR pszString, CHARFORMAT2 &CharFormat);

	//内部函数
protected:
	//处理图片
	bool LoadRichEditImage(HINSTANCE hInstance, UINT uResourceID, CImageDataObject & ImageDataObject);

	//消息映射
protected:
	//连接消息
	VOID OnEventLink(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif