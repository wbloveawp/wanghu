#ifndef MACRO_HEAD_FILE
#define MACRO_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//常用常量

//无效数值
#define INVALID_BYTE				((BYTE)(0xFF))						//无效数值
#define INVALID_WORD				((WORD)(0xFFFF))					//无效数值
#define INVALID_DWORD				((DWORD)(0xFFFFFFFF))				//无效数值


//////////////////////////////////////////////////////////////////////////

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//无效地址
#define INVALID_IP_ADDRESS(IPAddress) (((IPAddress==0L)||(IPAddress==INADDR_NONE)))

//////////////////////////////////////////////////////////////////////////

//存储长度
#ifdef _UNICODE
	#define CountStringBuffer CountStringBufferW
#else
	#define CountStringBuffer CountStringBufferA
#endif

//存储长度
#define CountStringBufferA(String) ((WORD)((lstrlenA(String)+1)*sizeof(CHAR)))
#define CountStringBufferW(String) ((WORD)((lstrlenW(String)+1)*sizeof(WCHAR)))

//////////////////////////////////////////////////////////////////////////

//接口释放
#define  SafeRelease(pObject) { if (pObject!=NULL) { pObject->Release(); pObject=NULL; } }

//删除指针
#define  SafeDelete(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//删除数组
#define  SafeDeleteArray(pData) { try { delete [] pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//关闭句柄
#define SafeCloseHandle(handle) { if (handle!=NULL){ CloseHandle(handle);handle=NULL; } }

//MAKECOMMD

#define MAKECMMD(a, b)      ((WORD)(((DWORD_PTR)(a)) & 0x3ff) | ((WORD)(((DWORD_PTR)(b)) & 0x3f)) << 10)
#define LOCMMD(w)           ((WORD)(((DWORD_PTR)(w)) & 0x3ff))
#define HICMMD(w)           ((WORD)((((DWORD_PTR)(w)) >> 10) & 0x3f))

//////////////////////////////////////////////////////////////////////////

//0-1随机浮点数
#define Random_Double ((double)rand()/(RAND_MAX+1))

//区间随机数
#define RANDVAL(a, b) (a >= b ? b : ((rand() % (b - a + 1) + a)))

//////////////////////////////////////////////////////////////////////////
// 
//数值缩放

//放大数值
#define SCORE_ZOOMIN(a) ((a)*SCORE_MULTIPLE)				
#define SCORE_ZOOMIN_D(a) ((a)/(SCORE_MULTIPLE*1.f))

//缩小数值
#define SCORE_ZOOMOUT(a) ((a)/SCORE_MULTIPLE)
#define SCORE_ZOOMOUT_D(a) ((a)/(SCORE_MULTIPLE*1.f))

//////////////////////////////////////////////////////////////////////////

//浮点运算
#define DOUBLE_ACCURACY					0.00000001
#define DOUBLE_EQUAL(a,b)				(fabs(a - b) < DOUBLE_ACCURACY)
#define DOUBLE_GREATER(a,b)				(a > (b + DOUBLE_ACCURACY))
#define DOUBLE_LESSER(a,b)				(a < (b - DOUBLE_ACCURACY))
#define DOUBLE_GREATER_AND_EQUAL(a,b)	(a >= (b - DOUBLE_ACCURACY))
#define DOUBLE_LESSER_AND_EQUAL(a,b)	(a <= (b + DOUBLE_ACCURACY))

//////////////////////////////////////////////////////////////////////////

#endif