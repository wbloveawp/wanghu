#ifndef MISSION_STOCK_HEAD_FILE
#define MISSION_STOCK_HEAD_FILE
#pragma once

#include "StdAfx.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////////////
//����ö��
enum emStockAction
{
	SAction_NULL=0,
	SAction_QueryStockInfo,			//��ȡ���
	SAction_AppendStockInfo,		//��ӿ��
	SAction_ModifyStockInfo,		//�޸Ŀ��
	SAction_DeleteStockInfo,		//ɾ�����
	SAction_AdjustStockScore,		//�������
	SAction_AdjustJackpotScore		//�����ʽ�
};

//////////////////////////////////////////////////////////////////////////////////
//�ӿڶ���
interface IStockOperateCenter
{
	//�ؼ�ʹ��
	virtual VOID OnEventEnableControls(BOOL bEnabled)=NULL;
	//�������
	virtual VOID OnEventStockOperateResult(WORD wSubCommdID, WORD wStockCount, tagStockInfo * pStockInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//��������
class CMissionStock : public CWnd, public CMissionItem
{
	//�洢����
protected:
	WORD								m_wDataSize;							//���ݴ�С
	BYTE								m_cbDataBuffer[SOCKET_PACKET];			//���ݻ���

	//��������
protected:
	emStockAction						m_StockAction;							//��涯��	

	//�ӿڱ���
protected:
	IStockOperateCenter *				m_pIStockOperateCenter;					//���ӿ�

	//��̬����
protected:
	static CMissionStock *				m_pMissionStock;						//����ָ��

	//��������
public:
	//���캯��
	CMissionStock();
	//��������
	virtual ~CMissionStock();

	//���غ���
public:
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//���ýӿ�
public:
	//���ýӿ�
	VOID SetStockOperateCenter(IStockOperateCenter* pIStockOperateCenter);

	//��������
public:
	//ɾ�����
	VOID DeleteStockInfo(WORD wStockID);
	//��ѯ���
	VOID QueryStockInfo(LPCTSTR pszKindIDList);
	//�޸Ŀ��
	VOID ModifyStockInfo(tagStockInfo* pStockInfo);
	//��ӿ��
	VOID AppendStockInfo(tagStockInfo* pStockInfo);
	//�������
	VOID AdjustStockScore(WORD wStockID,SCORE lChangeScore);
	//�����ʽ�
	VOID AdjustJackpotScore(WORD wStockID,WORD wLevelID,SCORE lChangeScore);

	//��̬����
public:
	//��ȡ����
	static CMissionStock * GetInstance() { return m_pMissionStock; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif