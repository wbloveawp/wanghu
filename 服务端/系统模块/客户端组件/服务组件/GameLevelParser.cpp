#include "StdAfx.h"
#include "GameLevelParser.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CLevelDescribeArray		CGameLevelParser::m_LevelDescribeStorage;		//�洢����

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLevelParser::CGameLevelParser()
{
	//���ñ���
	m_bLoadLevel=false;

	return;
}

//��������
CGameLevelParser::~CGameLevelParser()
{
	//�ͷ�����
	UnLoadGameLevel();

	//�ͷ���Դ
	for (INT_PTR i=0;i<m_LevelDescribeStorage.GetCount();i++)
	{
		if (m_LevelDescribeStorage[i]!=NULL) SafeDelete(m_LevelDescribeStorage[i]);
	}

	//�Ƴ�Ԫ��
	m_LevelDescribeStorage.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * __cdecl CGameLevelParser::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameLevelParser,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameLevelParser,Guid,dwQueryVer);
	return NULL;
}

//���صȼ�
bool __cdecl CGameLevelParser::LoadGameLevel(LPCTSTR pszDirectory, WORD wGameGenre)
{
	//�ͷŵȼ�
	ASSERT(m_bLoadLevel==false);
	if (m_bLoadLevel==true) UnLoadGameLevel();

	//��ҵȼ�
	if ((wGameGenre&GAME_GENRE_GOLD)!=0)
	{
		//�ȼ�����
		LONG lLevelScore[]=
		{	
			0L,2000L,4000L,8000L,20000L,40000L,80000L,150000L,300000L,500000L,1000000L,
				2000000L,5000000L,10000000L,50000000L,100000000L,500000000L,1000000000L,
		};	

		//�ȼ�����
		LPCTSTR pszLevelName[]=
		{		
			TEXT("��ũ"),TEXT("�軧"),TEXT("�͹�"),TEXT("������"),TEXT("ũ����"),TEXT("����"),
				TEXT("�����"),TEXT("����"),TEXT("����"),TEXT("����"),TEXT("С����"),TEXT("�����"),			
				TEXT("�Ĺ�"),TEXT("����"),TEXT("����"),TEXT("��ʥ"),TEXT("����"),TEXT("ְҵ����")	
		};

		//Ч������	
		ASSERT(CountArray(pszLevelName)==(CountArray(lLevelScore)));

		//���صȼ�
		for (WORD i=0;i<CountArray(lLevelScore);i++)
		{
			//��ȡ����
			tagLevelDescribe * pLevelDescribe=CreateLevelDescribe();

			//������
			if (pLevelDescribe==NULL)
			{
				ASSERT(FALSE);
				return false;
			}

			//���ñ���
			pLevelDescribe->lLevelScore=lLevelScore[i];
			lstrcpyn(pLevelDescribe->szLevelName,pszLevelName[i],CountArray(pLevelDescribe->szLevelName));

			//����ȼ�
			m_LevelDescribeActive.Add(pLevelDescribe);
		}

		//���ñ���
		m_bLoadLevel=true;

		return true;
	}

	//���ֵȼ�
	if (wGameGenre&(GAME_GENRE_SCORE|(GAME_GENRE_BATTLE|GAME_GENRE_MATCH| GAME_GENRE_RANKING)))
	{
		//�ļ�Ŀ¼
		TCHAR szFileName[MAX_PATH]=TEXT("");
		// add by humorly 2018/8/3
		// replace func
		_snwprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\GameLevel.ini"), pszDirectory);

		//��������
		WORD wItemIndex=0;
		TCHAR szItemName[16]=TEXT("");
		TCHAR szReadData[256]=TEXT("");

		//��ȡ����
		do
		{
			//��ȡ�ַ�
			// add by humorly 2018/8/3
			// replace func
			_snwprintf_s(szItemName, CountArray(szItemName), TEXT("LevelItem%d"), wItemIndex + 1);
			GetPrivateProfileString(TEXT("LevelDescribe"),szItemName,TEXT(""),szReadData,CountArray(szReadData),szFileName);

			//��ȡ�ַ�
			if (szReadData[0]!=0)
			{
				//��ȡ����
				tagLevelDescribe * pLevelDescribe=CreateLevelDescribe();

				//������
				if (pLevelDescribe==NULL)
				{
					ASSERT(FALSE);
					return false;
				}

				//���ñ���
				pLevelDescribe->lLevelScore=0L;
				pLevelDescribe->szLevelName[0]=0;

				//��ȡ�ȼ�
				WORD wStringIndex=0;
				// add by humorly 2018/8/1
				// make the value useful outside loop
				WORD i = 0;
				for (i=0;i<CountArray(pLevelDescribe->szLevelName)-1;i++)
				{
					//���Ǵ���
					if ((szReadData[i]==TEXT(','))||(szReadData[i]==0)) break;
					if ((wStringIndex==0)&&(szReadData[i]==TEXT(' '))) continue;

					//�����ַ�
					pLevelDescribe->szLevelName[wStringIndex++]=szReadData[i];
				}
				pLevelDescribe->szLevelName[wStringIndex]=0;

				//Ѱ�ҿ�ʼ
				LPCTSTR pszScore=&szReadData[i];
				while (((pszScore[0]>0)&&(pszScore[0]<TEXT('0')))||(pszScore[0]>TEXT('9'))) pszScore++;

                //��ȡ����
				while ((pszScore[0]>=TEXT('0'))&&(pszScore[0]<=TEXT('9')))
				{
					pLevelDescribe->lLevelScore=pLevelDescribe->lLevelScore*10L+pszScore[0]-TEXT('0');
					++pszScore;
				}

				//���ñ���
				wItemIndex++;
				m_LevelDescribeActive.Add(pLevelDescribe);
			}
			else break;

		} while (true);

		//���ñ���
		if (m_LevelDescribeActive.GetCount()>0L) m_bLoadLevel=true;

		return true;
	}

	return false;
}

//��ȡ�ȼ�
WORD __cdecl CGameLevelParser::GetGameLevelItem(tagLevelItem LevelItem[], WORD wMaxCount)
{
	//Ч�����
	ASSERT(wMaxCount>=m_LevelDescribeActive.GetCount());
	if (wMaxCount<m_LevelDescribeActive.GetCount()) return 0;

	//��������
	for (INT_PTR i=0;i<m_LevelDescribeActive.GetCount();i++)
	{
		//��ȡ����
		tagLevelDescribe * pLevelDescribe=m_LevelDescribeActive[i];

		//��������
		CT2CW strLevelName(pLevelDescribe->szLevelName);
		LevelItem[i].lLevelScore=pLevelDescribe->lLevelScore;
		lstrcpynW(LevelItem[i].szLevelName,(LPCWSTR)strLevelName,CountArray(LevelItem[i].szLevelName));
	}

	return (WORD)m_LevelDescribeActive.GetCount();
}

//���õȼ�
bool __cdecl CGameLevelParser::SetGameLevelItem(tagLevelItem LevelItem[], WORD wItemCount)
{
	//�ͷŵȼ�
	ASSERT(m_bLoadLevel==false);
	if (m_bLoadLevel==true) UnLoadGameLevel();

	//���õȼ�
	for (WORD i=0;i<wItemCount;i++)
	{
		//��ȡ����
		tagLevelDescribe * pLevelDescribe=CreateLevelDescribe();

		//������
		if (pLevelDescribe==NULL)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ñ���
		CW2CT strLevelName(LevelItem[i].szLevelName);
		pLevelDescribe->lLevelScore=LevelItem[i].lLevelScore;
		lstrcpyn(pLevelDescribe->szLevelName,(LPCTSTR)strLevelName,CountArray(pLevelDescribe->szLevelName));

		//���ñ���
		m_LevelDescribeActive.Add(pLevelDescribe);
	}

	//���ñ���
	if (m_LevelDescribeActive.GetCount()>0) m_bLoadLevel=true;

	return true;
}

//�ͷŵȼ�
bool __cdecl CGameLevelParser::UnLoadGameLevel()
{
	//���ñ���
	m_bLoadLevel=false;

	//�ͷŶ���
	m_LevelDescribeStorage.Append(m_LevelDescribeActive);
	m_LevelDescribeActive.RemoveAll();	

	return true;
}

//�ȼ�����
LPCTSTR __cdecl CGameLevelParser::GetLevelDescribe(LONGLONG lScore)
{
	//״̬Ч��
	ASSERT(m_LevelDescribeActive.GetCount()>0L);
	if (m_LevelDescribeActive.GetCount()==0) return GetDefaultLevel(lScore);

	//��������
	WORD wItemCount=(WORD)m_LevelDescribeActive.GetCount();

	//�ȼ�����
	for (WORD i=0;i<wItemCount;i++)
	{
		tagLevelDescribe * pLevelDescribe=m_LevelDescribeActive[wItemCount-i-1];
		if ((lScore>=pLevelDescribe->lLevelScore)||(i==(wItemCount-1))) return pLevelDescribe->szLevelName;
	}

	return NULL;
}

//�ȼ�����
LPCTSTR __cdecl CGameLevelParser::GetLevelDescribe(IClientUserItem * pIClientUserItem)
{
	//״̬Ч��
	ASSERT(m_LevelDescribeActive.GetCount()>0L);
	if (m_LevelDescribeActive.GetCount()==0) return GetDefaultLevel(pIClientUserItem->GetUserScore());

	//��������
	LONGLONG lUserScore=pIClientUserItem->GetUserScore();
	WORD wItemCount=(WORD)m_LevelDescribeActive.GetCount();

	//�ȼ�����
	for (WORD i=0;i<wItemCount;i++)
	{
		tagLevelDescribe * pLevelDescribe=m_LevelDescribeActive[wItemCount-i-1];
		if ((lUserScore>=pLevelDescribe->lLevelScore)||(i==(wItemCount-1))) return pLevelDescribe->szLevelName;
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//Ĭ�ϵȼ�
LPCTSTR CGameLevelParser::GetDefaultLevel(LONGLONG lScore)
{
	//�ȼ�����
	static LONGLONG lLevelScore[]=
	{	0L,			2000L,		4000L,		8000L,		20000L,
		40000L,		80000L,		150000L,	300000L,	500000L,
		1000000L,	2000000L,	5000000L,	10000000L,	50000000L,
		100000000L,	500000000L,	1000000000L,
	};	
	
	//�ȼ�����
	static LPCTSTR pszLevelName[]=	
	{		
		TEXT("��ũ"),TEXT("�軧"),TEXT("�͹�"),TEXT("������"),			
			TEXT("ũ����"),TEXT("����"),TEXT("�����"),TEXT("����"),			
			TEXT("����"),TEXT("����"),TEXT("С����"),TEXT("�����"),			
			TEXT("�Ĺ�"),TEXT("����"),TEXT("����"),TEXT("��ʥ"),			
			TEXT("����"),TEXT("ְҵ����")	
	};

	//Ч������	
	ASSERT(CountArray(pszLevelName)==(CountArray(lLevelScore)));	

	//��ȡ�ȼ�	
	for (INT i=0;i<CountArray(lLevelScore);i++)	
	{		
		if (lScore>=lLevelScore[CountArray(lLevelScore)-i-1]) return pszLevelName[i];	
	}	
	
	return pszLevelName[0];
}

//��ȡ����
tagLevelDescribe * CGameLevelParser::CreateLevelDescribe()
{
	//��ȡ����
	tagLevelDescribe * pLevelDescribe=NULL;
	INT_PTR nStorageCount=m_LevelDescribeStorage.GetCount();
	if (nStorageCount>0)
	{
		pLevelDescribe=m_LevelDescribeStorage[nStorageCount-1];
		ASSERT(pLevelDescribe!=NULL);
		m_LevelDescribeStorage.RemoveAt(nStorageCount-1);
	}
	else
	{
		pLevelDescribe=new tagLevelDescribe;
		ASSERT(pLevelDescribe!=NULL);
		if (pLevelDescribe==NULL) return NULL;
	}

	return pLevelDescribe;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(GameLevelParser);

//////////////////////////////////////////////////////////////////////////
