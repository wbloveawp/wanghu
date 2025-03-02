#include "StdAfx.h"
#include "GameVideoManager.h"

//////////////////////////////////////////////////////////////////////////////////
//��������
#define SERVER_MASK					0x270F										

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameVideoManager::CGameVideoManager()
{
	//����ӿ�
	m_pIGameVideoItemSink=NULL;

	//���ñ���
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;
	ZeroMemory(&m_VideoServer,sizeof(m_VideoServer));

	return;
}

//��������
CGameVideoManager::~CGameVideoManager()
{	
	//�ͷŶ���
	for (INT_PTR i=0;i<m_GameVideoItemActive.GetCount();i++)
	{
		m_GameVideoItemActive[i]->Release();
	}

	//�ͷŶ���
	for (INT_PTR i=0;i<m_GameVideoItemBuffer.GetCount();i++)
	{
		m_GameVideoItemBuffer[i]->Release();
	}

	//ɾ������
	m_GameVideoItemBuffer.RemoveAll();
	m_GameVideoItemActive.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CGameVideoManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameVideoManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameVideoManager,Guid,dwQueryVer);
	return NULL;
}

//���ýӿ�
bool CGameVideoManager::SetGameVideoItemSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameVideoItemSink)!=NULL);
		m_pIGameVideoItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGameVideoItemSink);

		//�ɹ��ж�
		if (m_pIGameVideoItemSink==NULL) return false;
	}
	else m_pIGameVideoItemSink=NULL;

	return true;
}

//�������
void CGameVideoManager::InitVideoManager(tagGameVideoParameter & GameVideoParameter)
{
	//���ñ���
	m_pGameServiceOption=GameVideoParameter.pGameServiceOption;
	m_pGameServiceAttrib=GameVideoParameter.pGameServiceAttrib;

	//������Ϣ
	m_VideoServer.wKindID=m_pGameServiceAttrib->wKindID;
	m_VideoServer.wChairCount=m_pGameServiceAttrib->wChairCount;
	StringCchCopy(m_VideoServer.szProcessName,CountArray(m_VideoServer.szProcessName), m_pGameServiceAttrib->szClientEXEName);

	//������Ϣ
	m_VideoServer.wServerID=m_pGameServiceOption->wServerID;
	m_VideoServer.wServerType=m_pGameServiceOption->wServerType;
	m_VideoServer.dwServerRule=m_pGameServiceOption->dwServerRule;
	StringCchCopy(m_VideoServer.szServerName,CountArray(m_VideoServer.szServerName), m_pGameServiceOption->szServerName);
}

//ɾ���û�
bool CGameVideoManager::DeleteVideoItem(IGameVideoItem * pIGameVideoItem)
{
	//����У��
	//ASSERT (pIGameVideoItem!=NULL);
	if (pIGameVideoItem==NULL) return false;

	//Ѱ�Ҷ���
	for (INT_PTR i=0;i<m_GameVideoItemActive.GetCount();i++)
	{
		//��ȡ�û�
		ASSERT(m_GameVideoItemActive[i]!=NULL);
		CGameVideoItem * pGameVideoItem=m_GameVideoItemActive[i];

		//�û��ж�
		if (pGameVideoItem==pIGameVideoItem)
		{
			//ɾ������
			m_GameVideoItemActive.RemoveAt(i);
			m_GameVideoItemBuffer.Add(pGameVideoItem);

			//���ö���
			pGameVideoItem->ResetVideoItem();

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//����¼��
IGameVideoItem * CGameVideoManager::ActiveVideoItem(WORD wTableID)
{
	//��������
	CGameVideoItem * pGameVideoItem=NULL;

	//��ȡָ��
	if (m_GameVideoItemBuffer.GetCount()==0L)
	{
		try
		{
			//��������
			pGameVideoItem=new CGameVideoItem;
			if (pGameVideoItem==NULL) throw 0;			
		}
		catch (...)
		{
			//�����ж�
			ASSERT(FALSE);
			return NULL;
		}
	}
	else
	{
		//�������
		INT_PTR nItemPostion=m_GameVideoItemBuffer.GetCount()-1L;
		pGameVideoItem=m_GameVideoItemBuffer[nItemPostion];
		m_GameVideoItemBuffer.RemoveAt(nItemPostion);
	}

	//���ö���	
	pGameVideoItem->m_wTableID=wTableID;
	pGameVideoItem->m_pVideoServer=&m_VideoServer;
	pGameVideoItem->m_pIGameVideoItemSink=m_pIGameVideoItemSink;

	//¼����
	BuildVideoNumber(pGameVideoItem->m_szVideoNumber,CountArray(pGameVideoItem->m_szVideoNumber));

	//��Ӷ���
	m_GameVideoItemActive.Add(pGameVideoItem);

	return pGameVideoItem;
}

//¼����
VOID CGameVideoManager::BuildVideoNumber(WCHAR szBuffer[], WORD wBufferLen)
{
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//��λ����
	DWORD dwSecond=SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;

	//��̬����
	static WORD wVideoCount=0;
	static DWORD dwLastSecond=dwSecond;	

	//���±���
	if (dwSecond!=dwLastSecond)
	{
		wVideoCount=0;
		dwLastSecond=dwSecond;
	}

	//�����ʶ
	WORD wServerID=m_pGameServiceOption?m_pGameServiceOption->wServerID:0;

	//��ʽ�����
	_snwprintf_s(szBuffer,wBufferLen,wBufferLen,TEXT("%05d%04d%02d%02d%05d%02d"),wServerID^SERVER_MASK,SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,dwSecond,++wVideoCount);
}

//////////////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(GameVideoManager);

//////////////////////////////////////////////////////////////////////////////////
