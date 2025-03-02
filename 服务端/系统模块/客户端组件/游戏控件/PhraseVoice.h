#ifndef PHRASE_VOICE_HEAD_FILE
#define PHRASE_VOICE_HEAD_FILE

#pragma once

#include "ClientControlsHead.h"
//#include "DirectSound.h"

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//����ṹ
struct tagPhraseVoice
{
	TCHAR							szPhrase[256];						//�����ַ�
	TCHAR							szGirlVoicePath[MAX_PATH];			//Ů��·��
	TCHAR							szBoyVoicePath[MAX_PATH];			//����·��
};

//////////////////////////////////////////////////////////////////////////

//��Ϣ����
class CPhraseVoice : public CDialog, public IPhraseVoice, public IDirectSoundEvent
{
	//���Ա���
protected:
	bool								m_bHovering;						//������־
	INT									m_nCurrentPhrase;					//��ǰ����
	INT									m_nPhraseHover;						//��ǰ����
	DWORD								m_dwSoundID[MAX_CHAIR];				//�������								
	
	//���ñ���
protected:
	bool								m_bConfig;							//���ñ�־
	CWnd								*m_pParentWnd;						//������
	CPoint								m_ptBenchmarkPos;					//��׼λ��
	tagPhraseVoiceInfo					m_PhraseVoiceInfo;					//�ؼ�����

	//�������
protected:
	CWHArray<tagPhraseVoice *>	m_arrPhraseVoice;					//��������
	//CDirectSound						m_DirectSound;						//�������
	CToolTipCtrl						m_ToolTipCtrl;						//��ʾ��Ϣ

	//��������
protected:
	IPhraseVoiceSink					*m_pIPhraseVoiceSink;				//�ص��ӿ�

	//��Դ����
protected:
	CBitImage							m_ImagePhraseBack;					//��ܱ���
	CPngImageWarpper 					m_PngFocusLine;						//����ͼ��

	//�ؼ�����
protected:
	CSkinButton							m_btUp;								//��ť�ؼ�
	CSkinButton							m_btDown;							//��ť�ؼ�

	//��Դ����
protected:

	//��������
public:
	//���캯��
	CPhraseVoice(CWnd * pParentWnd=NULL);
	//��������
	virtual ~CPhraseVoice();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ú���
public:
	//��ʼ����
	virtual VOID __cdecl InitPhraseVoice( const tagPhraseVoiceInfo &PhraseVoiceInfo, CWnd *pParentWnd=NULL );
	//��׼λ��
	virtual VOID __cdecl SetBenchmarkPos( INT nXPos, INT nYPos );
	//��׼λ��
	virtual VOID __cdecl SetBenchmarkPos( CPoint ptBenchmarkPos );
	//���ûص�
	virtual VOID __cdecl SetPhraseVoiceSink( IPhraseVoiceSink *pIPhraseVoiceSink );

	//���ܺ���
public:
	//��ʾ����
	virtual VOID __cdecl ShowPhraseWindow();
	//��ȡ����
	virtual LPCTSTR __cdecl GetPhraseString( INT nPhraseIndex, TCHAR szResult[], INT nMaxLength );
	//���Ŷ���
	virtual VOID __cdecl PlayPhraseVoice( WORD wChairID, INT nPhraseIndex, INT nGender );

	//�����¼�
public:
	//��ʼ����
	virtual VOID __cdecl OnEventSoundPlay( DWORD dwSoundID ) { return; }
	//���Ž���
	virtual VOID __cdecl OnEventSoundStop( DWORD dwSoundID ); 

	//���غ���
protected:
	//ӳ�亯��
	virtual VOID DoDataExchange(CDataExchange* pDX);
	//��ʼ����Ϣ
	virtual BOOL OnInitDialog();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//�ڲ�����
protected:
	//���¿ؼ�
	VOID UpdateButtons();
	//�����ؼ�
	VOID RectifyControl();
	//ת������
	INT SwitchToPhraseIndex( CPoint point );
	//������ʾ
	VOID UpdateToolTip();

	//��Ϣӳ��
protected:
	//�滭��Ϣ
	VOID OnPaint();
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//����ƶ�
	VOID OnMouseMove(UINT nFlags, CPoint point);
	//������Ϣ
	VOID OnKillFocus(CWnd* pNewWnd);
	//��ť���
	VOID OnBnClickedDown();
	//��ť���
	VOID OnBnClickedUp();
	//��ť����
	VOID OnBnKillFocus();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif