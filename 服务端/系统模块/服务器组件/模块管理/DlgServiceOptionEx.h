#ifndef SERVICEEX_DLG_HEAD_FILE
#define SERVICEEX_DLG_HEAD_FILE

#pragma once

#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////
//���鶨��
typedef CArrayTemplate<tagServiceOptionEx *>	CServiceOptionExArray;

//////////////////////////////////////////////////////////////////////////////////

//���ô���
class MODULE_MANAGER_CLASS CDlgServiceOptionEx : public CDialog
{
	//��ʶ����
protected:
	tagServiceOption				m_ServiceOption;					//����ѡ��
	bool							m_bInitDirectory;					//��־����
	TCHAR							m_szDataFileName[32];				//�ļ�����

	//״̬����
protected:
	int								m_nSelectPortItem;					//����ѡ��
	int								m_nSelectServiceItem;				//����ѡ��	

	//�洢����
protected:
	CServiceOptionExArray			m_ServiceOptionActive;				//��������
	CServiceOptionExArray			m_ServiceOptionBuffer;				//��������

	//��������
public:
	//���캯��
	CDlgServiceOptionEx();
	//��������
	virtual ~CDlgServiceOptionEx();

	//���غ���
protected:
	//���뺯��
	virtual BOOL OnInitDialog();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);

	//���ܺ���
public:
	//�ļ�����
	VOID SetDataFileName(LPCTSTR pszFileName);
	//��ȡ����
	tagServiceOption * GetServiceOption() { return &m_ServiceOption; }

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
	//��ʼ�б�
	VOID InitControl();
	//ѡ������
	VOID SelectServiceItem(int nIndex);
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