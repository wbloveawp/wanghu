#ifndef USER_LIST_CONTROL_HEAD_FILE
#define USER_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ClientControlsHead.h"

//////////////////////////////////////////////////////////////////////////

//��������
#define	MAX_COLUMN					32									//�������
#define MAX_EXPERIENCE				5									//�����

//��ȶ���
#define WIDTH_STATUS				20									//�б���
#define WIDTH_EXPERIENCE			16									//�б���

//��������
#define INDEX_STATUS				0									//״̬����
#define COUNT_STATUS				6									//״̬��Ŀ
#define INDEX_NORMAL				18									//��������
#define INDEX_FRIEND				19									//��������
#define INDEX_DETEST				20									//��������
#define INDEX_MEMBER				21									//��Ա����
#define INDEX_MASTER				43									//��������
#define INDEX_INVALID				0xFFFF								//��Ч����

//////////////////////////////////////////////////////////////////////////
//Ĭ����ɫ
#define COLOR_SELECT_TX				RGB(255,234,2)						//������ɫ
#define COLOR_SELECT_BK				RGB(85,72,53)						//������ɫ
#define COLOR_NORMAL_TX				RGB(255,255,255)					//������ɫ
#define COLOR_NORMAL_BK				RGB(50,30,17)						//������ɫ

//Ĭ����ɫ
#define COLOR_MYSELF_TX				RGB(255,167,132)					//������ɫ
#define COLOR_MYSELF_BK				RGB(85,72,53)						//������ɫ
#define COLOR_MASTER_TX				RGB(255,255,255)					//������ɫ
#define COLOR_MASTER_BK				RGB(90,45,27)						//������ɫ
#define COLOR_MEMBER_TX				RGB(67,44,12)						//������ɫ
#define COLOR_MEMBER_BK				RGB(255,157,2)						//������ɫ
#define COLOR_ANDROID_TX			RGB(255,255,255)					//������ɫ
#define COLOR_ANDROID_BK			RGB(255,0,0)						//������ɫ
//////////////////////////////////////////////////////////////////////////

//��ɫ����
struct tagColorUserList
{
	//������ɫ
	COLORREF						crSelectTX;							//������ɫ
	COLORREF						crSelectBK;							//������ɫ
	COLORREF						crNormalTX;							//������ɫ
	COLORREF						crNormalBK;							//������ɫ	

	//������ɫ
	COLORREF						crMyselfTX;							//������ɫ
	COLORREF						crMyselfBK;							//������ɫ
	COLORREF						crMasterTX;							//������ɫ
	COLORREF						crMasterBK;							//������ɫ
	COLORREF						crMemberTX;							//������ɫ
	COLORREF						crMemberBK;							//������ɫ
};

//////////////////////////////////////////////////////////////////////////

//��ͷ�ؼ�
class CLIENT_CONTROLS_CLASS CUserListHeaderCtrl : public CHeaderCtrl
{
	//�������
protected:
	INT								m_nColumn;							//��ǰ����
	enSortType						m_SortType;							//��������

	//��������
public:
	//���캯��
	CUserListHeaderCtrl();
	//��������
	virtual ~CUserListHeaderCtrl();

	//���غ���
protected:
	//�ؼ���Ϣ
	virtual BOOL OnChildNotify(UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT * pLResult);

	//��Ϣ����
protected:
	//�����Ϣ
    BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	//���ܺ���
public:
	//���ñ�־
	VOID SetSortArrow(INT nSortColumn, enSortType SortType);
	//��ȡ��־
	VOID GetSortArrow(INT& nSortColumn, enSortType& SortType);

	//��Ϣ����
protected:
	//�ػ���Ϣ
	VOID OnPaint();
	//�滭����
	BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//�û��б�
class CLIENT_CONTROLS_CLASS CUserListControl : public CListCtrl
{
	//״̬����
protected:
	bool							m_bAscendSort;						//�����־
	bool							m_bCreateColumn;					//��������
	DWORD							m_dwMySelfUserID;					//�Լ���Ϣ

	//�б���Ϣ
protected:
	WORD							m_wColumnCount;						//�б���Ŀ
	BYTE							m_cbDataDescribe[MAX_COLUMN];		//��������

	//�������
protected:
	IGameLevelParser *				m_pIGameLevelParser;				//�ȼ��ӿ�
	IGameUserManager *				m_pIGameUserManager;				//�û��ӿ�
	CUserListHeaderCtrl				m_UserListHeaderCtrl;				//��ͷ�ؼ�
	CSkinScrollBar					m_SkinScrollBar;					//��������

	//��Դ����
protected:
	tagColorUserList				m_ColorUserList;					//��ɫ����
	static CImageList				m_ExperienceImage;					//����λͼ
	static CImageList				m_UserStatusImage;					//״̬λͼ

	//��������
public:
	//���캯��
	CUserListControl();
	//��������
	virtual ~CUserListControl();

	//���غ���
public:
	//�ؼ���
	virtual VOID PreSubclassWindow();
	//�滭����
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//���ú���
public:
	//���ýӿ�
	bool SetMySelfUserID(DWORD dwMySelfUserID);
	//���ýӿ�
	bool SetGameLevelParser(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	bool SetGameUserManager(IUnknownEx * pIUnknownEx);
	//������ɫ
	bool SetColorUserList(const tagColorUserList & ColorUserList);
	//�����б�
	bool SetColumnDescribe(const tagColumnItem ColumnItem[], WORD wColumnCount);

	//���ܺ���
public:
	//�����û�
	bool InsertUserItem(IClientUserItem * pIClientUserItem);
	//�����û�
	bool UpdateUserItem(IClientUserItem * pIClientUserItem);
	//ɾ���û�
	bool DeleteUserItem(IClientUserItem * pIClientUserItem);

	//λͼ����
protected:
	//��ȡλ��
	INT GetInsertIndex(IClientUserItem * pIClientUserItem);
	//��ȡλͼ
	UINT GetImageIndex(IClientUserItem * pIClientUserItem, WORD wColumnIndex);
	//�����ַ�
	LPCTSTR GetDescribeString(IClientUserItem * pIClientUserItem, WORD wColumnIndex);

	//��̬����
protected:
	//���к���
	static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	//��Ϣӳ��
protected:
	//������Ϣ
	VOID OnNcDestroy();
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//����б�
	VOID OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//�û��б�
class CLIENT_CONTROLS_CLASS CSearchUserListControl : public CListCtrl
{
	//״̬����
protected:
	bool							m_bAscendSort;						//�����־
	bool							m_bCreateColumn;					//��������
	DWORD							m_dwMySelfUserID;					//�Լ���Ϣ

	//�б���Ϣ
protected:
	WORD							m_wColumnCount;						//�б���Ŀ
	BYTE							m_cbDataDescribe[MAX_COLUMN];		//��������

	//�������
protected:
	CUserListHeaderCtrl				m_UserListHeaderCtrl;				//��ͷ�ؼ�
	CSkinScrollBar					m_SkinScrollBar;					//��������

	//��Դ����
protected:
	tagColorUserList				m_ColorUserList;					//��ɫ����

	//��������
public:
	//���캯��
	CSearchUserListControl();
	//��������
	virtual ~CSearchUserListControl();

	//���غ���
public:
	//�ؼ���
	virtual VOID PreSubclassWindow();
	//�滭����
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//���ú���
public:
	//���ýӿ�
	bool SetMySelfUserID(DWORD dwMySelfUserID);
	//������ɫ
	bool SetColorUserList(const tagColorUserList & ColorUserList);
	//�����б�
	bool SetColumnDescribe(const tagColumnItem ColumnItem[], WORD wColumnCount);

	//���ܺ���
public:
	//�����û�
	bool InsertUserItem(IGlobalUserItem * pIGlobalUserItem);
	//�����û�
	bool UpdateUserItem(IGlobalUserItem * pIGlobalUserItem);
	//ɾ���û�
	bool DeleteUserItem(IGlobalUserItem * pIGlobalUserItem);

	//λͼ����
protected:
	//��ȡλ��
	INT GetInsertIndex(IGlobalUserItem * pIGlobalUserItem);
	//��ȡλͼ
	UINT GetImageIndex(IGlobalUserItem * pIGlobalUserItem, WORD wColumnIndex);
	//�����ַ�
	LPCTSTR GetDescribeString(IGlobalUserItem * pIGlobalUserItem, WORD wColumnIndex);

	//��̬����
protected:
	//���к���
	static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	//��Ϣӳ��
protected:
	//������Ϣ
	VOID OnNcDestroy();
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//����б�
	VOID OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif