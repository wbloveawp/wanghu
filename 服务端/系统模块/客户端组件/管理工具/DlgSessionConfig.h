#ifndef DLG_SESSION_CONFIG_HEAD_FILE
#define DLG_SESSION_CONFIG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "DlgSessionItem.h"
#include "CustomListCtrl.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ự
class CDlgSessionConfig : public CDlgSessionItem
{
	//��������
protected:
	WORD							m_wKindID;							//���ͱ�ʶ
	CListCtrl*						m_pSelListCtrl;						//ѡ��ؼ�
	int								m_nSelWeightItem;					//ѡ������
	
	//�ؼ�����
protected:
	CButton							m_btQuery;							//��ѯ��ť
	CButton							m_btUpdateBasicConfig;				//���水ť
	CButton							m_btSaveNormalWeight;				//���水ť
	CButton							m_btSaveWhiteListWeight;			//���水ť
	CComboBox						m_cbGameList;						//��Ϸ�б�
	CComboBox						m_cbWeightType;						//Ȩ������

	//�б�����
protected:
	CCustomListCtrl					m_ListNormalWeight;					//Ȩ���б�
	CCustomListCtrl					m_ListWhiteListWeight;				//Ȩ���б�

	//��������
public:
	//���캯��
	CDlgSessionConfig();
	//��������
	virtual ~CDlgSessionConfig();

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
	//��ѯ���
	VOID OnBnClickedBtQuery();
	//��ӿ��
	VOID OnBnClickedBtAppend();
	//�༭���
	VOID OnBnClickedBtModify();
	//ɾ�����
	VOID OnBnClickedBtDelete();	
	//��������
	VOID OnBnClickedBtUpdateBasicConfig();
	//����Ȩ��
	VOID OnBnClickedBtUpdateNormalWeight();
	//����Ȩ��
	VOID OnBnClickedBtUpdateWhiteListWeight();

	//�б��¼�
protected:
	//��������
	VOID OnHdnListClickWeightItem(NMHDR *pNMHDR, LRESULT *pResult);
	//ɾ������
	VOID OnLvnListDeleteWeightItem(NMHDR *pNMHDR, LRESULT *pResult);
	//���̰���
	VOID OnLvnListKeydownWeightItem(NMHDR* pNMHDR, LRESULT* pResult);	

	//���ܺ���
public:
	//��ʼ���ؼ�
	VOID InitControls();
	//�ؼ�ʹ��
	VOID EnableControls(BOOL bEnabled);

	//��������
public:
	//�޸���Ϣ
	VOID AddModifyWightItem(bool bModify);	
	//��ʽ������
	bool FormatJsonConfig(LPCTSTR pszBasicConfig, TCHAR szFormatConfig[], WORD wBuffSize);
	//ѹ������
	bool CompressJsonConfig(LPCTSTR pszFormatConfig, TCHAR szBasicConfig[], WORD wBuffSize);

	//��������
public:
	//ѡ����
	VOID SelectWeightItem(CListCtrl* pListCtrl,int nIndex);	
	//�ռ�Ȩ��
	int CollectWeightItem(CListCtrl* pListCtrl, int nBuffCount, tagTimesWeightItem TimesWeightItem[]);
	//��������
	VOID SetWeightItemToList(CListCtrl* pListCtrl, int nItemIndex, tagTimesWeightItem* pTimesWeightItem);

	//��������
public:
	//��ѯ���
	bool QueryGameConfig(WORD wKindID);
	//�����������
	bool UpdateBasicConfig(WORD wKindID, LPCTSTR pszBasiceConfig);
	//����Ȩ�ر�
	bool UpdateWeightTable(WORD wKindID, tagTimesWeightItem TimesWeightItem[], WORD wItemCount);	

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif