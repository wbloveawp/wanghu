#ifndef SKIN_CONTROLS_HEAD_HEAD_FILE
#define SKIN_CONTROLS_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//MFC �ļ�
#include <RichOle.h>
#include <AtlImage.h>
#include <afxtempl.h>

//ƽ̨�ļ�
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\�ͻ������\ͼ������\ImageAideHead.h"
#include "..\..\�ͻ������\������Դ\SkinResourceHead.h"

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef SKIN_CONTROL_CLASS
	#ifdef  SKIN_CONTROL_DLL
		#define SKIN_CONTROL_CLASS _declspec(dllexport)
	#else
		#define SKIN_CONTROL_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define SKIN_CONTROL_DLL_NAME	TEXT("SkinControls.dll")			//��� DLL ����
#else
	#define SKIN_CONTROL_DLL_NAME	TEXT("SkinControlsD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////

//��������
#define CX_DEF_FONT					6									//������
#define CY_DEF_FONT					12									//����߶�

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ISkinMouseEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinMouseEvent={0x89535ac0,0x2a14,0x4093,0x00ae,0xe1,0xd9,0x3f,0x22,0x92,0xfa,0x35};
#else
	#define VER_ISkinMouseEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinMouseEvent={0x491fbebf,0x8035,0x460c,0x00b1,0xa9,0x5a,0x60,0x24,0xa8,0xd6,0xf4};
#endif

//��ť�¼�
interface ISkinMouseEvent : public IUnknownEx
{
	//����¼�
	virtual bool __cdecl OnEventMouse(UINT uMessage, UINT uControlID, UINT nFlags, INT nXPos, INT nYPos)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//�����ļ�

#ifndef SKIN_CONTROL_DLL
	#include "SkinAide.h"
	#include "SkinMenu.h"
	#include "SkinEdit.h"
	#include "SkinButton.h"
	#include "SkinDialog.h"
	#include "SkinListCtrl.h"
	#include "SkinSplitter.h"
	#include "SkinRichEdit.h"
	#include "SkinComboBox.h"
	#include "SkinHyperLink.h"
	#include "SkinTabControl.h"
	#include "SkinScrollBar.h"
	#include "SkinResourceManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif