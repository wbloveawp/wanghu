#include "StdAfx.h"
#include "DibImage.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CDibImage::CDibImage()
{
	//存储变量
	m_dwOffset=0L;
	m_hSection=NULL;

	//数据变量
	m_nPitch=0L;
	m_pBitBuffer=NULL;

	//内核变量
	m_hBitmap=NULL;
	ZeroMemory(&m_BitmapInfo,sizeof(m_BitmapInfo));

	return;
}

//析构函数
CDibImage::~CDibImage()
{
}

//////////////////////////////////////////////////////////////////////////
