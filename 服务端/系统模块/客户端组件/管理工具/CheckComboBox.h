#ifndef CHECK_COMBOBOX_HEAD_FILE
#define CHECK_COMBOBOX_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////////
//��������
struct tagItemData
{
	BOOL								bChecked;						//ѡ�б�ʶ
	DWORD_PTR							dwDataPtr;						//���ݶ���
};

//////////////////////////////////////////////////////////////////////////////////////
//��ѡ������
class CCheckComboBox : public CComboBox
{
	//��������
protected:
	HWND								m_hListBox;						//�������	
	BOOL								m_bTextUpdated;					//���±�ʶ
	BOOL								m_bItemHeightSet;				//���ñ�ʶ
	CString								m_strText;						//��ѡ�ı�

	//��������
public:
	//���캯��
	CCheckComboBox();
	//��������
	virtual ~CCheckComboBox();

	//���ܺ���
public:
	//��������
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//ѡ������
	INT SetCheck(INT nIndex, BOOL bFlag);
	//��ȡѡ��
	BOOL GetCheck(INT nIndex);
	//ѡ����Ŀ
	INT GetCheckCount();
	//ȫѡ����
	void SelectAll(BOOL bCheck = TRUE);
	//��ȡ�ļ�
	CString GetText() { return m_strText; }

	//��д����
public:
	//����ִ�
	int AddString(LPCTSTR lpszString);	
	//�����ִ�
	int InsertString(int nIndex, LPCTSTR lpszString);

	//���غ���
protected:
	//�滭����
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//��������
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//ɾ������
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);

	//��������
protected:
	//�����ı�
	void RecalcText();

	//��Ϣ����
protected:
	//�ؼ���ɫ
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);
	//��ȡ�ı�
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	//�ļ�����
	afx_msg LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	//������Ϣ
	afx_msg void OnDropDown();

	DECLARE_MESSAGE_MAP()	
};

/////////////////////////////////////////////////////////////////////////////
#endif
