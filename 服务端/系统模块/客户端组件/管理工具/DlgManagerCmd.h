#ifndef DLG_MANAGER_CMD_HEAD_FILE
#define DLG_MANAGER_CMD_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgManagerOption.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
class CDlgManagerCmd : public CDlgOptionItem
{
	//��������
protected:
	tagCommandCode					m_CommandCode;						//�������

	//��������
public:
	//���캯��
	CDlgManagerCmd();
	//��������
	virtual ~CDlgManagerCmd();

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

//////////////////////////////////////////////////////////////////////////////////

#endif