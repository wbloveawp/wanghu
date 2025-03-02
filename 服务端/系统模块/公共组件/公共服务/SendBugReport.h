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
	virtual bool SendMail(	const CString &strSmtpSvrIp , //比如smtp.163.com
		const CString &strSmtpUserName , //Smtp的登录用户名
		const CString &strSmtpPwd , // Smtp的密码
		const CString &strSendUserName ,//发件人姓名（不支持中文）
		const CString &strSendUserMailAddr,//发件人邮件地址
		const CString &strRecvUserName ,//收件人姓名（不支持中文）
		const CString &strRecvMailAddr ,// 接收者的邮件地址
		const CString &strMailTitle ,//邮件标题
		const CString &strMailBodyText ,//邮件主体文本
		const CString &strAttachFileName //附件地址
		) ;

	//创建一个新建发送体
	CSMTPMessage* CreateMessage(
		const CString &strSendUserName ,
		const CString &strSendUserMailAddr , //发送者的邮件地址
		const CString &strRecvUserName ,//接受者姓名地址
		const CString &strMailTitle ,//邮件标题
		const CString &strMailBodyText ,//邮件正文文本
		const CString &strAttachFileName //附件地址
		);
};

#endif