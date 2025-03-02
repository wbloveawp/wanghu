#ifndef DLG_SESSION_STOCK_HEAD_FILE
#define DLG_SESSION_STOCK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "DlgSessionItem.h"
#include "CustomListCtrl.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ự
class CDlgSessionStock : public CDlgSessionItem
{
	//��������
protected:
	int								m_nSelStockItem;					//ѡ������
	int								m_nSelJackpotItem;					//ѡ������

	//��������
protected:
	BYTE							m_cbOperateMode;					//����ģʽ
	WORD							m_wJackpotCount;					//�ʳ�����
	tagJackpotItem					m_JackpotItem[MAX_JACKPOT_COUNT];	//�ʳ���Ϣ

	//�ؼ�����
protected:
	CButton							m_btConfirm;						//ȷ�ϰ�ť
	CButton							m_btQueryStock;						//��ѯ���
	CButton							m_btAppendStock;					//��ӿ��
	CButton							m_btModifyStock;					//�޸Ŀ��
	CButton							m_btDeleteStock;					//ɾ�����
	CButton							m_btAppendJackpot;					//��Ӳʳ�
	CButton							m_btModifyJackpot;					//�޸Ĳʳ�
	CButton							m_btDeleteJackpot;					//ɾ���ʳ�	

	//�ؼ�����
protected:
	CComboBox						m_cbStockLevel;						//���ȼ�
	CCheckComboBox					m_cbGameList;						//��Ϸ�б�
	

	//�б�����
protected:
	CCustomListCtrl					m_StockList;						//����б�
	CCustomListCtrl					m_JackpotList;						//�ʳ��б�

	//��������
public:
	//���캯��
	CDlgSessionStock();
	//��������
	virtual ~CDlgSessionStock();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK();
	//���ٴ���
	virtual VOID OnDestory();
	//���ú���
	virtual BOOL OnInitDialog();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);	

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

	//��ť�¼�
protected:
	//ȷ�ϲ���
	VOID OnBnClickedBtConfirm();
	//��ѯ���
	VOID OnBnClickedBtQueryStock();
	//��ӿ��
	VOID OnBnClickedBtAppendStock();
	//�༭���
	VOID OnBnClickedBtModifyStock();
	//ɾ�����
	VOID OnBnClickedBtDeleteStock();	
	//��Ӳʳ�
	VOID OnBnClickedBtAppendJackpot();
	//�༭�ʳ�
	VOID OnBnClickedBtModifyJackpot();
	//ɾ���ʳ�
	VOID OnBnClickedBtDeleteJackpot();

	//�б��¼�
protected:
	//��������
	VOID OnHdnListClickStockItem(NMHDR *pNMHDR, LRESULT *pResult);
	//ɾ������
	VOID OnLvnListDeleteStockItem(NMHDR *pNMHDR, LRESULT *pResult);
	//���̰���
	VOID OnLvnListKeydownStockItem(NMHDR* pNMHDR, LRESULT* pResult);
	//��������
	VOID OnHdnListClickJackpotItem(NMHDR* pNMHDR, LRESULT* pResult);
	//ɾ������
	VOID OnLvnListDeleteJackpotItem(NMHDR* pNMHDR, LRESULT* pResult);
	//���̰���
	VOID OnLvnListKeydownJackpotItem(NMHDR* pNMHDR, LRESULT* pResult);

	//��������
public:
	//��С����
	float ScoreZoomout(SCORE lScore);
	//�Ŵ����
	SCORE ScoreZoomin(LPCSTR pszScore);

	//���ܺ���
public:
	//��ʼ���ؼ�
	VOID InitControls();
	//����״̬
	VOID UpdateControlState();
	//�ؼ�ʹ��
	VOID EnableControls(BOOL bEnabled);
	//ѡ����
	VOID SelectStockItem(int nIndex);
	//ѡ��ʳ�
	VOID SelectJackpotItem(int nIndex);
	//�޸���Ϣ
	VOID AddModifyJackpotInfo(bool bModify);
	//��������
	VOID SetStockItemToList(int nItemIndex, tagStockInfo* pStockInfo);
	//��������
	VOID SetJackpotItemToList(int nItemIndex, tagJackpotItem* pJackpotItem);	

	//��������
public:
	//ɾ�����
	bool DeleteStockInfo(WORD wStockID);
	//��ѯ���
	bool QueryStockInfo(LPCTSTR pszKindIDList);
	//�޸Ŀ��
	bool ModifyStockInfo(tagStockInfo* pStockInfo);
	//��ӿ��
	bool AppendStockInfo(tagStockInfo* pStockInfo);
	//�������
	bool AdjustStockScore(WORD wStockID, SCORE lChangeScore);
	//�����ʽ�
	bool AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif