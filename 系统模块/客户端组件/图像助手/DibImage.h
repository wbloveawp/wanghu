#ifndef BIT_IMAGE_HEAD_FILE
#define BIT_IMAGE_HEAD_FILE

#pragma once

#include "ImageAideHead.h"

//////////////////////////////////////////////////////////////////////////

//设备位图
class IMAGE_AIDE_CLASS CDibImage
{
	//存储变量
protected:
	DWORD							m_dwOffset;							//存储偏移
	HANDLE							m_hSection;							//存储句柄

	//数据变量
protected:
	INT								m_nPitch;							//图行长度
	LPVOID							m_pBitBuffer;						//数据指针

	//内核变量
protected:
	HBITMAP							m_hBitmap;							//位图句柄
	BITMAPINFO						m_BitmapInfo;						//图形信息

	//函数定义
public:
	//构造函数
	CDibImage();
	//析构函数
	virtual ~CDibImage();

	//功能函数
public:
	//创建函数
	bool Create(INT nWidth, INT nHeight, INT nBPP);
};

//////////////////////////////////////////////////////////////////////////

#endif