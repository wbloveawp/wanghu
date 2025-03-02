#include "StdAfx.h"
#include "BitImage.h"

/////////////////////////////////////////////////////////////////////////////////////////

//��Ⱦ��Ϣ
struct tagImageRender
{
	INT								nXImage;										//ͼƬλ��
	INT								nYImage;										//ͼƬλ��
	INT								nXScreen;										//��Ⱦλ��
	INT								nYScreen;										//��Ⱦλ��
	INT								cxRender;										//��Ⱦ�ߴ�
	INT								cyRender;										//��Ⱦ�ߴ�
};

/////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CBitImage::CBitImage()
{
	//���ñ���
	m_nRefCount=0;
	m_bLoadAlways=false;

	//������Ϣ
	m_ImageLoadInfo.pszResource=NULL;
	m_ImageLoadInfo.hResourceDLL=NULL;

	return;
}

//��������
CBitImage::~CBitImage()
{
	Destroy();
}

//��������
INT CBitImage::LockResource()
{
	if (m_nRefCount==0)
	{
		if (m_ImageLoadInfo.hResourceDLL==NULL) Load(m_ImageLoadInfo.strFileName);
		else LoadFromResource(m_ImageLoadInfo.hResourceDLL,m_ImageLoadInfo.pszResource);
		if (IsNull()==true) return m_nRefCount;
	}

	return ++m_nRefCount;
}

//��������
INT CBitImage::UnLockResource()
{
	//�ͷŰ�
	ASSERT(m_nRefCount>0);
	if ((--m_nRefCount)==0) Destroy();

	return m_nRefCount;
}

//�Ƿ����
bool CBitImage::IsSetLoadInfo()
{
	return ((m_ImageLoadInfo.hResourceDLL)||(m_ImageLoadInfo.strFileName.IsEmpty()==false));
}

//�������
bool CBitImage::RemoveLoadInfo()
{
	//���ñ���
	m_ImageLoadInfo.pszResource=NULL;
	m_ImageLoadInfo.hResourceDLL=NULL;
	m_ImageLoadInfo.strFileName.Empty();

	return true;
}

//��ȡ����
bool CBitImage::GetLoadInfo(tagImageLoadInfo & LoadInfo)
{
	LoadInfo=m_ImageLoadInfo;
	return IsSetLoadInfo();
}

//���ò���
bool CBitImage::SetLoadInfo(LPCTSTR pszFileName, bool bLoadAlways)
{
	//�������
	INT nTempRefCount=m_nRefCount;
	bool bTempLoadAlways=m_bLoadAlways;

	//�ͷ���Դ
	m_nRefCount=0;
	m_bLoadAlways=bLoadAlways;
	if ((nTempRefCount>0)||(bTempLoadAlways==true)) Destroy();

	//������Ϣ
	m_ImageLoadInfo.pszResource=NULL;
	m_ImageLoadInfo.hResourceDLL=NULL;
	m_ImageLoadInfo.strFileName=pszFileName;

	//��������
	if (m_bLoadAlways==true) ++nTempRefCount;
	if (bTempLoadAlways==true) --nTempRefCount;

	//�ָ�����
	if ((nTempRefCount>0)&&(LockResource()>0)) m_nRefCount=nTempRefCount;

	return true;
}

//���ò���
bool CBitImage::SetLoadInfo(const tagImageLoadInfo & LoadInfo)
{
	//�������
	INT nTempRefCount=m_nRefCount;
	bool bTempLoadAlways=m_bLoadAlways;

	//�ͷ���Դ
	m_nRefCount=0;
	m_bLoadAlways=(LoadInfo.hResourceDLL==NULL);
	if ((nTempRefCount>0)||(bTempLoadAlways==true)) Destroy();

	//������Ϣ
	m_ImageLoadInfo.strFileName=LoadInfo.strFileName;
	m_ImageLoadInfo.pszResource=LoadInfo.pszResource;
	m_ImageLoadInfo.hResourceDLL=LoadInfo.hResourceDLL;

	//��������
	if (m_bLoadAlways==true) ++nTempRefCount;
	if (bTempLoadAlways==true) --nTempRefCount;

	//�ָ�����
	if ((nTempRefCount>0)&&(LockResource()>0)) m_nRefCount=nTempRefCount;

	return true;
}

//���ò���
bool CBitImage::SetLoadInfo(UINT uResourceID, HINSTANCE hResourceDLL, bool bLoadAlways)
{
	//�������
	INT nTempRefCount=m_nRefCount;
	bool bTempLoadAlways=m_bLoadAlways;

	//�ͷ���Դ
	m_nRefCount=0;
	m_bLoadAlways=bLoadAlways;
	if ((nTempRefCount>0)||(bTempLoadAlways==true)) Destroy();

	//������Ϣ
	m_ImageLoadInfo.strFileName.Empty();
	m_ImageLoadInfo.hResourceDLL=hResourceDLL;
	m_ImageLoadInfo.pszResource=MAKEINTRESOURCE(uResourceID);

	//��������
	if (m_bLoadAlways==true) ++nTempRefCount;
	if (bTempLoadAlways==true) --nTempRefCount;

	//�ָ�����
	if ((nTempRefCount>0)&&(LockResource()>0)) m_nRefCount=nTempRefCount;

	return true;
}

//���ò���
bool CBitImage::SetLoadInfo(LPCTSTR pszResource, HINSTANCE hResourceDLL, bool bLoadAlways)
{
	//�������
	INT nTempRefCount=m_nRefCount;
	bool bTempLoadAlways=m_bLoadAlways;

	//�ͷ���Դ
	m_nRefCount=0;
	m_bLoadAlways=bLoadAlways;
	if ((nTempRefCount>0)||(bTempLoadAlways==true)) Destroy();

	//������Ϣ
	m_ImageLoadInfo.strFileName.Empty();
	m_ImageLoadInfo.pszResource=pszResource;
	m_ImageLoadInfo.hResourceDLL=hResourceDLL;

	//��������
	if (m_bLoadAlways==true) ++nTempRefCount;
	if (bTempLoadAlways==true) --nTempRefCount;

	//�ָ�����
	if ((nTempRefCount>0)&&(LockResource()>0)) m_nRefCount=nTempRefCount;

	return true;
}

//��������
bool CBitImage::CreateImageRegion(CRgn & ImageRgn, COLORREF crTransColor)
{
	//��������
	CreateImageRegion(ImageRgn,0,0,GetWidth(),GetHeight(),crTransColor);

	return true;
}

//��������
bool CBitImage::CreateImageRegion(CRgn & ImageRgn, INT nXSrcPos, INT nYSrcPos, INT nSrcWidth, INT nSrcHeight, COLORREF crTransColor)
{
	//�ж�״̬
	ASSERT(IsNull()==false);
	if (IsNull()==true) return false;

	//��������
	INT nImageWidht=GetWidth();
	INT nImageHeight=GetHeight();
	if ((nXSrcPos+nSrcWidth)>nImageWidht) nSrcWidth=nImageWidht-nXSrcPos;
	if ((nYSrcPos+nSrcHeight)>nImageHeight) nSrcHeight=nImageHeight-nYSrcPos;

	//��������
	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	ImageRgn.CreateRectRgn(0,0,0,0);

	//����λͼ
	CImage ImageConsult;
	ImageConsult.Create(nSrcWidth,nSrcHeight,32);

	//����λͼ
	BITMAP ImageInfo;
	GetObject(ImageConsult,sizeof(ImageInfo),&ImageInfo);
	while ((ImageInfo.bmWidthBytes%4)!=0) ImageInfo.bmWidthBytes++;

	//�滭λͼ
	BitBlt(ImageConsult.GetDC(),0,0,nSrcWidth,nSrcHeight,nXSrcPos,nYSrcPos);
	ImageConsult.ReleaseDC();

	//��ȡ����
	BYTE cbRTranColor=GetRValue(crTransColor);
	BYTE cbGTranColor=GetGValue(crTransColor);
	BYTE cbBTranColor=GetBValue(crTransColor);
	BYTE * pcbImageBitIndex=(BYTE *)ImageInfo.bmBits+(ImageInfo.bmHeight-1)*ImageInfo.bmWidthBytes;

	//��������
	for (INT nYPos=0;nYPos<ImageInfo.bmHeight;nYPos++)
	{
		for (INT nXPos=0;nXPos<ImageInfo.bmWidth;nXPos++)
		{
			//��������
			INT nXStartPos=nXPos;

			//͸���ж�
			do
			{
				COLORREF crImageColor=*(((LONG *)pcbImageBitIndex)+nXPos);
				if ((cbRTranColor==GetRValue(crImageColor))&&(cbGTranColor==GetGValue(crImageColor))&&(cbBTranColor==GetBValue(crImageColor))) break;
			} while ((++nXPos)<ImageInfo.bmWidth);

			//��������
			if ((nXPos-nXStartPos)>0)
			{
				//��������
				CRgn RgnUnite;
				RgnUnite.CreateRectRgn(nXStartPos,nYPos,nXPos,nYPos+1);

				//�ϲ�����
				CombineRgn(ImageRgn,RgnUnite,ImageRgn,RGN_OR);
			}
		}

		//����λ��
		pcbImageBitIndex-=ImageInfo.bmWidthBytes;
	}

	return true;
}

//ƽ�̻滭
bool CBitImage::DrawImageTile(CDC * pDC, CRect & rcDestRect)
{
	//ƽ�̻滭
	DrawImageTile(pDC,rcDestRect.left,rcDestRect.top,rcDestRect.Width(),rcDestRect.Height());

	return true;
}

//ƽ�̻滭
bool CBitImage::DrawImageTile(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight)
{
	//Ч��״̬
	ASSERT(IsNull()==false);
	if (IsNull()==true) return false;

	//��Ч����
	CRect rcDirty;
	pDC->GetClipBox(&rcDirty);

	//��������
	INT nImageWidth=GetWidth();
	INT nImageHeight=GetHeight();

	//ƽ�̻滭
	for (INT nXDrawPos=0;nXDrawPos<nWidth;nXDrawPos+=nImageWidth)
	{
		for (INT nYDrawPos=0;nYDrawPos<nHeight;nYDrawPos+=nImageHeight)
		{
			//����λ��
			INT nDrawWidth=__min(nWidth-nXDrawPos,nImageWidth);
			INT nDrawHeight=__min(nHeight-nYDrawPos,nImageHeight);

			//�滭ͼ��
			bool bIntersect=IsRectIntersect(rcDirty,nXDrawPos+nXPos,nYDrawPos+nYPos,nDrawWidth,nDrawHeight);
			if (bIntersect==true) BitBlt(pDC->m_hDC,nXDrawPos+nXPos,nYDrawPos+nYPos,nDrawWidth,nDrawHeight,0,0);
		}
	}

	return true;
}

//͸���滭
bool CBitImage::AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, COLORREF crTransColor)
{
	//�滭ͼƬ
	AlphaDrawImage(pDestDC,xDest,yDest,GetWidth(),GetHeight(),0,0,crTransColor);

	return true;
}

//͸���滭
bool CBitImage::AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor)
{
	//��Ч����
	CRect rcDirty;
	pDestDC->GetClipBox(&rcDirty);

	//�滭�ж�
	if (IsNull()==true) return false;
	if (IsRectIntersect(rcDirty,xDest,yDest,cxDest,cyDest)==false) return true;

	//λ�õ���
	tagImageRender ImageRender;
	GetDrawImageArea(xDest,yDest,cxDest,cyDest,xSrc,ySrc,rcDirty,ImageRender);

	//��ɫ����
	COLORREF crTextColor=pDestDC->SetTextColor(RGB(0,0,0));
	COLORREF crBackColor=pDestDC->SetBkColor(RGB(255,255,255));

	//�������
	CDC MaskDC;
	CBitmap MaskImage;
	MaskDC.CreateCompatibleDC(pDestDC);
	MaskImage.CreateBitmap(ImageRender.cxRender,ImageRender.cyRender,1,1,NULL);

	//λͼ DC
	CDC * pDCImage=CDC::FromHandle(GetDC());
	COLORREF crImageBkColor=pDCImage->SetBkColor(crTransColor);

	//��������
	MaskDC.SelectObject(&MaskImage);
	MaskDC.BitBlt(0,0,ImageRender.cxRender,ImageRender.cyRender,pDCImage,ImageRender.nXImage,ImageRender.nYImage,SRCINVERT);

	//�滭����
	pDestDC->BitBlt(ImageRender.nXScreen,ImageRender.nYScreen,ImageRender.cxRender,ImageRender.cyRender,
		pDCImage,ImageRender.nXImage,ImageRender.nYImage,SRCINVERT);
	pDestDC->BitBlt(ImageRender.nXScreen,ImageRender.nYScreen,ImageRender.cxRender,ImageRender.cyRender,
		&MaskDC,0,0,SRCAND);
	pDestDC->BitBlt(ImageRender.nXScreen,ImageRender.nYScreen,ImageRender.cxRender,ImageRender.cyRender,
		pDCImage,ImageRender.nXImage,ImageRender.nYImage,SRCINVERT);

	//��ɫ��ԭ
	pDestDC->SetBkColor(crBackColor);
	pDestDC->SetTextColor(crTextColor);

	//�ͷ���Դ
	ReleaseDC();
	MaskDC.DeleteDC();
	MaskImage.DeleteObject();

	return true;
}

//��ϻ滭
bool CBitImage::BlendDrawImage(CDC * pDestDC, INT xDest, INT yDest, COLORREF crTransColor, BYTE cbAlphaDepth)
{
	//͸���滭
	BlendDrawImage(pDestDC,xDest,yDest,GetWidth(),GetHeight(),0,0,crTransColor,cbAlphaDepth);

	return true;
}

//��ϻ滭
bool CBitImage::BlendDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor, BYTE cbAlphaDepth)
{
	//��Ч����
	CRect rcDirty;
	pDestDC->GetClipBox(&rcDirty);

	//�滭�ж�
	if (IsNull()==true) return false;
	if (IsRectIntersect(rcDirty,xDest,yDest,cxDest,cyDest)==false) return true;

	//λ�õ���
	tagImageRender ImageRender;
	GetDrawImageArea(xDest,yDest,cxDest,cyDest,xSrc,ySrc,rcDirty,ImageRender);

	//����λͼ
	CImage ImageResult;
	CImage ImageSource;
	ImageResult.Create(ImageRender.cxRender,ImageRender.cyRender,32);
	ImageSource.Create(ImageRender.cxRender,ImageRender.cyRender,32);

	//�滭λͼ
	CDC * pDCImage=CDC::FromHandle(GetDC());
	CDC * pDCResult=CDC::FromHandle(ImageResult.GetDC());
	CDC * pDCSource=CDC::FromHandle(ImageSource.GetDC());
	pDCSource->BitBlt(0,0,ImageRender.cxRender,ImageRender.cyRender,pDCImage,ImageRender.nXImage,ImageRender.nYImage,SRCCOPY);
	pDCResult->BitBlt(0,0,ImageRender.cxRender,ImageRender.cyRender,pDestDC,ImageRender.nXScreen,ImageRender.nYScreen,SRCCOPY);

	//��ȡ����
	float fAlpha=(float)(cbAlphaDepth/255.0);
	INT nPitchResult=ImageResult.GetPitch();
	INT nPitchSource=ImageSource.GetPitch();

	//��ȡ����
	LPBYTE cbBitResult=(LPBYTE)ImageResult.GetBits();
	LPBYTE cbBitSource=(LPBYTE)ImageSource.GetBits();

	//��������
	for (INT nYPos=0;nYPos<ImageRender.cyRender;nYPos++)
	{
		for (INT nXPos=0;nXPos<ImageRender.cxRender;nXPos++)
		{
			//��ȡ��ɫ
			COLORREF * pcrResult=(COLORREF *)(cbBitResult+nYPos*nPitchResult+nXPos*4);
			COLORREF * pcrSource=(COLORREF *)(cbBitSource+nYPos*nPitchSource+nXPos*4);

			//��ϴ���
			if (*pcrSource!=crTransColor)
			{
				//�����ɫ
				BYTE cbResultR=GetRValue(*pcrResult);
				BYTE cbResultG=GetGValue(*pcrResult);
				BYTE cbResultB=GetBValue(*pcrResult);

				//ԭͼ��ɫ
				BYTE cbSourceR=GetRValue(*pcrSource);
				BYTE cbSourceG=GetGValue(*pcrSource);
				BYTE cbSourceB=GetBValue(*pcrSource);

				//��ɫ���
				cbResultR=(BYTE)(cbSourceR*fAlpha+cbResultR*(1.0-fAlpha));
				cbResultG=(BYTE)(cbSourceG*fAlpha+cbResultG*(1.0-fAlpha));
				cbResultB=(BYTE)(cbSourceB*fAlpha+cbResultB*(1.0-fAlpha));

				//��ɫ���
				*pcrResult=RGB(cbResultR,cbResultG,cbResultB);
			}
		}
	}

	//�滭����
	ImageResult.BitBlt(pDestDC->m_hDC,ImageRender.nXScreen,ImageRender.nYScreen);

	//�ͷŶ���
	ReleaseDC();
	ImageSource.ReleaseDC();
	ImageResult.ReleaseDC();

	return true;
}

//���μ��
bool CBitImage::IsRectIntersect(const CRect & rcRect, INT nXPos, INT nYPos, INT nWidth, INT nHeight)
{
	//��Χ���
	if ((nXPos>rcRect.right)||(nYPos>rcRect.bottom)) return false;
	if (((nXPos+nWidth)<rcRect.left)||((nYPos+nHeight)<rcRect.top)) return false;

	return true;
}

//�������
bool CBitImage::GetDrawImageArea(INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, CRect & rcDirty, tagImageRender & ImageRender)
{
	//�ཻ����
	CRect rcIntersect;
	rcIntersect.top=__max(yDest,rcDirty.top);
	rcIntersect.left=__max(xDest,rcDirty.left);
	rcIntersect.right=__min((xDest+cxDest),rcDirty.right);
	rcIntersect.bottom=__min((yDest+cyDest),rcDirty.bottom);

	//��Ⱦλ��
	ImageRender.nYScreen=rcIntersect.top;
	ImageRender.nXScreen=rcIntersect.left;

	//��Ⱦ��С
	ImageRender.cxRender=rcIntersect.Width();
	ImageRender.cyRender=rcIntersect.Height();

	//ͼ������
	ImageRender.nXImage=xSrc+ImageRender.nXScreen-xDest;
	ImageRender.nYImage=ySrc+ImageRender.nYScreen-yDest;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CImageHandle::CImageHandle(CBitImage * pBitImage)
{
	//���ñ���
	m_pBitImage=NULL;
	AttachResource(pBitImage);

	return;
}

//��������
CImageHandle::~CImageHandle()
{
	//�ͷ���Դ
	DetchResource();

	return;
}

//����Դ
bool CImageHandle::AttachResource(CBitImage * pBitImage)
{
	//�ͷ���Դ
	DetchResource();

	//�󶨶���
	if (pBitImage!=NULL)
	{
		if (pBitImage->LockResource()>0)
		{
			m_pBitImage=pBitImage;
			return true;
		}
	}

	return false;
}

//�����
bool CImageHandle::DetchResource()
{
	//״̬�ж�
	if (m_pBitImage==NULL) return false;

	//�ͷ���Դ
	m_pBitImage->UnLockResource();
	m_pBitImage=NULL;

	return true;
}

//�Ƿ���Ч
bool CImageHandle::IsValid()
{
	//״̬�ж�
	if (m_pBitImage==NULL) return false;
	if (m_pBitImage->IsNull()==true) return false;	

	return true;
}

//��ȡ���
HBITMAP CImageHandle::GetBitmapHandle()
{
	//Ч�����
	ASSERT(m_pBitImage!=NULL);
	ASSERT(m_pBitImage->IsNull()==false);

	//��ȡ���
	return (HBITMAP)(*m_pBitImage);
}

/////////////////////////////////////////////////////////////////////////////////////////
