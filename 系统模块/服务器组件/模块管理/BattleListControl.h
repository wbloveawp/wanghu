#ifndef BATTLE_LIST_CONTROL_HEAD_FILE
#define BATTLE_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ListControl.h"
#include "ModuleInfoManager.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CArrayTemplate<tagBattleOption *> CBattleOptionArray;

//��������
typedef CMap<WORD,WORD,tagBattleOption *,tagBattleOption *> CBattleOptionMap;

//////////////////////////////////////////////////////////////////////////

//Լս����
class MODULE_MANAGER_CLASS CBattleOptionBuffer
{
	//��������
public:
	CBattleOptionMap				m_BattleOptionMap;					//Լս����
	CBattleOptionArray				m_BattleOptionArray;				//Լս����

	//��������
public:
	//���캯��
	CBattleOptionBuffer();
	//��������
	virtual ~CBattleOptionBuffer();

	//������
public:
	//��������
	bool ResetBattleOption();
	//ɾ������
	bool DeleteBattleOption(WORD wBattleID);
	//��������
	bool InsertBattleOption(tagBattleOption * pBattleOption);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetBattleOptionCount();
	//��������
	tagBattleOption * SearchBattleOption(WORD wBattleID);

	//�ڲ�����
private:
	//��������
	tagBattleOption * CreateBattleOption();
};

//////////////////////////////////////////////////////////////////////////

//Լս�б�
class MODULE_MANAGER_CLASS CBattleListControl : public CListControl
{
	//�洢����
protected:
	CBattleOptionBuffer *				m_pBattleOptionBuffer;			//Լս����

	//��������
public:
	//���캯��
	CBattleListControl();
	//��������
	virtual ~CBattleListControl();

	//���غ���
public:
	//�����б�
	virtual VOID InitListControl();
	//��������
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//��ȡ��ɫ
	virtual VOID GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//���ܺ���
public:	
	//�����б�
	bool InsertModuleInfo(tagGameModuleInfo * pGameModuleInfo);	
	//���û���
	void SetBattleOptionBuffer(CBattleOptionBuffer * pBattleOptionBuffer);

	//��������
private:
	//��������
	WORD GetInsertIndex(tagGameModuleInfo * pGameModuleInfo);
	//�����ַ�
	LPCTSTR GetDescribeString(tagGameModuleInfo * pGameModuleInfo, WORD wColumnIndex);
};

//////////////////////////////////////////////////////////////////////////

//Լս��Ϣ
class MODULE_MANAGER_CLASS CBattleOptionManager
{
	//��������
public:
	//���캯��
	CBattleOptionManager();
	//��������
	virtual ~CBattleOptionManager();

	//���ݹ���
public:
	//�޸�����
	bool UpdateBattleOption(tagBattleOption & BattleOption);
	//��������
	bool InsertBattleOption(tagBattleOption & BattleOption);
	//��������
	bool LoadBattleOption(CBattleOptionBuffer & BattleOptionBuffer);	
};

//////////////////////////////////////////////////////////////////////////

//Լս����
class MODULE_MANAGER_CLASS CDlgBattleOption : public CDialog
{
	//�б����
protected:
	CModuleInfoBuffer				m_ModuleInfoBuffer;					//ģ����Ϣ	
	CModuleInfoManager				m_ModuleInfoManager;				//ģ�����

	//�б����
protected:	
	CBattleListControl				m_BattleListControl;				//Լս�б�
	CBattleOptionBuffer				m_BattleOptionBuffer;				//Լս��Ϣ	
	CBattleOptionManager			m_BattleOptionManager;				//Լս����

	//���ñ���
public:
	tagBattleOption					m_BattleOption;						//Լս����

	//�������
protected:
	tagGameModuleInfo *				m_pGameModuleInfo;					//ѡ��ģ��

	//��������
public:
	//���캯��
	CDlgBattleOption();
	//��������
	virtual ~CDlgBattleOption();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//���ܺ���
protected:
	//����ģ��
	bool LoadDBModuleItem();

	//�ؼ���Ϣ
protected:
	//�����б�
	VOID OnNMlclkModuleList(NMHDR * pNMHDR, LRESULT * pResult);
	//˫���б�
	VOID OnNMlDBclkModuleList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif