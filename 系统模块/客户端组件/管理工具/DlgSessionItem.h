#ifndef DLG_MANAGER_OPTION_HEAD_FILE
#define DLG_MANAGER_OPTION_HEAD_FILE

#pragma once

#include "CheckComboBox.h"
#include "VirtualLinkManager.h"

//////////////////////////////////////////////////////////////////////////
//��ɫ����
#define CR_NORMAL_TX				RGB(0,0,0)
#define CR_SELECT_TX				RGB(128,64,0)	
#define CR_CONTROL_BACK				RGB(240,240,240)		
#define CR_CONTROL_BORAD			RGB(0,0,0)

//////////////////////////////////////////////////////////////////////////

//��������
class CDlgSessionItem;
class CDlgSessionOption;
class CDlgSessionCommand;

//////////////////////////////////////////////////////////////////////////

//�滭����
class CDlgSessionItem : public CDialog, public CSessionItem
{
	//��������
protected:
	CServerListManager*				m_pServerListManager;				//�б����

	//��������
protected:
	//���캯��
	CDlgSessionItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgSessionItem();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK() { return; }
	//ȡ����Ϣ
	virtual VOID OnCancel() { return; }

	//���ú���
public:
	//�б����
	VOID SetServerListManager(CServerListManager* pServerListManager) { m_pServerListManager=pServerListManager;}

	//֪ͨ�¼�
public:
	//�б����
	virtual VOID OnEventListFinish() {};

	//���ܺ���
public:
	//�л�����
	VOID SwitchViewItem(bool bActive);
	//��������
	VOID CreateViewItem(const CRect & rcRect, CWnd * pParentWnd);	

	//�ص��ӿ�
public:
	//�����¼�
	virtual bool OnEventMissionLink(LONG lErrorCode) { return true; }
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason) { return true; }
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize) { return true; }	

	//��Ϣ����
private:
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);
	//������ɫ
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//ѡ��Ự
class CDlgSessionOption : public CDlgSessionItem
{
	//��������
protected:
	tagModifyOption					m_ModifyOption;						//�༭ѡ��

	//�ؼ�����
protected:
	CCheckComboBox					m_cbGameList;						//��Ϸ�б�
	CCheckComboBox					m_cbServerList;						//�����б�

	//��������
public:
	//���캯��
	CDlgSessionOption();
	//��������
	virtual ~CDlgSessionOption();

	//���غ���
protected:
	//��ʼ����
	virtual BOOL OnInitDialog();
	//��������
	virtual VOID DoDataExchange(CDataExchange* pDX);	
	//�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//֪ͨ�¼�
public:
	//�б����
	virtual VOID OnEventListFinish();

	//�ص��ӿ�
public:
	//�����¼�
	virtual bool OnEventMissionLink(LONG lErrorCode) { return true; }
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason) { return true; }
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//��������
private:
	//�ؼ�ʹ��
	VOID EnableControls(BOOL bEnabled);

	//�¼�����
protected:
	//�༭����
	VOID OnBnClickedBtModify();

	//��Ϣ����
protected:
	//���ٴ���
	VOID OnDestroy();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//����Ự
class CDlgSessionCommand : public CDlgSessionItem
{
	//��������
protected:
	tagExecuteCommand					m_CommandCode;						//�������

	//��������
public:
	//���캯��
	CDlgSessionCommand();
	//��������
	virtual ~CDlgSessionCommand();

	//���غ���
protected:
	//��ʼ����
	virtual BOOL OnInitDialog();
	//��������
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//�ص��ӿ�
public:
	//�����¼�
	virtual bool OnEventMissionLink(LONG lErrorCode) { return true; }
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason) { return true; }
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//��������
private:
	//�ؼ�ʹ��
	VOID EnableControls(BOOL bEnabled);

	//�¼�����
protected:
	//ִ������
	VOID OnBnClickedBtExecuteCmd();
	//��������
	VOID OnBnClickedBtReloadConfig();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
#endif