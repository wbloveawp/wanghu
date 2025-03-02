#ifndef CMD_COMMOM_HEAD_FILE
#define CMD_COMMOM_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

#define MDM_CM_SYSTEM				1000								//ϵͳ����

//��Ϣ����
#define SUB_CM_SYSTEM_MESSAGE		1									//ϵͳ��Ϣ
#define SUB_CM_ACTION_MESSAGE		2									//������Ϣ

//�û�����
#define SUB_CM_PACK_GOODS			10									//������Ʒ
#define SUB_CM_GOODS_UPDATE			11									//��Ʒ����
#define SUB_CM_WEALTH_UPDATE		12									//�Ƹ�����
#define SUB_CM_LOVELINESS_UPDATE	13									//��������
#define SUB_CM_CONFIG_UPDATE		14									//���ø���

//�������
#define SUB_CM_OPERATE_SUCCESS		20									//�����ɹ�
#define SUB_CM_OPERATE_FAILURE		31									//����ʧ��

//////////////////////////////////////////////////////////////////////////

//��������
#define SMT_CHAT					0x0001								//������Ϣ
#define SMT_EJECT					0x0002								//������Ϣ
#define SMT_GLOBAL					0x0004								//ȫ����Ϣ
#define SMT_PROMPT					0x0008								//��ʾ��Ϣ
#define SMT_TABLE_ROLL				0x0010								//������Ϣ

//��������
#define SMT_CLOSE_ROOM				0x0100								//�رշ���
#define SMT_CLOSE_GAME				0x0200								//�ر���Ϸ
#define SMT_CLOSE_LINK				0x0400								//�ж�����

//��ʾ����
#define SMT_SHOW_MOBILE				0x1000								//�ֻ���ʾ	
#define SMT_SHOW_FLASH				0x2000								//��ҳ��ʾ	

//ϵͳ��Ϣ
struct CMD_CM_SystemMessage
{
	WORD							wType;								//��Ϣ����
	WORD							wElpase;							//��ʱʱ��
	WORD							wLength;							//��Ϣ����
	TCHAR							szString[1024];						//ѡ������
};

//////////////////////////////////////////////////////////////////////////
// 
//������Ʒ
struct CMD_CM_PackGoods
{
	WORD							wGoodsCount;						//��Ʒ����
	tagGoodsHoldInfo				GoodsHoldInfo[0];					//��Ʒ��Ϣ
};

//��Ʒ����
struct CMD_CM_GoodsUpdate
{
	WORD							wGoodsCount;						//��Ʒ����
	tagGoodsHoldInfo				GoodsHoldInfo[0];					//��Ʒ��Ϣ
};

//�Ƹ�����
struct CMD_CM_WealthUpdate
{
	WORD							wItemCount;							//��������
	tagWealthItem					WealthItem[0];						//�Ƹ�����
};

//��������
struct CMD_CM_LoveLinessUpdate
{
	DWORD							dwLoveLiness;						//����ֵ
};

//�������Ͷ���
#define CM_CONFIG_TYPE_NICKNAME		0x00000001							//�ǳ��޸�����
//��������
struct CMD_CM_ConfigUpdate
{
	DWORD							dwType;								//��������
};
//�ǳ��޸�����
struct CMD_CM_Config_NickName
{
	WORD							wFreeModifyTimes;					//����޸Ĵ���
	WORD							wDailyModifyTimes;					//ÿ�տ��޸Ĵ���
	DWORD							dwGoodsID;							//������ƷID
	DWORD							dwGoodsCount;						//������Ʒ����
};
//////////////////////////////////////////////////////////////////////////

//������Ϣ
struct CMD_CM_ActionMessage
{
	WORD							wType;								//��Ϣ����
	WORD							wLength;							//��Ϣ����
	UINT							nButtonType;						//��ť����
	WCHAR							szString[1024];						//��Ϣ����
};

//////////////////////////////////////////////////////////////////////////

//������Ϣ
#define DTP_CM_DESCRIBE				1									//������Ϣ
#define DTP_CM_GOODSLIST			2									//��Ʒ�б�

//��������
#define OFC_SUCCESS					0									//�����ɹ�
#define OFC_FAILURE					1									//����ʧ��
#define OFC_DB_EXCEPTION			2									//���ݿ��쳣
#define OFC_MAIL_NOTEXIST			3									//�ʼ�������

//����ʧ��
struct CMD_CM_OperateFailure
{
	LONG							lErrorCode;							//�������
	WORD							wRequestCmdID;						//��������
};

//�����ɹ�
struct CMD_CM_OperateSuccess
{
	LONG							lErrorCode;							//��������
	WORD							wRequestCmdID;						//��������
};

//////////////////////////////////////////////////////////////////////////

//���д�ȡ
struct CMD_CM_BankAccess
{
	DWORD							dwUserID;							//�û�ID
	LONGLONG						lAmount;							//�������
	BYTE							cbGameAction;						//��Ϸ����
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//����ת��
struct CMD_CM_BankTransfer
{
	DWORD							dwUserID;						//����ID
	LONGLONG						lAmount;						//�������
	TCHAR							szRecvUser[LEN_ACCOUNTS];		//�����û�
	TCHAR							szPassword[LEN_PASSWORD];		//�û�����
	bool							bIsAccount;						//�Ƿ��ʺ�
};

//��ѯ�û�
struct CMD_CM_BankQuery
{
	TCHAR							szRecvUser[LEN_ACCOUNTS];		//�����û�
	bool							bIsAccount;						//�Ƿ��ʺ�
};

//���½��
struct CMD_CM_BankUpdate
{
	DWORD							dwUserID;						//�û�ID
	TCHAR							szPassword[LEN_PASSWORD];		//�û�����
};

enum BANK_OPERATE_BYTE
{
	BANK_TYPE_STORAGE = 0,
	BANK_TYPE_DRAWOUT,
	BANK_TYPE_TRANSFER,
	BANK_TYPE_QUERY,
	BANK_TYPE_MODIFYPASS,
	BANK_TYPE_UPDATE,
	BANK_TYPE_MAX

};
#define	MIN_BANK_COUNT					10000							//������Сֵ

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif