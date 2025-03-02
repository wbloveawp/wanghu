#ifndef DLG_BANK_HEAD_FILE
#define DLG_BANK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "ClientControlsHead.h"
#include "..\..\ȫ�ֶ���\array.h"
#include "..\����ؼ�\SkinControlsHead.h"
//#include "..\��Ϸ�㳡\ServiceManager.h"
//////////////////////////////////////////////////////////////////////////

//��˵��
class CBankItem;
struct tagGlobalUserData;

//����˵��
typedef CArrayTemplate<CBankItem *> CBankItemArray;

//////////////////////////////////////////////////////////////////////////

//ѡ�����
class CLIENT_CONTROLS_CLASS CBankBar : public CWnd
{
	//���Ϣ
protected:
	bool							m_bHovering;						//������־
	WORD							m_wCollocateDown;					//��������
	WORD							m_wCollocateHover;					//��������
	WORD							m_wCollocateActive;					//��������

	//������Ϣ
public:
	CRect							m_rcItemCreate;						//����λ��
	CBankItemArray					m_CollocateItemArray;				//��������

	//��Դ����
protected:
	CSize							m_SizeButton;						//��ť��С
	CBitImage						m_ImageBackGroud;					//������ͼ
	IPlatformResource *				m_pIPlatformResource;				//�����Դ
	tagPlazaCollocateImage			m_PlazaCollocateImage;				//�����Դ

	//��������
public:
	//���캯��
	CBankBar(IPlatformResource *pIPlatformResource);
	//��������
	virtual ~CBankBar();

	//���غ���
public:
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//���ܺ���
public:
	//������Դ
	bool UpdateSkinResource();

	//��������
public:
	//�������
	bool SaveParameter();
	//Ĭ�ϲ���
	bool DefaultParameter();

	//���ܺ���
public:
	//��������
	bool SetItemCreateRect(CRect rcItemCreate);
	//��������
	bool InsertCollocateItem(CBankItem * pCollocateItem);
	//��������
	bool SetActiveCollocateItem(CBankItem * pCollocateItem);
	//ȡ��ǰ��
	CBankItem * GetActiveCollocateItem();

	//�ڲ�����
private:
	//�����л�
	WORD SwitchToButtonIndex(CPoint MousePoint);

	//��Ϣ����
protected:
	//�ػ���Ϣ
	VOID OnPaint();
	//������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//����ƶ�
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//����뿪
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//�����Ϣ
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//��������
class  CLIENT_CONTROLS_CLASS CBankItem : public CDialog
{
	//������Դ
protected:
	tagEncircleInfo					m_EncircleInfoFrame;				//�����Ϣ
	tagEncircleImage				m_EncircleImageFrame;				//�����Դ
	IPlatformResource *				m_pIPlatformResource;				//�����Դ
	tagPlazaCollocateImage			m_PlazaCollocateImage;				//�����Դ
	tagGlobalUserData *				m_pGlobalUserData;					//�û�ָ��

public:
	IBankSink *						m_pIBankSink;						//�ص�ָ��

	//��������
public:
	//���캯��
	CBankItem(UINT uIDTemplate,IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//��������
	virtual ~CBankItem();

	//���غ���
protected:
	//ȷ����Ϣ
	virtual VOID OnOK() { GetParent()->PostMessage(WM_COMMAND,IDOK,0); }
	//ȡ����Ϣ
	virtual VOID OnCancel() { GetParent()->PostMessage(WM_COMMAND,IDCANCEL,0); }

	//���غ���
public:
	//�������
	virtual bool SaveParameter()=NULL;
	//Ĭ�ϲ���
	virtual bool DefaultParameter()=NULL;
	//���¿���
	virtual bool UpdateControlStatus()=NULL;
	//��ȡ����
	virtual LPCTSTR GetCollocateName()=NULL;
	//���ûص�
	virtual void SetBankSink(IBankSink *pIBankSink){m_pIBankSink=pIBankSink;}
	
	//���ܺ���
public:
	//�滭����
	VOID DrawBackGround(CDC * pDC, INT nWidth, INT nHeight);
	//��������
	bool CreateCollocateItem(CWnd * pParentWnd, CRect rcCreate);

	//��Ϣ����
protected:
	//������Ϣ
	BOOL OnEraseBkgnd(CDC * pDC);
	//��ɫ��Ϣ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//���д�ȡ
class CLIENT_CONTROLS_CLASS CBankAccess : public CBankItem
{
	//��������
public:
	//���캯��
	CBankAccess(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//��������
	virtual ~CBankAccess();
	enum {IDD=IDD_BANK_ACCESS};

	//���غ���
public:
	//�ؼ���
	virtual void DoDataExchange(CDataExchange* pDX);
	//���غ���
public:
	//�������
	virtual bool SaveParameter();
	//Ĭ�ϲ���
	virtual bool DefaultParameter();
	//���¿���
	virtual bool UpdateControlStatus();
	//��ȡ����
	virtual LPCTSTR GetCollocateName();

	DECLARE_MESSAGE_MAP()
public:
	CSkinButton m_btStorage;
	CSkinButton m_btDrawout;
	afx_msg void OnBnClickedBtnStorage();
	afx_msg void OnEnChangeEditAmount();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnDrawout();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//////////////////////////////////////////////////////////////////////////

//����ת��
class CLIENT_CONTROLS_CLASS CBankTransfer : public CBankItem
{
	//��������
public:
	//���캯��
	CBankTransfer(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//��������
	virtual ~CBankTransfer();
	enum {IDD=IDD_BANK_TRANSFER};

	//���غ���
public:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	
	//���غ���
public:
	//�������
	virtual bool SaveParameter();
	//Ĭ�ϲ���
	virtual bool DefaultParameter();
	//���¿���
	virtual bool UpdateControlStatus();
	//��ȡ����
	virtual LPCTSTR GetCollocateName();

	//��Ϣ����
public:
	//�������
	VOID OnBnClickedProxyTest();

	DECLARE_MESSAGE_MAP()
	CSkinButton m_btTransfer;
	int 		m_nAccounts;
	afx_msg void OnBnClickedRadioAccount();
	afx_msg void OnBnClickedRadioGameid();
	afx_msg void OnBnClickedBtnTransfer();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//////////////////////////////////////////////////////////////////////////

//�޸�����
class CLIENT_CONTROLS_CLASS CBankModifyPass : public CBankItem
{
	//��������
public:
	//���캯��
	CBankModifyPass(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//��������
	virtual ~CBankModifyPass();
	enum {IDD=IDD_BANK_MODIFYPASS};

	//���غ���
public:
	//�������
	virtual bool SaveParameter();
	//Ĭ�ϲ���
	virtual bool DefaultParameter();
	//���¿���
	virtual bool UpdateControlStatus();
	//��ȡ����
	virtual LPCTSTR GetCollocateName();
	//���غ���
public:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);

	DECLARE_MESSAGE_MAP()
	CSkinButton m_btModifyPass;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnModify();
};


//���д���
class  CLIENT_CONTROLS_CLASS CDlgBank : public CSkinDialog
{
	//���ñ���
protected:
	//IClientUserItem *				m_pMySelfUsreItem;					//�û�ָ��
	IPlatformResource *				m_pIPlatformResource;				//�����Դ
	tagGlobalUserData *				m_pGlobalUserData;					//�û�ָ��
	IBankSink		  *				m_pIBankSink;						//�ص�ָ��


	//�ؼ�����
protected:
	CSkinButton						m_btOk;								//ȷ����ť
	CSkinButton						m_btCancel;							//ȡ����ť
	CBankBar						m_CollocateBar;						//���ť
	
	//��������
public:
	CBankAccess						m_BankAccess;						//���д�ȡ
	CBankTransfer					m_BankTransfer;						//����ת��
	CBankModifyPass					m_BankModifyPass;					//�޸�����

	//��������
public:
	//���캯��
	CDlgBank(IPlatformResource *pIPlatformResource,tagGlobalUserData *pGlobalUserData);
	//��������
	virtual ~CDlgBank();
	enum {IDD=IDD_BANK};
	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();

	//���ܺ���
public:
	//���ò���
	bool InitBankSink(IBankSink *pIBankSink);
	//ˢ�½���
	bool UpdateView();

	//��ť��Ϣ
protected:
	//Ĭ�ϰ�ť
	VOID OnBnClickedDefault();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//////////////////////////////////////////////////////////////////////////

#endif