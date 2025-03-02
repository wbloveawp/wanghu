#ifndef DATA_FILE_HEAD_FILE
#define DATA_FILE_HEAD_FILE
#pragma once

//引入文件
#include "ModuleManagerHead.h"

/////////////////////////////////////////////////////////////////////////////////////////
//结构定义

//文件头部
struct tagFileHead
{
	BYTE									cbFileHead[8];						//文件头部
};

//数据收集
struct tagDataCollect
{
	WORD									wItemCount;							//子项数目
};

/////////////////////////////////////////////////////////////////////////////////////////
//数据文件
class CDataFile
{
	//内核变量
protected:
	CWHCustomFile							m_CustomFile;						//自定文件

	//函数定义
public:
	//构造函数
	CDataFile();
	//析构函数
	virtual ~CDataFile();

	//函数定义
public:
	//打开文件
	BOOL OpenDataFile(LPCTSTR pszFileName);
	//创建文件
	BOOL CreateDataFile(LPCTSTR pszFileName);	
	//更新子项
	VOID UpdateDataItem(LPVOID lpBuffer,WORD wBufferSize);
	//删除子项
	BOOL DeleteDataItem(WORD wItemIndex,WORD wItemSize);
	//写入子项
	VOID WriteDataItem(LPVOID lpBuffer,WORD wBufferSize);
  //枚举子项
	DWORD ReadDataItem(LPVOID lpBuffer,WORD wItemSize,WORD wItemIndex);

	//辅助函数
public:
	//子项数目
	WORD GetItemCount();	
	//关闭文件
	VOID CloseFile();

	//内部函数
protected:	
	//生成校验和
	VOID BuildCheckSum();

};

/////////////////////////////////////////////////////////////////////////////////////////
#endif

