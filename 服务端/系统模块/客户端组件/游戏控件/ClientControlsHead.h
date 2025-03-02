#ifndef CLIENT_CONTROLS_HEAD_HEAD_FILE
#define CLIENT_CONTROLS_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ϵͳͷ�ļ�
#include "AfxHtml.h"

//ƽ̨ͷ�ļ�
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\�ͻ������\����ؼ�\SkinControlsHead.h"
#include "..\..\�ͻ������\�������\ClientServiceHead.h"

//��Ϸ������Ϣ
#include "..\..\��Ϣ����\CMD_GameServer.h"

//////////////////////////////////////////////////////////////////////////
//���ݽṹ

//����ؼ��ṹ
struct tagPhraseVoiceInfo
{
	//��Դ��ʶ
	LPCTSTR						pszImageFrame;						//�����Դ
	LPCTSTR						pszImageBtUp;						//��ť��Դ
	LPCTSTR						pszImageBtDown;						//��ť��Դ
	LPCTSTR						pszPngFocus;						//������Դ

	//λ�ñ���
	INT							cxBorderWidth;						//�߿���
	INT							cyBorderHeight;						//�߿�߶�
	INT							cyBottomHeight;						//�ײ��߶�

	//���ñ���
	INT							nPageCount;							//ÿҳ��ʾ����
	INT							nTransPercent;						//͸���ٷֱ�
	COLORREF					crTextColor;						//������ɫ
};

//////////////////////////////////////////////////////////////////////////
//�����궨��

//��������
#ifndef CLIENT_CONTROLS_CLASS
	#ifdef  CLIENT_CONTROLS_DLL
		#define CLIENT_CONTROLS_CLASS _declspec(dllexport)
	#else
		#define CLIENT_CONTROLS_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define CLIENT_CONTROLS_DLL_NAME	TEXT("ClientControls.dll")			//��� DLL ����
#else
	#define CLIENT_CONTROLS_DLL_NAME	TEXT("ClientControlsD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IRichEditMessageSink INTERFACE_VERSION(1,1)
	static const GUID IID_IRichEditMessageSink={0x41579168,0x39fe,0x4862,0x00a7,0x0a,0xc2,0x93,0xb9,0xd3,0xb2,0x6e};
#else
	#define VER_IRichEditMessageSink INTERFACE_VERSION(1,1)
	static const GUID IID_IRichEditMessageSink={0xd4b0e8b0,0x05d5,0x4fa2,0x0095,0xb6,0x00,0xc5,0xc6,0x7a,0xde,0x5b};
#endif

//��Ϣ�ӿ�
interface IRichEditMessageSink : public IUnknownEx
{
	//����ӿ�
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

//��Ϣ�ӿ�
interface IStringMessage : public IUnknownEx
{
	//�¼���Ϣ
public:
	//�����¼�
	virtual bool __cdecl InsertUserEnter(LPCTSTR pszUserName)=NULL;
	//�뿪�¼�
	virtual bool __cdecl InsertUserLeave(LPCTSTR pszUserName)=NULL;
	//�����¼�
	virtual bool __cdecl InsertUserOffLine(LPCTSTR pszUserName)=NULL;

	//�ַ���Ϣ
public:
	//��ͨ��Ϣ
	virtual bool __cdecl InsertNormalString(LPCTSTR pszString)=NULL;
	//ϵͳ��Ϣ
	virtual bool __cdecl InsertSystemString(LPCTSTR pszString)=NULL;
	//��ʾ��Ϣ
	virtual bool __cdecl InsertPromptString(LPCTSTR pszString)=NULL;
	//������Ϣ
	virtual bool __cdecl InsertAfficheString(LPCTSTR pszString)=NULL;
	//������Ϣ
	virtual bool __cdecl InserBugleString(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, BYTE cbBugleType)=NULL;

	//������Ϣ
public:
	//������Ϣ
	virtual bool __cdecl InsertCustomString(LPCTSTR pszString, COLORREF crColor)=NULL;
	//������Ϣ
	virtual bool __cdecl InsertCustomString(LPCTSTR pszString, COLORREF crColor, COLORREF crBackColor)=NULL;

	//������Ϣ
public:
	//�û�����
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor)=NULL;
	//�û�����
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszString, COLORREF crColor,bool bSoundNotify)=NULL;
	//�û�˽��
	virtual bool __cdecl InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString)=NULL;
	//�û�˽��
	virtual bool __cdecl InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString, const SYSTEMTIME &time)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//���������ص�
interface IPhraseVoiceSink
{
	//ѡ���¼�
	virtual VOID __cdecl OnEventPhraseSelect(INT nPhraseIndex)=NULL;
};

//�����¼�
interface IDirectSoundEvent
{
	//��ʼ����
	virtual VOID __cdecl OnEventSoundPlay( DWORD dwSoundID )=NULL;
	//���Ž���
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

//��������
interface IPhraseVoice: public IUnknownEx
{
	//���ú���
public:
	//��ʼ����
	virtual VOID __cdecl InitPhraseVoice( const tagPhraseVoiceInfo &PhraseVoiceInfo, CWnd *pParentWnd=NULL )=NULL;
	//��׼λ��
	virtual VOID __cdecl SetBenchmarkPos( INT nXPos, INT nYPos )=NULL;
	//��׼λ��
	virtual VOID __cdecl SetBenchmarkPos( CPoint ptBenchmarkPos )=NULL;
	//���ûص�
	virtual VOID __cdecl SetPhraseVoiceSink( IPhraseVoiceSink *pIPhraseVoiceSink )=NULL;

	//���ܺ���
public:
	//��ʾ����
	virtual VOID __cdecl ShowPhraseWindow()=NULL;
	//��ȡ����
	virtual LPCTSTR __cdecl GetPhraseString( INT nPhraseIndex, TCHAR szResult[], INT nMaxLength )=NULL;
	//���Ŷ���
	virtual VOID __cdecl PlayPhraseVoice( WORD wChairID, INT nPhraseIndex, INT nGender )=NULL;
};

//////////////////////////////////////////////////////////////////////////

//�����ļ�
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
//���������

DECLARE_MODULE_HELPER(PhraseVoice, PhraseVoice, CLIENT_CONTROLS_DLL_NAME,"CreatePhraseVoice")

//////////////////////////////////////////////////////////////////////////

#endif