#ifndef DLG_LOGON_HEAD_FILE
#define DLG_LOGON_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "SessionLogon.h"

//////////////////////////////////////////////////////////////////////////

//��¼ TAB �ؼ�
class CTabCtrlLogon : public CTabCtrl
{
	//��������
public:
	//���캯��
	CTabCtrlLogon();
	//��������
	virtual ~CTabCtrlLogon();

	//��Ϣ����
protected:
	//�ػ�����	
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

//��¼����
class CDlgLogon : public CSkinDialog
{
	//��¼��Ϣ
public:
	DWORD							m_dwGameID;							//��Ϸ I D
	TCHAR							m_szAccounts[LEN_ACCOUNTS];			//��Ϸ�ʺ�
	TCHAR							m_szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							m_szLogonServer[LEN_SERVER];		//��¼��ַ

	//״̬����
protected:
	bool							m_bEditPass;						//�ı��־
	bool							m_bEditAccount;						//�ı��־
	bool							m_bEditUserId;						//�ı��־
	bool							m_bProxyMode;						//����ģʽ
	BYTE							m_cbVerifyMode;						//��֤ģʽ

	//ģʽ����
protected:
	enLogonMode						m_LogonMode;						//��¼ģʽ
	enNetworkKind					m_NetworkKind;						//��ַģʽ

	//λ�ñ���
protected:
	INT								m_nFullWidth;						//ȫ�����
	INT								m_nFullHeight;						//ȫ���߶�
	CRect							m_rcNormalFrame;					//���λ��

	//�ؼ�����
protected:
	CSkinButton						m_btLogon;							//��¼��ť
	CSkinButton						m_btProxyTest;						//���԰�ť
	CSkinButton						m_btProxyOption;					//���簴ť

	//�༭�ؼ�
protected:
	CSkinEdit						m_edPassword;						//�û�����
	CSkinEdit						m_edProxyPort;						//����˿�
	CSkinEdit						m_edProxyServer;					//�����ַ
	CSkinEdit						m_edProxyUserName;					//�����û�
	CSkinEdit						m_edProxyPassword;					//��������
	CSkinComboBox					m_cmProxyServerType;				//��������

	//�����ؼ�
protected:
	CSkinComboBox					m_cmGameID;							//��Ϸ I D
	CSkinComboBox					m_cmAccounts;						//��¼�ʺ�
	CSkinComboBox					m_cmLogonServer;					//��¼��ַ

	//�ؼ�����
protected:
	TCHAR							m_szAccountTitle[LEN_ACCOUNTS];		//�ʺű���

	//��������
public:
	//���캯��
	CDlgLogon();
	//��������
	virtual ~CDlgLogon();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();
	//��ʼ������
	virtual BOOL OnInitDialog();
	//�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//���ܺ���
public:
	//��ʵ����
	LPCTSTR GetPassword() { return m_szPassword; }
	//����ʺ�
	LPCTSTR GetAccounts() { return m_szAccounts; }
	//��¼ģʽ
	enLogonMode GetLogonMode() { return m_LogonMode; }
	//��������
	WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize);
	//�����ʺ�
	VOID SetAccounts(LPCTSTR lpszAccounts);
	//ɾ���ʺ�
	VOID DeleteAccounts(LPCTSTR lpszAccounts);

	//ģʽ����
private:
	//����ģʽ
	VOID SwitchProxyMode(bool bProxyMode);
	//�л�ģʽ
	VOID SwitchLogonMode(enLogonMode LogonMode);

	//��������
private:
	//��ȡ��Ϣ
	bool GetInformation();
	//�ʺ���Ϣ
	VOID LoadAccountsInfo();
	//��ַ��Ϣ
	VOID LoadLogonServerInfo();
	//������Ϣ
	VOID LoadProxyServerInfo();
	//����ѡ��
	VOID SetCurrentUserID(DWORD dwUserID);
	//��������
	VOID UpdateUserPassWord(DWORD dwUserID);
	//�����ַ�
	INT ComboBoxFindString(CComboBox * pComboBox, LPCTSTR pszFindString);

	//��Ϣ����
protected:
	//ѡ��ı�
	VOID OnSelchangeUserID();
	//ѡ��ı�
	VOID OnSelchangeAccounts();
	//����ı�
	VOID OnEnChangePassword();
	//����ı�
	VOID OnEditChangeAccounts();
	//����ı�
	VOID OnEditChangeUserID();
	//�����б�
	VOID OnCBDrawDownAccounts();
	//�����б�
	VOID OnCBDrawDownGameID();

	//��ť��Ϣ
protected:
	//ɾ���û�
	VOID OnBnClickedDelete();
	//�������
	VOID OnBnClickedProxyTest();
	//��������
	VOID OnBnClickedProxyOption();
	//��ס����
	VOID OnBnClickedRemPassword();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif