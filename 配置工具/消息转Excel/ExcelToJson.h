
// CJExcelToJson.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CExcelToJsonApp: 
// �йش����ʵ�֣������ CJExcelToJson.cpp
//

class CExcelToJsonApp : public CWinApp
{
public:
	CExcelToJsonApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CExcelToJsonApp theApp;
