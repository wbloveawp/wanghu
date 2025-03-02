#ifndef BIT_IMAGE_HEAD_FILE
#define BIT_IMAGE_HEAD_FILE

#pragma once

#include "ImageAideHead.h"

//////////////////////////////////////////////////////////////////////////

//结构说明
struct tagImageRender;

//////////////////////////////////////////////////////////////////////////

//位图对象类
class IMAGE_AIDE_CLASS CBitImage : public CImage
{
	friend class CImageHandle;

	//内部变量
private:
	INT								m_nRefCount;						//引用计数
	bool							m_bLoadAlways;						//是否常在
	tagImageLoadInfo				m_ImageLoadInfo;					//加载参数

	//函数定义
public:
	//构造函数
	CBitImage();
	//析构函数
	virtual ~CBitImage();

	//接口函数
protected:
	//增加引用
	INT LockResource();
	//减少引用
	INT UnLockResource();
	//引用计数
	INT GetReferenceCount() { return m_nRefCount; }

	//功能函数
public:
	//加载信息
	bool IsSetLoadInfo();
	//清除参数
	bool RemoveLoadInfo();
	//获取参数
	bool GetLoadInfo(tagImageLoadInfo & LoadInfo);
	//设置参数
	bool SetLoadInfo(const tagImageLoadInfo & LoadInfo);
	//设置参数
	bool SetLoadInfo(LPCTSTR pszFileName, bool bLoadAlways=true);
	//设置参数
	bool SetLoadInfo(UINT uResourceID, HINSTANCE hResourceDLL, bool bLoadAlways=false);
	//设置参数
	bool SetLoadInfo(LPCTSTR pszResource, HINSTANCE hResourceDLL, bool bLoadAlways=false);

	//重载函数
public:
	//位图指针
	operator CBitmap * () { return CBitmap::FromHandle((HBITMAP)(*this)); }

	//区域函数
public:
	//创建区域
	bool CreateImageRegion(CRgn & ImageRgn, COLORREF crTransColor);
	//创建区域
	bool CreateImageRegion(CRgn & ImageRgn, INT nXSrcPos, INT nYSrcPos, INT nSrcWidth, INT nSrcHeight, COLORREF crTransColor);

	//绘画图片
public:
	//平铺绘画
	bool DrawImageTile(CDC * pDC, CRect & rcDestRect);
	//平铺绘画
	bool DrawImageTile(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight);

	//透明绘画
public:
	//透明绘画
	bool AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, COLORREF crTransColor);
	//透明绘画
	bool AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor);

	//混合绘画
public:
	//混合绘画
	bool BlendDrawImage(CDC * pDestDC, INT xDest, INT yDest, COLORREF crTransColor, BYTE cbAlphaDepth);
	//混合绘画
	bool BlendDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, COLORREF crTransColor, BYTE cbAlphaDepth);

	//辅助函数
public:
	//矩形检查
	bool IsRectIntersect(const CRect & rcRect, INT nXPos, INT nYPos, INT nWidth, INT nHeight);
	//区域计算
	bool GetDrawImageArea(INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, CRect & rcDirty, tagImageRender & ImageRender);
};

//////////////////////////////////////////////////////////////////////////

//位图资源句柄
class IMAGE_AIDE_CLASS CImageHandle
{
	//变量定义
protected:
	CBitImage *						m_pBitImage;						//位图指针
	
	//函数定义
public:
	//构造函数
	CImageHandle(CBitImage * pBitImage=NULL);
	//析构函数
	virtual ~CImageHandle();
	
	//接口函数
public:
	//是否有效
	bool IsValid();
	//解除绑定
	bool DetchResource();
	//绑定资源
	bool AttachResource(CBitImage * pBitImage);

	//操作重载
public:
	//获取句柄
	HBITMAP GetBitmapHandle();
	//获取对象
	CBitImage * GetBitImage() { return m_pBitImage; }
	//指针重载
	CBitImage * operator->() { return GetBitImage(); }
};

//////////////////////////////////////////////////////////////////////////

#endif