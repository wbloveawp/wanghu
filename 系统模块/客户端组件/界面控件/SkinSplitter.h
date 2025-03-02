#ifndef SKIN_SPLITTER_HEAD_FILE
#define SKIN_SPLITTER_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CSkinSplitter;

//�������
enum enSplitterType
{
	SplitterType_Hor,				//������
	SplitterType_Vor,				//������
};

//������ӿ�
interface ISplitterSink
{
	//�����Ϣ
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, INT nXPos, INT nYPos)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//���������
class CSkinSplitterAttribute : public CSkinResourceManager
{
	friend class CSkinSplitter;

	//λͼ��Դ
public:
	COLORREF						m_crBorad;							//�߿���ɫ
	COLORREF						m_crBackGround;						//������ɫ

	//��������
protected:
	//���캯��
	CSkinSplitterAttribute();
	//��������
	virtual ~CSkinSplitterAttribute();

	//���ܺ���
public:
	//������Դ
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//�������
class SKIN_CONTROL_CLASS CSkinSplitter : public CWnd
{
	//״̬����
protected:
	bool							m_bMouseDown;						//�Ƿ���
	CRect							m_rcDrawLine;						//����λ��

	//���Ա���
protected:
	INT								m_nMaxPos;							//���λ��
	INT								m_nLessPos;							//��Сλ��
	enSplitterType					m_SplitterType;						//�������

	//��ɫ����
protected:
	COLORREF						m_crBorad;							//�߿���ɫ
	COLORREF						m_crBackGround;						//������ɫ

	//�������
protected:
	ISplitterSink *					m_pISplitterSink;					//����¼�

	//���������
public:
	static CSkinSplitterAttribute	m_SkinAttribute;					//��������

	//��������
public:
	//���캯��
	CSkinSplitter();
	//��������
	virtual ~CSkinSplitter();

	//���غ���
protected:
	//�󶨺���
	virtual VOID PreSubclassWindow();
	//������Ϣ
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//���ú���
public:
	//���÷�Χ
	VOID SetSplitterRange(INT nLessPos, INT nMaxPos);
	//��������
	VOID SetSplitterType(enSplitterType SplitterType);
	//���ýӿ�
	VOID SetSplitterSink(ISplitterSink * pISplitterSink);
	//���ñ���
	VOID SetSplitterColor(COLORREF crBorad, COLORREF crBackGround);

	//��ѯ����
public:
	//��ȡ����
	enSplitterType GetSplitterType() { return m_SplitterType; }
	//��ȡ�ӿ�
	ISplitterSink * GetSplitterSink() { return m_pISplitterSink; };
	//���÷�Χ
	VOID GetSplitterRange(INT & nLessPos, INT & nMaxPos) { nLessPos=m_nLessPos; nMaxPos=m_nMaxPos; }

	//��Ϣ����
protected:
	//�ػ���Ϣ
	VOID OnPaint();
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//ʧȥ����
	VOID OnKillFocus(CWnd * pNewWnd);
	//�����Ϣ
	VOID OnMouseMove(UINT nFlags, CPoint point);
	//�����Ϣ
	VOID OnLButtonUp(UINT nFlags, CPoint point);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint point);
	//�����Ϣ
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMmessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif