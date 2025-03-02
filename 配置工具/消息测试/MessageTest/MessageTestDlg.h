
// MessageTestDlg.h: 头文件
//

#pragma once


// CMessageTestDlg 对话框
class CMessageTestDlg : public CDialogEx, public ITCPSocketSink
{
private:
	WORD	m_wCurModule;
	bool	m_bServiceToken[SERVICE_COUNT];

private:
	CTCPSocketHelper				m_TCPSocketModule;					//网络连接

// 构造
public:
	CMessageTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESSAGETEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	//基础接口
public:
	//释放对象
	virtual VOID Release();
	//接口查询
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//网络接口
protected:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID* pData, WORD wDataSize);

public:
	bool Connect(TCHAR szAddr[LEN_SERVER], DWORD dwPort);
	bool Close();
	void Log(LPCTSTR pString);
	void SendData(WORD wModule,WORD wMainCmd,WORD wSubCmd,LPVOID pData,WORD wDataSize);
	void RegisteToken(WORD wModule);
	void RefreshCustomInfo(bool bUpdateUI = false);

public:
	void MessageModuleLogon(WORD wMainCmd, WORD wSubCmd, LPVOID pData, WORD wDataSize);

// 实现
protected:
	HICON m_hIcon;
	CString m_strlog;
	CEdit m_log;

	// 生成的消息映射函数
	void GetItemText(int nID, TCHAR szValue[]);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonConnect();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonTestLogon();
	afx_msg void OnBnClickedButtonToken();
	afx_msg void OnBnClickedButtonRealAuth();
	afx_msg void OnBnClickedButtonClearLog();
	afx_msg void OnBnClickedButtonBindphone();
	afx_msg void OnBnClickedButtonDisbindphone();
	afx_msg void OnBnClickedButtonRefreshData();
	afx_msg void OnBnClickedButtonInsureQuerry();
	afx_msg void OnBnClickedButtonInsureSave();
	afx_msg void OnBnClickedButtonInsureTake();
	afx_msg void OnBnClickedButtonInsureTransfer();
	afx_msg void OnBnClickedButtonInsureEnable();
	afx_msg void OnBnClickedButtonMessage();
};
