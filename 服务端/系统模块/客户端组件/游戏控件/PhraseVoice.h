#ifndef PHRASE_VOICE_HEAD_FILE
#define PHRASE_VOICE_HEAD_FILE

#pragma once

#include "ClientControlsHead.h"
//#include "DirectSound.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//短语结构
struct tagPhraseVoice
{
	TCHAR							szPhrase[256];						//短语字符
	TCHAR							szGirlVoicePath[MAX_PATH];			//女声路径
	TCHAR							szBoyVoicePath[MAX_PATH];			//男声路径
};

//////////////////////////////////////////////////////////////////////////

//消息窗口
class CPhraseVoice : public CDialog, public IPhraseVoice, public IDirectSoundEvent
{
	//属性变量
protected:
	bool								m_bHovering;						//盘旋标志
	INT									m_nCurrentPhrase;					//当前索引
	INT									m_nPhraseHover;						//当前焦点
	DWORD								m_dwSoundID[MAX_CHAIR];				//玩家声音								
	
	//配置变量
protected:
	bool								m_bConfig;							//配置标志
	CWnd								*m_pParentWnd;						//父窗口
	CPoint								m_ptBenchmarkPos;					//基准位置
	tagPhraseVoiceInfo					m_PhraseVoiceInfo;					//控件配置

	//组件变量
protected:
	CWHArray<tagPhraseVoice *>	m_arrPhraseVoice;					//短语数组
	//CDirectSound						m_DirectSound;						//声音组件
	CToolTipCtrl						m_ToolTipCtrl;						//提示信息

	//辅助变量
protected:
	IPhraseVoiceSink					*m_pIPhraseVoiceSink;				//回调接口

	//资源变量
protected:
	CBitImage							m_ImagePhraseBack;					//框架背景
	CPngImageWarpper 					m_PngFocusLine;						//焦点图像

	//控件变量
protected:
	CSkinButton							m_btUp;								//按钮控件
	CSkinButton							m_btDown;							//按钮控件

	//资源变量
protected:

	//函数定义
public:
	//构造函数
	CPhraseVoice(CWnd * pParentWnd=NULL);
	//析构函数
	virtual ~CPhraseVoice();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//设置函数
public:
	//初始函数
	virtual VOID __cdecl InitPhraseVoice( const tagPhraseVoiceInfo &PhraseVoiceInfo, CWnd *pParentWnd=NULL );
	//基准位置
	virtual VOID __cdecl SetBenchmarkPos( INT nXPos, INT nYPos );
	//基准位置
	virtual VOID __cdecl SetBenchmarkPos( CPoint ptBenchmarkPos );
	//设置回调
	virtual VOID __cdecl SetPhraseVoiceSink( IPhraseVoiceSink *pIPhraseVoiceSink );

	//功能函数
public:
	//显示短语
	virtual VOID __cdecl ShowPhraseWindow();
	//获取短语
	virtual LPCTSTR __cdecl GetPhraseString( INT nPhraseIndex, TCHAR szResult[], INT nMaxLength );
	//播放短语
	virtual VOID __cdecl PlayPhraseVoice( WORD wChairID, INT nPhraseIndex, INT nGender );

	//声音事件
public:
	//开始播放
	virtual VOID __cdecl OnEventSoundPlay( DWORD dwSoundID ) { return; }
	//播放结束
	virtual VOID __cdecl OnEventSoundStop( DWORD dwSoundID ); 

	//重载函数
protected:
	//映射函数
	virtual VOID DoDataExchange(CDataExchange* pDX);
	//初始化消息
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//内部函数
protected:
	//更新控件
	VOID UpdateButtons();
	//调整控件
	VOID RectifyControl();
	//转换索引
	INT SwitchToPhraseIndex( CPoint point );
	//更新提示
	VOID UpdateToolTip();

	//消息映射
protected:
	//绘画消息
	VOID OnPaint();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标移动
	VOID OnMouseMove(UINT nFlags, CPoint point);
	//焦点消息
	VOID OnKillFocus(CWnd* pNewWnd);
	//按钮点击
	VOID OnBnClickedDown();
	//按钮点击
	VOID OnBnClickedUp();
	//按钮焦点
	VOID OnBnKillFocus();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif