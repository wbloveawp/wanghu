#ifndef CRASH_CAPTURE_HEAD_FILE
#define CRASH_CAPTURE_HEAD_FILE
#pragma once

#include "CommonServiceHead.h"

////////////////////////////////////////////////////////////////////////////////////
//��������
class COMMON_SERVICE_CLASS CCrashCapture
{
	//��������
public:
	//���캯��
	CCrashCapture();
	//��������
	virtual ~CCrashCapture();

	//��̬����
public:
	//�쳣����
	static LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo);
};

////////////////////////////////////////////////////////////////////////////////////

#endif