#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////
//�궨��
#define MAX_COUNT 4
//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//�˿�����
#define D2H_VALUE0					0									//��Ч����
#define D2H_D2H						199									//��������
#define D2H_DOBLUE_Q				198									//�������
#define D2H_DOBLUE_2				197									//�ض�����
#define D2H_DOBLUE_8				196									//�ʶ�����
#define D2H_DOBLUE_4				195									//�Ͷ�����
#define D2H_DOBLUE_46A				194									//�ж�����
#define D2H_DOBLUE_67AJ				193									//�¶�����
#define D2H_DOBLUE_9875				192									//��������
#define D2H_TH						191									//�������
#define D2H_DH						190									//�ػ�����
#define D2H_TG						189									//�������
#define D2H_DG						188									//�ظ�����

//////////////////////////////////////////////////////////////////////////
static CString GetFileDialogPath()
{
	CString strFileDlgPath;
	TCHAR szModuleDirectory[MAX_PATH];	//ģ��Ŀ¼
	GetModuleFileName(AfxGetInstanceHandle(),szModuleDirectory,sizeof(szModuleDirectory));
	int nModuleLen=lstrlen(szModuleDirectory);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE")) + 1;
	if (nModuleLen<=nProcessLen) 
		return TEXT("");
	szModuleDirectory[nModuleLen-nProcessLen]=0;
	strFileDlgPath = szModuleDirectory;
	return strFileDlgPath;
}

static void NcaTextOut(CString strInfo)
{
	CString strName = GetFileDialogPath()+	TEXT("\\CheXuan.log");
	CTime time = CTime::GetCurrentTime() ;
	CString strTime ;
	strTime.Format( TEXT( "%d-%d-%d %d:%d:%d" ) ,
		time.GetYear() ,
		time.GetMonth() ,
		time.GetDay() ,
		time.GetHour() ,
		time.GetMinute() ,
		time.GetSecond()
		);
	CString strMsg;
	strMsg.Format(TEXT("%s,%s\r\n"),strTime,strInfo);

	CFile fLog;
	if(fLog.Open( strName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite ))
	{

		fLog.SeekToEnd(); 	
		int strLength=strMsg.GetLength();
#ifdef _UNICODE
		BYTE bom[2] = {0xff, 0xfe};
		fLog.Write(bom,sizeof(BYTE)*2);
		strLength*=2;
#endif
		fLog.Write((LPCTSTR)strMsg,strLength);
		fLog.Close();
	}

}
//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
	//��������
private:
	BYTE							m_cbRuleOption;						//��������
	static BYTE						m_cbCardListData[32];				//�˿˶���

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ú���
public:
	//��������
	VOID SetRuleOption(BYTE cbRuleOption) { m_cbRuleOption=cbRuleOption; }

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//���ƺ���
public:
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//���ܺ���
public:	
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//���⻻��
	void ChangeSpeCard( BYTE cbCardData[], BYTE cbCardCount);
	//������ֵ
	BYTE GetSpeCardValue(BYTE cbCardData[], BYTE cbCardCount);
	//��������
	bool IsHoldSpecialCard(BYTE cbCardData[], BYTE cbCardCount);	
	//�Ա��˿�
	BYTE CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount);
	//���⴦����5��38
	BYTE DealSpeCard(BYTE cbFirstData[], BYTE cbNextData[]);
};

//////////////////////////////////////////////////////////////////////////

#endif
