#ifndef CUSTOM_LISTCTRL_HEAD_FILE
#define CUSTOM_LISTCTRL_HEAD_FILE

#pragma once

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//��������
enum enCustomSortType
{
	CST_NoSort,					//û������
	CST_Ascend,					//��������
	CST_Descale,				//��������
};

//////////////////////////////////////////////////////////////////////////

//�б�����
class CListCtrlAttribute
{
	//��Ԫ����
	friend class CCustomListCtrl;
	friend class CCustomHeaderCtrl;

	//�б�����
public:
	WORD							m_wItemHeight;						//����߶�

	//��ɫ����
public:
	COLORREF						m_crTitle;							//������ɫ
	COLORREF						m_crNormalTX;						//�б�����
	COLORREF						m_crNormalBK;						//�б��ɫ
	COLORREF						m_crSelectedTX;						//�б�����
	COLORREF						m_crSelectedBK;						//�б��ɫ

	//��������
protected:
	//���캯��
	CListCtrlAttribute();
	//��������
	virtual ~CListCtrlAttribute();
};

//////////////////////////////////////////////////////////////////////////

//��ͷ�ؼ�
class CCustomHeaderCtrl : public CHeaderCtrl
{
	//�������
protected:
	INT								m_nColumn;							//��ǰ����
	enCustomSortType				m_SortType;							//��������

	//��������
public:
	//���캯��
	CCustomHeaderCtrl();
	//��������
	virtual ~CCustomHeaderCtrl();

	//���ܺ���
public:
	//���ñ�־
	VOID SetSortArrow(INT nSortColumn, enCustomSortType SortType);
	//��ȡ��־
	VOID GetSortArrow(INT & nSortColumn, enCustomSortType& SortType);

	//��Ϣ����
protected:
	//�ػ���Ϣ
	VOID OnPaint();
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//�б���
class CCustomListCtrl : public CListCtrl
{
	//��������
protected:
	CCustomHeaderCtrl					m_CustomHeadCtrl;					//��ͷ�ؼ�
	
	//���Ա���
public:
	CListCtrlAttribute					m_ListCtrlAttribute;				//�б�����

	//��������
public:
	//���캯��
	CCustomListCtrl();
	//��������
	virtual ~CCustomListCtrl();

	//���غ���
public:
	//�ؼ���
	virtual VOID PreSubclassWindow();	
	//�滭����
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//���ú���
public:
	//�б�����
	CListCtrlAttribute* GetListCtrlAttribute() { return &m_ListCtrlAttribute; }

	//���ܺ���
public:
	//��ȡ��ͷ
	CCustomHeaderCtrl & GetHeadCtrl() { return m_CustomHeadCtrl; }
	//��ȡ��־
	VOID GetSortArrow(INT & nSortCol, enCustomSortType& SortType) { m_CustomHeadCtrl.GetSortArrow(nSortCol,SortType); };
	//���ñ�־
	VOID SetSortArrow(INT nSortCol, enCustomSortType SortType) { return m_CustomHeadCtrl.SetSortArrow(nSortCol,SortType); }

	//��Ϣ����
protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);

	//��Ϣ��Ӧ
protected:
	//�滭����
	afx_msg VOID MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);


	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif