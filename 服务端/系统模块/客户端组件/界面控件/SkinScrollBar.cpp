#include "StdAfx.h"
#include "Resource.h"
#include "SkinScrollBar.h"

//////////////////////////////////////////////////////////////////////////////////
//��̬����

CSkinScrollBarAttribute			CSkinScrollBar::m_SkinAttribute;		//��Դ����

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CSkinScrollBarAttribute::CSkinScrollBarAttribute()
{
	return;
}
	
//��������
CSkinScrollBarAttribute::~CSkinScrollBarAttribute()
{
	//������Դ
	if (m_ImageScroll.IsNull()==false)
	{
		m_ImageScroll.Destroy();
	}

	return;
}

//������Դ
bool CSkinScrollBarAttribute::Initialization(HINSTANCE hInstance, UINT nIDResource)
{
	//�ͷ���Դ
	if(m_ImageScroll.IsNull()==false) m_ImageScroll.Destroy();

	//������Դ
	m_ImageScroll.LoadFromResource(hInstance,nIDResource);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CSkinScrollBar::CSkinScrollBar()
{
}

//��������
CSkinScrollBar::~CSkinScrollBar()
{
}

//���ù���
VOID CSkinScrollBar::InitScroolBar(CWnd * pWnd)
{
	//����У��
	ASSERT(pWnd!=NULL);
	if(pWnd==NULL) return;

	//���ù���
	//SkinSB_Init(pWnd->GetSafeHwnd(),m_SkinAttribute.m_ImageScroll);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
