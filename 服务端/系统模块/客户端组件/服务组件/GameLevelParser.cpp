#include "StdAfx.h"
#include "GameLevelParser.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CLevelDescribeArray		CGameLevelParser::m_LevelDescribeStorage;		//存储数组

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLevelParser::CGameLevelParser()
{
	//设置变量
	m_bLoadLevel=false;

	return;
}

//析构函数
CGameLevelParser::~CGameLevelParser()
{
	//释放子项
	UnLoadGameLevel();

	//释放资源
	for (INT_PTR i=0;i<m_LevelDescribeStorage.GetCount();i++)
	{
		if (m_LevelDescribeStorage[i]!=NULL) SafeDelete(m_LevelDescribeStorage[i]);
	}

	//移除元素
	m_LevelDescribeStorage.RemoveAll();

	return;
}

//接口查询
VOID * __cdecl CGameLevelParser::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameLevelParser,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameLevelParser,Guid,dwQueryVer);
	return NULL;
}

//加载等级
bool __cdecl CGameLevelParser::LoadGameLevel(LPCTSTR pszDirectory, WORD wGameGenre)
{
	//释放等级
	ASSERT(m_bLoadLevel==false);
	if (m_bLoadLevel==true) UnLoadGameLevel();

	//金币等级
	if ((wGameGenre&GAME_GENRE_GOLD)!=0)
	{
		//等级积分
		LONG lLevelScore[]=
		{	
			0L,2000L,4000L,8000L,20000L,40000L,80000L,150000L,300000L,500000L,1000000L,
				2000000L,5000000L,10000000L,50000000L,100000000L,500000000L,1000000000L,
		};	

		//等级名称
		LPCTSTR pszLevelName[]=
		{		
			TEXT("务农"),TEXT("佃户"),TEXT("雇工"),TEXT("作坊主"),TEXT("农场主"),TEXT("地主"),
				TEXT("大地主"),TEXT("财主"),TEXT("富翁"),TEXT("大富翁"),TEXT("小财神"),TEXT("大财神"),			
				TEXT("赌棍"),TEXT("赌侠"),TEXT("赌王"),TEXT("赌圣"),TEXT("赌神"),TEXT("职业赌神")	
		};

		//效验数据	
		ASSERT(CountArray(pszLevelName)==(CountArray(lLevelScore)));

		//加载等级
		for (WORD i=0;i<CountArray(lLevelScore);i++)
		{
			//获取子项
			tagLevelDescribe * pLevelDescribe=CreateLevelDescribe();

			//错误处理
			if (pLevelDescribe==NULL)
			{
				ASSERT(FALSE);
				return false;
			}

			//设置变量
			pLevelDescribe->lLevelScore=lLevelScore[i];
			lstrcpyn(pLevelDescribe->szLevelName,pszLevelName[i],CountArray(pLevelDescribe->szLevelName));

			//插入等级
			m_LevelDescribeActive.Add(pLevelDescribe);
		}

		//设置变量
		m_bLoadLevel=true;

		return true;
	}

	//积分等级
	if (wGameGenre&(GAME_GENRE_SCORE|(GAME_GENRE_BATTLE|GAME_GENRE_MATCH| GAME_GENRE_RANKING)))
	{
		//文件目录
		TCHAR szFileName[MAX_PATH]=TEXT("");
		// add by humorly 2018/8/3
		// replace func
		_snwprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\GameLevel.ini"), pszDirectory);

		//变量定义
		WORD wItemIndex=0;
		TCHAR szItemName[16]=TEXT("");
		TCHAR szReadData[256]=TEXT("");

		//读取资料
		do
		{
			//读取字符
			// add by humorly 2018/8/3
			// replace func
			_snwprintf_s(szItemName, CountArray(szItemName), TEXT("LevelItem%d"), wItemIndex + 1);
			GetPrivateProfileString(TEXT("LevelDescribe"),szItemName,TEXT(""),szReadData,CountArray(szReadData),szFileName);

			//读取字符
			if (szReadData[0]!=0)
			{
				//获取子项
				tagLevelDescribe * pLevelDescribe=CreateLevelDescribe();

				//错误处理
				if (pLevelDescribe==NULL)
				{
					ASSERT(FALSE);
					return false;
				}

				//设置变量
				pLevelDescribe->lLevelScore=0L;
				pLevelDescribe->szLevelName[0]=0;

				//读取等级
				WORD wStringIndex=0;
				// add by humorly 2018/8/1
				// make the value useful outside loop
				WORD i = 0;
				for (i=0;i<CountArray(pLevelDescribe->szLevelName)-1;i++)
				{
					//过虑处理
					if ((szReadData[i]==TEXT(','))||(szReadData[i]==0)) break;
					if ((wStringIndex==0)&&(szReadData[i]==TEXT(' '))) continue;

					//设置字符
					pLevelDescribe->szLevelName[wStringIndex++]=szReadData[i];
				}
				pLevelDescribe->szLevelName[wStringIndex]=0;

				//寻找开始
				LPCTSTR pszScore=&szReadData[i];
				while (((pszScore[0]>0)&&(pszScore[0]<TEXT('0')))||(pszScore[0]>TEXT('9'))) pszScore++;

                //读取积分
				while ((pszScore[0]>=TEXT('0'))&&(pszScore[0]<=TEXT('9')))
				{
					pLevelDescribe->lLevelScore=pLevelDescribe->lLevelScore*10L+pszScore[0]-TEXT('0');
					++pszScore;
				}

				//设置变量
				wItemIndex++;
				m_LevelDescribeActive.Add(pLevelDescribe);
			}
			else break;

		} while (true);

		//设置变量
		if (m_LevelDescribeActive.GetCount()>0L) m_bLoadLevel=true;

		return true;
	}

	return false;
}

//获取等级
WORD __cdecl CGameLevelParser::GetGameLevelItem(tagLevelItem LevelItem[], WORD wMaxCount)
{
	//效验参数
	ASSERT(wMaxCount>=m_LevelDescribeActive.GetCount());
	if (wMaxCount<m_LevelDescribeActive.GetCount()) return 0;

	//拷贝数据
	for (INT_PTR i=0;i<m_LevelDescribeActive.GetCount();i++)
	{
		//获取数据
		tagLevelDescribe * pLevelDescribe=m_LevelDescribeActive[i];

		//拷贝数据
		CT2CW strLevelName(pLevelDescribe->szLevelName);
		LevelItem[i].lLevelScore=pLevelDescribe->lLevelScore;
		lstrcpynW(LevelItem[i].szLevelName,(LPCWSTR)strLevelName,CountArray(LevelItem[i].szLevelName));
	}

	return (WORD)m_LevelDescribeActive.GetCount();
}

//设置等级
bool __cdecl CGameLevelParser::SetGameLevelItem(tagLevelItem LevelItem[], WORD wItemCount)
{
	//释放等级
	ASSERT(m_bLoadLevel==false);
	if (m_bLoadLevel==true) UnLoadGameLevel();

	//设置等级
	for (WORD i=0;i<wItemCount;i++)
	{
		//获取子项
		tagLevelDescribe * pLevelDescribe=CreateLevelDescribe();

		//错误处理
		if (pLevelDescribe==NULL)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置变量
		CW2CT strLevelName(LevelItem[i].szLevelName);
		pLevelDescribe->lLevelScore=LevelItem[i].lLevelScore;
		lstrcpyn(pLevelDescribe->szLevelName,(LPCTSTR)strLevelName,CountArray(pLevelDescribe->szLevelName));

		//设置变量
		m_LevelDescribeActive.Add(pLevelDescribe);
	}

	//设置变量
	if (m_LevelDescribeActive.GetCount()>0) m_bLoadLevel=true;

	return true;
}

//释放等级
bool __cdecl CGameLevelParser::UnLoadGameLevel()
{
	//设置变量
	m_bLoadLevel=false;

	//释放对象
	m_LevelDescribeStorage.Append(m_LevelDescribeActive);
	m_LevelDescribeActive.RemoveAll();	

	return true;
}

//等级描述
LPCTSTR __cdecl CGameLevelParser::GetLevelDescribe(LONGLONG lScore)
{
	//状态效验
	ASSERT(m_LevelDescribeActive.GetCount()>0L);
	if (m_LevelDescribeActive.GetCount()==0) return GetDefaultLevel(lScore);

	//变量定义
	WORD wItemCount=(WORD)m_LevelDescribeActive.GetCount();

	//等级搜索
	for (WORD i=0;i<wItemCount;i++)
	{
		tagLevelDescribe * pLevelDescribe=m_LevelDescribeActive[wItemCount-i-1];
		if ((lScore>=pLevelDescribe->lLevelScore)||(i==(wItemCount-1))) return pLevelDescribe->szLevelName;
	}

	return NULL;
}

//等级描述
LPCTSTR __cdecl CGameLevelParser::GetLevelDescribe(IClientUserItem * pIClientUserItem)
{
	//状态效验
	ASSERT(m_LevelDescribeActive.GetCount()>0L);
	if (m_LevelDescribeActive.GetCount()==0) return GetDefaultLevel(pIClientUserItem->GetUserScore());

	//变量定义
	LONGLONG lUserScore=pIClientUserItem->GetUserScore();
	WORD wItemCount=(WORD)m_LevelDescribeActive.GetCount();

	//等级搜索
	for (WORD i=0;i<wItemCount;i++)
	{
		tagLevelDescribe * pLevelDescribe=m_LevelDescribeActive[wItemCount-i-1];
		if ((lUserScore>=pLevelDescribe->lLevelScore)||(i==(wItemCount-1))) return pLevelDescribe->szLevelName;
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//默认等级
LPCTSTR CGameLevelParser::GetDefaultLevel(LONGLONG lScore)
{
	//等级积分
	static LONGLONG lLevelScore[]=
	{	0L,			2000L,		4000L,		8000L,		20000L,
		40000L,		80000L,		150000L,	300000L,	500000L,
		1000000L,	2000000L,	5000000L,	10000000L,	50000000L,
		100000000L,	500000000L,	1000000000L,
	};	
	
	//等级名称
	static LPCTSTR pszLevelName[]=	
	{		
		TEXT("务农"),TEXT("佃户"),TEXT("雇工"),TEXT("作坊主"),			
			TEXT("农场主"),TEXT("地主"),TEXT("大地主"),TEXT("财主"),			
			TEXT("富翁"),TEXT("大富翁"),TEXT("小财神"),TEXT("大财神"),			
			TEXT("赌棍"),TEXT("赌侠"),TEXT("赌王"),TEXT("赌圣"),			
			TEXT("赌神"),TEXT("职业赌神")	
	};

	//效验数据	
	ASSERT(CountArray(pszLevelName)==(CountArray(lLevelScore)));	

	//获取等级	
	for (INT i=0;i<CountArray(lLevelScore);i++)	
	{		
		if (lScore>=lLevelScore[CountArray(lLevelScore)-i-1]) return pszLevelName[i];	
	}	
	
	return pszLevelName[0];
}

//获取子项
tagLevelDescribe * CGameLevelParser::CreateLevelDescribe()
{
	//获取子项
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

//组件创建函数
DECLARE_CREATE_MODULE(GameLevelParser);

//////////////////////////////////////////////////////////////////////////
