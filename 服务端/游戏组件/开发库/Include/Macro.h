#ifndef MACRO_HEAD_FILE
#define MACRO_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//���ó���

//��Ч��ֵ
#define INVALID_BYTE				((BYTE)(0xFF))						//��Ч��ֵ
#define INVALID_WORD				((WORD)(0xFFFF))					//��Ч��ֵ
#define INVALID_DWORD				((DWORD)(0xFFFFFFFF))				//��Ч��ֵ


//////////////////////////////////////////////////////////////////////////

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//��Ч��ַ
#define INVALID_IP_ADDRESS(IPAddress) (((IPAddress==0L)||(IPAddress==INADDR_NONE)))

//////////////////////////////////////////////////////////////////////////

//�洢����
#ifdef _UNICODE
	#define CountStringBuffer CountStringBufferW
#else
	#define CountStringBuffer CountStringBufferA
#endif

//�洢����
#define CountStringBufferA(String) ((WORD)((lstrlenA(String)+1)*sizeof(CHAR)))
#define CountStringBufferW(String) ((WORD)((lstrlenW(String)+1)*sizeof(WCHAR)))

//////////////////////////////////////////////////////////////////////////

//�ӿ��ͷ�
#define  SafeRelease(pObject) { if (pObject!=NULL) { pObject->Release(); pObject=NULL; } }

//ɾ��ָ��
#define  SafeDelete(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//ɾ������
#define  SafeDeleteArray(pData) { try { delete [] pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//�رվ��
#define SafeCloseHandle(handle) { if (handle!=NULL){ CloseHandle(handle);handle=NULL; } }

//MAKECOMMD

#define MAKECMMD(a, b)      ((WORD)(((DWORD_PTR)(a)) & 0x3ff) | ((WORD)(((DWORD_PTR)(b)) & 0x3f)) << 10)
#define LOCMMD(w)           ((WORD)(((DWORD_PTR)(w)) & 0x3ff))
#define HICMMD(w)           ((WORD)((((DWORD_PTR)(w)) >> 10) & 0x3f))

//////////////////////////////////////////////////////////////////////////

//0-1���������
#define Random_Double ((double)rand()/(RAND_MAX+1))

//���������
#define RANDVAL(a, b) (a >= b ? b : ((rand() % (b - a + 1) + a)))

//////////////////////////////////////////////////////////////////////////
// 
//��ֵ����

//�Ŵ���ֵ
#define SCORE_ZOOMIN(a) ((a)*SCORE_MULTIPLE)				
#define SCORE_ZOOMIN_D(a) ((a)/(SCORE_MULTIPLE*1.f))

//��С��ֵ
#define SCORE_ZOOMOUT(a) ((a)/SCORE_MULTIPLE)
#define SCORE_ZOOMOUT_D(a) ((a)/(SCORE_MULTIPLE*1.f))

//////////////////////////////////////////////////////////////////////////

//��������
#define DOUBLE_ACCURACY					0.00000001
#define DOUBLE_EQUAL(a,b)				(fabs(a - b) < DOUBLE_ACCURACY)
#define DOUBLE_GREATER(a,b)				(a > (b + DOUBLE_ACCURACY))
#define DOUBLE_LESSER(a,b)				(a < (b - DOUBLE_ACCURACY))
#define DOUBLE_GREATER_AND_EQUAL(a,b)	(a >= (b - DOUBLE_ACCURACY))
#define DOUBLE_LESSER_AND_EQUAL(a,b)	(a <= (b + DOUBLE_ACCURACY))

//////////////////////////////////////////////////////////////////////////

#endif