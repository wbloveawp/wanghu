#ifndef PNG_IMAGE_HEAD_FILE
#define PNG_IMAGE_HEAD_FILE

#pragma once

#include "ImageAideHead.h"

//////////////////////////////////////////////////////////////////////////
//ͼƬ����
class IMAGE_AIDE_CLASS CPngImageWarpper
{
	//��������
protected:
	Image *							m_pImage;							//ͼƬ����

	//��������
public:
	//���캯��
	CPngImageWarpper();
	//��������
	virtual ~CPngImageWarpper();

	//״̬����
public:
	//�Ƿ����
	bool IsNull();
	//��ȡ���
	INT GetWidth();
	//��ȡ�߶�
	INT GetHeight();

	//������
public:
	//����ͼƬ
	bool DestroyImage();
	//����ͼƬ
	bool LoadImage(LPCTSTR pszFileName);
	//����ͼƬ
	bool LoadImage(HINSTANCE hInstance, LPCTSTR pszResourceName);

	//�滭����
public:
	//�滭ͼ��
	bool DrawImage(CDC * pDC, INT nXPos, INT nYPos);
	//�滭ͼ��
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc);
	//�滭ͼ��
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc, INT nSrcWidth, INT nSrcHeight);

	//��������
public:
	//��ȡ����
	Image * GetImage() { return m_pImage; }
	//ָ������
	Image * operator->() { return m_pImage; }
};

//////////////////////////////////////////////////////////////////////////

#endif