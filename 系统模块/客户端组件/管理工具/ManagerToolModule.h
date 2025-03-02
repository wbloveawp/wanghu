#ifndef GAME_PLAZA_MODULE_HEAD_FILE
#define GAME_PLAZA_MODULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//����˵��

//��˵��
class CTableResource;

//�ӿ�˵��
interface ITableView;
interface ITableFrameView;
interface IServerViewItem;

//�ṹ˵��
struct tagTableMirrorAttrib;

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//��������
struct tagSearchTable
{
	bool							bOneNull;							//һ����λ
	bool							bTwoNull;							//������λ
	bool							bAllNull;							//ȫ��λ��
	bool							bNotFull;							//��ȫ��λ
	bool							bFilterPass;						//��������
	WORD							wStartTableID;						//��ʼ����
	WORD							wResultTableID;						//�������
	WORD							wResultChairID;						//�������
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGlobalUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IGlobalUnits={0xe5090ee7,0x0df4,0x4b96,0x00ac,0x51,0x3d,0x34,0x9c,0x16,0xe8,0xb6};
#else
	#define VER_IGlobalUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IGlobalUnits={0x7438c9bb,0x672d,0x4d04,0x00b7,0xd2,0x52,0xb7,0x7f,0xaa,0x8d,0xd1};
#endif

//ȫ�ֵ�Ԫ
interface IGlobalUnits : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableView INTERFACE_VERSION(1,1)
	static const GUID IID_ITableView={0x3257c494,0x8c75,0x4b5a,0x00b2,0x77,0xbd,0xfd,0xb9,0x3c,0xf7,0x62};
#else
	#define VER_ITableView INTERFACE_VERSION(1,1)
	static const GUID IID_ITableView={0x98298df9,0x8eb6,0x4618,0x0089,0xa3,0xe6,0x37,0xec,0xc6,0xe7,0x44};
#endif

//������ͼ
interface ITableView : public IUnknownEx
{
	//���ܽӿ�
public:
	//��������
	virtual WORD __cdecl GetNullChairCount(WORD & wNullChairID)=NULL;
	//���ú���
	virtual VOID __cdecl InitTableView(WORD wTableID, WORD wChairCount, ITableFrameView * pITableFrameView)=NULL;

	//�û��ӿ�
public:
	//��ȡ�û�
	virtual IClientUserItem * __cdecl GetUserInfo(WORD wChairID)=NULL;
	//������Ϣ
	virtual bool __cdecl SetUserInfo(WORD wChairID, IClientUserItem * pIClientUserItem)=NULL;

	//���ýӿ�
public:
	//��갴��
	virtual VOID __cdecl SetMouseDown(bool bMouseDown)=NULL;
	//������
	virtual VOID __cdecl SetFocusFrame(bool bFocusFrame)=NULL;
	//����λ��
	virtual VOID __cdecl SetHoverChairID(WORD wHoverChairID)=NULL;
	//����״̬ 
	virtual VOID __cdecl SetTableStatus(bool bPlaying, bool bLocker)=NULL;

	//�׷ֽӿ�
public:
	//���õ׷�
	virtual VOID __cdecl SetTableBaseScore(LONGLONG lBaseScore)=NULL;

	//��ѯ�ӿ�
public:
	//��Ϸ��־
	virtual bool __cdecl GetPlayFlag()=NULL;
	//�����־
	virtual bool __cdecl GetLockerFlag()=NULL;
	//������
	virtual bool __cdecl GetFocusFrame()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableViewMirror INTERFACE_VERSION(1,1)
	static const GUID IID_ITableViewMirror={0x31a64dfd,0xe5c2,0x4975,0x00a6,0x25,0xc8,0xc1,0xc7,0x6d,0x0e,0x5d};
#else
	#define VER_ITableViewMirror INTERFACE_VERSION(1,1)
	static const GUID IID_ITableViewMirror={0x52198850,0x071c,0x49c6,0x00a6,0x4f,0x20,0x80,0x13,0xec,0x45,0x70};
#endif

//����ӿ�
interface ITableViewMirror : public IUnknownEx
{
	//����ӿ�
public:
	//������ͼ
	virtual bool __cdecl UpdateMirrorView()=NULL;
	//�������
	virtual WORD __cdecl GetMirrorTableID()=NULL;

	//���ýӿ�
public:
	//��������
	virtual VOID __cdecl SetTableFrameView(ITableFrameView * pITableFrameView)=NULL;
	//���÷���
	virtual VOID __cdecl SetServerViewItem(IServerViewItem * pIServerViewItem)=NULL;
	//���ò���
	virtual bool __cdecl SetTableMirrorAttrib(tagTableMirrorAttrib & TableMirrorAttrib)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameView INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameView={0xac262468,0x4312,0x468f,0x0095,0xe7,0xae,0x32,0xa9,0x50,0xe5,0x7d};
#else
	#define VER_ITableFrameView INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameView={0xd53c2971,0xb7b1,0x48ed,0x008f,0xb0,0x9e,0x35,0x07,0x82,0xeb,0xaa};
#endif

//���ӿ��
interface ITableFrameView : public IUnknownEx
{
	//���ýӿ�
public:
	//��������
	virtual bool __cdecl CreateTableFrame(CWnd * pParentWnd, UINT uWndID)=NULL;
	//��������
	virtual bool __cdecl InitTableFrame(WORD wTableCount, WORD wChairCount, LPCTSTR pszResDirectory, IUnknownEx * pIUnknownEx,bool bHideUserInfo,bool bDrawBaseScore)=NULL;

	//��Ϣ�ӿ�
public:
	//������Ŀ
	virtual WORD __cdecl GetTableCount()=NULL;
	//������Ŀ
	virtual WORD __cdecl GetChairCount()=NULL;
	//��ȡ��Դ
	virtual CTableResource * __cdecl GetTableResource()=NULL;

	//��ͼ�ӿ�
public:
	//��ͼ����
	virtual CWnd * GetViewWnd()=NULL;
	//��ͼ����
	virtual bool OnEventViewData(WORD wCmdID, VOID * pData, WORD wDataSize)=NULL; 

	//�û��ӿ�
public:
	//��ȡ�û�
	virtual IClientUserItem * __cdecl GetUserInfo(WORD wTableID, WORD wChairID)=NULL;
	//������Ϣ
	virtual bool __cdecl SetUserInfo(WORD wTableID, WORD wChairID, IClientUserItem * pIClientUserItem)=NULL;

	//״̬�ӿ�
public:
	//��Ϸ��־
	virtual bool __cdecl GetPlayFlag(WORD wTableID)=NULL;
	//�����־
	virtual bool __cdecl GetLockerFlag(WORD wTableID)=NULL;	
	//������
	virtual bool __cdecl GetFocusFrame(WORD wTableID)=NULL;
	//�����ʶ
	virtual VOID __cdecl SetGameLoadFlag(bool bGameload)=NULL;
	//������
	virtual VOID __cdecl SetFocusFrame(WORD wTableID, bool bFocusFrame)=NULL;
	//����״̬ 
	virtual VOID __cdecl SetTableStatus(WORD wTableID, bool bPlaying, bool bLocker)=NULL;

	//�׷ֽӿ�
public:
	//���õ׷�
	virtual VOID __cdecl SetTableBaseScore(WORD wTableID,LONGLONG lBaseScore)=NULL;

	//��ͼ����
public:
	//���ӿ���
	virtual bool __cdecl EnsureVisibleTable(WORD wTableID)=NULL;
	//��������
	virtual bool __cdecl FlashGameTable(WORD wTableID)=NULL;
	//��������
	virtual bool __cdecl FlashGameChair(WORD wTableID, WORD wChairID)=NULL;

	//���ܽӿ�
public:
	//��������
	virtual bool __cdecl UpdateTableView(WORD wTableID)=NULL;
	//��ȡ����
	virtual ITableView * __cdecl GetTableArrayPtr(WORD wTableID)=NULL;
	//��������
	virtual WORD __cdecl GetNullChairCount(WORD wTableID, WORD & wNullChairID)=NULL;

	//����ӿ�
public:
	//ע������
	virtual bool __cdecl UnRegisterTableMirror(ITableViewMirror * pITableViewMirror)=NULL;
	//���¾���
	virtual bool __cdecl UpdateTableMirror(ITableViewMirror * pITableViewMirror, WORD wTableID)=NULL;
	//ע�᾵��
	virtual bool __cdecl RegisterTableMirror(WORD wTableID, ITableViewMirror * pITableViewMirror)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameViewSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameViewSink={0x94bf9ba0,0x48dc,0x4834,0x00a1,0x52,0xf4,0x67,0xae,0x34,0xe2,0xf8};
#else
	#define VER_ITableFrameViewSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameViewSink={0xf6b4805d,0x2792,0x4795,0x0087,0x01,0x57,0xf4,0x2b,0x6a,0x65,0x0b};
#endif

//����֪ͨ
interface ITableFrameViewSink : public IUnknownEx
{
	//���Ӱ�ť
	virtual VOID __cdecl OnHitTableButton(BYTE cbButton)=NULL;
	//���˫��
	virtual VOID __cdecl OnDoubleHitTable(WORD wTableID, bool bMirror)=NULL;
	//�������
	virtual VOID __cdecl OnLButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror)=NULL;
	//�Ҽ�����
	virtual VOID __cdecl OnRButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IMessageHandler INTERFACE_VERSION(1,1)
static const GUID IID_IMessageHandler={0xe28ce454,0xf7eb,0x49c2,0x0087,0x32,0x52,0xda,0xf3,0xb9,0x20,0x92};
#else
#define VER_IMessageHandler INTERFACE_VERSION(1,1)
static const GUID IID_IMessageHandler={0xfdd17562,0xf0e1,0x419c,0x0083,0x9c,0x53,0xbf,0xc0,0x3,0x2a,0x56};
#endif

//����ӿ�
interface IMessageHandler : public IUnknownEx
{
	//���ܽӿ�
public:
	//�����¼�
	virtual bool __cdecl OnEventNetworkLink(WORD wSocketID, INT nErrorCode)=NULL;
	//�����¼�
	virtual bool __cdecl OnEventNetworkMessage(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;
	//�����¼�
	virtual bool __cdecl OnEventNetworkShut(WORD wSocketID, BYTE cbShutReason)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerViewItem INTERFACE_VERSION(1,2)
	static const GUID IID_IServerViewItem={0x4cf09fc5,0x7a5e,0x4e41,0x00b4,0xe5,0xfd,0x40,0x1b,0xb0,0xf2,0xd9};
#else
	#define VER_IServerViewItem INTERFACE_VERSION(1,2)
	static const GUID IID_IServerViewItem={0xfbc533f9,0x9961,0x4ad2,0x008d,0x29,0xa2,0xf1,0x80,0x69,0x37,0x59};
#endif

//����ӿ�
interface IServerViewItem : public IUnknownEx
{
	//���ܽӿ�
public:
	//��������
	virtual bool __cdecl SearchGameTable(tagSearchTable & SearchTable)=NULL;
	//�������
	virtual VOID __cdecl SetChatTargetUser(IClientUserItem * pIClientUserItem)=NULL;
	//˽�ĻỰ
	virtual bool __cdecl WhisperConversation(IClientUserItem * pIClientUserItem[], WORD wUserCount)=NULL;
	//��ʾ����
	virtual VOID __cdecl ShowExpressionControl(CWnd * pParentWnd, IExpressionSink * pIExpressionSink, INT nXPos, INT nYPos)=NULL;

	//�û��˵�
public:
	//��ʾ�˵�
	virtual VOID __cdecl TrackSelectChatMenu()=NULL;
	//��ʾ�˵�
	virtual VOID __cdecl TrackUserItemMenu(LPCWSTR pszUserAccounts)=NULL;
	//��ʾ�˵�
	virtual VOID __cdecl TrackUserItemMenu(CUserListControl * pUserListControl, INT nItemIndex)=NULL;
	//��ʾ�˵�
	virtual VOID __cdecl TrackUserItemMenu(IGlobalUserItem *pIGlobalUserItem[], INT nItemCount)=NULL;

	//��Ϣ����
public:
	//��Ӵ���ӿ�
	virtual bool __cdecl AddMessageHandler(IUnknownEx *pIMessageHandler)=NULL;
	//ɾ������ӿ�
	virtual bool __cdecl RemoveMessageHandler(IUnknownEx *pIMessageHandler)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IChatMsgItem INTERFACE_VERSION(1,1)
static const GUID IID_IChatMsgItem={0x3a168ac6,0xd09c,0x4d26,0x00aa,0x45,0x1b,0xa,0x62,0x6,0x29,0x66};
#else
#define VER_IChatMsgItem INTERFACE_VERSION(1,1)
static const GUID IID_IChatMsgItem={0x8f04a9a0,0x5737,0x43e5,0x009a,0x1b,0xea,0x81,0x79,0x89,0xb0,0xa0};
#endif

//��Ϣ�ӿ�
interface IChatMsgItem : public IUnknownEx
{
	//���ܽӿ�
public:
	//������Ŀ
	virtual VOID __cdecl ResetChatMsg()=NULL;
	//��ȡ���
	virtual LPCTSTR __cdecl GetSendUserAccounts()=NULL;
	//��ȡ������Ϣ
	virtual LPCTSTR __cdecl GetMsg()=NULL;
	//��ȡ������ɫ
	virtual COLORREF __cdecl GetChatColor()=NULL;
	//�Ƿ��Լ���Ϣ
	virtual bool __cdecl IsSelfMsg()=NULL;
	//��ȡʱ��
	virtual const SYSTEMTIME& GetMsgTime()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IChatHistory INTERFACE_VERSION(1,1)
static const GUID IID_IChatHistory={0xb0a24da4,0x7e14,0x47ef,0x00ab,0xdc,0xd0,0xc3,0x65,0x48,0xc8,0xf9};
#else
#define VER_IChatHistory INTERFACE_VERSION(1,1)
static const GUID IID_IChatHistory={0xf00991f3,0xd009,0x4ca9,0x008b,0xd1,0x78,0x24,0x95,0xe,0xbf,0x8};
#endif

//�����¼
interface IChatHistory : public IUnknownEx
{
	//��ѯ����
public:
	//��ȡ��¼ID
	virtual DWORD __cdecl GetChatID()=NULL;
	//��ȡ��Ϣ��
	virtual INT_PTR __cdecl GetMsgItemCount()=NULL;
	//ö����Ϣ
	virtual IChatMsgItem* __cdecl EnumChatMsgItem(INT_PTR nEnumIndex)=NULL;

	//���ܺ���
public:
	//���ü�¼
	virtual VOID __cdecl ResetChatHistory()=NULL;
	//�����Ϣ
	virtual bool __cdecl AddMsgItem(LPCTSTR lpszSendUser,LPCTSTR lpszMsg,COLORREF crColor,bool bMySelfString)=NULL;
	//�����¼
	virtual bool __cdecl ClearHistory()=NULL;

	//�ļ�����
public:
	//�����¼
	virtual bool __cdecl Save()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IChatHistoryManager INTERFACE_VERSION(1,1)
static const GUID IID_IChatHistoryManager={0x94afc65f,0x2c55,0x46d3,0x00a0,0x84,0x6c,0x17,0x84,0x66,0xf0,0x15};
#else
#define VER_IChatHistoryManager INTERFACE_VERSION(1,1)
static const GUID IID_IChatHistoryManager={0x4e39ada4,0x239,0x4209,0x00a0,0x9b,0xc3,0xeb,0xd2,0x77,0xae,0xb4};
#endif

//�����¼����ӿ�
interface IChatHistoryManager : public IUnknownEx
{
	//���ýӿ�
public:
	//��ʼ��
	virtual VOID SetMyselftUserItem(IClientUserItem *pIMyselfUserItem)=NULL;

	//��ѯ�ӿ�
public:
	//��ȡ��ҽӿ�
	virtual IClientUserItem * __cdecl GetMyselftUserItem()=NULL;

	//���ܽӿ�
public:
	//�����¼
	virtual IChatHistory* __cdecl ActiveChatHistory(DWORD dwChatID)=NULL;
	//�رռ�¼
	virtual bool __cdecl CloseChatHistory(DWORD dwChatID)=NULL;
	//�����¼
	virtual bool __cdecl SaveChatHistory(DWORD dwChatID)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#endif