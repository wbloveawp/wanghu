#ifndef DLG_AI_MANAGER_HEAD_FILE
#define DLG_AI_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "MissionAndroid.h"
#include "DlgManagerOption.h"

//////////////////////////////////////////////////////////////////////////////////
//�������
class CDlgAIManager : public CDlgOptionItem, public IAndroidOperateCenter
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
	CButton							m_btAddItem;						//�������
	CButton							m_btModifyItem;						//�޸�����
	CButton							m_btDeleteItem;						//ɾ������
	CCheckComboBox					m_ComboServiceMode;					//����ģʽ

	//�������
protected:
	CMissionAndroid					m_MissionAndroid;					//��������
	CMissionManager					m_MissionManager;					//�������

	//�ؼ�����
protected:
	CCheckComboBox *				m_pServerList;						//�����б�

	//�б�����
protected:
	CListCtrl						m_ParemeterList;					//�����б�

	//��������
protected:
	tagPCGameServer					m_GameServer;						//������Ϣ


	//��������
public:
	//���캯��
	CDlgAIManager();
	//��������
	virtual ~CDlgAIManager();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//���ú���
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();
	//���ٴ���
	virtual VOID OnDestory();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//�¼�����
protected:
	//�������
	VOID OnBnClickedBtAdd();
	//�༭����
	VOID OnBnClickedBtModify();
	//ɾ������
	VOID OnBnClickedBtDelete();
	//��ѯ����
	VOID OnBnClickedBtQuery();
	//��������
	VOID OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	//ɾ������
	VOID OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	//���̰���
	VOID OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult);

	//���غ���
protected:
	//�滭��Ϣ
	virtual VOID OnDrawClientAreaBegin(CDC * pDC, INT nWidth, INT nHeight);

	//�����ӿ�
public:
	//�ؼ�ʹ��
	virtual VOID OnEventEnableControls(BOOL bEnabled);
	//��������
	virtual VOID OnEventAndroidParenter(WORD wSubCommdID, WORD wParameterCount, tagRobotParameter* pAndroidParameter);

	//���ú���
public:
	//�ؼ�ʹ��
	virtual VOID EnableControls(BOOL bEnabled);
	//��������
	virtual WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize);

	//���ܺ���
public:
	//���ù���
	VOID Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem, CCheckComboBox * pCheckComboBox, tagPCGameServer GameServer);
	//��������
	VOID SetServerType(WORD wServerType){m_wServerType=wServerType;}
	//��ʼ���ؼ�
	VOID InitControls();
	//����޸�
	VOID AddModifyParameter(bool bModify);
	//ѡ������
	VOID SelectItem(int nIndex);
	//��������
	VOID SetItemToParameterList(int nItemIndex, tagRobotParameter* pAndroidParameter);

	//��Ϣӳ��
protected:
	//�ؼ���ɫ
	//HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif