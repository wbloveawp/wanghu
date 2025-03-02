#ifndef DLG_SERVER_LIST_HEAD_FILE
#define DLG_SERVER_LIST_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//���Ի���
class CDlgServerList : public CDialog
{
	//�б����
protected:
	CServerInfoBuffer				m_ServerInfoBuffer;					//ģ����Ϣ
	CServerListControl				m_ServerListControl;				//�����б�

	//���ñ���
public:
	bool							m_bOptionSuccess;					//���ñ�־
	tagModuleInitParameter			m_ModuleInitParameter;				//���ò���

	//��������
public:
	//���캯��
	CDlgServerList();
	//��������
	virtual ~CDlgServerList();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ����Ϣ
	virtual VOID OnOK();

	//���ܺ���
protected:
	//���ط���
	bool LoadDBServerItem();
	//��ʼ������
	bool InitGameServer(tagGameServerInfo *pGameServerInfo);

	//���ƺ���
public:
	//����ָ������
	bool LoadGameServer(WORD wServerID);

	//��ť��Ϣ
protected:
	//���ط���
	VOID OnBnClickedLoadServer();
	//ˢ���б�
	VOID OnBnClickedReLoadList();
	//ɾ������
	VOID OnBnClickedDeleteServer();
	//�������
	VOID OnBnClickedServiceAttrib();

	//�ؼ���Ϣ
protected:
	//˫���б�
	VOID OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult);
	//ѡ��ı�
	VOID OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif