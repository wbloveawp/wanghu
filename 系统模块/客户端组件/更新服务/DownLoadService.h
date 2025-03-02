#ifndef DOWN_LOAD_SERVICE_HEAD_FILE
#define DOWN_LOAD_SERVICE_HEAD_FILE

#pragma once

#include "DlgDownLoad.h"
#include "DownLoadMission.h"
#include "UpdateServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//���ط���
class CDownLoadService : public IDownLoadService
{
	//��Ԫ����
	friend class CDlgDownLoad;
	friend class CDownLoadMission;

	//��������
protected:
	DWORD							m_dwMissionID;						//�����ʶ

	//�������
protected:
	CDlgDownLoadArray				m_DlgDownLoadActive;				//���ش���
	CDlgDownLoadArray				m_DlgDownLoadRelease;				//���ش���
	CDownLoadMissionArray			m_DownLoadMissionActive;			//��������
	CDownLoadMissionArray			m_DownLoadMissionRelease;			//��������
	CDlgDownLoad *					m_pDlgDownLoad;						//�������

	IPlazaResource					*m_pPlazaResource;					//��Դ�ӿ�
	HINSTANCE						m_hInstance;						//��Դ���
	//��̬����
protected:
	static CDownLoadService *		m_pDownLoadService;					//���ط���

	//��������
public:
	//���캯��
	CDownLoadService();
	//��������
	virtual ~CDownLoadService();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ؽӿ�
public:
	//��������
	virtual DWORD __cdecl RequestDownLoad(const tagMissionRequest & MissionRequest);
	//��������
	virtual DWORD __cdecl RequestDownLoad(const tagDownLoadRequest & DownLoadRequest,DWORD dwPlazaStationID,DWORD dwUserStationID);
	//������Դ
	virtual	void __cdecl SetResourceInterface(IPlazaResource *pPlazaResource,HINSTANCE hInstance){m_pPlazaResource=pPlazaResource,m_hInstance=hInstance;}

	//�ͷź���
protected:
	//�ͷ�����
	bool ReleaseDownLoad(CDlgDownLoad * pDlgDownLoad);
	//�ͷ�����
	bool ReleaseDownLoad(CDownLoadMission * pDownLoadMission);

	//���Һ���
protected:
	//��������
	CDlgDownLoad * SearchDownLoad(const tagDownLoadRequest & DownLoadRequest);
	//��������
	CDownLoadMission * SearchDownLoad(const tagMissionRequest & MissionRequest);

	//��̬����
public:
	//��ȡʵ��
	static CDownLoadService * GetInstance() { return m_pDownLoadService; }
};

//////////////////////////////////////////////////////////////////////////

#endif