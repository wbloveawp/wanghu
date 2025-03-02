#ifndef SERVICE_DLG_HEAD_FILE
#define SERVICE_DLG_HEAD_FILE

#pragma once

#include "ModuleManagerHead.h"
#include "ModuleInfoManager.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////
//���鶨��
typedef CWHArray<tagServiceKind> CServiceKindArray;
typedef CWHArray<tagServiceOption *>	CServiceOptionArray;

//////////////////////////////////////////////////////////////////////////////////

//���ô���
class MODULE_MANAGER_CLASS CDlgServiceOption : public CDialog
{
	//��ʶ����
protected:
	tagServiceOption				m_ServiceOption;					//����ѡ��
	bool							m_bInitDirectory;					//��־����

	//���ñ���
protected:
	bool							m_bOptionModule;					//����ģ��
	TCHAR							m_szDataFileName[32];				//�ļ�����

	//״̬����
protected:
	int								m_nSelectPortItem;					//����ѡ��
	int								m_nSelectServiceItem;				//����ѡ��	

	//���ñ���
protected:
	CServiceKindArray				m_ServiceKindArray;					//��������

	//�б����
protected:
	CModuleInfoBuffer				m_ModuleInfoBuffer;					//ģ����Ϣ	
	CModuleInfoManager				m_ModuleInfoManager;				//ģ�����

	//�洢����
protected:
	CServiceOptionArray				m_ServiceOptionActive;				//��������
	CServiceOptionArray				m_ServiceOptionBuffer;				//��������

	//��������
public:
	//���캯��
	CDlgServiceOption();
	//��������
	virtual ~CDlgServiceOption();

	//���غ���
protected:
	//���뺯��
	virtual BOOL OnInitDialog();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);

	//���ú���
public:
	//��������
	VOID SetServiceKind(tagServiceKind ServiceKind[], WORD wKindCount);	

	//���ܺ���
public:
	//���ñ�ʶ
	VOID SetOptionModule(bool bOptionModule);
	//�ļ�����
	VOID SetDataFileName(LPCTSTR pszFileName);
	//��ȡ����
	tagServiceOption * GetServiceOption();
	//��ȡ����
	tagServiceOption * GetServiceOption(WORD wServiceID);

	//��������
protected:
	//��������
	bool LoadServiceOption();
	//��������
	bool SaveServiceOption();

	//��������
protected:
	//�������
	tagServiceOption * ActiveServiceOption();
	//�ͷŶ���
	void FreeServiceOption(tagServiceOption * pServiceOption);

	//��������
private:
	//���ñ�ʶ
	bool SetServiceID(WORD wServiceID);
	//��ȡ��ʶ
	bool GetServiceID(WORD & wServiceID);

	//��������
private:
	//��ʼ�б�
	VOID InitControl();
	//�����б�
	VOID LoadDBModuleItem();
	//ѡ������
	VOID SelectServiceItem(int nIndex);
	//��ȡ·��
	VOID GetDataFilePath(TCHAR szFilePath[], WORD wBufferCount);
	//���ÿؼ�
	VOID SetServiceToControl(tagServiceOption *pServiceOption,CListCtrl * pListCtrl,int nIndex);	

	//�������
protected:
	//�˳�����
	VOID OnBnClickedQuit();	
	//�������
	VOID OnBnClickedAddServiceInfo();	
	//�޸�����
	VOID OnBnClickedModifyServiceInfo();
	//ɾ������
	VOID OnBnClickedDeleteServiceInfo();		

	//�ؼ�����
public:		
	//��������
	VOID OnHdnItemclickListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult);	
	//˫������
	VOID OnHdnItemDblclkListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult);
	//���̰���
	VOID OnLvnItemKeydownListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////////////

#endif