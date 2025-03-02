#ifndef BIT_IMAGE_HEAD_FILE
#define BIT_IMAGE_HEAD_FILE

#pragma once

#include "ImageAideHead.h"

//////////////////////////////////////////////////////////////////////////

//�ṹ˵��
struct tagImageRender;

//////////////////////////////////////////////////////////////////////////

//λͼ������
class IMAGE_AIDE_CLASS CBitImage : public CImage
{
	friend class CImageHandle;

	//�ڲ�����
private:
	INT								m_nRefCount;						//���ü���
	bool							m_bLoadAlways;						//�Ƿ���
	tagImageLoadInfo				m_ImageLoadInfo;					//���ز���

	//��������
public:
	//���캯��
	CBitImage();
	//��������
	virtual ~CBitImage();

	//�ӿں���
protected:
	//��������
	INT LockResource();
	//��������
	INT UnLockResource();
	//���ü���
	INT GetReferenceCount() { return m_nRefCount; }

	//���ܺ���
public:
	//������Ϣ
	bool IsSetLoadInfo();
	//�������
	bool RemoveLoadInfo();
	//��ȡ����
	bool GetLoadInfo(tagImageLoadInfo & LoadInfo);
	//���ò���
	bool SetLoadInfo(const tagImageLoadInfo & LoadInfo);
	//���ò���
	bool SetLoadInfo(LPCTSTR pszFileName, bool bLoadAlways=true);
	//���ò���
	bool SetLoadInfo(UINT uResourceID, HINSTANCE hResourceDLL, bool bLoadAlways=false);
	//���ò���
	bool SetLoadInfo(LPCTSTR pszResource, HINSTANCE hResourceDLL, bool bLoadAlways=false);

	//���غ���
public:
	//λͼָ��
	operator CBitmap * () { return CBitmap::FromHandle((HBITMAP)(*this)); }

	//������
public:
	//��������
	bool CreateImageRegion(CRgn & ImageRgn, COLORREF crTransColor);
	//��������
	bool CreateImageRegion(CRgn & ImageRgn, INT nXSrcPos, INT nYSrcPos, INT nSrcWidth, INT nSrcHeight, COLORREF crTransColor);

	//�滭ͼƬ
public:
	//ƽ�̻滭
	bool DrawImageTile(CDC * pDC, CRect & rcDestRect);
	//ƽ�̻滭
	bool DrawImageTile(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight);

	//͸���滭
public:
	//͸���滭
	bool AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, COLORREF crTransColor);
	//͸���滭
	bool AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor);

	//��ϻ滭
public:
	//��ϻ滭
	bool BlendDrawImage(CDC * pDestDC, INT xDest, INT yDest, COLORREF crTransColor, BYTE cbAlphaDepth);
	//��ϻ滭
	bool BlendDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor, BYTE cbAlphaDepth);

	//��������
public:
	//���μ��
	bool IsRectIntersect(const CRect & rcRect, INT nXPos, INT nYPos, INT nWidth, INT nHeight);
	//�������
	bool GetDrawImageArea(INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, CRect & rcDirty, tagImageRender & ImageRender);
};

//////////////////////////////////////////////////////////////////////////

//λͼ��Դ���
class IMAGE_AIDE_CLASS CImageHandle
{
	//��������
protected:
	CBitImage *						m_pBitImage;						//λͼָ��
	
	//��������
public:
	//���캯��
	CImageHandle(CBitImage * pBitImage=NULL);
	//��������
	virtual ~CImageHandle();
	
	//�ӿں���
public:
	//�Ƿ���Ч
	bool IsValid();
	//�����
	bool DetchResource();
	//����Դ
	bool AttachResource(CBitImage * pBitImage);

	//��������
public:
	//��ȡ���
	HBITMAP GetBitmapHandle();
	//��ȡ����
	CBitImage * GetBitImage() { return m_pBitImage; }
	//ָ������
	CBitImage * operator->() { return GetBitImage(); }
};

//////////////////////////////////////////////////////////////////////////

#endif