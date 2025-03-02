
// DataDefine.h : ͷ�ļ�
//

#pragma once

//#pragma pack(1)
//////////////////////////////////////////////////////////////////////////

#define VESION							3				//��ǰ�汾
#define MAX_ALLOT_COUNT					54				//��������
#define MAX_CARDS_NUMBER					4				//�˿˸���
#define MAX_PLAYER_COUNT					8				//��λ��Ŀ
#define MAX_DIPAI_COUNT					5				//������Ŀ
#define DIPAI_CHAIRID						0xFFFF			//������λ
#define DIPAI_CTRL_INDEX					8				//�ؼ�����
#define MAX_GAME_PLAYER					8					//�������
#define MAX_GAME_HAND_CARD				20					//�������

//////////////////////////////////////////////////////////////////////////
//�ļ��ṹ
struct tagFileHead
{
	BYTE cbFlags1;
	BYTE cbFlags2;
	BYTE cbFlags3;
	BYTE cbFlags4;
	DWORD dwVesion;	
};

//������Ϣ
struct tagAllotCardInfo
{
	BYTE cbDipaiCard;
	BYTE cbLaiziCard;
	DWORD dwCardsNum;
	DWORD dwChairCount;
};

//��λ����
struct tagChairCardData
{
	WORD wChairID;
	WORD wCardCount;
	BYTE cbCardData[MAX_ALLOT_COUNT];
};


//��������
struct tagConfigCard
{
	BOOL									bAllotFlag;						//���Ʊ�ʶ
	BYTE									cbCardData[200];				//�˿�����
};