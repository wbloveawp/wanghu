#ifndef GLOBAL_OPTION_MANAGER_HEAD_FILE
#define GLOBAL_OPTION_MANAGER_HEAD_FILE

#pragma once

#include "ConfigServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������
class CGlobalOptionItem;
class CCheckInOption;
class CTurntableOption;
class CMarqueeNoticeOption;

//////////////////////////////////////////////////////////////////////////////////
//���Ͷ���

//���Ͷ���
typedef CWHAutoPtr<CCheckInOption> CCheckInOptionPtr;
typedef CWHAutoPtr<CTurntableOption> CTurntableOptionPtr;
typedef CWHAutoPtr<CGlobalOptionItem> CGlobalOptionItemPtr;
typedef CWHAutoPtr<CMarqueeNoticeOption> CMarqueeNoticeOptionPtr;

//���Ͷ���
typedef CWHArray<CGlobalOptionItem>	CGlobalOptionArray;
typedef CWHArray<tagNoticeRollOption> CNoticeRollOptionArray;

//���Ͷ���
typedef CMap<DWORD, DWORD, CGlobalOptionItemPtr, CGlobalOptionItemPtr>	CGlobalOptionItemMap;

//////////////////////////////////////////////////////////////////////////////////
//��������
class CONFIG_SERVICE_CLASS CGlobalOptionItem : public CHandleBase
{
	friend class CGlobalOptionManager;

	//�ڱ�����
protected:
	DWORD							m_dwOptionMask;						//��������
	CString							m_strOptionValue;					//����ѡ��	
	TCHAR							m_szOptionDigest[LEN_MD5];			//����ժҪ

	//��������
public:
	//���캯��
	CGlobalOptionItem();
	//��������
	virtual ~CGlobalOptionItem();

	//��������
public:
	//��������
	DWORD GetOptionMask() { return m_dwOptionMask; }
	//����ժҪ
	LPCTSTR GetOptionDigest() { return m_szOptionDigest; }
	//��������
	LPCTSTR GetOptionValue() { return m_strOptionValue.GetString(); }

	//���ܺ���
public:
	//��������
	bool ConstructOptionItem(DWORD dwOptionMask, LPCTSTR pszOptionValue);

	//��ֵ����
private:
	//��������ѡ��
	virtual bool ParseOptionItem(LPCTSTR pszOptionValue);
};

//////////////////////////////////////////////////////////////////////////////////
//ǩ��ѡ��
class CONFIG_SERVICE_CLASS CCheckInOption : public CGlobalOptionItem
{
	friend class CGlobalOptionManager;

	//����ѡ��
protected:
	INT								m_nCheckInEnabled;							//ǩ������	
	TCHAR							m_szCheckInRewards[2048];					//ǩ������

	//��������
public:
	//���캯��
	CCheckInOption();
	//��������
	virtual ~CCheckInOption();

	//����ѡ��
public:
	//ǩ������
	bool IsCheckInEnabled() { return m_nCheckInEnabled > 0; }
	//ǩ������
	LPCTSTR GetCheckInRewards() { return m_szCheckInRewards; }

	//��ֵ����
private:
	//��������ѡ��
	virtual bool ParseOptionItem(LPCTSTR pszOptionValue);
};


//////////////////////////////////////////////////////////////////////////////////
//ת������
class CONFIG_SERVICE_CLASS CTurntableOption : public CGlobalOptionItem
{
	friend class CGlobalOptionManager;

	//����ѡ��
protected:
	INT								m_TurntableEnabled;						//ת�̿���	
	TCHAR							m_szTurntableAwards[2048];					//ת�̽���

	//��������
public:
	//���캯��
	CTurntableOption();
	//��������
	virtual ~CTurntableOption();

	//����ѡ��
public:	
	//ת�̿���
	bool IsTurntableEnabled() { return m_TurntableEnabled; }	
	//ת�̽���
	LPCTSTR GetTurntableAwards() { return m_szTurntableAwards; }

	//��ֵ����
private:
	//��������ѡ��
	virtual bool ParseOptionItem(LPCTSTR pszOptionValue);
};

//////////////////////////////////////////////////////////////////////////////////

//���������
class CONFIG_SERVICE_CLASS CMarqueeNoticeOption : public CGlobalOptionItem
{
	friend class CGlobalOptionManager;

	//��¼֪ͨ
protected:
	WORD							m_wReachRankID;						//���α�ʶ
	CByteArray						m_ListTypeArray;					//������

	//��Ϸ��ʤ֪ͨ
protected:
	SCORE							m_lReachWinScore;					//Ӯȡ����

	//�����н�֪ͨ
protected:
	SCORE							m_lReachAwardAmount;				//�������

	//����֪ͨ
protected:
	SCORE							m_lReachWithdrawAmount;				//���ֽ��

	//����ѡ��
protected:	
	DWORD							m_dwDurationTime;					//����ʱ��
	CNoticeRollOptionArray			m_NoticeRollOptionArray;			//��������

	//��������
public:
	//���캯��
	CMarqueeNoticeOption();
	//��������
	virtual ~CMarqueeNoticeOption();

	//֪ͨ����
public:
	//������ʶ
	WORD GetReachRankID() { return m_wReachRankID; }
	//Ӯȡ����
	SCORE GetReachWinScore() { return m_lReachWinScore; }
	//�������
	SCORE GetReachAwardAmount() { return m_lReachAwardAmount; }
	//���ֽ��
	SCORE GetReachWithdrawAmount() { return m_lReachWithdrawAmount;	}
	//������
	INT GetListTypeArray(BYTE cbLisType[], WORD wBufferSize);

	//��������
public:
	//����ʱ��
	DWORD GetDurationTime() { return m_dwDurationTime; }	
	//��������
	tagNoticeRollOption * GetNoticeRollOption(BYTE cbNoticeKind);

	//��ֵ����
private:
	//��������ѡ��
	virtual bool ParseOptionItem(LPCTSTR pszOptionValue);
};

//////////////////////////////////////////////////////////////////////////////////


//���ù���
class CONFIG_SERVICE_CLASS CGlobalOptionManager : public CSingleton<CGlobalOptionManager>
{	
	//�洢����
protected:
	CGlobalOptionItemMap			m_GlobalOptionItemMap;					//�����ֵ�		
	CGlobalOptionItemMap			m_GlobalOptionTempMap;					//��ʱ����

	//��������
public:
	//���캯��
	CGlobalOptionManager();
	//��������
	virtual ~CGlobalOptionManager();

	//������
public:
	//��������
	VOID ResetData();

	//�ڱ�����
public:	
	//�л�����
	VOID SwitchOptionItem(DWORD dwOptionMask);
	//�Ƴ�����
	VOID RemoveOptionItem(DWORD dwOptionMask);
	//�������
	bool AppendOptionItem(DWORD dwOptionMask, LPCTSTR pszOptionValue);	

	//���ù���
public:
	//��ȡ����
	template <class TYPE> TYPE GetOptionItem(DWORD dwOptionMask);

	//��������
private:	
	//��������
	CGlobalOptionItemPtr CreateGlobalOptionItem(DWORD dwOptionMask);
};

//////////////////////////////////////////////////////////////////////////////////

//��������
template <class TYPE> TYPE CGlobalOptionManager::GetOptionItem(DWORD dwOptionMask)
{
	//������Ʒ
	CGlobalOptionItemPtr GlobalOptionItemPtr;
	if (m_GlobalOptionItemMap.Lookup(dwOptionMask, GlobalOptionItemPtr) == FALSE)
	{
		return NULL;
	}

	return (typename TYPE::element_type*)GlobalOptionItemPtr.get();
}

//////////////////////////////////////////////////////////////////////////////////

#define g_GlobalOptionManager	(CGlobalOptionManager::getInstance())

//////////////////////////////////////////////////////////////////////////////////


#endif