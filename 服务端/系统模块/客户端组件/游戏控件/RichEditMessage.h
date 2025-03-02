#ifndef RICH_EDIT_MESSAGE_HEAD_FILE
#define RICH_EDIT_MESSAGE_HEAD_FILE

#pragma once

#include "Expression.h"
#include "ClientControlsHead.h"
//#include "DirectSound.h"
//////////////////////////////////////////////////////////////////////////

//��ɫ����
#define COLOR_DEBUG					RGB(0,128,128)						//������ɫ
#define COLOR_EXCEPTION				RGB(200,0,0)						//�쳣��ɫ

//��ɫ����
#define COLOR_WARN					RGB(255,128,0)						//������ɫ
#define COLOR_EVENT					RGB(125,125,125)					//�¼���Ϣ
#define COLOR_NORMAL				RGB(0,0,0)							//��ͨ��Ϣ
#define COLOR_SYSTEM_STRING			RGB(154,0,2)						//ϵͳ��Ϣ

//˽����ɫ
#define COLOR_USER					RGB(0,0,255)						//�Է���ɫ
#define COLOR_MYSELF				RGB(0,128,64)						//�Լ���ɫ
 
//λ�ö���
#define STRING_OFFSET				90									//����λ��

//////////////////////////////////////////////////////////////////////////

//�ַ���Ϣ
class CLIENT_CONTROLS_CLASS CRichEditMessage : public CSkinRichEdit, public IStringMessage
{
	//ͼƬ��Դ
protected:
	COLORREF						m_crFillColor;						//�����ɫ
	CImageDataObject				m_ImageDataSystem;					//��Դ����
	CImageDataObject				m_ImageDataPrompt;					//��Դ����
	CImageDataObject				m_ImageDataAffiche;					//��Դ����
	CImageDataObject				m_ImageBigBugle;					//��Դ����
	CImageDataObject				m_ImageSmallBugle;					//��Դ����

	//�������
protected:
	CSkinScrollBar					m_SkinScrollBar;					//��������

	//�������
protected:
	CExpressionManager *			m_pExpressionManager;				//�������
	IRichEditMessageSink *			m_pIRichEditMessageSink;			//�ӿڱ���
	//CDirectSound					m_DirectSound;						//��������
	DWORD							m_dwMsgSoundID;						//����ID

	//��������
public:
	//���캯��
	CRichEditMessage();
	//��������
	virtual ~CRichEditMessage();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�¼���Ϣ
public:
	//�����¼�
	virtual bool __cdecl InsertUserEnter(LPCTSTR pszUserName);
	//�뿪�¼�
	virtual bool __cdecl InsertUserLeave(LPCTSTR pszUserName);
	//�����¼�
	virtual bool __cdecl InsertUserOffLine(LPCTSTR pszUserName);

	//�ַ���Ϣ
public:
	//��ͨ��Ϣ
	virtual bool __cdecl InsertNormalString(LPCTSTR pszString);
	//ϵͳ��Ϣ
	virtual bool __cdecl InsertSystemString(LPCTSTR pszString);
	//��ʾ��Ϣ
	virtual bool __cdecl InsertPromptString(LPCTSTR pszString);
	//������Ϣ
	virtual bool __cdecl InsertAfficheString(LPCTSTR pszString);
	//������Ϣ
	virtual bool __cdecl InserBugleString(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, BYTE cbBugleType);

	//������Ϣ
public:
	//������Ϣ
	virtual bool __cdecl InsertCustomString(LPCTSTR pszString, COLORREF crColor);
	//������Ϣ
	virtual bool __cdecl InsertCustomString(LPCTSTR pszString, COLORREF crColor, COLORREF crBackColor);

	//������Ϣ
public:
	//�û�����
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor);
	//�û�����
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszString, COLORREF crColor,bool bSoundNotify);
	//�û�˽��
	virtual bool __cdecl InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString);
	//�û�˽��
	virtual bool __cdecl InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString, const SYSTEMTIME &time);

	//���غ���
protected:
	//�ؼ���
	virtual VOID PreSubclassWindow();

	//���ú���
public:
	//���ýӿ�
	bool SetRichEditMessageSink(IUnknownEx * pIUnknownEx);
	//�������
	bool SetExpressionManager(CExpressionManager * pExpressionManager, COLORREF crFillColor);

	//���ܺ���
public:
	//����ʱ��
	bool InsertSystemTime(COLORREF crColor);
	//�û�����
	bool InsertUserAccounts(LPCTSTR pszAccounts);
	//�����ַ�
	bool TranslateInsertString(LPCTSTR pszString, COLORREF crColor);
	//�����ַ�
	bool TranslateInsertString(LPCTSTR pszString, CHARFORMAT2 &CharFormat);

	//�ڲ�����
protected:
	//����ͼƬ
	bool LoadRichEditImage(HINSTANCE hInstance, UINT uResourceID, CImageDataObject & ImageDataObject);

	//��Ϣӳ��
protected:
	//������Ϣ
	VOID OnEventLink(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif