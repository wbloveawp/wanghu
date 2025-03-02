#ifndef CMD_MATCH_SERVER_HEAD_FILE
#define CMD_MATCH_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����

//��������
#define MDM_AM_MATCH_SERVICE		10									//��������

//////////////////////////////////////////////////////////////////////////

//��������
#define SUB_AM_C_ENTER_MATCH		   1							 	//�������
#define SUB_AM_C_LEAVE_MATCH		   2								//�뿪����
#define SUB_AM_C_QUERY_MATCH_SESSION   3								//��ѯ����
#define SUB_AM_C_QUERY_MATCH_RANKING   4								//��ѯ����
#define SUB_AM_C_QUERY_MATCH_TABLE	   5								//��������
#define SUB_AM_C_QUERY_SIGNUP_USER	   6								//�����û�

//��ȯ����
#define SUB_AM_C_BUY_MATCH_TICKET	   10								//������ȯ
#define SUB_AM_C_QUERY_MATCH_TICKET	   11								//��ѯ��ȯ

//������¼
#define SUB_AM_C_QUERY_MATCH_RECORD	   20								//��ѯ��¼
#define SUB_AM_C_QUERY_MATCH_RECORD_BOARD 21							//��¼����

//��������
#define SUB_AM_S_MATCH_OPTION_LIST	   100								//�����б�
#define SUB_AM_S_MATCH_OPTION_STATUS   101								//����״̬

//��������
#define SUB_AM_S_MATCH_SESSION_ITEM	   102								//��������
#define SUB_AM_S_MATCH_SESSION_LIST	   103								//�����б�
#define SUB_AM_S_MATCH_SESSION_DETAIL  104								//��������
#define SUB_AM_S_MATCH_SESSION_SIGNUP  105								//���α���
#define SUB_AM_S_MATCH_SESSION_STATUS  106								//����״̬
#define SUB_AM_S_MATCH_SESSION_RANKING 107								//��������
#define SUB_AM_S_MATCH_ENTER_FINISH	   109								//�������

//��ȯ���
#define SUB_AM_S_BUY_TCIKET_RESULT	   110								//��ȯ���
#define SUB_AM_S_QUERY_TCIKET_RESULT   111								//��ȯ��Ϣ

//������¼
#define SUB_AM_S_MATCH_RECORD_LIST	   120								//��¼�б�
#define SUB_AM_S_MATCH_RECORD_BOARD	   121								//��¼����

//��������
#define SUB_AM_S_MATCH_TABLE_LIST	   130								//�����б�

//�����û�
#define SUB_AM_S_SIGNUP_USER_LIST	   140								//�û��б�	

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//��ѯ����
struct CMD_AM_C_QueryMatchSession
{
	DWORD							dwMatchID;							//������ʶ	
};

//��ѯ����
struct CMD_AM_C_QueryMatchRanking
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
};

//������
struct CMD_AM_C_BuyMatchTicket
{
	DWORD							dwMatchID;							//������ʶ	
	SCORE							lMatchNo;							//��������	
};

//��ѯ��ȯ
struct CMD_AM_C_QueryMatchTicket
{
	DWORD							dwMatchID;							//������ʶ	
	SCORE							lMatchNo;							//��������
};

//��ѯ��¼����
struct CMD_AM_C_QueryMatchRecordBoard 
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
};

//���������б�
struct CMD_AM_C_QueryMatchTable
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
};

//���ұ����û�
struct CMD_AM_C_QuerySignupUser
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
};

//����״̬
struct CMD_AM_S_MatchOptionStatus
{
	DWORD							dwMatchID;							//������ʶ
	BYTE							cbOptionStatus;						//����״̬
};

//��������
struct CMD_AM_S_MatchSessionItem
{
	tagMatchSessionItem				MatchSessionItem;					//��������
};

//��������
struct CMD_AM_S_MatchSessionDetail
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������	
	BYTE							cbMatchType;						//��������
	WORD							wStartCount;						//��ʼ����
	WORD							wSignupCount;						//��������
};

//������Ϣ
struct CMD_AM_S_MatchOptionSignup
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
	WORD							wStartCount;						//��ʼ����
	WORD							wSignupCount;						//��������		
};

//����״̬
struct CMD_AM_S_MatchSessionStatus
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
	BYTE							cbMatchStatus;						//����״̬
};

//��������
struct CMD_AM_S_MatchSessionRanking
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	SCORE							lMatchScore;						//��������
};

//������
struct CMD_AM_S_BuyTicketResult
{
	DWORD							dwMatchID;							//������ʶ	
	LONGLONG						lMatchNo;							//��������
	DWORD							dwTicketCount;						//��ȯ����
};

//��ѯ��ȯ
struct CMD_AM_S_QueryTicketResult
{
	DWORD							dwMatchID;							//������ʶ	
	LONGLONG						lMatchNo;							//��������
	DWORD							dwTicketCount;						//��ȯ����
};


//��¼����
struct CMD_AM_S_MatchRecordBoard
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	tagMatchRecordUserItem			MatchRecordUserItem[0];				//�û�����
};

//�����б�
struct CMD_AM_S_MatchTableList
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	WORD							wTableCount;						//��������
	tagMatchTableInfo				MatchTableInfo[0];					//������Ϣ
};

//�û��б�
struct CMD_AM_S_SignupUserList
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	WORD							wSignupCount;						//��������
	WORD							wUserCount;							//�û�����
	tagSignupUserItem				SignupUserItem[0];					//�û��б�
};

//////////////////////////////////////////////////////////////////////////

//��������
#define MDM_MS_MATCH_SERVICE		11									//��������

//////////////////////////////////////////////////////////////////////////

//��������
#define SUB_MS_C_CREATE_MATCH		300									//��������
#define SUB_MS_C_REMOVE_MATCH		301									//�Ƴ�����
#define SUB_MS_C_DISMISS_MATCH		302									//��ɢ����
#define SUB_MS_C_COLLECT_MATCH		303									//�ռ�����

//����ͬ��
#define SUB_MS_S_COLLECT_MATCH		410									//�����ռ�
#define SUB_MS_S_COLLECT_FINISH		411									//�ռ����

//��������
#define SUB_MS_S_MATCH_GROUP		420									//��������
#define SUB_MS_S_MATCH_START		421									//������ʼ
#define SUB_MS_S_MATCH_CANCEL		422									//����ȡ��
#define SUB_MS_S_MATCH_FINISH		423									//��������
#define SUB_MS_S_MATCH_COMPLETE		424									//�������
#define SUB_MS_S_MATCH_SIGNUP		425									//��������
#define SUB_MS_S_MATCH_UNSIGNUP		426									//ȡ������
#define SUB_MS_S_MATCH_GIVEUP		427									//��������
#define SUB_MS_S_MATCH_REVIVE		428									//��������
#define SUB_MS_S_MATCH_ELIMINATE	429									//������̭
#define SUB_MS_S_CREATE_FAILED		430									//����ʧ��

//��������
#define SUB_MS_S_MATCH_TABLE_APPEND 450									//�������
#define SUB_MS_S_MATCH_TABLE_UPDATE	451									//��������
#define SUB_MS_S_MATCH_TABLE_REMOVE	452									//�Ƴ�����		
#define SUB_MS_S_MATCH_TABLE_RECYCLE 453								//��������

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//��������
struct CMD_MS_C_CreateMatch
{
	DWORD							dwTimeStamp;						//ʱ���
	WORD							wTableCount;						//��������	
	tagMatchOption					MatchOption;						//��������
};

//�Ƴ�����
struct CMD_MS_C_RemoveMatch
{
	DWORD							dwMatchID;							//������ʶ
};

//��ɢ����
struct CMD_MS_C_DismissMatch
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
};

//�ռ�����
struct CMD_MS_S_CollectMatch
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������	
	WORD							wTableCount;						//��������
	bool							bActiveGroup;						//��ǰ����
	BYTE							cbMatchStatus;						//����״̬	
	SYSTEMTIME						MatchStartTime;						//����ʱ��
	SYSTEMTIME						MatchSignupStart;					//������ʼ
	SYSTEMTIME						MatchSignupCutOff;					//������ֹ
	SYSTEMTIME						MatchSaleTicketTime;				//��Ʊʱ��

	//�����Ϣ
	WORD							wStartCount;						//��������
	WORD							wPlayerCount;						//��Ϸ����
	WORD							wMaxPlayerCount;					//�������

};

//��������
struct CMD_MS_S_MatchGroup
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
	WORD							wTableCount;						//��������
	WORD							wStartCount;						//��������
	BYTE							cbMatchStatus;						//����״̬
	WORD							wMaxPlayerCount;					//�������
	SYSTEMTIME						MatchStartTime;						//����ʱ��
	SYSTEMTIME						MatchSignupStart;					//������ʼ
	SYSTEMTIME						MatchSignupCutOff;					//������ֹ
	SYSTEMTIME						MatchSaleTicketTime;				//��Ʊʱ��
};

//������ʼ
struct CMD_MS_S_MatchStart
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������	
};

//����ȡ��
struct CMD_MS_S_MatchCancel
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������	
};

//������ʼ
struct CMD_MS_S_MatchFinish
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������	
};

//�������
struct CMD_MS_S_MatchComplete
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������	

	//�û���Ϣ
	WORD							wUserCount;							//�û�����
	tagMatchUserItem				MatchUserItem[16];					//�����û�
};

//��������
struct CMD_MS_S_MatchSignup
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������

	WORD							wSignupCount;						//��������
	tagMatchUserItem				MatchUserItem;						//�����û�	
};

//ȡ������
struct CMD_MS_S_MatchUnSignup
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������

	WORD							wSignupCount;						//��������
	tagMatchUserItem				MatchUserItem;						//�����û�	
};

//��������
struct CMD_MS_S_MatchGiveup
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������

	tagMatchUserItem				MatchUserItem;						//�����û�
};

//��������
struct CMD_MS_S_MatchRevive
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������

	WORD							wSignupCount;						//��������
	tagMatchUserItem				MatchUserItem;						//�����û�
};

//������̭
struct CMD_MS_S_MatchEliminate
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
	tagMatchUserItem				MatchUserItem;						//�����û�
};

//����ʧ��
struct SUB_MS_S_CreateFailed
{
	DWORD							dwMatchID;							//������ʶ
	WORD							wTableCount;						//��������
	WORD							wErrorCode;							//�������
};

//���Ӽ���
struct SUB_MS_S_MatchTableAppend
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	WORD							wTableCount;						//��������
	tagMatchTableInfo				MatchTableInfo[0];					//������Ϣ
};

//���Ӹ���
struct SUB_MS_S_MatchTableUpdate
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	WORD							wTableID;							//���ӱ�ʶ
	BYTE							cbGameStatus;						//��Ϸ״̬
	BYTE							cbTeamCount;						//�Ŷ�����
	SCORE							lTeamScore[4];						//�Ŷӻ���
};

//����ɾ��
struct SUB_MS_S_MatchTableRemove
{
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	WORD							wTableID;							//���ӱ�ʶ	
};

//�ͷ�����
struct CMD_MS_S_MatchTableRecycle
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
	WORD							wTableCount;						//��������
};

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif
