#ifndef DLG_ROOM_OPTION_HEAD_FILE
#define DLG_ROOM_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgManagerOption.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
class CDlgRoomOption : public CDlgOptionItem
{
	//��������
protected:
	tagModifyRule					m_ModifyRule;						//�޸Ĺ���	

	//��������
public:
	//���캯��
	CDlgRoomOption();
	//��������
	virtual ~CDlgRoomOption();

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
	virtual WORD ConstructManagePacket(BYTE cbBuffer[], WORD wBufferSize);

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

//////////////////////////////////////////////////////////////////////////////////

#endif