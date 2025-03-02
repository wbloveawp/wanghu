#ifndef SKIN_SCROLL_BAR_HEAD_FILE
#define SKIN_SCROLL_BAR_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////////////

//������Դ
class SKIN_CONTROL_CLASS CSkinScrollBarAttribute
{
	//��Դ����
public:
	CBitImage						m_ImageScroll;						//������Դ

	//��������
public:
	//���캯��
	CSkinScrollBarAttribute();
	//��������
	virtual ~CSkinScrollBarAttribute();

	//������
public:
	//������Դ
	bool Initialization(HINSTANCE hInstance, UINT nIDResource);
};

//////////////////////////////////////////////////////////////////////////////////

//��������
class SKIN_CONTROL_CLASS CSkinScrollBar
{
	//��Դ����
public:
	static CSkinScrollBarAttribute	m_SkinAttribute;					//��Դ����

	//��������
public:
	//���캯��
	CSkinScrollBar();
	//��������
	virtual ~CSkinScrollBar();

	//���ܺ���
public:
	//���ù���
	VOID InitScroolBar(CWnd * pWnd);
};

//////////////////////////////////////////////////////////////////////////////////

#endif