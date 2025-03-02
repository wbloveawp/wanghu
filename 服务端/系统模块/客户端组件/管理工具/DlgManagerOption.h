#ifndef DLG_MANAGER_OPTION_HEAD_FILE
#define DLG_MANAGER_OPTION_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////
//��ɫ����
#define CR_NORMAL_TX				RGB(0,0,0)
#define CR_SELECT_TX				RGB(128,64,0)	
#define CR_CONTROL_BACK				RGB(240,240,240)		
#define CR_CONTROL_BORAD			RGB(0,0,0)

//////////////////////////////////////////////////////////////////////////

//��������
class CDlgOptionItem;
class CDlgServerOption;
class CDlgCustomOption;

//////////////////////////////////////////////////////////////////////////

//��������
class CDlgOptionItem : public CDialog
{
	//��������
protected:
	//���캯��
	CDlgOptionItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgOptionItem();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK() { return; }
	//ȡ����Ϣ
	virtual VOID OnCancel() { return; }

	//���ú���
public:			
	//�ؼ�ʹ��
	virtual VOID EnableControls(BOOL bEnabled)=NULL;	
	//��������
	virtual WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)=NULL;

	//���ܺ���
public:
	//��ʾ����
	VOID ShowViewItem(const CRect & rcRect, CWnd * pParentWnd); 
	//��������
	VOID CreateViewItem(const CRect & rcRect, CWnd * pParentWnd);

	//��Ϣ����
private:
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);
	//������ɫ
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//��������
class CDlgServerOption : public CDlgOptionItem
{
	//��������
protected:
	tagModifyRule					m_ModifyRule;						//�޸Ĺ���	

	//��������
public:
	//���캯��
	CDlgServerOption();
	//��������
	virtual ~CDlgServerOption();

	//���غ���
protected:
	//��ʼ����
	virtual BOOL OnInitDialog();
	//��������
	virtual VOID DoDataExchange(CDataExchange* pDX);	

	//���ú���
public:			
	//�ؼ�ʹ��
	virtual VOID EnableControls(BOOL bEnabled);	
	//��������
	virtual WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize);

	//��������
private:
	//��ʼ���ؼ�
	VOID InitControls();	

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

//�Զ�����ͼ
class CDlgCustomOption : public CDlgOptionItem
{
	//��������
protected:
	tagCommandCode					m_CommandCode;						//�������

	//��������
public:
	//���캯��
	CDlgCustomOption();
	//��������
	virtual ~CDlgCustomOption();

	//���غ���
protected:
	//��ʼ����
	virtual BOOL OnInitDialog();
	//��������
	virtual VOID DoDataExchange(CDataExchange* pDX);	

	//���ú���
public:		
	//�ؼ�ʹ��
	virtual VOID EnableControls(BOOL bEnabled);	
	//��������
	virtual WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize);

	//��������
private:
	//��ʼ���ؼ�
	VOID InitControls();	

	//�¼�����
protected:
	//�༭����
	VOID OnBnClickedBtExecuteCmd();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
#endif