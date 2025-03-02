#ifndef SKIN_LISTCTRL_HEAD_FILE
#define SKIN_LISTCTRL_HEAD_FILE

#pragma once

#include "SkinScrollBar.h"
#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//��������
enum enSortType
{
	SortType_NoSort,				//û������
	SortType_Ascend,				//��������
	SortType_Descale,				//��������
};

//////////////////////////////////////////////////////////////////////////

//�б�����
class CSkinListCtrlAttribute : public CSkinResourceManager
{
	//��Ԫ����
	friend class CSkinListCtrl;
	friend class CSkinHeaderCtrl;

	//��ɫ����
public:
	COLORREF						m_crTitle;							//������ɫ
	COLORREF						m_crListTX;							//�б�����
	COLORREF						m_crListBK;							//�б��ɫ

	//��Դ����
public:
	CBitImage						m_SkinImageL;						//����λͼ
	CBitImage						m_SkinImageM;						//����λͼ
	CBitImage						m_SkinImageR;						//����λͼ

	//��������
protected:
	//���캯��
	CSkinListCtrlAttribute();
	//��������
	virtual ~CSkinListCtrlAttribute();

	//���ܺ���
public:
	//������Դ
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//��ͷ�ؼ�
class SKIN_CONTROL_CLASS CSkinHeaderCtrl : public CHeaderCtrl
{
	//�������
protected:
	INT								m_nColumn;							//��ǰ����
	enSortType						m_SortType;							//��������

	//��������
public:
	//���캯��
	CSkinHeaderCtrl();
	//��������
	virtual ~CSkinHeaderCtrl();

	//���ܺ���
public:
	//���ñ�־
	VOID SetSortArrow(INT nSortColumn, enSortType SortType);
	//��ȡ��־
	VOID GetSortArrow(INT & nSortColumn, enSortType & SortType);

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
class SKIN_CONTROL_CLASS CSkinListCtrl : public CListCtrl
{
	//��������
protected:
	CSkinScrollBar					m_SkinScrollBar;					//��������
	CSkinHeaderCtrl					m_SkinHeadCtrl;						//��ͷ�ؼ�
	
	//���Ա���
public:
	static CSkinListCtrlAttribute	m_SkinAttribute;					//�б�����

	//��������
public:
	//���캯��
	CSkinListCtrl();
	//��������
	virtual ~CSkinListCtrl();

	//���غ���
public:
	//�ؼ���
	virtual VOID PreSubclassWindow();

	//���ܺ���
public:
	//��ȡ��ͷ
	CSkinHeaderCtrl & GetHeadCtrl() { return m_SkinHeadCtrl; }
	//��ȡ��־
	VOID GetSortArrow(INT & nSortCol, enSortType & SortType) { m_SkinHeadCtrl.GetSortArrow(nSortCol,SortType); };
	//���ñ�־
	VOID SetSortArrow(INT nSortCol, enSortType SortType) { return m_SkinHeadCtrl.SetSortArrow(nSortCol,SortType); }

	//��Ϣ����
protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif