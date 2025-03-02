#ifndef DLG_SERVER_WIZARD_HEAD_FILE
#define DLG_SERVER_WIZARD_HEAD_FILE

#pragma once

#include "ModuleListControl.h"
#include "ModuleInfoManager.h"
#include "ServerInfoManager.h"
#include "ModuleManagerHead.h"
#include "DataBaseListManager.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CDlgServerWizard;

//////////////////////////////////////////////////////////////////////////

//�򵼻�����
class MODULE_MANAGER_CLASS CDlgServerWizardItem : public CDialog
{
	//��Ԫ����
	friend class CDlgServerWizard;

	//״̬����
private:
	bool							m_bSaveData;						//�����־

	//��������
protected:
	CDlgServerWizard *				m_pDlgServerWizard;					//��ָ��
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//��������
protected:
	//���캯��
	CDlgServerWizardItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgServerWizardItem();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo()=NULL;
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//���ܺ���
private:
	//��������
	bool SaveItemData();
	//������
	bool ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd);	

	//��Ϣ����
private:
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//����ѡ����
class MODULE_MANAGER_CLASS CDlgServerOptionItem : public CDialog
{
	//��Ԫ����
	friend class CDlgServerWizardStep2;

	//��������
protected:	
	CDlgServerWizard *				m_pDlgServerWizard;					//��ָ��
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������	

	//��������
protected:
	//���캯��
	CDlgServerOptionItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgServerOptionItem();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo()=NULL;
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//�¼��ӿ�
protected:
	//���ͱ��
	virtual VOID OnEventServerTypeChange(WORD wServerType) {};

	//���ܺ���
private:
	//��������
	void CreateOptionItem(const CRect & rcRect, CWnd * pParentWnd);

	//��Ϣ����
private:
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

//����ѡ��1
class MODULE_MANAGER_CLASS CDlgServerOption1 : public CDlgServerOptionItem
{
	//��������
public:
	//���캯��
	CDlgServerOption1();
	//��������
	virtual ~CDlgServerOption1();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//�¼��ӿ�
protected:
	//���ͱ��
	virtual VOID OnEventServerTypeChange(WORD wServerType);
};

//////////////////////////////////////////////////////////////////////////

//����ѡ��2
class MODULE_MANAGER_CLASS CDlgServerOption2 : public CDlgServerOptionItem
{
	//��������
public:
	//���캯��
	CDlgServerOption2();
	//��������
	virtual ~CDlgServerOption2();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//����ѡ��3
class MODULE_MANAGER_CLASS CDlgServerOption3 : public CDlgServerOptionItem
{
	//��������
public:
	//���캯��
	CDlgServerOption3();
	//��������
	virtual ~CDlgServerOption3();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//����ѡ��4
class MODULE_MANAGER_CLASS CDlgServerOption4 : public CDlgServerOptionItem
{
	//��������
public:
	//���캯��
	CDlgServerOption4();
	//��������
	virtual ~CDlgServerOption4();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgServerOptionCustom : public CDlgServerOptionItem
{
	//�ؼ�����
protected:
	HWND							m_hCustomRule;						//���ƹ���

	//��������
public:
	//���캯��
	CDlgServerOptionCustom();
	//��������
	virtual ~CDlgServerOptionCustom();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//��Ϣ����
private:
	//������Ϣ
	VOID OnNcDestroy();
	//������Ϣ
	VOID OnSetFocus(CWnd * pNewWnd);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//������
class MODULE_MANAGER_CLASS CDlgServerWizardStep1 : public CDlgServerWizardItem
{
	//�б����
protected:
	CStatic							m_StaticPrompt;						//��ʾ�ؼ�
	CModuleInfoBuffer				m_ModuleInfoBuffer;					//ģ����Ϣ
	CModuleListControl				m_ModuleListControl;				//ģ���б�
	CModuleInfoManager				m_ModuleInfoManager;				//ģ�����

	//�������
protected:
	tagGameModuleInfo *				m_pGameModuleInfo;					//ѡ��ģ��
	CGameServiceManagerHelper		m_GameServiceManager;				//��Ϸģ��

	//��������
public:
	//���캯��
	CDlgServerWizardStep1();
	//��������
	virtual ~CDlgServerWizardStep1();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//���ܺ���
protected:
	//����ģ��
	bool LoadDBModuleItem();

	//�ؼ���Ϣ
protected:
	//˫���б�
	VOID OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//������
class MODULE_MANAGER_CLASS CDlgServerWizardStep2 : public CDlgServerWizardItem
{
	//�ؼ�����
protected:
	BYTE							m_cbItemIndex;					//��������
	CDlgServerOption1				m_DlgServerOption1;				//����ѡ��
	CDlgServerOption2				m_DlgServerOption2;				//����ѡ��
	CDlgServerOption3				m_DlgServerOption3;				//����ѡ��
	CDlgServerOption4				m_DlgServerOption4;				//����ѡ��
	CDlgServerOptionCustom			m_DlgServerOption5;				//����ѡ��
	CDlgServerOptionItem *			m_pDlgServerOptionItem[5];		//ѡ������

	//��������
public:
	//���캯��
	CDlgServerWizardStep2();
	//��������
	virtual ~CDlgServerWizardStep2();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//��������
protected:
	//��������
	VOID InitBaseOption();
	//��������
	bool SaveBaseOption();
	//��������
	VOID ActiveOptionItem(BYTE cbItemIndex);

	//��Ϣ����
public:
	//ѡ����
	void OnCbnSelchangeServerType();
	//ѡ����
	void OnTcnSelchangeServerOption(NMHDR *pNMHDR, LRESULT *pResult);
	
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//������
class MODULE_MANAGER_CLASS CDlgServerWizard : public CDialog
{
	//��Ԫ����
	friend class CDlgServerWizardStep1;
	friend class CDlgServerWizardStep2;
	friend class CDlgServerOptionCustom;

	//��������
protected:
	WORD							m_wActiveIndex;						//�����
	CDlgServerWizardItem *			m_pWizardItem[2];					//��ָ��

	//������Ϣ
protected:
	tagModuleInitParameter			m_ModuleInitParameter;				//���ò���

	//�������
protected:
	CDatabaseListManager			m_DBListMgr;						//���ݿ��б�

	//�ؼ�����
protected:
	CDlgServerWizardStep1			m_ServerWizardStep1;				//���ò���
	CDlgServerWizardStep2			m_ServerWizardStep2;				//���ò���

	//�ӿڱ���
protected:
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IGameServiceCustomRule *		m_pIGameServiceCustomRule;			//�Զ�����

	//��������
public:
	//���캯��
	CDlgServerWizard();
	//��������
	virtual ~CDlgServerWizard();

	//���غ���
public:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();

	//���ú���
public:
	//��ȡ����
	VOID GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//��ȡ����
	VOID GetGameServiceOption(tagGameServiceOption & GameServiceOption);
	//��������
	VOID SetGameServiceOption(tagGameServiceOption & GameServiceOption);
	//���ýӿ�
	VOID SetGameServiceManager(IGameServiceManager * pIGameServiceManager);

	//�ڲ�����
private:
	//������
	bool ActiveWizardItem(WORD wIndex);

	//��Ϣӳ��
public:
	//��һ��
	VOID OnBnClickedNext();
	//��ɰ�ť
	VOID OnBnClickedFinish();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif