#include "StdAfx.h"
#include "BitImage.h"

/////////////////////////////////////////////////////////////////////////////////////////

//渲染信息
struct tagImageRender
{
	INT								nXImage;										//图片位置
	INT								nYImage;										//图片位置
	INT								nXScreen;										//渲染位置
	INT								nYScreen;										//渲染位置
	INT								cxRender;										//渲染尺寸
	INT								cyRender;										//渲染尺寸
};

/////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CBitImage::CBitImage()
{
	//设置变量
	m_nRefCount=0;
	m_bLoadAlways=false;

	//加载信息
	m_ImageLoadInfo.pszResource=NULL;
	m_ImageLoadInfo.hResourceDLL=NULL;

	return;
}

//析构函数
CBitImage::~CBitImage()
{
	Destroy();
}

//增加引用
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

//减少引用
INT CBitImage::UnLockResource()
{
	//释放绑定
	ASSERT(m_nRefCount>0);
	if ((--m_nRefCount)==0) Destroy();

	return m_nRefCount;
}

//是否加载
bool CBitImage::IsSetLoadInfo()
{
	return ((m_ImageLoadInfo.hResourceDLL)||(m_ImageLoadInfo.strFileName.IsEmpty()==false));
}

//清除参数
bool CBitImage::RemoveLoadInfo()
{
	//设置变量
	m_ImageLoadInfo.pszResource=NULL;
	m_ImageLoadInfo.hResourceDLL=NULL;
	m_ImageLoadInfo.strFileName.Empty();

	return true;
}

//获取参数
bool CBitImage::GetLoadInfo(tagImageLoadInfo & LoadInfo)
{
	LoadInfo=m_ImageLoadInfo;
	return IsSetLoadInfo();
}

//设置参数
bool CBitImage::SetLoadInfo(LPCTSTR pszFileName, bool bLoadAlways)
{
	//保存变量
	INT nTempRefCount=m_nRefCount;
	bool bTempLoadAlways=m_bLoadAlways;

	//释放资源
	m_nRefCount=0;
	m_bLoadAlways=bLoadAlways;
	if ((nTempRefCount>0)||(bTempLoadAlways==true)) Destroy();

	//加载信息
	m_ImageLoadInfo.pszResource=NULL;
	m_ImageLoadInfo.hResourceDLL=NULL;
	m_ImageLoadInfo.strFileName=pszFileName;

	//计算引用
	if (m_bLoadAlways==true) ++nTempRefCount;
	if (bTempLoadAlways==true) --nTempRefCount;

	//恢复锁定
	if ((nTempRefCount>0)&&(LockResource()>0)) m_nRefCount=nTempRefCount;

	return true;
}

//设置参数
bool CBitImage::SetLoadInfo(const tagImageLoadInfo & LoadInfo)
{
	//保存变量
	INT nTempRefCount=m_nRefCount;
	bool bTempLoadAlways=m_bLoadAlways;

	//释放资源
	m_nRefCount=0;
	m_bLoadAlways=(LoadInfo.hResourceDLL==NULL);
	if ((nTempRefCount>0)||(bTempLoadAlways==true)) Destroy();

	//加载信息
	m_ImageLoadInfo.strFileName=LoadInfo.strFileName;
	m_ImageLoadInfo.pszResource=LoadInfo.pszResource;
	m_ImageLoadInfo.hResourceDLL=LoadInfo.hResourceDLL;

	//计算引用
	if (m_bLoadAlways==true) ++nTempRefCount;
	if (bTempLoadAlways==true) --nTempRefCount;

	//恢复锁定
	if ((nTempRefCount>0)&&(LockResource()>0)) m_nRefCount=nTempRefCount;

	return true;
}

//设置参数
bool CBitImage::SetLoadInfo(UINT uResourceID, HINSTANCE hResourceDLL, bool bLoadAlways)
{
	//保存变量
	INT nTempRefCount=m_nRefCount;
	bool bTempLoadAlways=m_bLoadAlways;

	//释放资源
	m_nRefCount=0;
	m_bLoadAlways=bLoadAlways;
	if ((nTempRefCount>0)||(bTempLoadAlways==true)) Destroy();

	//加载信息
	m_ImageLoadInfo.strFileName.Empty();
	m_ImageLoadInfo.hResourceDLL=hResourceDLL;
	m_ImageLoadInfo.pszResource=MAKEINTRESOURCE(uResourceID);

	//计算引用
	if (m_bLoadAlways==true) ++nTempRefCount;
	if (bTempLoadAlways==true) --nTempRefCount;

	//恢复锁定
	if ((nTempRefCount>0)&&(LockResource()>0)) m_nRefCount=nTempRefCount;

	return true;
}

//设置参数
bool CBitImage::SetLoadInfo(LPCTSTR pszResource, HINSTANCE hResourceDLL, bool bLoadAlways)
{
	//保存变量
	INT nTempRefCount=m_nRefCount;
	bool bTempLoadAlways=m_bLoadAlways;

	//释放资源
	m_nRefCount=0;
	m_bLoadAlways=bLoadAlways;
	if ((nTempRefCount>0)||(bTempLoadAlways==true)) Destroy();

	//加载信息
	m_ImageLoadInfo.strFileName.Empty();
	m_ImageLoadInfo.pszResource=pszResource;
	m_ImageLoadInfo.hResourceDLL=hResourceDLL;

	//计算引用
	if (m_bLoadAlways==true) ++nTempRefCount;
	if (bTempLoadAlways==true) --nTempRefCount;

	//恢复锁定
	if ((nTempRefCount>0)&&(LockResource()>0)) m_nRefCount=nTempRefCount;

	return true;
}

//创建区域
bool CBitImage::CreateImageRegion(CRgn & ImageRgn, COLORREF crTransColor)
{
	//创建区域
	CreateImageRegion(ImageRgn,0,0,GetWidth(),GetHeight(),crTransColor);

	return true;
}

//创建区域
bool CBitImage::CreateImageRegion(CRgn & ImageRgn, INT nXSrcPos, INT nYSrcPos, INT nSrcWidth, INT nSrcHeight, COLORREF crTransColor)
{
	//判断状态
	ASSERT(IsNull()==false);
	if (IsNull()==true) return false;

	//参数调整
	INT nImageWidht=GetWidth();
	INT nImageHeight=GetHeight();
	if ((nXSrcPos+nSrcWidth)>nImageWidht) nSrcWidth=nImageWidht-nXSrcPos;
	if ((nYSrcPos+nSrcHeight)>nImageHeight) nSrcHeight=nImageHeight-nYSrcPos;

	//创建对象
	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	ImageRgn.CreateRectRgn(0,0,0,0);

	//创建位图
	CImage ImageConsult;
	ImageConsult.Create(nSrcWidth,nSrcHeight,32);

	//调整位图
	BITMAP ImageInfo;
	GetObject(ImageConsult,sizeof(ImageInfo),&ImageInfo);
	while ((ImageInfo.bmWidthBytes%4)!=0) ImageInfo.bmWidthBytes++;

	//绘画位图
	BitBlt(ImageConsult.GetDC(),0,0,nSrcWidth,nSrcHeight,nXSrcPos,nYSrcPos);
	ImageConsult.ReleaseDC();

	//获取属性
	BYTE cbRTranColor=GetRValue(crTransColor);
	BYTE cbGTranColor=GetGValue(crTransColor);
	BYTE cbBTranColor=GetBValue(crTransColor);
	BYTE * pcbImageBitIndex=(BYTE *)ImageInfo.bmBits+(ImageInfo.bmHeight-1)*ImageInfo.bmWidthBytes;

	//创建区域
	for (INT nYPos=0;nYPos<ImageInfo.bmHeight;nYPos++)
	{
		for (INT nXPos=0;nXPos<ImageInfo.bmWidth;nXPos++)
		{
			//变量定义
			INT nXStartPos=nXPos;

			//透明判断
			do
			{
				COLORREF crImageColor=*(((LONG *)pcbImageBitIndex)+nXPos);
				if ((cbRTranColor==GetRValue(crImageColor))&&(cbGTranColor==GetGValue(crImageColor))&&(cbBTranColor==GetBValue(crImageColor))) break;
			} while ((++nXPos)<ImageInfo.bmWidth);

			//创建区域
			if ((nXPos-nXStartPos)>0)
			{
				//创建区域
				CRgn RgnUnite;
				RgnUnite.CreateRectRgn(nXStartPos,nYPos,nXPos,nYPos+1);

				//合并区域
				CombineRgn(ImageRgn,RgnUnite,ImageRgn,RGN_OR);
			}
		}

		//设置位置
		pcbImageBitIndex-=ImageInfo.bmWidthBytes;
	}

	return true;
}

//平铺绘画
bool CBitImage::DrawImageTile(CDC * pDC, CRect & rcDestRect)
{
	//平铺绘画
	DrawImageTile(pDC,rcDestRect.left,rcDestRect.top,rcDestRect.Width(),rcDestRect.Height());

	return true;
}

//平铺绘画
bool CBitImage::DrawImageTile(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight)
{
	//效验状态
	ASSERT(IsNull()==false);
	if (IsNull()==true) return false;

	//无效区域
	CRect rcDirty;
	pDC->GetClipBox(&rcDirty);

	//变量定义
	INT nImageWidth=GetWidth();
	INT nImageHeight=GetHeight();

	//平铺绘画
	for (INT nXDrawPos=0;nXDrawPos<nWidth;nXDrawPos+=nImageWidth)
	{
		for (INT nYDrawPos=0;nYDrawPos<nHeight;nYDrawPos+=nImageHeight)
		{
			//计算位置
			INT nDrawWidth=__min(nWidth-nXDrawPos,nImageWidth);
			INT nDrawHeight=__min(nHeight-nYDrawPos,nImageHeight);

			//绘画图形
			bool bIntersect=IsRectIntersect(rcDirty,nXDrawPos+nXPos,nYDrawPos+nYPos,nDrawWidth,nDrawHeight);
			if (bIntersect==true) BitBlt(pDC->m_hDC,nXDrawPos+nXPos,nYDrawPos+nYPos,nDrawWidth,nDrawHeight,0,0);
		}
	}

	return true;
}

//透明绘画
bool CBitImage::AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, COLORREF crTransColor)
{
	//绘画图片
	AlphaDrawImage(pDestDC,xDest,yDest,GetWidth(),GetHeight(),0,0,crTransColor);

	return true;
}

//透明绘画
bool CBitImage::AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor)
{
	//无效区域
	CRect rcDirty;
	pDestDC->GetClipBox(&rcDirty);

	//绘画判断
	if (IsNull()==true) return false;
	if (IsRectIntersect(rcDirty,xDest,yDest,cxDest,cyDest)==false) return true;

	//位置调整
	tagImageRender ImageRender;
	GetDrawImageArea(xDest,yDest,cxDest,cyDest,xSrc,ySrc,rcDirty,ImageRender);

	//颜色保存
	COLORREF crTextColor=pDestDC->SetTextColor(RGB(0,0,0));
	COLORREF crBackColor=pDestDC->SetBkColor(RGB(255,255,255));

	//掩码对象
	CDC MaskDC;
	CBitmap MaskImage;
	MaskDC.CreateCompatibleDC(pDestDC);
	MaskImage.CreateBitmap(ImageRender.cxRender,ImageRender.cyRender,1,1,NULL);

	//位图 DC
	CDC * pDCImage=CDC::FromHandle(GetDC());
	COLORREF crImageBkColor=pDCImage->SetBkColor(crTransColor);

	//构造掩码
	MaskDC.SelectObject(&MaskImage);
	MaskDC.BitBlt(0,0,ImageRender.cxRender,ImageRender.cyRender,pDCImage,ImageRender.nXImage,ImageRender.nYImage,SRCINVERT);

	//绘画对象
	pDestDC->BitBlt(ImageRender.nXScreen,ImageRender.nYScreen,ImageRender.cxRender,ImageRender.cyRender,
		pDCImage,ImageRender.nXImage,ImageRender.nYImage,SRCINVERT);
	pDestDC->BitBlt(ImageRender.nXScreen,ImageRender.nYScreen,ImageRender.cxRender,ImageRender.cyRender,
		&MaskDC,0,0,SRCAND);
	pDestDC->BitBlt(ImageRender.nXScreen,ImageRender.nYScreen,ImageRender.cxRender,ImageRender.cyRender,
		pDCImage,ImageRender.nXImage,ImageRender.nYImage,SRCINVERT);

	//颜色还原
	pDestDC->SetBkColor(crBackColor);
	pDestDC->SetTextColor(crTextColor);

	//释放资源
	ReleaseDC();
	MaskDC.DeleteDC();
	MaskImage.DeleteObject();

	return true;
}

//混合绘画
bool CBitImage::BlendDrawImage(CDC * pDestDC, INT xDest, INT yDest, COLORREF crTransColor, BYTE cbAlphaDepth)
{
	//透明绘画
	BlendDrawImage(pDestDC,xDest,yDest,GetWidth(),GetHeight(),0,0,crTransColor,cbAlphaDepth);

	return true;
}

//混合绘画
bool CBitImage::BlendDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor, BYTE cbAlphaDepth)
{
	//无效区域
	CRect rcDirty;
	pDestDC->GetClipBox(&rcDirty);

	//绘画判断
	if (IsNull()==true) return false;
	if (IsRectIntersect(rcDirty,xDest,yDest,cxDest,cyDest)==false) return true;

	//位置调整
	tagImageRender ImageRender;
	GetDrawImageArea(xDest,yDest,cxDest,cyDest,xSrc,ySrc,rcDirty,ImageRender);

	//创建位图
	CImage ImageResult;
	CImage ImageSource;
	ImageResult.Create(ImageRender.cxRender,ImageRender.cyRender,32);
	ImageSource.Create(ImageRender.cxRender,ImageRender.cyRender,32);

	//绘画位图
	CDC * pDCImage=CDC::FromHandle(GetDC());
	CDC * pDCResult=CDC::FromHandle(ImageResult.GetDC());
	CDC * pDCSource=CDC::FromHandle(ImageSource.GetDC());
	pDCSource->BitBlt(0,0,ImageRender.cxRender,ImageRender.cyRender,pDCImage,ImageRender.nXImage,ImageRender.nYImage,SRCCOPY);
	pDCResult->BitBlt(0,0,ImageRender.cxRender,ImageRender.cyRender,pDestDC,ImageRender.nXScreen,ImageRender.nYScreen,SRCCOPY);

	//获取属性
	float fAlpha=(float)(cbAlphaDepth/255.0);
	INT nPitchResult=ImageResult.GetPitch();
	INT nPitchSource=ImageSource.GetPitch();

	//获取数据
	LPBYTE cbBitResult=(LPBYTE)ImageResult.GetBits();
	LPBYTE cbBitSource=(LPBYTE)ImageSource.GetBits();

	//创建区域
	for (INT nYPos=0;nYPos<ImageRender.cyRender;nYPos++)
	{
		for (INT nXPos=0;nXPos<ImageRender.cxRender;nXPos++)
		{
			//获取颜色
			COLORREF * pcrResult=(COLORREF *)(cbBitResult+nYPos*nPitchResult+nXPos*4);
			COLORREF * pcrSource=(COLORREF *)(cbBitSource+nYPos*nPitchSource+nXPos*4);

			//混合处理
			if (*pcrSource!=crTransColor)
			{
				//结果颜色
				BYTE cbResultR=GetRValue(*pcrResult);
				BYTE cbResultG=GetGValue(*pcrResult);
				BYTE cbResultB=GetBValue(*pcrResult);

				//原图颜色
				BYTE cbSourceR=GetRValue(*pcrSource);
				BYTE cbSourceG=GetGValue(*pcrSource);
				BYTE cbSourceB=GetBValue(*pcrSource);

				//颜色混合
				cbResultR=(BYTE)(cbSourceR*fAlpha+cbResultR*(1.0-fAlpha));
				cbResultG=(BYTE)(cbSourceG*fAlpha+cbResultG*(1.0-fAlpha));
				cbResultB=(BYTE)(cbSourceB*fAlpha+cbResultB*(1.0-fAlpha));

				//颜色混合
				*pcrResult=RGB(cbResultR,cbResultG,cbResultB);
			}
		}
	}

	//绘画界面
	ImageResult.BitBlt(pDestDC->m_hDC,ImageRender.nXScreen,ImageRender.nYScreen);

	//释放对象
	ReleaseDC();
	ImageSource.ReleaseDC();
	ImageResult.ReleaseDC();

	return true;
}

//矩形检查
bool CBitImage::IsRectIntersect(const CRect & rcRect, INT nXPos, INT nYPos, INT nWidth, INT nHeight)
{
	//范围检测
	if ((nXPos>rcRect.right)||(nYPos>rcRect.bottom)) return false;
	if (((nXPos+nWidth)<rcRect.left)||((nYPos+nHeight)<rcRect.top)) return false;

	return true;
}

//区域计算
bool CBitImage::GetDrawImageArea(INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, CRect & rcDirty, tagImageRender & ImageRender)
{
	//相交矩形
	CRect rcIntersect;
	rcIntersect.top=__max(yDest,rcDirty.top);
	rcIntersect.left=__max(xDest,rcDirty.left);
	rcIntersect.right=__min((xDest+cxDest),rcDirty.right);
	rcIntersect.bottom=__min((yDest+cyDest),rcDirty.bottom);

	//渲染位置
	ImageRender.nYScreen=rcIntersect.top;
	ImageRender.nXScreen=rcIntersect.left;

	//渲染大小
	ImageRender.cxRender=rcIntersect.Width();
	ImageRender.cyRender=rcIntersect.Height();

	//图形索引
	ImageRender.nXImage=xSrc+ImageRender.nXScreen-xDest;
	ImageRender.nYImage=ySrc+ImageRender.nYScreen-yDest;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CImageHandle::CImageHandle(CBitImage * pBitImage)
{
	//设置变量
	m_pBitImage=NULL;
	AttachResource(pBitImage);

	return;
}

//析构函数
CImageHandle::~CImageHandle()
{
	//释放资源
	DetchResource();

	return;
}

//绑定资源
bool CImageHandle::AttachResource(CBitImage * pBitImage)
{
	//释放资源
	DetchResource();

	//绑定对象
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

//解除绑定
bool CImageHandle::DetchResource()
{
	//状态判断
	if (m_pBitImage==NULL) return false;

	//释放资源
	m_pBitImage->UnLockResource();
	m_pBitImage=NULL;

	return true;
}

//是否有效
bool CImageHandle::IsValid()
{
	//状态判断
	if (m_pBitImage==NULL) return false;
	if (m_pBitImage->IsNull()==true) return false;	

	return true;
}

//获取句柄
HBITMAP CImageHandle::GetBitmapHandle()
{
	//效验参数
	ASSERT(m_pBitImage!=NULL);
	ASSERT(m_pBitImage->IsNull()==false);

	//获取句柄
	return (HBITMAP)(*m_pBitImage);
}

/////////////////////////////////////////////////////////////////////////////////////////
