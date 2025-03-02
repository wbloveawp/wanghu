#ifndef BIT_IMAGE_HEAD_FILE
#define BIT_IMAGE_HEAD_FILE

#pragma once

#include "ImageAideHead.h"

//////////////////////////////////////////////////////////////////////////

//�豸λͼ
class IMAGE_AIDE_CLASS CDibImage
{
	//�洢����
protected:
	DWORD							m_dwOffset;							//�洢ƫ��
	HANDLE							m_hSection;							//�洢���

	//���ݱ���
protected:
	INT								m_nPitch;							//ͼ�г���
	LPVOID							m_pBitBuffer;						//����ָ��

	//�ں˱���
protected:
	HBITMAP							m_hBitmap;							//λͼ���
	BITMAPINFO						m_BitmapInfo;						//ͼ����Ϣ

	//��������
public:
	//���캯��
	CDibImage();
	//��������
	virtual ~CDibImage();

	//���ܺ���
public:
	//��������
	bool Create(INT nWidth, INT nHeight, INT nBPP);
};

//////////////////////////////////////////////////////////////////////////

#endif