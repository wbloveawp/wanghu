#ifndef GLOBAL_STATION_HEAD_FILE
#define GLOBAL_STATION_HEAD_FILE

#pragma once

#include "PlatformDataHead.h"

//////////////////////////////////////////////////////////////////////////

//վ������
class PLATFORM_DATA_CLASS CGlobalStation
{
	//վ�����
protected:
	DWORD							m_dwStationID;						//վ���ʶ
	TCHAR							m_szStationName[32];				//վ������
	TCHAR							m_szStationHome[128];				//ƽ̨ҳ��
	TCHAR							m_szStationPlaza[256];				//�㳡ҳ��
	CWHArray<DWORD,DWORD>			m_CompStationArr;					//����վ��

	//��Դ����
protected:
	HICON							m_hStationIcon;						//ƽ̨ͼ��
	CImage							m_ImagePublicize;					//����ͼƬ
	CPngImageWarpper 				m_ImageStationLogo;					//ƽ̨��־
	CImage							m_ImagePlazaPublicize;				//��������ͼƬ

	//��̬����
protected:
	static CGlobalStation *			m_pGlobalStation;					//վ����Ϣ

	//��������
public:
	//���캯��
	CGlobalStation();
	//��������
	virtual ~CGlobalStation();

	//ƽ̨����
public:
	//վ���ʶ
	DWORD GetStationID();
	//վ������
	LPCTSTR GetStationName();
	//����վ��
	bool IsCompStation(DWORD dwStationID);

	//վ����Դ
public:
	//վ��ͼ��
	HICON GetStationIcon();
	//����ͼƬ
	CImage & GetPublicizeImage();
	//ƽ̨��־
	CPngImageWarpper & GetStationLogoImage();
	//�������
	CImage & GetPlazaPublicizeImage();

	//����ҳ��
public:
	//վ����ҳ
	LPCTSTR GetStationHomeLink();
	//վ������
	LPCTSTR GetStationPlazaLink();

	//ȫ��ҳ��
public:
	//�ɼ�ҳ��
	LPCTSTR GetCollectScore(TCHAR szBrowseUrl[], UINT uCharCount);
	//��������
	LPCTSTR GetNativeIniLink(WORD wLinkID, TCHAR szBrowseUrl[], UINT uCharCount);
	//��������
	LPCTSTR GetNavigationLink(DWORD dwIndexID, TCHAR szBrowseUrl[], UINT uCharCount);

	//��Ϸҳ��
public:
	//����ҳ��
	LPCTSTR GetRedirectionRule(WORD wKindID, TCHAR szBrowseUrl[], UINT uCharCount);
	//����ҳ��
	LPCTSTR GetRedirectionChart(WORD wKindID, TCHAR szBrowseUrl[], UINT uCharCount);
	//����ҳ��
	LPCTSTR GetRedirectionCustom(WORD wCustomID, TCHAR szBrowseUrl[], UINT uCharCount);

	//ͨ��ҳ��
public:
	//��תҳ��
	LPCTSTR GetRedirectionLink(DWORD dwResourceID, TCHAR szBrowseUrl[], UINT uCharCount);
	//��תҳ��
	LPCTSTR GetRedirectionLink(DWORD dwResourceID, WORD wKindID, WORD wServerID, TCHAR szBrowseUrl[], UINT uCharCount);

	//����ҳ��
public:
	//����ҳ��
	LPCTSTR GetRedirectionServer( DWORD dwServerType, TCHAR szBrowseUrl[], UINT uCharCount );

	//������Դ
public:
	//��ȡ��Դ
	LPCTSTR GetCustomString( BOOL bPlatform, LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpRetString, DWORD dwSize );

	//�ڲ�����
private:
	//վ���ļ�
	LPCTSTR GetStationFile(TCHAR szFileName[], UINT uCharCount);
	//ƽ̨�ļ�
	LPCTSTR GetPlatformFile(TCHAR szFileName[], UINT uCharCount);
	
	//��̬����
public:
	//��ȡ����
	static CGlobalStation * GetInstance() { return m_pGlobalStation; }
};

//////////////////////////////////////////////////////////////////////////

#endif