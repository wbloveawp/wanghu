#ifndef SHARE_STOCK_MANAGER_HEAD_FILE
#define SHARE_STOCK_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "MonitorServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������
class CShareMutex;

//////////////////////////////////////////////////////////////////////////////////
//��������
#define MAX_STOCK_GAME					50							//��Ϸ����	
#define MAX_STOCK_KIND					1							//��������
#define MAX_STOCK_ITEM					12							//��������

//��泣��
#define ACTIVITY_STOCK_COUNT			(MAX_STOCK_GAME * 2)
#define ROUTINE_STOCK_COUNT				(MAX_STOCK_GAME * MAX_STOCK_ITEM)
#define SHARE_STOCK_COUNT				(ROUTINE_STOCK_COUNT+ACTIVITY_STOCK_COUNT)	

//////////////////////////////////////////////////////////////////////////////////

//�������
struct tagShareStock
{
	DWORD						dwMutexNo;							//������
	tagStockItem				StockItem;							//�������	
	tagStockControl				StockControl;						//������	
};

//����б�����
struct tagStockList
{
	WORD						wKindID;							//���ͱ�ʶ
	DWORD						dwMutexNo;							//������
	tagShareStock*				pRelationStock;						//�������

	//����б�
	tagShareStock				ShareStock[MAX_STOCK_ITEM];			//�������	
};

//��������
struct tagShareData
{
	tagStockList				RoutineStock[MAX_STOCK_GAME];		//������	
	tagShareStock				ActivityStock[ACTIVITY_STOCK_COUNT];//����	
};

//�ϲ���¼
struct tagStockMergeRecord
{
	//������Ϣ
	WORD						wKindCount;							//��������
	WORD						wKindIDList[MAX_STOCK_GAME];		//�����б�

	//�¼����
	WORD						wStockCount;						//�������
	WORD						wSublayerStockID[MAX_STOCK_GAME];	//����ʶ

	//ͳ����Ϣ
	SCORE						wCollectStockScore;					//���ܿ��
	SCORE						wCollectJackpotScore[MAX_JACKPOT_COUNT];//���ܲʽ�	
};

//�����Ϣ
struct tagStockManagerItem
{
	tagStockItem				StockItem;							//�������
	tagStockMergeRecord			MergeRecord;						//�ϲ���¼
	LONGLONG					lStockControlID;					//���Ʊ�ʶ
};

//////////////////////////////////////////////////////////////////////////////////

//�������
class CShareMutex
{
	//�洢����
protected:
	HANDLE						m_hMutex;						//������
	DWORD						m_dwMutexNo;					//������
	INT							m_nLockCount;					//��������

	//ͬ������
protected:
	CCriticalSection			m_CriticalSection;				//�ٽ����
	ULONGLONG					m_ulLockTickCount;				//����ʱ���	

	//��������
public:
	//���캯��
	CShareMutex();
	//��������
	virtual ~CShareMutex();

	//��������
protected:
	//��ֵ����
	CShareMutex(const CShareMutex& ShareMutex) = delete;
	//��ֵ����
	CShareMutex& operator=(const CShareMutex& other) = delete;

	//״̬����
public:
	//У����
	bool IsMutexValid();
	//������
	DWORD GetMutexNo() { return m_dwMutexNo; }

	//���ܺ���
public:	
	//�رվ��
	VOID CloseMutex();
	//��ʼ��
	VOID InitMutex(LPCTSTR pszMutexPrefix, DWORD dwMutexNo);

	//��Ϣ����
public:
	//��ȡʱ���
	ULONGLONG GetLockTickCount();
	//����ʱ���
	VOID SetLockTickCount(ULONGLONG ulTickCount);

	//ͬ������
public:
	//��������
	VOID Unlock(WORD wStockKind=0,bool bForce=false);
	//��������
	DWORD Lock(DWORD dwMilliseconds=INFINITE);
};

//////////////////////////////////////////////////////////////////////////////////

//����������
class MONITOR_SERVICE_CLASS CShareStockItem : public CHandleBase
{
	//�洢����
protected:
	DWORD						m_dwUserLabel;						//�û���ǩ
	CShareMutex *				m_pShareMutex;						//�������
	tagShareStock*				m_pShareStock;						//�����Ϣ

	//��������
public:
	//���캯��
	CShareStockItem() = delete;
	//���캯��
	CShareStockItem(DWORD dwUserLabel,CShareMutex* pShareMutex, tagShareStock* pShareStock);
	//��������
	virtual ~CShareStockItem();

	//��������
public:
	//��ֵ����
	CShareStockItem(const CShareStockItem& ShareStockItem) = delete;
	//��ֵ����
	CShareStockItem& operator=(const CShareStockItem& other) = delete;

	//������
public:	
	//����ˮ
	bool StockShrinkAndUpdate(SCORE lBetScore,WORD wJackpotCount);

	//�������
public:
	//��ȡ״̬
	WORD GetStockID();
	//�������
	WORD GetStockKind();
	//��ǰ���
	SCORE GetStockScore();
	//��ˮ����
	WORD GetShrinkRatio();
	//�����Ϣ
	tagStockItem* GetStockItem();	
	//��ȡ�ʳ�
	tagJackpotItem* GetJackpotItem(WORD wLevelID);	
};

//////////////////////////////////////////////////////////////////////////////////

//�����б�
struct tagMutexList
{
	CShareMutex					ListMutex;							//�������
	CShareMutex					RelationMutex;						//�������
	CShareMutex					StockMutext[MAX_STOCK_ITEM];		//��������		
};

//��������
struct tagMutexData
{
	tagMutexList				RoutineMutex[MAX_STOCK_GAME];		//�������
	CShareMutex					ActivityMutex[ACTIVITY_STOCK_COUNT];//�������
};

//���Ͷ���
typedef CWHAutoPtr<CShareStockItem>	 CShareStockItemPtr;

//////////////////////////////////////////////////////////////////////////////////

//������
class MONITOR_SERVICE_CLASS CShareStockVisitor : public CServiceThread
{
	//���ñ���
protected:
	WORD								m_wKindID;							//���ͱ�ʶ
	tagStockList*						m_pStockList;						//����б�
	
	//�������
protected:
	CShareMutex							m_InitMutex;						//������
	tagMutexList						m_MutexList;						//�����б�	

	//��������
public:
	//���캯��
	CShareStockVisitor();
	//��������
	virtual ~CShareStockVisitor();

	//��������
public:
	//���ö���
	VOID ResetStockVisitor();
	//��ʼ��
	bool InitStockVisitor(WORD wKindID);	

	//���Һ���
public:
	//���ҿ��
	virtual CShareStockItemPtr GetStockItem(WORD wStockKind, SCORE lBetAmount, DWORD dwUserLabel);	

	//�¼�����
protected:
	//�����¼�
	virtual bool OnEventThreadRun();

	//��������
protected:
	//��ȡ��־
	bool GetStockInit();
	//��������
	int GetStockIndex(WORD wStockKind, LONG lBetAmount);	
};

//////////////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHStockData<WORD, tagStockManagerItem>  CStockStockItem;

//�ӿڶ���
interface IStockManagerSink
{
	//Ԥ���ӿ�
public:
	//���Ԥ��
	virtual VOID OnEventStockScoreAlarm(tagStockAlarmItem StockAlarmList[], WORD wItemCount) = NULL;
	//�ʳ�Ԥ��
	virtual VOID OnEventJackpotScoreAlarm(tagJackpotAlarmItem JackpotAlarmList[], WORD wItemCount) = NULL;

	//���½ӿ�
public:
	//������
	virtual VOID OnEventStockScoreUpdate(tagStockVariationItem StockVariationList[], WORD wItemCount)=NULL;
	//״̬����
	virtual VOID OnEventStockStateUpdate(tagStateVariationItem StateVariationList[], WORD wItemCount)=NULL;
	//�ʳظ���
	virtual VOID OnEventJackpotScoreUpdate(tagJackpotVariationItem JackpotVariationList[], WORD wItemCount)=NULL;
	
};

//////////////////////////////////////////////////////////////////////////////////
// 
//������
class MONITOR_SERVICE_CLASS CShareStockManager
{
	//�ӿڱ���
protected:
	IStockManagerSink*					m_pStockManagerSink;				//�ص��ӿ�	

	//�������
protected:
	bool								m_bManageState;						//����״̬
	LONGLONG							m_lStockControlID;					//���Ʊ�ʶ
	CStockStockItem						m_StockStockItem;					//���ֿ�	
	tagSuperPlayerControl				m_SuperPlayerControl;				//��R����	

	//�������
protected:
	CShareMutex							m_InitMutex;						 //������
	tagMutexData						m_MutexData;						 //�������

	//��������
public:
	//���캯��
	CShareStockManager();
	//��������
	virtual ~CShareStockManager();

	//���ú���
public:
	//ֹͣ����
	bool CloseStockManager();
	//��������
	VOID StartStockManager(IStockManagerSink* pIStockManagerSink);

	//�¼�����
public:
	//ʱ������
	VOID OnEventTimerPulse();

	//������
public:
	//ɾ�����
	VOID DeleteStockItem(WORD wStockID);
	//�޸Ŀ��
	bool ModifyStockItem(tagStockItem * pStockItem);	
	//��ӿ��
	bool AppendStockItem(tagStockItem StockItem[], WORD wStockCount);
	//�������ֵ
	bool AdjustStockScore(WORD wStockID,SCORE lChangeScore);
	//�����ʽ�
	bool AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore);	
	//ͳ�Ʋʽ�
	bool CollectJackpotScore(WORD wStockKind,WORD wKindID, SCORE &lJackpotScore);
	//Ӧ�ô�R����
	VOID ApplySuperPlayerControl(tagSuperPlayerControl* pSuperPlayerControl);

	//������
protected:
	//�������
	int ActiveActivityStockItem();
	//���ҹ�����
	int SearchActivityStockItem(WORD wStockID);
	//�������б�
	int ActiveRoutineStockList(WORD wStockKind);
	//���ҿ���б�
	int SearchRoutineStockList(WORD wStockKind,WORD wKindID);
	//�������
	int ActiveRoutineStockItem(int nListIndex, WORD wStockKind);
	//���ҹ�����
	int SearchRoutineStockItem(int nListIndex, WORD wStockID);
	//���ҹ�����
	int SearchRoutineStockItem(int nListIndex, WORD wStockKind, LONG lBetAmount);	

	//��������
protected:
	//���仯
	VOID DetectStockChange();
	//���״̬
	VOID DetectStockState();	
	//�ϲ����
	bool MergeRoutineStock(int nStockIndex);
	//���ѿ��
	bool FissionActivityStock(int nStockIndex);
	//�ռ�����
	bool CollectRelationKindID(tagStockMergeRecord* pStockMergeRecord, tagStockItem* pStockItem);
};

//////////////////////////////////////////////////////////////////////////////////

#endif