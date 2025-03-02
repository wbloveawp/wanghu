#include "stdafx.h"
#include "Resource.h"
#include "DlgAddMatchDate.h"

//////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CDlgAddMatchDate, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgAddMatchDate::CDlgAddMatchDate(): CDialog(IDD_DATETIME_MATCH)
{
	m_TimeBegin = CTime::GetCurrentTime();
	m_TimeEnd = CTime::GetCurrentTime();
}

//析构函数
CDlgAddMatchDate::~CDlgAddMatchDate()
{
}

//数据交换
void CDlgAddMatchDate::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control( pDX,IDC_DATE_BEGIN,m_CtrlDateBegin );
	DDX_Control( pDX,IDC_TIME_BEGIN,m_CtrlTimeBegin );
	DDX_Control( pDX,IDC_DATE_END,m_CtrlDateEnd );
	DDX_Control( pDX,IDC_TIME_END,m_CtrlTimeEnd );
}

//初始化
BOOL CDlgAddMatchDate::OnInitDialog()
{
	__super::OnInitDialog();

	//更新控件
	m_CtrlDateBegin.SetTime( &m_TimeBegin );
	m_CtrlTimeBegin.SetTime( &m_TimeBegin );
	m_CtrlDateEnd.SetTime( &m_TimeEnd );
	m_CtrlTimeEnd.SetTime( &m_TimeEnd );

	return TRUE;  
}

//确定函数
VOID CDlgAddMatchDate::OnOK()
{
	UpdateData();

	//获取开始时间
	CTime DateBegin;
	m_CtrlDateBegin.GetTime( DateBegin );
	CTime TimeBegin;
	m_CtrlTimeBegin.GetTime( TimeBegin );
	CTime Time1( DateBegin.GetYear(),DateBegin.GetMonth(),DateBegin.GetDay(),TimeBegin.GetHour(),TimeBegin.GetMinute(),TimeBegin.GetSecond() );
	m_TimeBegin = Time1;

	//获取结束时间
	CTime DateEnd;
	m_CtrlDateEnd.GetTime( DateEnd );
	CTime TimeEnd;
	m_CtrlTimeEnd.GetTime( TimeEnd );
	CTime Time2( DateEnd.GetYear(),DateEnd.GetMonth(),DateEnd.GetDay(),TimeEnd.GetHour(),TimeEnd.GetMinute(),TimeEnd.GetSecond() );
	m_TimeEnd = Time2;

	//效验
	if( m_TimeBegin >= m_TimeEnd )
	{
		AfxMessageBox( TEXT("结束时间必须大于开始时间,请重新输入"),MB_ICONERROR );
		return;
	}
// 	CTime Time = CTime::GetCurrentTime();
// 	if( m_TimeBegin <= Time || m_TimeEnd <= Time )
// 	{
// 		AfxMessageBox( TEXT("比赛时间必须大于当前时间,请重新输入"),MB_ICONERROR );
// 		return;
// 	}

	__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////

