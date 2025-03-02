#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������
class CServerUserPack;
class CServerUserItem;

//////////////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHStockData<WORD, tagGoodsHoldInfo> CStockGoodsHoldInfo;
typedef CWHStockData<DWORD, tagGoodsVariation> CStockGoodsVariation;

//���Ͷ���
typedef CWHStockList<tagWealthVariation> CListWealthVariation;

//////////////////////////////////////////////////////////////////////////////////
//�û�����
class CServerUserPack
{
	friend class CServerUserItem;

	//״̬����
protected:
	DWORD						m_dwRecordID;							//��¼��ʶ
	bool						m_bTransMode;							//����ģʽ

	//���±���
protected:
	DWORD						m_dwUpdateTime;							//����ʱ��
	WORD						m_wUpdateMaxCount;						//��������
	WORD						m_wUpdateInterval;						//���¼��

	//�洢����
protected:
	CWordArray					m_GoodsIDList;							//��Ʒ��ʶ
	CStockGoodsHoldInfo			m_StockGoodsHoldInfo;					//��Ʒ��Ϣ	
	CStockGoodsHoldInfo			m_StockGoodsHoldTemp;					//��ʱ��Ʒ
	CStockGoodsVariation		m_StockGoodsVariation;					//�����¼	

	//�������
protected:
	CServerUserItem*			m_pServerUserItem;						//�û�ָ��	

	//����ӿ�
protected:
	IServerUserItemSink*		m_pIServerUserItemSink;					//�ص��ӿ�

	//��������
protected:
	//���캯��
	CServerUserPack(CServerUserItem* pServerUserItem);
	//��������
	virtual ~CServerUserPack();

	//��Ϣ����
public:
	//��Ʒ����
	WORD GetGoodsCount() { return (WORD)m_StockGoodsHoldInfo.GetItemCount(); }
	//�������
	WORD GetVariationCount() { return (WORD)m_StockGoodsVariation.GetItemCount(); }

	//��Ʒ����
public:
	//��ʼ����
	VOID BeginTrans();
	//�ύ����
	VOID SubmitTrans();
	//�Ƴ���Ʒ
	bool RemoveGoods(WORD wGoodsID, WORD wReason);
	//�Ƴ���Ʒ
	bool RemoveGoods(WORD wGoodsID, DWORD dwGoodsCount, WORD wReason);
	//�����Ʒ
	WORD AppendGoods(tagGoodsHoldInfo GoodsHoldInfo[], WORD wGoodsCount);
	//������Ʒ
	bool UpdateGoods(WORD wGoodsID, WORD wGoodsLevel, DWORD dwIndate, WORD wReason);
	//�����Ʒ
	bool AppendGoods(WORD wGoodsID, DWORD dwGoodsCount, WORD wGoodsLevel, DWORD dwIndate, WORD wReason);

	//�������
public:
	//������
	VOID CleanVariation();
	//�ύ���
	bool IsSubmitVariation();

	//��ȡ����
public:
	//��ȡ��Ʒ
	bool DistillVariationGoods(TCHAR szBuffer[], WORD wBufferCount);
	//��ȡ��¼
	bool DistillVariationRecord(TCHAR szBuffer[], WORD wBufferCount);

	//ö����Ʒ
public:
	//��ȡ����
	tagGoodsHoldInfo* GetHeadGoodsItem();
	//��ȡ����
	tagGoodsHoldInfo* GetNextGoodsItem(tagGoodsHoldInfo* pGoodsHoldInfo);

	//���ܺ���
public:
	//������Ʒ
	tagGoodsHoldInfo* SearchGoods(WORD wGoodsID);

	//��������
public:
	//��ȡ���
	inline tagGoodsVariation* GetGoodsVariation();
	//������Ϣ
	inline tagGoodsHoldInfo* GetGoodsHoldInfo(WORD wGoodsID);

	//��������
protected:
	//���ñ���
	VOID ResetUserPack();
};

//////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class GAME_SERVICE_CLASS CServerUserItem : public IServerUserItem
{
	//��Ԫ����
	friend class CServerUserManager;

	//���Ա���
protected:
	tagServerUserRule				m_UserRule;							//�û�����
	tagServerUserInfo				m_UserInfo;							//�û���Ϣ	
	tagServerUserScore				m_ScoreFormer;						//��ǰ����
	tagServerUserScore				m_ScoreVariation;					//�������
	tagVariationScore				m_RecordInfo;						//��¼��Ϣ

	//���ֱ���
protected:
	SCORE							m_lRevenue;							//��Ϸ˰��
	SCORE							m_lFreeScore;						//���ɻ���
	SCORE							m_lLockScore;						//��������
	SCORE							m_lFreezeScore;						//�������
	SCORE							m_lLockServiceScore;				//���������

	//״̬����
protected:
	WORD							m_wLastTableID;						//���ӱ��
	WORD							m_wLastChairID;						//���ӱ��
	DWORD							m_dwLookonTime;						//�Թ�ʱ��

	//λ����Ϣ
protected:
	double							m_dGlobalPosLat;					//��λ����
	double							m_dGlobalPosLng;					//��λγ��
	TCHAR							m_szPlaceName[LEN_PLACE_NAME];		//��ַ��Ϣ

	//Ȩ�ޱ���
protected:
	DWORD							m_dwUserRight;						//�û�Ȩ��
	DWORD							m_dwMasterRight;					//����Ȩ��

	//�ȼ���Ϣ
protected:
	BYTE							m_cbGameOrder;						//��Ϸ�ȼ�
	BYTE							m_cbMemberOrder;					//��Ա�ȼ�

	//״̬��Ϣ
protected:
	BYTE							m_cbShutUpStatus;					//����״̬
	DWORD							m_dwShutUpOverTime;					//���ʱ��

	//�ڰ�����
protected:
	WORD							m_wAddupWinRate;					//�ۼ�ʤ��
	LONGLONG						m_lAddupWinScore;					//�ۼ�Ӯ��
	LONGLONG						m_lAddupLoseScore;					//�ۼ����
	tagUserControlItem				m_UserControlItem;					//��������
	tagControlParameter*			m_pControlParameter;				//���Ʋ���

	//״̬����
protected:
	LONG							m_lNearInfo;						//�����Ϣ
	LONG							m_lNearCount;						//�����Ŀ
	DWORD							m_dwLogonTime;						//��¼ʱ��
	DWORD							m_dwPlayTimeCount;					//��Ϸʱ��
	DWORD							m_dwTotalPlayTimeCount;				//����Ϸʱ��
	DWORD							m_dwStartTimeCount;					//��Ϸ��ʼʱ��
	DWORD							m_dwPlayTimeVariation;				//��Ϸʱ��仯

	//��������
protected:
	bool							m_bMobileUser;						//�ֻ��û�
	bool							m_bTrusteeUser;						//ϵͳ�й�
	bool							m_bRobotUser;						//�����û�
	bool							m_bClientReady;						//���ӱ�־
	bool							m_bModifyScore;						//�޸ı�־
	TCHAR							m_szPassword[LEN_MD5];				//�û�����
	TCHAR							m_szTmpPassword[LEN_MD5];			//�û�����
	TCHAR							m_szMachineID[LEN_MACHINE_ID];		//��������

	//������Ϣ
protected:
	DWORD							m_dwMatchID;						//������ʶ
	SCORE							m_lMatchNo;							//��������
	WORD							m_wSignupCount;						//��������
	DWORD							m_dwTicketCount;					//��ȯ����	
	DWORD							m_dwSignUpTime;						//����ʱ��
	BYTE							m_cbMatchStatus;					//����״̬	

	//�ֻ�����
protected:
	WORD							m_wDeskPos;				             //��ǰ��ҳ
	WORD							m_wDeskCount;						 //��ҳ����
	WORD							m_wMobileUserRule;					 //�ֻ�����

	//��������
protected:
	WORD							m_wBindIndex;						//������
	DWORD							m_dwTokenID;						//��������
	DWORD							m_dwClientIP;						//���ӵ�ַ
	DWORD							m_dwInoutIndex;						//��������

	//�ͱ���Ϣ
public:
	tagSubsidyInfo					m_SubsidyInfo;						//�ͱ�����

	//������Ϣ
protected:
	WORD							m_wServerType;						//��������

	//�������
protected:
	CServerUserPack					m_ServerUserPack;					//�û�����	

	//����ӿ�
protected:
	IServerUserItemSink *			m_pIServerUserItemSink;				//�ص��ӿ�

	//��������
protected:
	//���캯��
	CServerUserItem();
	//��������
	virtual ~CServerUserItem();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//������Ϣ
public:
	//�û�����
	virtual WORD GetBindIndex() { return m_wBindIndex; }
	//��������
	virtual DWORD GetTokenID() { return m_dwTokenID; }
	//�û���ַ
	virtual DWORD GetClientIP() { return m_dwClientIP; }
	//�û���Ϣ
	virtual tagServerUserInfo * GetUserInfo() { return &m_UserInfo; }
	//�û�����
	virtual tagServerUserRule * GetUserRule() { return &m_UserRule; }

	//������Ϣ
public:
	//�û���ʶ
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//��Ϸ��ʶ
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//�û��ǳ�
	virtual LPCTSTR GetAccounts() { return m_UserInfo.szAccounts; }

	//״̬�ӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID() { return m_UserInfo.wTableID; }
	//���Ӻ���
	virtual WORD GetChairID() { return m_UserInfo.wChairID; }
	//�û�״̬
	virtual BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }

	//Ȩ����Ϣ
public:
	//�û�Ȩ��
	virtual DWORD GetUserRight() { return m_dwUserRight; } 
	//����Ȩ��
	virtual DWORD GetMasterRight() { return m_dwMasterRight; } 

	//��Ϸ�ȼ�
public:
	//��Ϸ�ȼ�
	virtual BYTE GetGameOrder() { return m_cbGameOrder; }
	//��Ϸ����
	virtual DWORD GetExperience() { return m_UserInfo.lExperience; }

	//��Ա�ȼ�
public:	
	//���µ���
	virtual VOID UpdateMemberPoint(DWORD dwIncPoint);
	//��Ա�ȼ�
	virtual BYTE GetMemberOrder() { return m_cbMemberOrder; }
	//����ȼ�
	virtual BYTE GetMasterOrder() { return m_UserInfo.cbMasterOrder; } 
	//��Ա����
	virtual DWORD GetMemberPoint() { return m_UserInfo.dwMemberPoint; }	

	//ʱ����Ϣ
public:
	//��¼ʱ��
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//��Ϸʱ��
	virtual DWORD GetPlayTimeCount() { return m_dwPlayTimeCount; }
	//����Ϸʱ��
	virtual DWORD GetTotalPlayTimeCount() { return m_dwTotalPlayTimeCount; }

	//������Ϣ
public:
	//��������
	virtual BYTE GetScoreKind() { return m_UserInfo.cbScoreKind; }
	//�û�����
	virtual SCORE GetUserScore() { return m_UserInfo.lScore; }
	//���ɻ���
	virtual SCORE GetUserFreeScore() { return m_lFreeScore-m_lLockServiceScore-m_lFreezeScore; }
	//��������
	virtual SCORE GetUserLockScore() { return m_lLockScore-m_lFreezeScore; }	

	//������Ϣ
public:
	//�û�ʤ��
	virtual WORD GetUserWinRate();
	//�û�����
	virtual WORD GetUserLostRate();
	//�û�����
	virtual WORD GetUserDrawRate();
	//�û�����
	virtual WORD GetUserFleeRate();
	//��Ϸ����
	virtual LONG GetUserPlayCount() { return m_UserInfo.lWinCount+m_UserInfo.lLostCount+m_UserInfo.lDrawCount+m_UserInfo.lFleeCount; }

	//Ч��ӿ�
public:
	//�Ա��ʺ�
	virtual bool ContrastAccounts(LPCTSTR pszAccounts);
	//�Ա�����
	virtual bool ContrastPassword(LPCTSTR pszPassword);
	//�Ա���ʱ����
	virtual bool ContrastTempPassword(LPCTSTR pszPassword);
	//�Ա���������
	virtual bool ContrastTablePassword(LPCTSTR pszPassword);

	//��Ϸ״̬
public:
	//��������
	virtual VOID SetClientReady(bool bClientReady);
	//����״̬
	virtual bool IsClientReady() { return m_bClientReady; };	

	//�й�״̬
public:
	//�ж�״̬
	virtual bool IsTrusteeUser() { return m_bTrusteeUser; }
	//����״̬
	virtual VOID SetTrusteeUser(bool bTrusteeUser) { m_bTrusteeUser=bTrusteeUser; }

	//�����û�
public:
	//����״̬
	virtual bool IsRobotUser() { return m_bRobotUser; }
	//���ÿ���
	virtual VOID SetRobotUser(bool bRobotUser) { m_bRobotUser=bRobotUser; }

	//�ֻ��û�
public:
	//����״̬
	virtual bool IsMobileUser() { return m_bMobileUser; }
	//���ÿ���
	virtual VOID SetMobileUser(bool bMobileUser) { m_bMobileUser=bMobileUser; }

	//�˺�״̬
public:
	//����״̬
	virtual BYTE GetShutUpStatus() { return m_cbShutUpStatus;  }
	//����״̬
	virtual VOID SetShutUpStatus(BYTE cbShutUpStatus) { m_cbShutUpStatus = cbShutUpStatus; }
	//���ʱ��
	virtual DWORD GetShutUpOverTime() { return m_dwShutUpOverTime;  }
	//����ʱ��
	virtual VOID SetShutUpOverTime(DWORD dwShutUpOverTime) { m_dwShutUpOverTime = dwShutUpOverTime;	}

	//�ڰ�����
public:
	//����״̬
	virtual BYTE GetRegulateStatus(SCORE lScore);
	//�������
	virtual bool IsExpectScoreChange(SCORE lScore);
	//��ǰʤ��
	virtual WORD GetAddupWinRate() { return m_wAddupWinRate; }
	//�ۼ�Ӯ��
	virtual SCORE GetAddupWinScore() { return m_lAddupWinScore; }
	//�ۼ����
	virtual SCORE GetAddupLoseScore() { return m_lAddupLoseScore; }	
	//����״̬
	virtual BYTE GetControlStatus() { return m_UserControlItem.cbControlStatus; }	
	//��������
	virtual tagUserControlItem* GetUserControlItem() { return &m_UserControlItem; }

	//�����ӿ�
public:	
	//������ʶ
	virtual DWORD GetMatchID() { return m_dwMatchID; }
	//������ʶ
	virtual VOID SetMatchID(DWORD dwMatchID) { m_dwMatchID=dwMatchID; }
	//��������
	virtual SCORE GetMatchNo() { return m_lMatchNo; }
	//������ʶ
	virtual VOID SetMatchNo(SCORE lMatchNo) { m_lMatchNo=lMatchNo; }	
	//��������
	virtual WORD GetSignupCount() { return m_wSignupCount; }
	//��������
	virtual VOID SetSignupCount(WORD wSignupCount) { m_wSignupCount = wSignupCount; }
	//����ʱ��
	virtual DWORD GetSignUpTime() { return m_dwSignUpTime; }
	//����ʱ��
	virtual VOID SetSignUpTime(DWORD dwSignUpTime) { m_dwSignUpTime=dwSignUpTime; }	
	//��ȯ����
	virtual DWORD GetTicketCount() { return m_dwTicketCount; }
	//��ȯ����
	virtual VOID SetTicketCount(DWORD dwTicketCount) { m_dwTicketCount = dwTicketCount; }
	//����״̬
	virtual BYTE GetUserMatchStatus() { return m_cbMatchStatus; }
	//����״̬
	virtual VOID SetUserMatchStatus(BYTE cbMatchStatus) { m_cbMatchStatus=cbMatchStatus; }

	//����ӿ�
public:
	//����״̬
	virtual bool SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID);
	//д��Ƹ�
	virtual bool WriteUserWealth(WORD wMoneyID, SCORE lScore, WORD wReason);	
	//д�����
	virtual bool WriteUserScore(SCORE lGameScore, SCORE lRevenue, SCORE lBetAmount, LONG lExperience, enScoreType ScoreType);
	//�޸Ļ���
	virtual VOID ModifyUserScore(SCORE lModifyScore, const tagLockScoreInfo& LockScoreInfo);
	//������ʱ����
	virtual VOID SetTmpPassword( LPCTSTR lpszPassword );	
	//�޸�Ȩ��
	virtual VOID ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight );
	//��ѯ��ʱ����
	virtual bool IsTmpPasswdEmpty() { return m_szTmpPassword[0] == 0; }

	//�����ӿ�
public:
	//�û�����
	virtual CServerUserPack* GetUserPack() { return &m_ServerUserPack; }

	//�߼��ӿ�
public:
	//����ж�
	virtual bool IsVariation() { return m_bModifyScore; }
	//���ò���
	virtual bool SetUserParameter(DWORD dwClientIP, WORD wBindIndex, DWORD dwTokenID, bool bRobotUser, bool bClientReady);
	//��ȡ���
	virtual bool DistillVariation(tagVariationScore & VariationScore, const tagLockScoreInfo & LockScoreInfo);
	//�������
	virtual bool FreezeUserScore( SCORE lScore, bool bNotify );

	//�����ӿ�
public:
	//������ʶ
	virtual LPCTSTR GetMachineID() { return m_szMachineID; }
	//��ȡ����
	virtual LPCTSTR GetPassword(){return m_szPassword;}

	//�ͱ��ӿ�
public:	
	//�ͱ���Ϣ
	virtual tagSubsidyInfo& GetSubsidyInfo() { return m_SubsidyInfo; }
	//ʣ�����
	virtual BYTE GetResidualTimes() { return m_SubsidyInfo.cbResidualTimes; }
	//���ͱ�ʶ
	virtual bool IsLargessSubsidy() { return m_SubsidyInfo.bLargessSubsidy; }
	//ʣ�����
	virtual void SetResidualTimes(BYTE cbTimes) { m_SubsidyInfo.cbResidualTimes=cbTimes; }
	//���ͱ�ʶ
	virtual void SetLargessSubsidy(bool bLargessSubsidy) { m_SubsidyInfo.bLargessSubsidy=bLargessSubsidy; }
	//�ͱ�����
	virtual SCORE GetSubsidyScore() { return m_UserInfo.lScore + m_UserInfo.lInsureGold + m_SubsidyInfo.lConsumeScore; };

	//λ����Ϣ
public:
	//��λ����
	virtual double GetGlobalPosLng() { return m_dGlobalPosLng; }
	//��λγ��
	virtual double GetGlobalPosLat() { return m_dGlobalPosLat; }	
	//��ַ��Ϣ
	virtual PTCHAR GetGlobalPlace() { return m_szPlaceName; }

	//�߼��ӿ�
public:
	//�����
	virtual bool DetachBindStatus();
	//��¼����
	virtual DWORD GetInoutIndex() { return m_dwInoutIndex; }
	//��ѯ��¼
	virtual bool QueryRecordInfo(tagVariationScore & UserRecordInfo);
	//�������ɻ���
	void SetFreeScore(SCORE lFreeScore){m_lFreeScore=lFreeScore;}
	//��������
	void SetLockScore(SCORE lLockScore){m_lLockScore=lLockScore;}

	//��������
public:
	//��ȡʱ��
	DWORD GetLookonTime() { return m_dwLookonTime; }
	//����ʱ��
	VOID SetLookonTime(DWORD dwLookonTime) { m_dwLookonTime = dwLookonTime;	}	
	//���ýӿ�
	VOID SetServerUserItemSink(IServerUserItemSink* pIServerUserItemSink);

	//��������
private:
	//��������
	VOID ResetUserItem();
	//����״̬
	VOID UpdateControlStatus(SCORE lScore,SCORE lBetAmount);

};

//////////////////////////////////////////////////////////////////////////

//�û�������
typedef CWHArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

//�û�������
class GAME_SERVICE_CLASS CServerUserManager : public IServerUserManager
{
	//������Ϣ
protected:
	tagControlParameter*			m_pControlParameter;				//���Ʋ���
	tagGameServiceOption *			m_pGameServiceOption;				//����ѡ��

	//�û�����
protected:
	CServerUserItemMap				m_UserIDMap;						//I D ����
	CServerUserItemArray			m_UserItemArray;					//�û�����
	CServerUserItemArray			m_UserItemStore;					//�洢�û�

	//����ӿ�
protected:
	IServerUserItemSink *			m_pIServerUserItemSink;				//�ص��ӿ�

	//��������
public:
	//���캯��
	CServerUserManager();
	//��������
	virtual ~CServerUserManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ú���
public:
	//���ýӿ�
	virtual bool SetServerUserItemSink(IUnknownEx * pIUnknownEx);
	//���Ʋ���
	virtual VOID SetControlParamter(tagControlParameter* pControlParameter);
	//�������
	virtual VOID SetServiceParameter(tagGameServiceOption* pGameServiceOption);	

	//���Һ���
public:
	//ö���û�
	virtual IServerUserItem * EnumUserItem(WORD wEnumIndex);
	//�����û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//�����û�
	virtual IServerUserItem * SearchUserItem(LPCTSTR pszAccounts);

	//ͳ�ƺ���
public:
	//��������
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }	

	//������
public:
	//ɾ���û�
	virtual bool DeleteUserItem();
	//ɾ���û�
	virtual bool DeleteUserItem(IServerUserItem* pIServerUserItem);
	//�����û�
	virtual IServerUserItem * ActiveUserItem(tagServerUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus, LPCTSTR pszPassword);	
};

//////////////////////////////////////////////////////////////////////////

#endif