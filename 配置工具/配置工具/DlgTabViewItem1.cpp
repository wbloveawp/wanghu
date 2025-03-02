#include "StdAfx.h"
#include "Resource.h"
#include "DlgTabViewItem1.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgTabViewItem1, CDialog)
	ON_BN_CLICKED(IDC_CREATE_ACCREDIT, OnBnClickedCreateAccredit)
	ON_BN_CLICKED(IDC_DELECT_ACCREDIT, OnBnClickedDeleteAccredit)
	ON_BN_CLICKED(IDC_VALIDATE_ACCREDIT, OnBnClickedValidateAccredit)
	ON_BN_CLICKED(IDC_RELOAD_VALIDATE_INFO, OnBnClickedReLoadValidateInfo)
	ON_BN_CLICKED(IDC_ENTER_PRODUCT_MANAGER, OnBnClickedEnterProductManager)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgTabViewItem1::CDlgTabViewItem1() : CDialog(IDD_TAB_VIEW_ITEM_2)
{
}

//析构函数
CDlgTabViewItem1::~CDlgTabViewItem1()
{
}

//控件绑定
VOID CDlgTabViewItem1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

//消息解释
BOOL CDlgTabViewItem1::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message == WM_KEYDOWN) && ((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN)))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//配置函数
BOOL CDlgTabViewItem1::OnInitDialog()
{
	__super::OnInitDialog();

	//创建组件
	if (m_ValidateDataModule.CreateInstance() == true)
	{
		//加载证书
		m_ValidateDataModule->LoadValidateData();

		//更新界面
		SetControlStatus(m_ValidateDataModule->GetValidateInfo());
	}

	//设置控件
	((CEdit *)GetDlgItem(IDC_ACCOUNTS))->LimitText(LEN_ACCOUNTS - 1L);
	((CEdit *)GetDlgItem(IDC_LOGON_PASS))->LimitText(LEN_PASSWORD - 1L);
	((CEdit *)GetDlgItem(IDC_CONTRACT_ID))->LimitText(LEN_CONTRACT_ID - 1L);

	return FALSE;
}

//更新证书
VOID CDlgTabViewItem1::SetControlStatus(tagValidateInfo & ValidateInfo)
{
	//启用控件
	GetDlgItem(IDC_CREATE_ACCREDIT)->EnableWindow((ValidateInfo.dwProductID == 0L) ? TRUE : FALSE);
	GetDlgItem(IDC_DELECT_ACCREDIT)->EnableWindow((ValidateInfo.dwProductID == 0L) ? FALSE : TRUE);
	GetDlgItem(IDC_VALIDATE_ACCREDIT)->EnableWindow((ValidateInfo.dwProductID == 0L) ? FALSE : TRUE);

	//设置控件
	SetDlgItemText(IDC_NATIVE_CONTRACT_ID, (ValidateInfo.dwProductID != 0L) ? ValidateInfo.szContractID : TEXT(""));
	SetDlgItemText(IDC_NATIVE_PRODUCT_NAME, (ValidateInfo.dwProductID != 0L) ? ValidateInfo.szProductName : TEXT(""));
	SetDlgItemText(IDC_NATIVE_ACCREDIT_ADDR, (ValidateInfo.dwProductID != 0L) ? ValidateInfo.szAccreditAddr : TEXT(""));
	SetDlgItemText(IDC_NATIVE_ACCREDIT_MACHINE, (ValidateInfo.dwProductID != 0L) ? ValidateInfo.szAccreditMachine : TEXT(""));

	return;
}

//获取信息
bool CDlgTabViewItem1::GetClientAccounts(tagClientAccounts & ClientAccounts)
{
	//变量定义
	CString strAccounts;
	CString strContractID;
	TCHAR szPassword[LEN_PASSWORD] = TEXT("");

	//获取信息
	GetDlgItemText(IDC_ACCOUNTS, strAccounts);
	GetDlgItemText(IDC_CONTRACT_ID, strContractID);
	GetDlgItemText(IDC_LOGON_PASS, szPassword, CountArray(szPassword));

	//调整参数
	strAccounts.Trim();
	strContractID.Trim();

	//构造参数
	CMD5Encrypt::EncryptData(szPassword, ClientAccounts.szLogonPass);
	StringCchCopy(ClientAccounts.szAccounts, CountArray(ClientAccounts.szAccounts), strAccounts);
	StringCchCopy(ClientAccounts.szContractID, CountArray(ClientAccounts.szContractID), strContractID);

	//效验输入
	if (ClientAccounts.szContractID[0] == 0L)
	{
		//提示信息
		AfxMessageBox(TEXT("请输入产品序列，产品序列是由一串数字组成的序列号码！"), MB_ICONERROR);

		//设置焦点
		GetDlgItem(IDC_CONTRACT_ID)->SetFocus();

		return false;
	}

	//效验输入
	if (ClientAccounts.szAccounts[0] == 0L)
	{
		//提示信息
		AfxMessageBox(TEXT("请输入客户帐号，客户帐号是由中文字符、英文字母以及数字符号所组成！"), MB_ICONERROR);

		//设置焦点
		GetDlgItem(IDC_ACCOUNTS)->SetFocus();

		return false;
	}

	//效验输入
	if (szPassword[0] == 0L)
	{
		//提示信息
		AfxMessageBox(TEXT("请输入配置密码，配置密码是由英文字母以及数字符号所组成！"), MB_ICONERROR);

		//设置焦点
		GetDlgItem(IDC_LOGON_PASS)->SetFocus();

		return false;
	}

	return true;
}

//创建授权
VOID CDlgTabViewItem1::OnBnClickedCreateAccredit()
{
	//变量定义
	tagClientAccounts ClientAccounts;
	if (GetClientAccounts(ClientAccounts) == false) return;

	//变量定义
	tagCreateAccreditResult CreateAccreditResult;
	ZeroMemory(&CreateAccreditResult, sizeof(CreateAccreditResult));

	//创建组件
	if ((m_ValidateServiceModule.GetInterface() == NULL) && (m_ValidateServiceModule.CreateInstance() == false))
	{
		KERNEL_ASSERT(FALSE);
		return;
	}

	//执行创建
	if (m_ValidateServiceModule->PerformCreateAccredit(ClientAccounts, CreateAccreditResult) == true)
	{
		//变量定义
		tagValidateInfo ValidateInfo;
		tagValidateResult ValidateResult;
		ZeroMemory(&ValidateInfo, sizeof(ValidateInfo));
		ZeroMemory(&ValidateResult, sizeof(ValidateResult));

		//验证信息
		ValidateInfo.dwProductID = CreateAccreditResult.dwProductID;
		StringCchCopy(ValidateInfo.szContractID, CountArray(ValidateInfo.szContractID), CT2CW(CreateAccreditResult.szContractID));
		StringCchCopy(ValidateInfo.szProductName, CountArray(ValidateInfo.szProductName), CT2CW(CreateAccreditResult.szProductName));
		StringCchCopy(ValidateInfo.szAccreditSerial, CountArray(ValidateInfo.szAccreditSerial), CT2CW(CreateAccreditResult.szAccreditSerial));

		//授权信息
		StringCchCopy(ValidateInfo.szCompilation, CountArray(ValidateInfo.szCompilation), CT2CW(CreateAccreditResult.szCompilation));
		StringCchCopy(ValidateInfo.szAccreditAddr, CountArray(ValidateInfo.szAccreditAddr), CT2CW(CreateAccreditResult.szAccreditAddr));
		StringCchCopy(ValidateInfo.szAccreditMachine, CountArray(ValidateInfo.szAccreditMachine), CT2CW(CreateAccreditResult.szAccreditMachine));

		//设置信息
		m_ValidateDataModule->SetValidateInfo(ValidateInfo);
		m_ValidateDataModule->SetValidateResult(ValidateResult);

		//保存证书
		m_ValidateDataModule->SaveValidateData();

		//设置控件
		SetDlgItemText(IDC_ACCOUNTS, TEXT(""));
		SetDlgItemText(IDC_LOGON_PASS, TEXT(""));
		SetDlgItemText(IDC_CONTRACT_ID, TEXT(""));

		//更新控件
		SetControlStatus(m_ValidateDataModule->GetValidateInfo());

		//提示消息
		if (CreateAccreditResult.szErrorDescribe[0] != 0)
		{
			m_TraceServiceControl.TraceString(CreateAccreditResult.szErrorDescribe, TraceLevel_Normal);
		}
	}
	else
	{
		//设置控件
		SetDlgItemText(IDC_LOGON_PASS, TEXT(""));

		//提示消息
		if (CreateAccreditResult.szErrorDescribe[0] != 0)
		{
			m_TraceServiceControl.TraceString(CreateAccreditResult.szErrorDescribe, TraceLevel_Exception);
		}
	}

	return;
}

//注销授权
VOID CDlgTabViewItem1::OnBnClickedDeleteAccredit()
{
	//变量定义
	tagClientAccounts ClientAccounts;
	if (GetClientAccounts(ClientAccounts) == false) return;

	//提示消息
	BYTE cbString[] =
	{
		//您确实要注销本机的产品授权证书吗？
		//
		//证书注销后，需等待一段时间才能恢复您的授权配额、
		//
		//并需要等待一段时间后才能在本机再次获取授权证书。
		0xEB,0xF6,0xFC,0x66,0x70,0x66,0xBD,0x36,0x52,0xB5,0x7E,0xBF,0xA6,0x74,0x22,0xA0,
		0x71,0x9F,0xEB,0x25,0xF0,0xF7,0xD4,0x43,0xD4,0x0C,0x95,0x5B,0x06,0x9D,0xD4,0xDC,
		0x1B,0xD4,0x7B,0x8C,0x4F,0x7B,0x12,0x74,0x2E,0x45,0xCF,0x7A,0xF6,0x47,0x4E,0xE6,
		0x23,0x64,0xE8,0xB0,0x6F,0xE1,0x51,0x55,0x93,0x42,0x95,0xA0,0x9D,0xA3,0x79,0x5F,
		0x2E,0x9D,0x9D,0x3D,0xB1,0xB0,0x2B,0xF6,0xE3,0x4D,0xB1,0xDC,0xB4,0x47,0xB4,0x97,
		0xC9,0xF4,0x09,0xA7,0xE9,0x97,0x53,0x38,0x89,0x53,0xE3,0xE1,0x3C,0xBE,0xC3,0xA0,
		0xC9,0x2F,0xB4,0xE7,0x91,0x17,0xDD,0xF4,0x9B,0x7D,0x00,0x30,0x00,0xE7,0x17,0x55,
		0x55,0x03,0xFA,0x03,0xA5,0x73,0x4F,0x0A,0xAE,0x32,0xF1,0xAC,0x0F,0x28,0xFA,0x17,
		0xD0,0x8F,0xD0,0xF2,0x44,0x2F,0xC6,0x26,0x2F,0x3C,0x1A
	};

	//提示消息
	TCHAR szString[CountArray(cbString)] = TEXT("");
	CStringTransform::Crevasse(cbString, CountArray(cbString), szString, CountArray(szString));

	//提示消息
	if (AfxMessageBox(szString, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES)
	{
		return;
	}

	//变量定义
	tagDeleteAccreditResult DeleteAccreditResult;
	ZeroMemory(&DeleteAccreditResult, sizeof(DeleteAccreditResult));

	//创建组件
	if ((m_ValidateServiceModule.GetInterface() == NULL) && (m_ValidateServiceModule.CreateInstance() == false))
	{
		KERNEL_ASSERT(FALSE);
		return;
	}

	//执行创建
	if (m_ValidateServiceModule->PerformDeleteAccredit(ClientAccounts, DeleteAccreditResult) == true)
	{
		//设置控件
		SetDlgItemText(IDC_ACCOUNTS, TEXT(""));
		SetDlgItemText(IDC_LOGON_PASS, TEXT(""));
		SetDlgItemText(IDC_CONTRACT_ID, TEXT(""));

		//删除信息
		m_ValidateDataModule->DeleteValidateData();

		//更新控件
		SetControlStatus(m_ValidateDataModule->GetValidateInfo());

		//提示消息
		if (DeleteAccreditResult.szErrorDescribe[0] != 0)
		{
			m_TraceServiceControl.TraceString(DeleteAccreditResult.szErrorDescribe, TraceLevel_Normal);
		}
	}
	else
	{
		//设置控件
		SetDlgItemText(IDC_LOGON_PASS, TEXT(""));

		//提示消息
		if (DeleteAccreditResult.szErrorDescribe[0] != 0)
		{
			m_TraceServiceControl.TraceString(DeleteAccreditResult.szErrorDescribe, TraceLevel_Exception);
		}
	}

	return;
}

//授权验证
VOID CDlgTabViewItem1::OnBnClickedValidateAccredit()
{
	//获取机器
	TCHAR szMachine[LEN_MACHINE_ID];
	CWHService::GetMachineID(szMachine);

	//获取证书
	ASSERT(m_ValidateDataModule.GetInterface() != NULL);
	tagValidateInfo & ValidateInfo = m_ValidateDataModule->GetValidateInfo();

	//机器效验
	if (lstrcmp(szMachine, ValidateInfo.szAccreditMachine) != 0L)
	{
		//提示消息
		BYTE cbString[] =
		{
			//您的服务器与当前授权证书不一致，此证书无效！
			0xEB,0xF6,0x3F,0x31,0xC7,0xAA,0xD7,0xF8,0x70,0x20,0x64,0x6C,0x0C,0xEA,0x9F,0x5B,
			0xF0,0x65,0xF0,0xC7,0x51,0x0B,0xD4,0x20,0xFC,0xD7,0x18,0xC4,0x2E,0x52,0x0C,0x74,
			0xD7,0x18,0xF8,0x31,0xA3,0x21,0x42,0xBF,0xF8,0xA8,0x7A,0xF7,0x3C
		};

		//提示消息
		TCHAR szString[CountArray(cbString)] = TEXT("");
		m_TraceServiceControl.TraceString(CStringTransform::Crevasse(cbString, CountArray(cbString), szString, CountArray(szString)), TraceLevel_Exception);

		return;
	}

	//变量定义
	tagVerificationResult VerificationResult;
	ZeroMemory(&VerificationResult, sizeof(VerificationResult));

	//创建组件
	if ((m_ValidateServiceModule.GetInterface() == NULL) && (m_ValidateServiceModule.CreateInstance() == false))
	{
		KERNEL_ASSERT(FALSE);
		return;
	}

	//执行创建
	if (m_ValidateServiceModule->PerformVerification(ValidateInfo, VerificationResult) == true)
	{
		//禁用控件
		GetDlgItem(IDC_VALIDATE_ACCREDIT)->EnableWindow(FALSE);

		//提示消息
		if (VerificationResult.szErrorDescribe[0] != 0)
		{
			m_TraceServiceControl.TraceString(VerificationResult.szErrorDescribe, TraceLevel_Normal);
		}
	}
	else
	{
		//禁用控件
		GetDlgItem(IDC_VALIDATE_ACCREDIT)->EnableWindow(FALSE);

		//提示消息
		if (VerificationResult.szErrorDescribe[0] != 0)
		{
			m_TraceServiceControl.TraceString(VerificationResult.szErrorDescribe, TraceLevel_Exception);
		}
	}

	return;
}

//加载证书
VOID CDlgTabViewItem1::OnBnClickedReLoadValidateInfo()
{
	//创建组件
	CValidateDataHelper ValidateDataModule;
	if (ValidateDataModule.CreateInstance() == false)
	{
		KERNEL_ASSERT(FALSE);
		return;
	}

	//加载证书
	bool bSuccess = ValidateDataModule->LoadValidateData();

	//更新控件
	if (memcmp(&ValidateDataModule->GetValidateInfo(), &m_ValidateDataModule->GetValidateInfo(), sizeof(tagValidateInfo)) != 0L)
	{
		//设置控件
		SetControlStatus(ValidateDataModule->GetValidateInfo());

		//设置变量
		m_ValidateDataModule->SetValidateInfo(ValidateDataModule->GetValidateInfo());
		m_ValidateDataModule->SetValidateResult(ValidateDataModule->GetValidateResult());
	}

	//提示消息
	if (bSuccess == false)
	{
		//提示消息
		BYTE cbString[] =
		{
			//产品注册授权证书加载失败，请先获取本机授权证书！
			0x33,0xF6,0x3A,0x66,0x8B,0x2D,0xF5,0x4E,0xEC,0xA4,0xEC,0xFE,0x5E,0x36,0xBD,0xF8,
			0xF7,0x37,0x06,0x65,0x40,0xD8,0x88,0x59,0xC7,0x71,0x1C,0x9D,0x65,0x57,0x7E,0x0E,
			0x4E,0x6E,0x23,0x1C,0x52,0x3D,0x95,0xCF,0x95,0x61,0x20,0xD9,0x6C,0xB4,0x61,0xA8,
			0xC3
		};

		//提示消息
		TCHAR szString[CountArray(cbString)] = TEXT("");
		m_TraceServiceControl.TraceString(CStringTransform::Crevasse(cbString, CountArray(cbString), szString, CountArray(szString)), TraceLevel_Exception);
	}

	return;
}

//进入系统
VOID CDlgTabViewItem1::OnBnClickedEnterProductManager()
{
	//变量定义
	BYTE cbWebUrl[] =
	{
		//http://crm.foxuc.cn
		0x5C,0x96,0x34,0xE4,0x3B,0x55,0xF4,0x86,0xEF,0x34,0xCE,0x0A,0x5D,0x8D,0x08,0x35,
		0x3B,0x3E,0x5A,0x4A
	};

	//打开页面
	TCHAR szWebUrl[CountArray(cbWebUrl)] = TEXT("");
	ShellExecute(NULL, TEXT("OPEN"), CStringTransform::Crevasse(cbWebUrl, CountArray(cbWebUrl), szWebUrl, CountArray(szWebUrl)), NULL, NULL, SW_MAXIMIZE);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
