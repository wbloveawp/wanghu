#include "StdAfx.h"
#include "DibImage.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CDibImage::CDibImage()
{
	//�洢����
	m_dwOffset=0L;
	m_hSection=NULL;

	//���ݱ���
	m_nPitch=0L;
	m_pBitBuffer=NULL;

	//�ں˱���
	m_hBitmap=NULL;
	ZeroMemory(&m_BitmapInfo,sizeof(m_BitmapInfo));

	return;
}

//��������
CDibImage::~CDibImage()
{
}

//////////////////////////////////////////////////////////////////////////
