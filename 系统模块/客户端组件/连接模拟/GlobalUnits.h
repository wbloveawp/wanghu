#pragma once

#include "Stdafx.h"
#include "SimulateLinkHead.h"

//////////////////////////////////////////////////////////////////////////

//ȫ�ֵ�Ԫ
class CGlobalUnits
{
	//״̬����
public:
	BYTE							m_cbTestMode;						//����ģʽ
	BYTE							m_cbConnectMode;					//����ģʽ	
	bool							m_bConnectStatus;					//����״̬

	//ȫ������
public:
	WORD							m_wKindID;							//���ͱ�ʶ
	WORD							m_wServerType;						//��������
	WORD							m_wServerPort;						//����˿�
	WORD							m_wServerLevel;						//����ȼ�
	TCHAR							m_szServerAddr[32];					//�����ַ	
	DWORD							m_dwProcessVersion;					//���̰汾

	//��������
public:
	//���캯��
	CGlobalUnits();
	//��������
	virtual ~CGlobalUnits();
};

//////////////////////////////////////////////////////////////////////////

//ȫ�ֵ�Ԫ
extern CGlobalUnits g_GlobalUnits;

//////////////////////////////////////////////////////////////////////////
