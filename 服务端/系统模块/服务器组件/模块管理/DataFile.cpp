#include "StdAfx.h"
#include "Datafile.h"
//#include "MD5CheckSum.h"


////////////////////////////////////////////////////////////////////////////////////////
//文件头部
const BYTE g_cbFileHead[] = {'H','M','X','T','T','D','O','0'};

////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CDataFile::CDataFile()
{
}

//析构函数
CDataFile::~CDataFile()
{
	//关闭文件
	CloseFile();
}

//打开文件
BOOL CDataFile::OpenDataFile(LPCTSTR pszFileName)
{
	//打开文件
	m_CustomFile.Create(pszFileName,GENERIC_READ|GENERIC_WRITE);
	if(m_CustomFile.IsValid()==FALSE) return FALSE;

	//变量定义
	tagFileHead FileHead;
	
	//读取头部
	m_CustomFile.SeekToBegin();
	m_CustomFile.Read(&FileHead,sizeof(FileHead));

	//变量定义
	BOOL bSuccessed=TRUE;

  //校验头部
	for(BYTE cbIndex=0;cbIndex<CountArray(FileHead.cbFileHead);cbIndex++)
	{
		if(FileHead.cbFileHead[cbIndex]!=g_cbFileHead[cbIndex])
		{
			bSuccessed=FALSE;
			break;
		}
	}

	//匹配失败
	if (bSuccessed==FALSE) m_CustomFile.Close();

	return bSuccessed;
}

//创建文件
BOOL CDataFile::CreateDataFile(LPCTSTR pszFileName)
{
	//创建文件
	m_CustomFile.Create(pszFileName,GENERIC_READ|GENERIC_WRITE,CREATE_ALWAYS);

	//写入版本
	if (m_CustomFile.IsValid()==TRUE)
	{
		//构造结构
		tagFileHead FileHead;
		CopyMemory(FileHead.cbFileHead,g_cbFileHead,CountArray(FileHead.cbFileHead));

		//写入数据
		m_CustomFile.Write(&FileHead,sizeof(FileHead));

		//构造结构
		tagDataCollect DataCollect;
		ZeroMemory(&DataCollect,sizeof(DataCollect));

		//移动指针
		m_CustomFile.SeekToEnd();
		m_CustomFile.Write(&DataCollect,sizeof(DataCollect));
	}

	return m_CustomFile.IsValid();
}

//更新子项
VOID CDataFile::UpdateDataItem(LPVOID lpBuffer,WORD wBufferSize)
{
	//参数校验
	if(lpBuffer==NULL) return;

	//有效判断
	if(m_CustomFile.IsValid()==FALSE) return;

	//写入数据
	return m_CustomFile.Write(lpBuffer,wBufferSize);
}

//删除子项
BOOL CDataFile::DeleteDataItem(WORD wItemIndex,WORD wItemSize)
{
	//变量定义
	WORD wItemCount = GetItemCount();
	if(wItemIndex>=wItemCount) return FALSE;

	//计算偏移
	SCORE lOffset = sizeof(tagFileHead)+sizeof(tagDataCollect)+wItemIndex*wItemSize;	

	//位置判断
	if(lOffset<m_CustomFile.GetFileLength()-wItemSize)
	{
		//申请内存
		LPBYTE pcbFileData = new BYTE[(wItemCount-1)*wItemSize];
		if(pcbFileData==NULL) return FALSE;

		//读取数据
		m_CustomFile.Seek(lOffset+wItemSize,FILE_BEGIN);
		m_CustomFile.Read(pcbFileData,(wItemCount-1)*wItemSize);

		//写入数据
		m_CustomFile.Seek(lOffset,FILE_BEGIN);
		m_CustomFile.Write(pcbFileData,(wItemCount-1)*wItemSize);	

		//读取数据
		tagDataCollect DataCollect;
		m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);
		m_CustomFile.Read(&DataCollect,sizeof(DataCollect));

		//更新变量
		DataCollect.wItemCount--;
		m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);
		m_CustomFile.Write(&DataCollect,sizeof(DataCollect));

		//释放资源
		SafeDelete(pcbFileData);

		//设置结尾
		return m_CustomFile.SetEndOfFile();
	}

	return TRUE;
}

//写入数据
VOID CDataFile::WriteDataItem(LPVOID lpBuffer,WORD wBufferSize)
{
	//参数校验
	if (lpBuffer==NULL) return;

	//有效判断
	if (m_CustomFile.IsValid()==FALSE) return;

	//变量定义
	tagDataCollect DataCollect;

	//移动指针
	m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);

	//读取数据
	m_CustomFile.Read(&DataCollect,sizeof(DataCollect));

	//更新变量
	++DataCollect.wItemCount;
	m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);
	m_CustomFile.Write(&DataCollect,sizeof(DataCollect));

	//写入数据
	m_CustomFile.SeekToEnd();
	m_CustomFile.Write(lpBuffer,wBufferSize);

	return;
}


//读取数据
DWORD CDataFile::ReadDataItem(LPVOID lpBuffer,WORD wItemSize,WORD wItemIndex)
{
	//参数校验
	if (lpBuffer==NULL) return 0L;

	//有效判断
	if (m_CustomFile.IsValid()==FALSE) return 0L;

	//移动指针
	m_CustomFile.Seek(sizeof(tagFileHead)+sizeof(tagDataCollect)+wItemIndex*wItemSize,FILE_BEGIN);

  //读取数据
	return m_CustomFile.Read(lpBuffer,wItemSize);
}

//子项数目
WORD CDataFile::GetItemCount()
{
	//有效判断
	if (m_CustomFile.IsValid()==FALSE) return 0;

	//变量定义
	tagDataCollect DataCollect;

	//移动指针
	m_CustomFile.Seek(sizeof(tagFileHead),FILE_BEGIN);

	//读取数据
	m_CustomFile.Read(&DataCollect,sizeof(DataCollect));

	//更新变量
	return DataCollect.wItemCount;
}

//关闭文件
VOID CDataFile::CloseFile()
{
	if (m_CustomFile.IsValid()==TRUE)
	{
		//生成校验和
		//BuildCheckSum();

		//关闭文件
		m_CustomFile.Close();
	}
}

//生成校验和
VOID CDataFile::BuildCheckSum()
{
	if (m_CustomFile.IsValid()==FALSE) return;

	//变量定义
	DWORD dwDataSize = m_CustomFile.GetFileLength()-sizeof(tagFileHead);

	try
	{
		//申请资源
		LPBYTE pData = new BYTE[dwDataSize];
		if(pData==NULL) return;

		//读取文件头
		tagFileHead FileHead;
		m_CustomFile.SeekToBegin();
		m_CustomFile.Read(&FileHead,sizeof(FileHead));

		//读取数据
		m_CustomFile.Read(pData,dwDataSize);

		//更新数据
		m_CustomFile.SeekToBegin();
		m_CustomFile.Write(&FileHead,sizeof(FileHead));

		//释放资源
		SafeDelete(pData);
	}
	catch(...){}

	return;
};

////////////////////////////////////////////////////////////////////////////////////////