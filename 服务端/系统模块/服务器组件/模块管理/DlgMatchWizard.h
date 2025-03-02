#ifndef DLG_MATCH_WIZARD_HEAD_FILE
#define DLG_MATCH_WIZARD_HEAD_FILE

#pragma once

//�����ļ�
#include "CheckComboBox.h"
#include "MatchInfoManager.h"
#include "MatchListControl.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CDlgMatchItem;
class CDlgMatchWizard;
class CDlgMatchWizardStep1;
class CDlgMatchWizardStep2;
class CDlgMatchWizardStep3;

//��˵��
class CDlgMatchRound;
class CDlgMatchReward;
class CDlgMatchSingup1;
class CDlgMatchSingup2;
class CDlgMatchDistribute;
class CDlgMatchLockTime;
class CDlgMatchImmediate;

//��˵��
class CDlgPromoteModeItem;
class CDlgPromoteOutGame;
class CDlgPromoteFixedGame;
class CDlgPromoteFixedTable;

//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgPromoteModeItem : public CDialog
{
	friend class CDlgMatchWizardStep2;	

	//��������
protected:		
	tagMatchRoundItem *			m_pMatchRoundItem;					//�ִ�����

	//��������
protected:
	//���캯��
	CDlgPromoteModeItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgPromoteModeItem();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK() { return; }
	//ȡ����Ϣ
	virtual VOID OnCancel() { return; }

	//�ӿں���
protected:	
	//��������
	virtual bool SaveInputInfo()=NULL;
	//ˢ�¿ؼ�
	virtual bool RefreshControl()=NULL;
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//���ú���
protected:		
	//��������
	VOID SetMatchRoundItem(tagMatchRoundItem * pMatchRoundItem);
	//��ȡ����
	tagMatchRoundItem * GetMatchRoundItem() { return m_pMatchRoundItem; }

	//���ܺ���
private:
	//��ʾ����
	bool ShowPromoteItem(const CRect & rcRect, CWnd * pParentWnd); 
	//��������
	VOID CreatePromoteItem(const CRect & rcRect, CWnd * pParentWnd);

	//��Ϣ����
private:
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgPromoteOutGame : public CDlgPromoteModeItem
{
	//��������
public:
	//���캯��
	CDlgPromoteOutGame();
	//��������
	virtual ~CDlgPromoteOutGame();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//ˢ�¿ؼ�
	virtual bool RefreshControl();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);		
};

//////////////////////////////////////////////////////////////////////////

//���ֻ���
class MODULE_MANAGER_CLASS CDlgPromoteFixedGame: public CDlgPromoteModeItem
{
	//��������
public:
	//���캯��
	CDlgPromoteFixedGame();
	//��������
	virtual ~CDlgPromoteFixedGame();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//ˢ�¿ؼ�
	virtual bool RefreshControl();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);		
};

//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgPromoteFixedTable: public CDlgPromoteModeItem
{
	//��������
public:
	//���캯��
	CDlgPromoteFixedTable();
	//��������
	virtual ~CDlgPromoteFixedTable();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();	
	//ˢ�¿ؼ�
	virtual bool RefreshControl();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);		
};

//////////////////////////////////////////////////////////////////////////

//����������
class MODULE_MANAGER_CLASS CDlgMatchItem : public CDialog
{
	friend class CDlgMatchWizardStep1;
	friend class CDlgMatchWizardStep3;

	//��������
protected:		
	tagMatchOption *			m_pMatchOption;					//��������
	CMatchInfoBuffer *				m_pMatchInfoBuffer;					//ģ����Ϣ

	//��������
protected:
	//���캯��
	CDlgMatchItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgMatchItem();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK() { return; }
	//ȡ����Ϣ
	virtual VOID OnCancel() { return; }

	//�ӿں���
protected:	
	//��������
	virtual bool SaveInputInfo()=NULL;	
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//���ú���
protected:	
	//���ñ���
	VOID SetMatchInfoBuffer(CMatchInfoBuffer * pMatchInfoBuffer);
	//��������
	VOID SetMatchOption(tagMatchOption * pMatchOption);
	//��ȡ����
	tagMatchOption * GetMatchOption() { return m_pMatchOption; }

	//���ܺ���
private:
	//��ʾ����
	bool ShowMatchItem(const CRect & rcRect, CWnd * pParentWnd); 
	//��������
	VOID CreateMatchItem(const CRect & rcRect, CWnd * pParentWnd);

	//�¼�����
public:
	//���ͱ��
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);

	//��Ϣ����
private:
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgMatchSignup1 : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//��������
public:
	//���캯��
	CDlgMatchSignup1();
	//��������
	virtual ~CDlgMatchSignup1();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//�¼�����
public:
	//���ͱ��
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);

	//��Ϣ����
protected:
	//ѡ����
	void OnCbnSelchangeComboFromMatch();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgMatchSignup2 : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//��������
public:
	//���캯��
	CDlgMatchSignup2();
	//��������
	virtual ~CDlgMatchSignup2();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//�������
class MODULE_MANAGER_CLASS CDlgMatchDistribute : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//��������
public:
	//���캯��
	CDlgMatchDistribute();
	//��������
	virtual ~CDlgMatchDistribute();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

////////////////////////////////////////////////////////////////////////////
//
////������ʽ
//class MODULE_MANAGER_CLASS CDlgMatchRankingMode : public CDlgMatchItem
//{
//	friend class CDlgMatchWizardStep1;
//
//	//��������
//public:
//	//���캯��
//	CDlgMatchRankingMode();
//	//��������
//	virtual ~CDlgMatchRankingMode();
//
//	//���غ���
//protected:
//	//��ʼ������
//	virtual BOOL OnInitDialog();
//
//	//�ӿں���
//protected:
//	//��������
//	virtual bool SaveInputInfo();
//	//�����ؼ�
//	virtual VOID RectifyControl(INT nWidth, INT nHeight);
//
//	//�¼�����
//public:
//	//���ͱ��
//	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);
//};


//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgMatchReward : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//�ؼ�����
protected:
	CRewardInfoBuffer 			m_RewardInfoBuffer;				//������Ϣ	
	CRewardListControl			m_RewardListControl;			//�б�ؼ�

	//��������
public:
	//���캯��
	CDlgMatchReward();
	//��������
	virtual ~CDlgMatchReward();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//��������
protected:
	//��տؼ�
	VOID CleanControl();
	//��������
	VOID SetMatchRewardItem(tagMatchRewardItem * pMatchRewardItem);
	//��ȡ����
	bool GetMatchRewardItem(tagMatchRewardItem & MatchRewardItem);	

	//��Ϣ����
public:
	//��ӽ���
	VOID OnBnClickedAddReward();
	//�޸Ľ���
	VOID OnBnClickedModifyReward();
	//ɾ������
	VOID OnBnClickedDeleteReward();

	//�ؼ���Ϣ
protected:
	//������
	VOID OnLvnItemchangedListReward(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//��ʱ����
class MODULE_MANAGER_CLASS CDlgMatchLockTime : public CDlgMatchItem
{
	//�ؼ�����
protected:
	CCheckComboBox						m_ComboWeekDay;					//��Ͽؼ�
	CCheckComboBox						m_ComboMonthDay;				//��Ͽؼ�

	//��������
public:
	//���캯��
	CDlgMatchLockTime();
	//��������
	virtual ~CDlgMatchLockTime();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//��������
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//��������
protected:
	//����ʱ��
	VOID GetMatchTime(SYSTEMTIME &MatchTime,UINT uHourID,UINT uMinuteID,UINT uSecondID);
	//����ʱ��
	VOID SetMatchTime(const SYSTEMTIME &MatchTime,UINT uHourID,UINT uMinuteID,UINT uSecondID);	

	//��������
protected:
	//�����б�
	VOID SetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount);
	//��ȡ�б�
	VOID GetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount);
};

//////////////////////////////////////////////////////////////////////////

//��ʱ����
class MODULE_MANAGER_CLASS CDlgMatchImmediate : public CDlgMatchItem
{
	//��������
public:
	//���캯��
	CDlgMatchImmediate();
	//��������
	virtual ~CDlgMatchImmediate();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//����������
class MODULE_MANAGER_CLASS CDlgMatchTournament : public CDlgMatchItem
{
	//�ؼ�����
protected:
	CCheckComboBox						m_ComboWeekDay;					//��Ͽؼ�
	CCheckComboBox						m_ComboMonthDay;				//��Ͽؼ�
	CCheckComboBox						m_ComboYearMonth;				//��Ͽؼ�

	//��������
public:
	//���캯��
	CDlgMatchTournament();
	//��������
	virtual ~CDlgMatchTournament();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//��������
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//��������
protected:
	//����ʱ��
	VOID GetMatchTime(SYSTEMTIME& MatchTime, UINT uHourID, UINT uMinuteID, UINT uSecondID);
	//����ʱ��
	VOID SetMatchTime(const SYSTEMTIME& MatchTime, UINT uHourID, UINT uMinuteID, UINT uSecondID);

	//��������
protected:
	//�����б�
	VOID SetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount);
	//��ȡ�б�
	VOID GetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//�򵼻�����
class MODULE_MANAGER_CLASS CDlgMatchWizardItem : public CDialog
{
	//��Ԫ����
	friend class CDlgMatchWizard;

	//״̬����
private:
	bool							m_bSaveData;						//�����־

	//��������
protected:
	CDlgMatchWizard *				m_pDlgMatchWizard;					//��ָ��
	tagMatchOption *				m_pMatchOption;						//��������
	CMatchInfoBuffer *				m_pMatchInfoBuffer;					//������Ϣ	
	CModuleInfoBuffer *				m_pModuleInfoBuffer;				//ģ����Ϣ		 

	//��������
protected:
	//���캯��
	CDlgMatchWizardItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgMatchWizardItem();

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
	//��������
	virtual VOID OnMatchItemActive()=NULL;
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

//������
class MODULE_MANAGER_CLASS CDlgMatchWizardStep1 : public CDlgMatchWizardItem
{
	//�ؼ�����
protected:
	CComboBox						m_ModuleComboBox;				//ģ���б�

	//�ؼ�����
protected:
	BYTE							m_cbItemIndex;					//��������
	CDlgMatchItem *					m_pDlgMatchItem[4];				//��������
	CDlgMatchReward					m_DlgMatchReward;				//��������
	CDlgMatchSignup1				m_DlgMatchSignup1;				//��������
	CDlgMatchSignup2				m_DlgMatchSignup2;				//��������
	CDlgMatchDistribute				m_DlgMatchDistribute;			//��������	
	//CDlgMatchRankingMode			m_DlgMatchRankingMode;			//��������

	//��������
public:
	//���캯��
	CDlgMatchWizardStep1();
	//��������
	virtual ~CDlgMatchWizardStep1();

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
	//��������
	virtual VOID OnMatchItemActive();
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
protected:
	//ѡ����
	void OnCbnSelchangeComboMatchType();	
	//ѡ����
	void OnTcnSelchangeMatchOption(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//������
class MODULE_MANAGER_CLASS CDlgMatchWizardStep2 : public CDlgMatchWizardItem
{
	//�ؼ�����
protected:
	CRoundInfoBuffer 			m_RoundInfoBuffer;				//�ִ���Ϣ	
	CRoundListControl			m_RoundListControl;				//�б�ؼ�

	//�ؼ�����
protected:
	BYTE						m_cbItemIndex;					//��������
	tagMatchRoundItem			m_MatchRoundItem;				//�ִ�����	
	CDlgPromoteOutGame			m_PromoteOutGame;				//��������
	CDlgPromoteFixedGame		m_PromoteFixedGame;				//���ֻ���
	CDlgPromoteFixedTable		m_PromoteFixedTable;			//��������
	CDlgPromoteModeItem	*		m_pPromoteModeItem[3];			//�ؼ�����

	//��������
public:
	//���캯��
	CDlgMatchWizardStep2();
	//��������
	virtual ~CDlgMatchWizardStep2();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();

	//��������
protected:
	//����ؼ�
	VOID CleanControl();
	//��ʾ����
	VOID ShowPromoteItem(BYTE cbItemIndex);
	//��������
	VOID SetMatchRoundItem(tagMatchRoundItem * pMatchRoundItem);
	//��ȡ����
	bool GetMatchRoundItem(tagMatchRoundItem & MatchRoundItem);	

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����¼�
	virtual VOID OnMatchItemActive();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);	

	//��Ϣ����
public:
	//����ִ�
	VOID OnBnClickedAddRound();
	//�޸��ִ�
	VOID OnBnClickedModifyRound();
	//�޸��ִ�
	VOID OnBnClickedDeleteRound();

	//�ؼ���Ϣ
protected:
	//ѡ����
	void OnCbnSelchangeComboPromoteMode();
	//������
	VOID OnLvnItemchangedListRound(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//������
class MODULE_MANAGER_CLASS CDlgMatchWizardStep3 : public CDlgMatchWizardItem
{
	//��������
protected:
	CDlgMatchItem *				m_pDlgMatchItem;				//��������
	CDlgMatchLockTime			m_DlgMatchLockTime;				//��ʱ��
	CDlgMatchImmediate			m_DlgMatchImmediate;			//��ʱ��
	CDlgMatchTournament			m_DlgMatchTournament;			//������

	//��������
public:
	//���캯��
	CDlgMatchWizardStep3();
	//��������
	virtual ~CDlgMatchWizardStep3();

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
	//�����¼�
	virtual VOID OnMatchItemActive();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);	
};

//////////////////////////////////////////////////////////////////////////

//������
class MODULE_MANAGER_CLASS CDlgMatchWizard : public CDialog
{
	//��Ԫ����
	friend class CDlgMatchWizardStep1;
	friend class CDlgMatchWizardStep2;
	friend class CDlgMatchWizardStep3;

	//��������
protected:
	WORD							m_wActiveIndex;						//�����
	CDlgMatchWizardItem *			m_pWizardItem[3];					//��ָ��

	//���ñ���
protected:	
	tagMatchOption					m_MatchOption;						//��������		
	CMatchInfoBuffer				m_MatchInfoBuffer;					//ģ����Ϣ
	CModuleInfoBuffer				m_ModuleInfoBuffer;					//ģ����Ϣ		

	//�ؼ�����
protected:
	CDlgMatchWizardStep1			m_MatchWizardStep1;					//���ò���
	CDlgMatchWizardStep2			m_MatchWizardStep2;					//���ò���
	CDlgMatchWizardStep3			m_MatchWizardStep3;					//���ò���

	//��������
public:
	//���캯��
	CDlgMatchWizard();
	//��������
	virtual ~CDlgMatchWizard();

	//���غ���
public:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();

	//���ú���
public:	
	//��ȡ����
	VOID GetMatchOption(tagMatchOption & MatchOption);	
	//��������
	VOID SetMatchOption(tagMatchOption & MatchOption);	

	//�ڲ�����
private:	
	//������
	bool ActiveWizardItem(WORD wIndex);		

	//��Ϣӳ��
public:
	//��һ��
	VOID OnBnClickedLast();
	//��һ��
	VOID OnBnClickedNext();
	//��ɰ�ť
	VOID OnBnClickedFinish();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif