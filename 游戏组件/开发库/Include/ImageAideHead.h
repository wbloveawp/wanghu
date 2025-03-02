#ifndef IMAGE_AIDE_HEAD_HEAD_FILE
#define IMAGE_AIDE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//MFC �ļ�
#include <GdiPlus.h>
#include <AtlImage.h>

//�����ռ�
using namespace Gdiplus;

//ƽ̨ͷ�ļ�
#include "Platform.h"
#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//�����궨��

//��������
#ifndef IMAGE_AIDE_CLASS
	#ifdef  IMAGE_AIDE_DLL
		#define IMAGE_AIDE_CLASS _declspec(dllexport)
	#else
		#define IMAGE_AIDE_CLASS _declspec(dllimport)
	#endif
#endif

//////////////////////////////////////////////////////////////////////////

//λͼ����
struct tagImageLoadInfo
{
	//�ⲿ��Ϣ
	CString							strFileName;						//�ļ�����

	//��Դ��Ϣ
	LPCTSTR							pszResource;						//��Դ��ʶ
	HINSTANCE						hResourceDLL;						//��Դ���
};

//////////////////////////////////////////////////////////////////////////
//�����ļ�

#ifndef IMAGE_AIDE_DLL
	#include "BitImage.h"
	#include "DibImage.h"
	#include "PngImage.h"
	#include "GIFImage.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif