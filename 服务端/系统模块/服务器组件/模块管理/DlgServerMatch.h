#ifndef DLG_SERVER_MATCH_HEAD_FILE
#define DLG_SERVER_MATCH_HEAD_FILE

#pragma once

//�����ļ�
#include "MatchListControl.h"
#include "MatchInfoManager.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

//���Ի���
class MODULE_MANAGER_CLASS CDlgServerMatch : public CDialog
{
	//�б����
protected:
	CMatchInfoBuffer				m_MatchInfoBuffer;						//������Ϣ
	CMatchListControl				m_MatchListControl;						//�����б�

	//���ñ���
public:	
	tagMatchOption				m_MatchOption;						//��������

	//��������
public:
	//���캯��
	CDlgServerMatch();
	//��������
	virtual ~CDlgServerMatch();

	//�̳к���
protected:
	//��ʼ����
	virtual BOOL OnInitDialog();
	//���ݽ���
	virtual void DoDataExchange(CDataExchange* pDX);

	//���ú���
public:
	//�򿪱���
	bool OpenGameMatch();

	//���ܺ���
protected:
	//���ر���
	bool LoadDBMatchItem();

	//��ť��Ϣ
protected:	
	//���ñ���
	VOID OnBnClickedOptionMatch();
	//ɾ������
	VOID OnBnClickedDeleteMatch();
	//��������
	VOID OnBnClickedCreateMatch();

	//�ؼ���Ϣ
protected:
	//˫���б�
	VOID OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult);
	//ѡ��ı�
	VOID OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////////////////////
#endif