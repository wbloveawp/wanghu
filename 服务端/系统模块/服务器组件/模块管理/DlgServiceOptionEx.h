#ifndef SERVICEEX_DLG_HEAD_FILE
#define SERVICEEX_DLG_HEAD_FILE

#pragma once

#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////
//数组定义
typedef CArrayTemplate<tagServiceOptionEx *>	CServiceOptionExArray;

//////////////////////////////////////////////////////////////////////////////////

//配置窗口
class MODULE_MANAGER_CLASS CDlgServiceOptionEx : public CDialog
{
	//标识变量
protected:
	tagServiceOption				m_ServiceOption;					//服务选项
	bool							m_bInitDirectory;					//标志变量
	TCHAR							m_szDataFileName[32];				//文件名称

	//状态变量
protected:
	int								m_nSelectPortItem;					//子项选择
	int								m_nSelectServiceItem;				//子项选择	

	//存储变量
protected:
	CServiceOptionExArray			m_ServiceOptionActive;				//服务数组
	CServiceOptionExArray			m_ServiceOptionBuffer;				//服务数组

	//函数定义
public:
	//构造函数
	CDlgServiceOptionEx();
	//析构函数
	virtual ~CDlgServiceOptionEx();

	//重载函数
protected:
	//进入函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);

	//功能函数
public:
	//文件名次
	VOID SetDataFileName(LPCTSTR pszFileName);
	//获取配置
	tagServiceOption * GetServiceOption() { return &m_ServiceOption; }

	//辅助函数
protected:
	//加载数据
	bool LoadServiceOption();
	//保存数据
	bool SaveServiceOption();

	//辅助函数
protected:
	//激活对象
	tagServiceOption * ActiveServiceOption();
	//释放对象
	void FreeServiceOption(tagServiceOption * pServiceOption);

	//辅助函数
private:
	//初始列表
	VOID InitControl();
	//选择子项
	VOID SelectServiceItem(int nIndex);
	//设置控件
	VOID SetServiceToControl(tagServiceOption *pServiceOption,CListCtrl * pListCtrl,int nIndex);

	//点击函数
protected:
	//退出窗口
	VOID OnBnClickedQuit();	
	//添加配置
	VOID OnBnClickedAddServiceInfo();	
	//修改配置
	VOID OnBnClickedModifyServiceInfo();
	//删除配置
	VOID OnBnClickedDeleteServiceInfo();		

	//控件函数
public:		
	//单击子项
	VOID OnHdnItemclickListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult);	
	//双击子项
	VOID OnHdnItemDblclkListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult);
	//键盘按下
	VOID OnLvnItemKeydownListServiceInfo(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////////////

#endif