#ifndef CRASH_CAPTURE_HEAD_FILE
#define CRASH_CAPTURE_HEAD_FILE
#pragma once

#include "CommonServiceHead.h"

////////////////////////////////////////////////////////////////////////////////////
//崩溃捕获
class COMMON_SERVICE_CLASS CCrashCapture
{
	//函数定义
public:
	//构造函数
	CCrashCapture();
	//析构函数
	virtual ~CCrashCapture();

	//静态函数
public:
	//异常过滤
	static LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo);
};

////////////////////////////////////////////////////////////////////////////////////

#endif