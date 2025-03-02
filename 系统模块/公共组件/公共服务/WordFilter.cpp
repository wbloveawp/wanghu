#include "stdafx.h"
#include "WordFilter.h"
#include "WHService.h"
#include <algorithm>

using namespace std;

////////////////////////////////////////////////////////////////////////
//��������
#define LEN_KEYWORD					128							//��󳤶�


////////////////////////////////////////////////////////////////////////

//ȥ���ո�
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
//���캯��
CWordNode::CWordNode()
{
	m_character = 0;
	m_pParentNode = NULL;
}

//���캯��
CWordNode::CWordNode(CHAR character)
{	
	m_pParentNode = NULL;
	m_character = character;	
}

//��������
CWordNode::~CWordNode()
{
	//���ñ���
	m_strKeywords.Empty();

	//ɾ���ڵ�
	CHAR character=0;
	CWordNode * pWordNode=NULL;
	POSITION Position=m_TreeMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TreeMap.GetNextAssoc(Position,character,pWordNode);
		SafeDelete(pWordNode);
	}	
}

//���ҽڵ�
CWordNode* CWordNode::FindChild(CHAR character)
{
	//���Ҷ���
	CWordNode * pWordNode=NULL;
	if (m_TreeMap.Lookup(character,pWordNode)==FALSE)
	{
		return NULL;
	}

	return pWordNode;  
}

//����ڵ�
CWordNode* CWordNode::InsertChild(CHAR character)
{
	//���ҽڵ�
  if (FindChild(character)) return NULL;

	//�����ڵ�
	try
	{
		CWordNode * pWordNode=new CWordNode(character);
		if (pWordNode==NULL) throw TEXT("�ڴ治��,���󴴽�ʧ�ܣ�");

		//���ýڵ�
		m_TreeMap[character]=pWordNode; 

		return pWordNode;
	}
	catch (...)
	{
		ASSERT (FALSE);			
	}  

  return NULL;
}

//���ҹؼ���
bool CWordNode::FindKeyword(LPCSTR pszKeyword)
{
	//У�����
	ASSERT (pszKeyword!=NULL);
	if (pszKeyword == NULL) return false;

	//��������
	CHAR pcKeyword[LEN_KEYWORD+1];
	_snprintf_s(pcKeyword,CountArray(pcKeyword),"%s|",pszKeyword);

	//�����ַ���
	return m_strKeywords.Find(pcKeyword)!=-1;
}

//����ؼ���
bool CWordNode::InsertKeyword(LPCSTR pszKeyword)
{
	//У�����
	ASSERT(pszKeyword != NULL);
	if (pszKeyword == NULL) return false;

	//��������
	CHAR pcKeyword[LEN_KEYWORD+1];
	_snprintf_s(pcKeyword,CountArray(pcKeyword),"%s|",pszKeyword);

	//���ҹؼ���
	if (m_strKeywords.Find(pcKeyword)!=-1)
	{
		return false;
	}

	//����ؼ���
	m_strKeywords.Append(pcKeyword);

	return true;
}

//ɾ���ؼ���
void CWordNode::DeleteKeyword(LPCSTR pszKeyword)
{
	//У�����
	ASSERT(pszKeyword != NULL);
	if (pszKeyword == NULL) return;

	//��������
	CHAR pcKeyword[LEN_KEYWORD + 1];
	_snprintf_s(pcKeyword, CountArray(pcKeyword), "%s|", pszKeyword);

	//�Ƴ��ؼ���
	m_strKeywords.Replace(pcKeyword,"");	
}

////////////////////////////////////////////////////////////////////////////////////

//���캯��
CWordTree::CWordTree() : m_nCount(0),m_pEmptyRoot(NULL)
{
}

//��������
CWordTree::~CWordTree()
{
	//�ͷŽڵ�
	if (m_pEmptyRoot!=NULL) SafeDelete(m_pEmptyRoot);
}

//���ö���
VOID CWordTree::RebuildTree()
{
	//�ͷŽڵ�
	if (m_pEmptyRoot!=NULL) SafeDelete(m_pEmptyRoot);

	//�����ڵ�
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

//ɾ���ؼ���
VOID CWordTree::Delete(LPCSTR pszKeyword)
{
	//���ҹؼ���
	m_nCount = 0;
	auto pFindNode = Find(m_pEmptyRoot, pszKeyword, pszKeyword);
	if (pFindNode != NULL)
	{
		//ɾ���ؼ���
		pFindNode->DeleteKeyword(pszKeyword);

		do
		{
			//�˳��ж�
			if (!pFindNode->m_TreeMap.IsEmpty() || !pFindNode->m_strKeywords.IsEmpty())
			{
				break;
			}

			//�Ƴ��ӽڵ�
			if (pFindNode->m_pParentNode)
			{
				pFindNode->m_pParentNode->m_TreeMap.RemoveKey(pFindNode->GetCharacter());
			}		

			//�л��ڵ�
			auto pLastNode = pFindNode;
			pFindNode = pFindNode->m_pParentNode;

			//�ͷŽڵ�
			SafeDelete(pLastNode);

		} while (pFindNode != NULL && pFindNode != m_pEmptyRoot);
	}	
}

//���ҹؼ���
CWordNode* CWordTree::Find(LPCSTR pszKeyword)
{
	m_nCount=0;
	return Find(m_pEmptyRoot, pszKeyword, pszKeyword);
}

//����ؼ���
CWordNode* CWordTree::Insert(LPCSTR pszKeyword)
{
	//У�鳤��
	ASSERT (lstrlenA(pszKeyword)<=LEN_KEYWORD);
	if (lstrlenA(pszKeyword)>LEN_KEYWORD) return NULL;

	return Insert(m_pEmptyRoot, pszKeyword, pszKeyword);
}

//�����ո�
VOID CWordTree::SkipSpace(LPCSTR & pszKeyword)
{
	while (*pszKeyword==32) 
	{
		m_nCount++;
		pszKeyword++;	
	}
}

//���ҹؼ���
CWordNode* CWordTree::Find(CWordNode * pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//����У��
	if (lstrlenA(pszSubKeyword) == 0) return NULL;

	//�����ո�
	SkipSpace(pszSubKeyword);

	//�������ַ�
	CHAR chFirst = pszSubKeyword[0];
	CWordNode* pFirstNode = pParent->FindChild(chFirst);
	if (pFirstNode == NULL) return NULL;

	//��������
	m_nCount++;

	//���ø��ڵ�
	pFirstNode->m_pParentNode=pParent;

	//�жϽڵ�
	if (pFirstNode->m_TreeMap.IsEmpty())
	{
		return pFirstNode;
	}

	//�ݹ����
	CWordNode* pFindNode = Find(pFirstNode, pszKeyword, pszSubKeyword + 1);
	if (pFindNode == NULL)
	{
		//�������
		string sKeyword(pszKeyword, pszSubKeyword - pszKeyword + 1);

		//ȥ���ո�
		EraseSpace(sKeyword);

		//���ҹؼ���
		if (pFirstNode->FindKeyword(sKeyword.c_str()) == false)
		{
			//���ñ���
			m_nCount -= 1;

			return NULL;
		}

		return pFirstNode;
	}

	return pFindNode;
	
}

//����ؼ���
CWordNode* CWordTree::Insert(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//����У��
	if (lstrlenA(pszSubKeyword)==0) 
	{
		//����ؼ���
		pParent->InsertKeyword(pszKeyword);

		return NULL;
	}

	//�����ַ�
	CHAR chFirst=pszSubKeyword[0];
	CWordNode* pFirstNode=pParent->FindChild(chFirst);

	//�����֧
	if (pFirstNode==NULL)
	{
		return InsertBranch(pParent,pszKeyword,pszSubKeyword);
	}

	return Insert(pFirstNode,pszKeyword,pszSubKeyword+1);
}

//����ؼ���
CWordNode* CWordTree::InsertBranch(CWordNode* pParent, LPCSTR pszKeyword, LPCSTR pszSubKeyword)
{
	//����У��
	if (lstrlenA(pszSubKeyword)==0)
	{
		//����ؼ���
		pParent->InsertKeyword(pszKeyword);

		return NULL;
	}
	
	//�����ַ�
	CHAR chFirst=pszSubKeyword[0];
	CWordNode* pFirstNode=pParent->InsertChild(chFirst);

	//�����֧
	if (pFirstNode!=NULL && lstrlenA(pszSubKeyword)>0)
	{
		return InsertBranch(pFirstNode,pszKeyword,pszSubKeyword+1);
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////

//���캯��
CWordFilter::CWordFilter()
{
	//���ñ���
	m_szFilter[0] = 0;

	//��������
	m_WordTree.RebuildTree();
}

//��������
CWordFilter::~CWordFilter()
{
}

//��������
VOID CWordFilter::ResetFilter()
{	
	//���ñ���
	m_szFilter[0]=0;

	//��������
	m_WordTree.RebuildTree();
}

//���˺���
bool CWordFilter::FilterWords(LPCSTR pszKeyword)
{
	//����У��
	if (pszKeyword==NULL) return false;

	//ת������
	string sKeyword(pszKeyword);

	//ת��Сд
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);

	//���ҹؼ���
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

//���˺���
bool CWordFilter::FilterWords(LPCWSTR pszKeyword)
{
	//ת������
	string sKeyword;
	CWHService::Unicode2Utf8(pszKeyword, sKeyword);

	return FilterWords(sKeyword.c_str());
}

//���˺���
bool CWordFilter::FilterWords(CHAR szKeyword[], WORD wBufferSize)
{
	//����У��
	if (szKeyword[0]==0) return false;

	//�������
	LPCSTR pszKeyword=szKeyword;

	//ת������
	string sKeyword(szKeyword);
	
	//ת��Сд
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);
	
	//��������
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

	//���ý�����	
	szKeyword[nDstLen]=0;

	return true;
}

//���˺���
bool CWordFilter::FilterWords(WCHAR szKeyword[], WORD wBufferSize)
{
	//����У��
	if (szKeyword[0]==0) return false;

	//ת������
	string sKeyword;
	CWHService::Unicode2Utf8(szKeyword, sKeyword);

	//�ַ�������
	int nLength = sKeyword.size();

	//�жϻ���
	if (nLength >=sizeof(m_szFilter)) return false;

	//�����ַ���
	StringCchCopyA(m_szFilter, nLength, sKeyword.c_str());	

	//�����ַ�
	FilterWords(m_szFilter,nLength);

	//ת������
	CWHService::Utf82Unicode(m_szFilter, szKeyword, wBufferSize);

	return true;
}

//����ؼ���
bool CWordFilter::InsertKeyword(LPCSTR pszKeyword)
{
	//ת������
	string sKeyword(pszKeyword);

	//ȥ���ո�
	EraseSpace(sKeyword);

	//ת��Сд
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);

	return m_WordTree.Insert(sKeyword.c_str())!=NULL;
}

//����ؼ���
bool CWordFilter::InsertKeyword(LPCWSTR pszKeyword)
{
	//ת������
	string sKeyword;
	CWHService::Unicode2Utf8(pszKeyword, sKeyword);

	//ȥ���ո�
	EraseSpace(sKeyword);

	//ת��Сд
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);
	
	return m_WordTree.Insert(sKeyword.c_str())!=NULL;
}

//ɾ���ؼ���
VOID CWordFilter::DeleteKeyword(LPCSTR pszKeyword)
{
	//ת������
	string sKeyword(pszKeyword);

	//ȥ���ո�
	EraseSpace(sKeyword);

	//ת��Сд
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);

	return m_WordTree.Delete(sKeyword.c_str());
}

//ɾ���ؼ���
VOID CWordFilter::DeleteKeyword(LPCWSTR pszKeyword)
{
	//ת������
	string sKeyword;
	CWHService::Unicode2Utf8(pszKeyword, sKeyword);

	//ȥ���ո�
	EraseSpace(sKeyword);

	//ת��Сд
	transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);

	return m_WordTree.Delete(sKeyword.c_str());
}

////////////////////////////////////////////////////////////////////////////////////