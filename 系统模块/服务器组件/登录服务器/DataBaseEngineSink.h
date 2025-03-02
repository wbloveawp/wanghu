#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "redis++.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

using namespace redis;
using namespace lhchat;


//////////////////////////////////////////////////////////////////////////

//���ݿ⹳��
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//�������
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�
	CDataBaseAide					m_AccountsDBAide;					//�û����ݿ�
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�
	CDataBaseHelper					m_AccountsDBModule;					//�û����ݿ�

	//��������
protected:
	DBO_GP_LogonFailure				m_GPLogonFailure;					//��¼ʧ��
	DBO_GP_LogonSuccess				m_GPLogonSuccess;					//��¼�ɹ�
	DBO_MB_LogonFailure				m_MBLogonFailure;					//��¼ʧ��
	DBO_MB_LogonSuccess				m_MBLogonSuccess;					//��¼�ɹ�
	DBO_GP_UserIndividual			m_UserIndividual;					//�û�����

	//�������
protected:
	Redis *							m_pUserClient;						//�������
	Redis *							m_pFriendClient;					//�������
	Redis *							m_pTokenClient;						//�������
	Redis *							m_pMasterClient;					//�������

	//�ű�����
protected:
	string							m_sScriptDeleteFriend;				//ɾ������
	string							m_sScriptAppendFriend;				//��Ӻ���

	//�������
protected:
	tagServiceOption *				m_pServiceOption;					//��������
	CInitParameter *				m_pInitParameter;					//���ò���
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�

	//��������
public:
	//���캯��
	CDataBaseEngineSink();
	//��������
	virtual ~CDataBaseEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnDataBaseEngineStop(IUnknownEx * pIUnknownEx);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//ϵͳ����
protected:
	//��������
	bool OnRequestLoadConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//���ذ�
	bool OnRequestLoadRankingList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��������
	bool OnRequestResetUserData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//������Ϣ
	bool OnRequestOnLineCountInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	

	//��¼����
protected:
	//�ʺŵ�¼
	bool OnRequestPCLogonAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//�ʺ�ע��
	bool OnRequestPCRegisterAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//�˺ŷ���
protected:
	//��������
	bool OnRequestPCBindMachine(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��������
	bool OnRequestPCUnbindMachine(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸��ʺ�
	bool OnRequestPCModifyAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸�����
	bool OnRequestPCModifyLogonPass(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸�����
	bool OnRequestPCModifyInsurePass(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸�����
	bool OnRequestPCModifyIndividual(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//���÷���
protected:
	//��ѯ����
	bool OnRequestPCQueryGameConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��������
	bool OnRequestPCUpdateBasicConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��������
	bool OnRequestPCUpdateWeightConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��������
protected:
	//��ȡ����
	bool OnRequestPCQueryParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��Ӳ���
	bool OnRequestPCAppendParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸Ĳ���
	bool OnRequestPCModifyParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//ɾ������
	bool OnRequestPCDeleteParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//������
protected:
	//��ѯ���
	bool OnRequestPCQueryStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ӿ��
	bool OnRequestPCAppendStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸Ŀ��
	bool OnRequestPCModifyStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//ɾ�����
	bool OnRequestPCDeleteStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);


	//��ѯ����
protected:
	//��ѯ����
	bool OnRequestPCQueryIndividual(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//�ֻ���¼
protected:
	//�����½
	bool OnRequestMBLogonEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��֤���¼
	bool OnRequestMBLogonEMailCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��֤���¼
	bool OnRequestMBLogonMobileCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�ֻ���½
	bool OnRequestMBLogonMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�ο͵�¼
	bool OnRequestMBLogonVisitor(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�ʺŵ�¼
	bool OnRequestMBLogonAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	
	//��Կ��¼
	bool OnRequestMBLogonSecretKey(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//������¼
	bool OnRequestMBLogonThirdParty(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	
	//����ע�� 
	bool OnRequestMBRegisterEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�ʺ�ע�� 
	bool OnRequestMBRegisterAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	
	//�ֻ�ע�� 
	bool OnRequestMBRegisterMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	

	//�ֻ��ǳ�
protected:
	//�ʺŵǳ�
	bool OnRequestMBLogoutAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//�˺ŷ���
protected:
	//�������
	bool OnRequestMBLikePlayer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸�����
	bool OnRequestMBModifyLogonPass(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸�����
	bool OnRequestMBModifyIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸�ͷ��
	bool OnRequestMBModifySystemFace(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�޸�ͷ��
	bool OnRequestMBModifyCustomFace(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//ʵ����֤
	bool OnRequestMBRequestRealAuthent(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��Ʒ����
protected:
	//�һ���Ʒ
	bool OnRequestMBExchangeMallGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�һ������
	bool OnRequestMBExchangeGiftbagCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��֤����
protected:
	//������
	bool OnRequestMBBindEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�������
	bool OnRequestMBUnbindEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ȡ��֤��
	bool OnRequestMBAcquireAuthCode(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//���ֻ���
	bool OnRequestMBBindMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//����ֻ���
	bool OnRequestMBUnbindMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	
	//��������
	bool OnRequestMBResetLogonPasswd(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

    //������
protected:
	//����ǩ��
	bool OnRequestMBRequestCheckIn(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
    //��������
    bool OnRequestMBBindInviteCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//�ύ����
	bool OnRequestMBSubmitInformInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//���˺�
	bool OnRequestMBBindExternalAccount(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//ת�˷���
protected:
	//����ת��
	bool OnRequestMBWealthTranser(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//Ӷ�����
protected:
	//ȡ��Ӷ��
	bool OnRequestMBDrawCommission(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��������
protected:
    //��������
    bool OnRequestMBSendGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
    //��������
    bool OnRequestMBLoadGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
    //����״̬����
    bool OnRequestMBGiftStatusUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��ѯ����
protected:
	//��ѯ�Ƹ�
	bool OnRequestMBQueryWealth(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ѯ��λ
	bool OnRequestMBQueryRankingInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
    //��ѯ����
    bool OnRequestMBQueryLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//��ѯ�ͱ�
	bool OnRequestMBQuerySubsidy(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//ת�̳齱
protected:
	//��ѯ�齱
	bool OnRequestMBQueryTurntableLottery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//����齱
	bool OnRequestMBRequestTurntableLottery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//���й���
protected:
	//���в�ѯ
	bool OnRequestMBRequestInsureQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//���п�ͨ
	bool OnRequestMBRequestInsureEnable(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//���д洢
	bool OnRequestMBRequestInsureSave(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//������ȡ
	bool OnRequestMBRequestInsureTake(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//����ת��
	bool OnRequestMBRequestInsureTransfer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//������������
	bool OnRequestMBInsureResetPassword(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//��¼����
protected:
	//���ر���
	bool LoadUserPackGoods();
	//��λ��Ϣ
	bool LoadUserRankingInfo();

	//���غ���
private:
	//��Ϸ�б�
	bool LoadGameList(DWORD dwContextID, DWORD dwTokenID);
	//���дʻ�
	bool LoadFilterWords(DWORD dwContextID, DWORD dwTokenID);
	//��Ϸ�汾
	bool LoadPlazaVersion(DWORD dwContextID, DWORD dwTokenID);
	//ת������
	bool LoadTransferParemter(DWORD dwContextID, DWORD dwTokenID);
	//ȫ������
	bool LoadGlobalOption(DWORD dwContextID, DWORD dwTokenID, DWORD dwOptionMask);
	//�ǳ�����
	bool LoadNickNameOption(DWORD dwContextID, DWORD dwTokenID);

	//��������
protected:
	//������Ϣ
	bool CleanOnlineInfo(WORD wLogonID);
	//������Ϣ
	bool CacheUserInfo(DWORD dwContextID, DWORD dwTokenID);	
	//���»���
	bool UpdateUserCache(DWORD dwContextID, DWORD dwTokenID, UserPublicInfo& PublicInfo);
	//����Token
	bool BuildLogonToken(DWORD dwUserID, LPCTSTR pszAccounts, LPCTSTR pszSecretKey, TCHAR szUserToken[LEN_LOGON_TOKEN]);

	//��������
protected:	
	//���²Ƹ�
	bool UpdateUserWealth(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID);
	//������Ʒ
	bool UpdateUserGoods(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID, tagGoodsItem GoodsItemList[], WORD wGoodsCount);
    //��������
    bool UpdateUserLoveLiness(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID);

	//��渨��
protected:
	//�����ʳ�����	
	WORD ParseJackpotOption(LPCSTR szJackpotOption, tagJackpotItem JackpotItem[], WORD wBufferCount);
	//���л��ʳ�
	bool SerializeJackpotOption(tagJackpotItem JackpotItem[], WORD wItemCount, TCHAR szJackpotOption[], WORD wBufferSize);
	//���л�Ȩ��
	bool SerializeWeightConfig(tagTimesWeightItem WeightItem[], WORD wItemCount, TCHAR szWeightConfig[], WORD wBufferSize);

	//��������
protected:		
	//����쳣
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine, IDataBaseException * pIException);
	//�����
	VOID OnStockDisposeResult(DWORD dwContextID, DWORD dwTokenID, DWORD dwErrorCode, WORD wSubCommdID);
	//�������
	VOID OnRobotDisposeResult(DWORD dwContextID, DWORD dwTokenID, DWORD dwErrorCode,WORD wSubCommdID,WORD wServerID);	
	//��¼���
	VOID OnLogonDisposeResult(DWORD dwContextID, DWORD dwTokenID, LONG lResultCode,LPCTSTR pszErrorString, bool bMobileClient=false,bool isThirdLogon=false);
};

//////////////////////////////////////////////////////////////////////////

#endif