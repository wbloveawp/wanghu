#ifndef ROBOT_USER_MANAGER_HEAD_FILE
#define ROBOT_USER_MANAGER_HEAD_FILE

//引入文件
#include "RobotUserItem.h"
#include "SimulateLinkHead.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// 类型定义

typedef CWHArray<CRobotUserItem*> CRobotUserItemArray;
typedef CWHStockList<CRobotUserItem> CStockRobotUserItem;

/////////////////////////////////////////////////////////////////////////////////////////////
// 
//机器管理
class CRobotUserManager : public CSingleton<CRobotUserManager>
{
	//配置变量
protected:
	TCHAR							m_szModuleDLLName[32];			//模块名称
	CHAR							m_szCreateProcName[64];			//过程名称

	//组件变量
protected:
	CRobotServiceHelper				m_AndroidServiceHelper;			//机器人服务
	HINSTANCE						m_hDllInstance;					//DLL 句柄

	//用户数组
protected:
	CStockRobotUserItem				m_StockRobotUserItem;			//用户索引	

	//函数定义
public:
	//构造函数
	CRobotUserManager();
	//析构函数
	virtual ~CRobotUserManager();

	//配置函数
public:
	//模块名称
	VOID SetModuleDLLName(LPCTSTR pszModuleDLLName);
	//创建过程名
	VOID SetCreateProcName(LPCSTR pszCreateProcName);

	//机器管理
public:
	//激活对象
	CRobotUserItem * ActiveRobotUserItem();
	//释放对象
	VOID FreeRobotUserItem(CRobotUserItem* pRobotUserItem);

	//功能函数
public:
	//重置数据
	VOID ResetUserManager();

	//辅助函数
private:
	//创建机器
	VOID* CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer);
};

/////////////////////////////////////////////////////////////////////////////////////////////

#define g_RobotUserManager (CRobotUserManager::getInstance())

//////////////////////////////////////////////////////////////////////////////////


#endif