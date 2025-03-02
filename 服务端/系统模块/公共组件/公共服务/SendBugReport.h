#ifndef SEND_BUG_REPORT_HEAD_FILE
#define SEND_BUG_REPORT_HEAD_FILE

#pragma once

#include "CommonServiceHead.h"

class CSMTPMessage;

class COMMON_SERVICE_CLASS CSendBugReport
{
public:
	CSendBugReport(void);
	virtual ~CSendBugReport(void);

public:
	virtual bool SendMail(	const CString &strSmtpSvrIp , //����smtp.163.com
		const CString &strSmtpUserName , //Smtp�ĵ�¼�û���
		const CString &strSmtpPwd , // Smtp������
		const CString &strSendUserName ,//��������������֧�����ģ�
		const CString &strSendUserMailAddr,//�������ʼ���ַ
		const CString &strRecvUserName ,//�ռ�����������֧�����ģ�
		const CString &strRecvMailAddr ,// �����ߵ��ʼ���ַ
		const CString &strMailTitle ,//�ʼ�����
		const CString &strMailBodyText ,//�ʼ������ı�
		const CString &strAttachFileName //������ַ
		) ;

	//����һ���½�������
	CSMTPMessage* CreateMessage(
		const CString &strSendUserName ,
		const CString &strSendUserMailAddr , //�����ߵ��ʼ���ַ
		const CString &strRecvUserName ,//������������ַ
		const CString &strMailTitle ,//�ʼ�����
		const CString &strMailBodyText ,//�ʼ������ı�
		const CString &strAttachFileName //������ַ
		);
};

#endif