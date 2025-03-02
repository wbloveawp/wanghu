#include "stdafx.h"
#include "WordFilter.h"
#include "WHService.h"
#include <algorithm>

using namespace std;

////////////////////////////////////////////////////////////////////////
//常量定义
#define LEN_KEYWORD					128							//最大长度


////////////////////////////////////////////////////////////////////////

//去除空格
void EraseSpace(string& sKeyword)
{
	int index = 0;
	if (!sKeyword.empty())
	{
		while ((index = sKeyword.find(' ', index)) != string::npos)
		{
			sKeyword.erase(index, 1);
		}
	}
}

////////////////////////////////////////////////////////////////////////
//构造函数
CWordNode::CWordNode()
{
	m_character = 0;
	m_pParentNode = NULL;
}

//构造函数
CWordNode::CWordNode(CHAR character)
{	
	m_pParentNode = NULL;
	m_character = character;	
}

//析构函数
CWordNode::~CWordNode()
{
	//设置变量
	m_strKeywords.Empty();

	//删除节点
	CHAR character=0;
	CWordNode * pWordNode=NULL;
	POSITION Position=m_TreeMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TreeMap.GetNextAssoc(Position,character,pWordNode);
		SafeDelete(pWordNode);
	}	
}

//查找节点
CWordNode* CWordNode::FindChild(CHAR character)
{
	//查找对象
	CWordNode * pWordNode=NULL;
	if (m_TreeMap.Lookup(character,pWordNode)==FALSE)
	{
		return NULL;
	}

	return pWordNode;  
}

//插入节点
CWordNode* CWordNode::InsertChild(CHAR character)
{
	//查找节点
  if (FindChild(character)) return NULL;

	//创建节点
	try
	{
		CWordNode * pWordNode=new CWordNode(character);
		if (pWordNode==NULL) throw TEXT("内存不足,对象创建失败！");

		//设置节点
		m_TreeMap[character]=pWordNode; 

		return pWordNode;
	}
	catch (...)
	{
		ASSERT (FALSE);			
	}  

  return NULL;
}

//查找关键词
bool CWordNode::FindKeyword(LPCSTR pszKeyword)
{
	//校验参数
	ASSERT (pszKeyword!=NULL);
	if (pszKeyword == NULL) return false;

	//变量定义
	CHAR pcKeyword[LEN_KEYWORD+1];
	_snprintf_s(pcKeyword,CountArray(pcKeyword),"%s|",pszKeyword);

	//查找字符串
	return m_strKeywords.Find(pcKeyword)!=-1;
}

//插入关键词
bool CWordNode::InsertKeyword(LPCSTR pszKeyword)
{
	//校验参数
	ASSERT(pszKeyword != NULL);
	if (pszKeyword == NULL) return false;

	//变量定义
	CHAR pcKeyword[LEN_KEYWORD+1];
	_snprintf_s(pcKeyword,CountArray(pcKeyword),"%s|",pszKeyword);

	//查找关键词
	if (m_strKeywords.Find(pcKeyword)!=-1)
	{
		return false;
	}

	//插入关键词
	m_strKeywords.Append(pcKeyword);

	return true;
}

//删除关键词
void CWordNode::DeleteKeyword(LPCSTR pszKeyword)
{
	//校验参数
	ASSERT(pszKeyword != NULL);
	if (pszKeyword == NULL) return;

	//变量定义
	CHAR pcKeyword[LEN_KEYWORD + 1];
	_snprintf_s(pcKeyword, CountArray(pcKeyword), "%s|", pszKeyword);

	//移除关键词
	m_strKeywords.Replace(pcKeyword,"");	
}

////////////////////////////////////////////////////////////////////////////////////

//构造函数
CWordTree::CWordTree() : m_nCount(0),m_pEmptyRoot(NULL)
{
}

//析构函数
CWordTree::~CWordTree()
{
	//释放节点
	if (m_pEmptyRoot!=NULL) SafeDelete(m_pEmptyRoot);
}

//重置对象
VOID CWordTree::RebuildTree()
{
	//释放节点
	if (m_pEmptyRoot!=NULL) SafeDelete(m_pEmptyRoot);

	//创建节点
	try
	{
		m_pEmptyRoot=new CWordNode();
	}
	catch (...)
	{
		ASSERT (FALSE);
		m_pEmptyRoot=NULL;
	}
}

//删除关键词
VOID CWordTree::Delete(LPCSTR pszKeyword)
{
	//查找关键词
	m_nCount = 0;
	auto pFindNode = Find(m_pEmptyRoot, pszKeyword, pszKeyword);
	if (pFindNode != NULL)
	{
		//删除关键词
		pFindNode->DeleteKeyword(pszKeyword);

		do
		{
			//退出判断
			if (!pFindNode->m_TreeMap.IsEmpty() || !pFindNode->m_strKeywords.IsEmpty())
			{
				break;
			}

			//移除子节点
			if (pFindNode->m_pParentNode)
			{
				pFindNode->m_pParentNode->m_TreeMap.RemoveKey(pFindNode->GetCharacter());
			}		

			//切换节点
			auto pLastNode = pFindNode;
			pFindNode = pFindNode->m_pParentNode;

			//释放节点
			SafeDelete(pLastNode);

		} while (pFindNode != NULL && pFindNode != m_pEmptyRoot);
	}	
}

//查找关键词
CWordNode* CWordTree::Find(LPCSTR pszKeyword)
{
	m_nCount=0;
	return Find(m_pEmptyRoot, pszKeyword, pszKeyword);
}

//插入关键词
CWordNode* CWordTree::Insert(LPCSTR pszKeyword)
{
	//校验长度
	ASSERT (lstrlenA(pszKeyword)<=LEN_KEYWORD);
	if (lstrlenA(pszKeyword)>LEN_KEYWORD) return NULL;

	return Insert(m_pEmptyRoot, pszKeyword, pszKeyword);
}

//跳过空格
VOID CWordTree::SkipSpace(LPCSTR & pszKeyword)
{
	while (*pszKeyword==32) 
	{
		m_nCount++;
		pszKeyword++;	
	}
}

//查找关键词
CWordNode* CWordTree::Find(CWordNode * pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//参数校验
	if (lstrlenA(pszSubKeyword) == 0) return NULL;

	//跳过空格
	SkipSpace(pszSubKeyword);

	//查找首字符
	CHAR chFirst = pszSubKeyword[0];
	CWordNode* pFirstNode = pParent->FindChild(chFirst);
	if (pFirstNode == NULL) return NULL;

	//设置数量
	m_nCount++;

	//设置父节点
	pFirstNode->m_pParentNode=pParent;

	//判断节点
	if (pFirstNode->m_TreeMap.IsEmpty())
	{
		return pFirstNode;
	}

	//递归查找
	CWordNode* pFindNode = Find(pFirstNode, pszKeyword, pszSubKeyword + 1);
	if (pFindNode == NULL)
	{
		//构造对象
		string sKeyword(pszKeyword, pszSubKeyword - pszKeyword + 1);

		//去除空格
		EraseSpace(sKeyword);

		//查找关键词
		if (pFirstNode->FindKeyword(sKeyword.c_str()) == false)
		{
			//设置变量
			m_nCount -= 1;

			return NULL;
		}

		return pFirstNode;
	}

	return pFindNode;
	
}

//插入关键词
CWordNode* CWordTree::Insert(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//参数校验
	if (lstrlenA(pszSubKeyword)==0) 
	{
		//插入关键词
		pParent->InsertKeyword(pszKeyword);

		return NULL;
	}

	//查找字符
	CHAR chFirst=pszSubKeyword[0];
	CWordNode* pFirstNode=pParent->FindChild(chFirst);

	//插入分支
	if (pFirstNode==NULL)
	{
		return InsertBranch(pParent,pszKeyword,pszSubKeyword);
	}

	return Insert(pFirstNode,pszKeyword,pszSubKeyword+1);
}

//插入关键词
CWordNode* CWordTree::InsertBranch(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//参数校验
	if (lstrlenA(pszSubKeyword)==0)
	{
		//插入关键词
		pParent->InsertKeyword(pszKeyword);

		return NULL;
	}
	
	//查找字符
	CHAR chFirst=pszSubKeyword[0];
	CWordNode* pFirstNode=pParent->InsertChild(chFirst);

	//插入分支
	if (pFirstNode!=NULL && lstrlenA(pszSubKeyword)>0)
	{
		return InsertBranch(pFirstNode,pszKeyword,pszSubKeyword+1);
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////

//构造函数
CWordFilter::CWordFilter()
{
	//设置变量
	m_szFilter[0] = 0;

	//构建词树
	m_WordTree.RebuildTree();
}

//析构函数
CWordFilter::~CWordFilter()
{
}

//重置数据
VOID CWordFilter::ResetFilter()
{	
	//设置变量
	m_szFilter[0]=0;

	//构建词树
	m_WordTree.RebuildTree();
}

//过滤函数
bool CWordFilter::FilterWords(LPCSTR pszKeyword)
{
	//参数校验
	if (pszKeyword==NULL) return false;

	//转换编码
	string sKeyword(pszKeyword);

	//转换小写
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);

	//查找关键字
	for (int i=0; i< sKeyword.size(); i++)
	{
		LPCSTR pSubstring= sKeyword.c_str()+i;
		if (m_WordTree.Find(pSubstring)!=NULL)
		{
			return true;
		}
	}
  	  
	return false;
}

//过滤函数
bool CWordFilter::FilterWords(LPCWSTR pszKeyword)
{
	//转换编码
	string sKeyword;
	CWHService::Unicode2Utf8(pszKeyword, sKeyword);

	return FilterWords(sKeyword.c_str());
}

//过滤函数
bool CWordFilter::FilterWords(CHAR szKeyword[], WORD wBufferSize)
{
	//参数校验
	if (szKeyword[0]==0) return false;

	//构造对象
	LPCSTR pszKeyword=szKeyword;

	//转换编码
	string sKeyword(szKeyword);
	
	//转换小写
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);
	
	//变量定义
	int i=0;
	int nDstLen=0;
	int nSrcLen=lstrlenA(pszKeyword);		

	while (i<nSrcLen)
	{
		LPCSTR pSubstring = sKeyword.c_str() + i;
		if (m_WordTree.Find(pSubstring)!=NULL)
		{			
			i+=m_WordTree.m_nCount;
			*(szKeyword +nDstLen++)=TEXT('*');
		}
		else
		{
			i++;
			*(szKeyword +nDstLen++)=*pSubstring;
		}	
	}	

	//设置结束符	
	szKeyword[nDstLen]=0;

	return true;
}

//过滤函数
bool CWordFilter::FilterWords(WCHAR szKeyword[], WORD wBufferSize)
{
	//参数校验
	if (szKeyword[0]==0) return false;

	//转换编码
	string sKeyword;
	CWHService::Unicode2Utf8(szKeyword, sKeyword);

	//字符串长度
	int nLength = sKeyword.size();

	//判断缓冲
	if (nLength >=sizeof(m_szFilter)) return false;

	//拷贝字符串
	StringCchCopyA(m_szFilter, nLength, sKeyword.c_str());	

	//过滤字符
	FilterWords(m_szFilter,nLength);

	//转换编码
	CWHService::Utf82Unicode(m_szFilter, szKeyword, wBufferSize);

	return true;
}

//插入关键词
bool CWordFilter::InsertKeyword(LPCSTR pszKeyword)
{
	//转换编码
	string sKeyword(pszKeyword);

	//去除空格
	EraseSpace(sKeyword);

	//转换小写
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);

	return m_WordTree.Insert(sKeyword.c_str())!=NULL;
}

//插入关键词
bool CWordFilter::InsertKeyword(LPCWSTR pszKeyword)
{
	//转换编码
	string sKeyword;
	CWHService::Unicode2Utf8(pszKeyword, sKeyword);

	//去除空格
	EraseSpace(sKeyword);

	//转换小写
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);
	
	return m_WordTree.Insert(sKeyword.c_str())!=NULL;
}

//删除关键词
VOID CWordFilter::DeleteKeyword(LPCSTR pszKeyword)
{
	//转换编码
	string sKeyword(pszKeyword);

	//去除空格
	EraseSpace(sKeyword);

	//转换小写
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);

	return m_WordTree.Delete(sKeyword.c_str());
}

//删除关键词
VOID CWordFilter::DeleteKeyword(LPCWSTR pszKeyword)
{
	//转换编码
	string sKeyword;
	CWHService::Unicode2Utf8(pszKeyword, sKeyword);

	//去除空格
	EraseSpace(sKeyword);

	//转换小写
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);

	return m_WordTree.Delete(sKeyword.c_str());
}

////////////////////////////////////////////////////////////////////////////////////