#ifndef ROBOT_USER_MANAGER_HEAD_FILE
#define ROBOT_USER_MANAGER_HEAD_FILE

//�����ļ�
#include "RobotUserItem.h"
#include "SimulateLinkHead.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// ���Ͷ���

typedef CWHArray<CRobotUserItem*> CRobotUserItemArray;
typedef CWHStockList<CRobotUserItem> CStockRobotUserItem;

/////////////////////////////////////////////////////////////////////////////////////////////
// 
//��������
class CRobotUserManager : public CSingleton<CRobotUserManager>
{
	//���ñ���
protected:
	TCHAR							m_szModuleDLLName[32];			//ģ������
	CHAR							m_szCreateProcName[64];			//��������

	//�������
protected:
	CRobotServiceHelper				m_AndroidServiceHelper;			//�����˷���
	HINSTANCE						m_hDllInstance;					//DLL ���

	//�û�����
protected:
	CStockRobotUserItem				m_StockRobotUserItem;			//�û�����	

	//��������
public:
	//���캯��
	CRobotUserManager();
	//��������
	virtual ~CRobotUserManager();

	//���ú���
public:
	//ģ������
	VOID SetModuleDLLName(LPCTSTR pszModuleDLLName);
	//����������
	VOID SetCreateProcName(LPCSTR pszCreateProcName);

	//��������
public:
	//�������
	CRobotUserItem * ActiveRobotUserItem();
	//�ͷŶ���
	VOID FreeRobotUserItem(CRobotUserItem* pRobotUserItem);

	//���ܺ���
public:
	//��������
	VOID ResetUserManager();

	//��������
private:
	//��������
	VOID* CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer);
};

/////////////////////////////////////////////////////////////////////////////////////////////

#define g_RobotUserManager (CRobotUserManager::getInstance())

//////////////////////////////////////////////////////////////////////////////////


#endif