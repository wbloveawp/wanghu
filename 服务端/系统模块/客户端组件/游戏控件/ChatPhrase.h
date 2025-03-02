#ifndef CHAT_PHRASE_HEAD_FILE
#define CHAT_PHRASE_HEAD_FILE

#pragma once

#include "ClientControlsHead.h"

//////////////////////////////////////////////////////////////////////////

//�����б�
class CChatPhraseList : public CSkinListCtrl
{
	//״̬����
protected:
	bool							m_bAscendSort;						//�����־

	//��Դ����
protected:
	static CImageList				m_PhraseFlagImage;					//��־λͼ

	//��������
public:
	//���캯��
	CChatPhraseList();
	//��������
	virtual ~CChatPhraseList();

	//���ú���
public:
	//�����б�
	bool InitListControl();

	//�ڲ�����
protected:
	//���к���
	static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	//��Ϣӳ��
protected:
	//������Ϣ
	VOID OnNcDestroy();
	//����б�
	VOID OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//���춯��
class CLIENT_CONTROLS_CLASS CChatPhrase : public CSkinDialog
{
	//�ؼ�����
protected:
	CSkinButton						m_btSelect;							//ѡ��ť
	CSkinButton						m_btCancel;							//ȡ����ť
	CChatPhraseList					m_ChatPhraseList;					//�����б�

	//��������
public:
	//���캯��
	CChatPhrase();
	//��������
	virtual ~CChatPhrase();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK() { return; }

	//���ܺ���
protected:
	//�����ؼ�
	VOID RectifyControl(INT nWidth, INT nHeight);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif