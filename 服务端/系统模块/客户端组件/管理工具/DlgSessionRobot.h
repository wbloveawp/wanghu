#ifndef DLG_ROBOT_MANAGER_HEAD_FILE
#define DLG_ROBOT_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "CustomListCtrl.h"
#include "DlgSessionItem.h"

//////////////////////////////////////////////////////////////////////////////////
//�����Ự
class CDlgSessionRobot : public CDlgSessionItem
{
	//��������
protected:
	WORD							m_wServerType;						//��������

	//��Դ����
protected:
	CPen							m_BoradPen;							//��Դ����
	CBrush							m_FrameBrush;						//��Դ����

	//��������
protected:
	ITCPSocket *					m_pITCPSocket;						//����ӿ�
	IClientUserItem *				m_pIMySelfUserItem;					//�Լ�ָ��
	int								m_nSelectItem;						//ѡ������

	//�ؼ�����
protected:
	CButton							m_btQuery;							//��ѯ��ť
	CButton							m_btAppendItem;						//�������
	CButton							m_btModifyItem;						//�޸�����
	CButton							m_btDeleteItem;						//ɾ������
	CCheckComboBox					m_ComboServiceMode;					//����ģʽ

	//�ؼ�����
protected:
	CCheckComboBox					m_cbGameList;						//��Ϸ�б�
	CComboBox						m_cbServerList;						//�����б�	

	//�б�����
protected:
	CCustomListCtrl					m_ParemeterList;					//�����б�

	//��������
protected:
	tagPCGameServer					m_GameServer;						//������Ϣ


	//��������
public:
	//���캯��
	CDlgSessionRobot();
	//��������
	virtual ~CDlgSessionRobot();

	//���غ���
protected:	
	//ȷ������
	virtual VOID OnOK();
	//���ٴ���
	virtual VOID OnDestory();
	//���ú���
	virtual BOOL OnInitDialog();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//�ؼ���
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

	//��ť�¼�
protected:
	//��ѯ����
	VOID OnBnClickedBtQuery();
	//�������
	VOID OnBnClickedBtAppend();
	//�༭����
	VOID OnBnClickedBtModify();
	//ɾ������
	VOID OnBnClickedBtDelete();	

	//�б��¼�
protected:
	//���̰���
	VOID OnLvnKeydownListParameter(NMHDR* pNMHDR, LRESULT* pResult);
	//��������
	VOID OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	//ɾ������
	VOID OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);	

	//���غ���
protected:
	//�滭��Ϣ
	virtual VOID OnDrawClientAreaBegin(CDC * pDC, INT nWidth, INT nHeight);

	//���ú���
public:
	//��������
	VOID SetServerType(WORD wServerType) { m_wServerType = wServerType; }
	//���ù���
	VOID Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem, tagPCGameServer GameServer);	

	//���ܺ���
public:
	//��ʼ���ؼ�
	VOID InitControls();
	//ѡ������
	VOID SelectItem(int nIndex);
	//�ؼ�ʹ��
	VOID EnableControls(BOOL bEnabled);
	//����޸�
	VOID AddModifyParameter(bool bModify);
	//��������
	VOID SetItemToParameterList(int nItemIndex, tagRobotParameter* pRobotParameter);

	//��������
public:
	//��ѯ����
	bool QueryRobotParameter(WORD wServerID);
	//ɾ������
	bool DeleteRobotParameter(WORD wServerID, DWORD dwBatchID);
	//�޸Ĳ���
	bool ModifyRobotParameter(WORD wServerID, tagRobotParameter* pRobotParameter);
	//��Ӳ���
	bool AppendRobotParameter(WORD wServerID, tagRobotParameter* pRobotParameter);

	//��Ϣӳ��
protected:
	//�ؼ���ɫ
	//HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif