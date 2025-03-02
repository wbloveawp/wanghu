#include "StdAfx.h"
#include ".\sendbugreport.h"
#include "Smtp.h" 


CSendBugReport::CSendBugReport(void)
{
}

CSendBugReport::~CSendBugReport(void)
{
}



bool CSendBugReport::SendMail(	const CString &strSmtpSvrIp , //����smtp.163.com
							   const CString &strSmtpUserName , //Smtp�ĵ�¼�û���
							   const CString &strSmtpPwd , // Smtp������
							   const CString &strSendUserName ,//����������
							   const CString &strSendUserMailAddr,//�������ʼ���ַ
							   const CString &strRecvUserName ,//�ռ�������
							   const CString &strRecvMailAddr ,// �����ߵ��ʼ���ַ
							   const CString &strMailTitle ,//�ʼ�����
							   const CString &strMailBodyText ,//�ʼ������ı�
							   const CString &strAttachFileName //������ַ
							   ) 
{
	//�����ж�SMTP�������Ƿ�Ϊ��
	if ( strSmtpSvrIp.IsEmpty())
	{
		return false;
	}
	else 
	{

		//����SMTP��Ϣ�壬ͨ������ʵ��
		CString strRecvUser ;
		strRecvUser.Format(_T("\"%s\" <%s>") , strRecvUserName , strSendUserMailAddr) ;
		CSMTPMessage* pMessage = CreateMessage( strSendUserName ,
			strSendUserMailAddr,
			strRecvUser ,
			strMailTitle ,
			strMailBodyText ,
			strAttachFileName );
		//���ӵ�������
		CSMTPConnection connection;
		//		UpdateData(true);
		//		m_IP=_T("");
		BOOL bConnect = connection.Connect(strSmtpSvrIp, 
			CSMTPConnection::AuthLoginMethod , 
			strSmtpUserName ,strSmtpPwd,
			25 , TEXT(""));
		//�ж������Ƿ�ɹ�
		if (!bConnect) 
		{
			AfxMessageBox(TEXT("�������ӵ�������!"));
		}
		else 
		{
			//���;�����Ϣ
			if (!connection.SendMessage(*pMessage)) 
			{
				AfxMessageBox(TEXT("������Ϣʧ��!"));
			}
		}
		//ɾ����Ϣ��
		delete pMessage;
	}	

	return true ;
}

//����һ���½�������
CSMTPMessage* CSendBugReport::CreateMessage(
							const CString &strSendUserName ,
							const CString &strSendUserMailAddr , //�����ߵ��ʼ���ַ
							const CString &strRecvUserName ,//������������ַ
							const CString &strMailTitle ,//�ʼ�����
							const CString &strMailBodyText ,//�ʼ������ı�
							const CString &strAttachFileName //������ַ
							)
{
	//������Ϣ�� 
	CSMTPMessage* pMessage = new CSMTPMessage;
	CSMTPBodyPart attachment;

	bool m_bMime = false ;
	CString m_IP=TEXT("ANY_IP_ADDRESS");
	m_bMime=false;
	bool m_bHTML=false;
	CString m_sEncodingFriendly=TEXT("Chinese Simplified (GB2312)");
	CString m_sEncodingCharset=TEXT("gb2312");
	CString m_CC ;//���͵�ַ
	CString m_BCC ;//���͵�ַ

	//�趨mine��־mime
	pMessage->SetMime(m_bMime);

	//�趨��Ϣ�͸����ı��뷽ʽ
	pMessage->SetCharset(m_sEncodingCharset);
	attachment.SetCharset(m_sEncodingCharset);

	//��ø��ֻ�����Ϣ
	pMessage->AddMultipleRecipients(strRecvUserName , CSMTPMessage::TO);
	if (!m_CC.IsEmpty()) 
		pMessage->AddMultipleRecipients(m_CC, CSMTPMessage::CC);
	if (!m_BCC.IsEmpty()) 
		pMessage->AddMultipleRecipients(m_BCC, CSMTPMessage::BCC);
	if (!strMailTitle.IsEmpty()) 
		pMessage->m_sSubject = strMailTitle ;
	if (!strMailBodyText.IsEmpty())
	{
		//��ʾ�Ƿ���HTML�ĸ�ʽ�����ʼ�
		if (m_bHTML)
			pMessage->AddHTMLBody(strMailBodyText , _T(""));
		else
			pMessage->AddTextBody(strMailBodyText );
	}

	//���Ӹ��������Ҫ���͸���
	if (!strAttachFileName.IsEmpty()) 
		pMessage->AddMultipleAttachments(strAttachFileName );		

	//�趨�����ߵ�ַ
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

	//����һ���Զ���ͷ��������
	//	#ifdef _DEBUG
	//	pMessage->AddCustomHeader(_T("X-Program: CSTMPMessageTester"));
	//	#endif

	return pMessage;
}