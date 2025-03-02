#ifndef SERVICE_DLG_HEAD_FILE
#define SERVICE_DLG_HEAD_FILE

#pragma once

#include "ModuleManagerHead.h"
#include "ModuleInfoManager.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////
//数组定义
typedef CWHArray<tagServiceKind> CServiceKindArray;
typedef CWHArray<tagServiceOption *>	CServiceOptionArray;

//////////////////////////////////////////////////////////////////////////////////

//配置窗口
class MODULE_MANAGER_CLASS CDlgServiceOption : public CDialog
{
	//标识变量
protected:
	tagServiceOption				m_ServiceOption;					//服务选项
	bool							m_bInitDirectory;					//标志变量

	//配置变量
protected:
	bool							m_bOptionModule;					//配置模块
	TCHAR							m_szDataFileName[32];				//文件名称

	//状态变量
protected:
	int								m_nSelectPortItem;					//子项选择
	int								m_nSelectServiceItem;				//子项选择	

	//配置变量
protected:
	CServiceKindArray				m_ServiceKindArray;					//服务类型

	//列表变量
protected:
	CModuleInfoBuffer				m_ModuleInfoBuffer;					//模块信息	
	CModuleInfoManager				m_ModuleInfoManager;				//模块管理

	//存储变量
protected:
	CServiceOptionArray				m_ServiceOptionActive;				//服务数组
	CServiceOptionArray				m_ServiceOptionBuffer;				//服务数组

	//函数定义
public:
	//构造函数
	CDlgServiceOption();
	//析构函数
	virtual ~CDlgServiceOption();

	//重载函数
protected:
	//进入函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);

	//配置函数
public:
	//设置类型
	VOID SetServiceKind(tagServiceKind ServiceKind[], WORD wKindCount);	

	//功能函数
public:
	//设置标识
	VOID SetOptionModule(bool bOptionModule);
	//文件名次
	VOID SetDataFileName(LPCTSTR pszFileName);
	//获取配置
	tagServiceOption * GetServiceOption();
	//获取配置
	tagServiceOption * GetServiceOption(WORD wServiceID);

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
	//设置标识
	bool SetServiceID(WORD wServiceID);
	//获取标识
	bool GetServiceID(WORD & wServiceID);

	//辅助函数
private:
	//初始列表
	VOID InitControl();
	//加载列表
	VOID LoadDBModuleItem();
	//选择子项
	VOID SelectServiceItem(int nIndex);
	//获取路径
	VOID GetDataFilePath(TCHAR szFilePath[], WORD wBufferCount);
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