#include "StdAfx.h"
#include ".\sendbugreport.h"
#include "Smtp.h" 


CSendBugReport::CSendBugReport(void)
{
}

CSendBugReport::~CSendBugReport(void)
{
}



bool CSendBugReport::SendMail(	const CString &strSmtpSvrIp , //比如smtp.163.com
							   const CString &strSmtpUserName , //Smtp的登录用户名
							   const CString &strSmtpPwd , // Smtp的密码
							   const CString &strSendUserName ,//发件人姓名
							   const CString &strSendUserMailAddr,//发件人邮件地址
							   const CString &strRecvUserName ,//收件人姓名
							   const CString &strRecvMailAddr ,// 接收者的邮件地址
							   const CString &strMailTitle ,//邮件标题
							   const CString &strMailBodyText ,//邮件主体文本
							   const CString &strAttachFileName //附件地址
							   ) 
{
	//首先判断SMTP服务器是否为空
	if ( strSmtpSvrIp.IsEmpty())
	{
		return false;
	}
	else 
	{

		//创建SMTP信息体，通过类来实现
		CString strRecvUser ;
		strRecvUser.Format(_T("\"%s\" <%s>") , strRecvUserName , strSendUserMailAddr) ;
		CSMTPMessage* pMessage = CreateMessage( strSendUserName ,
			strSendUserMailAddr,
			strRecvUser ,
			strMailTitle ,
			strMailBodyText ,
			strAttachFileName );
		//连接到服务器
		CSMTPConnection connection;
		//		UpdateData(true);
		//		m_IP=_T("");
		BOOL bConnect = connection.Connect(strSmtpSvrIp, 
			CSMTPConnection::AuthLoginMethod , 
			strSmtpUserName ,strSmtpPwd,
			25 , TEXT(""));
		//判断连接是否成功
		if (!bConnect) 
		{
			AfxMessageBox(TEXT("不能连接到服务器!"));
		}
		else 
		{
			//发送具体信息
			if (!connection.SendMessage(*pMessage)) 
			{
				AfxMessageBox(TEXT("发送信息失败!"));
			}
		}
		//删除消息体
		delete pMessage;
	}	

	return true ;
}

//创建一个新建发送体
CSMTPMessage* CSendBugReport::CreateMessage(
							const CString &strSendUserName ,
							const CString &strSendUserMailAddr , //发送者的邮件地址
							const CString &strRecvUserName ,//接受者姓名地址
							const CString &strMailTitle ,//邮件标题
							const CString &strMailBodyText ,//邮件正文文本
							const CString &strAttachFileName //附件地址
							)
{
	//创建信息体 
	CSMTPMessage* pMessage = new CSMTPMessage;
	CSMTPBodyPart attachment;

	bool m_bMime = false ;
	CString m_IP=TEXT("ANY_IP_ADDRESS");
	m_bMime=false;
	bool m_bHTML=false;
	CString m_sEncodingFriendly=TEXT("Chinese Simplified (GB2312)");
	CString m_sEncodingCharset=TEXT("gb2312");
	CString m_CC ;//抄送地址
	CString m_BCC ;//暗送地址

	//设定mine标志mime
	pMessage->SetMime(m_bMime);

	//设定信息和附件的编码方式
	pMessage->SetCharset(m_sEncodingCharset);
	attachment.SetCharset(m_sEncodingCharset);

	//获得各种基本信息
	pMessage->AddMultipleRecipients(strRecvUserName , CSMTPMessage::TO);
	if (!m_CC.IsEmpty()) 
		pMessage->AddMultipleRecipients(m_CC, CSMTPMessage::CC);
	if (!m_BCC.IsEmpty()) 
		pMessage->AddMultipleRecipients(m_BCC, CSMTPMessage::BCC);
	if (!strMailTitle.IsEmpty()) 
		pMessage->m_sSubject = strMailTitle ;
	if (!strMailBodyText.IsEmpty())
	{
		//表示是否以HTML的格式发送邮件
		if (m_bHTML)
			pMessage->AddHTMLBody(strMailBodyText , _T(""));
		else
			pMessage->AddTextBody(strMailBodyText );
	}

	//增加附件，如果要发送附件
	if (!strAttachFileName.IsEmpty()) 
		pMessage->AddMultipleAttachments(strAttachFileName );		

	//设定发送者地址
	if (strSendUserMailAddr.IsEmpty()) 
	{
		pMessage->m_From = strSendUserMailAddr;
		pMessage->m_ReplyTo = strSendUserMailAddr;
	}
	else 
	{
		CSMTPAddress address(strSendUserName, strSendUserMailAddr);
		pMessage->m_From = address;
		pMessage->m_ReplyTo = address;
	}

	//	m_Authenticate=(CSMTPConnection::LoginMethod) m_Authenticate_1;

	//增加一个自定义头，测试用
	//	#ifdef _DEBUG
	//	pMessage->AddCustomHeader(_T("X-Program: CSTMPMessageTester"));
	//	#endif

	return pMessage;
}