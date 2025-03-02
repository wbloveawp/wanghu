#ifndef ANDROID_INTERFACE_HEAD_FILE
#define ANDROID_INTERFACE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//�ӿ�����
interface IAndroidService;
interface IAndroidUserItem;
interface IAndroidUserItemSink;

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IAndroidService INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidService = { 0x5468bfb,0xeb56,0x480c,0xa9,0x82,0x62,0x1a,0xc2,0x20,0xf9,0xa2 };
#else
#define VER_IAndroidService INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidService = { 0xcf77eef2,0xbe9,0x49d8,0xb4,0x50,0xcb,0x27,0xe5,0x5f,0xc8,0x44 };
#endif

//�����˽ӿ�
interface IAndroidService : public IUnknownEx
{
	//���ܽӿ�
public:
	//����������
	virtual VOID* __cdecl CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer) = NULL;
};

DECLARE_MODULE_DYNAMIC(AndroidService);

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAndroidUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItem={0x014b4ef4,0xa9d5,0x45cb,0x0092,0xa1,0x01,0xcf,0xe9,0xe7,0x0f,0x3c};
#else
	#define VER_IAndroidUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItem={0xebb22c87,0x7612,0x4707,0x00b3,0xf8,0x95,0xdd,0x51,0xad,0x93,0x98};
#endif

//�����˽ӿ�
interface IAndroidUserItem : public IUnknownEx
{
	//״̬����
public:
	//��ȡ״̬
	virtual BYTE __cdecl GetGameStatus()=NULL;
	//����״̬
	virtual VOID __cdecl SetGameStatus(BYTE cbGameStatus)=NULL;

	//��Ϣ�ӿ�
public:
	//��ȡ I D
	virtual DWORD __cdecl GetUserID()=NULL;
	//���Ӻ���
	virtual WORD __cdecl GetTableID()=NULL;
	//���Ӻ���
	virtual WORD __cdecl GetChairID()=NULL;
	//�û�״̬
	virtual BYTE __cdecl GetUserStatus()=NULL;
	//�û�����
	virtual LPCWSTR __cdecl GetAccounts()=NULL;	

	//������Ϣ
public:
	//��������
	virtual BYTE __cdecl GetScoreKind()=NULL;
	//�û�����
	virtual LONGLONG __cdecl GetUserScore()=NULL;

	//����ӿ�
public:
	//���ͺ���
	virtual bool __cdecl SendSocketData(WORD wSubCmdID)=NULL;
	//���ͺ���
	virtual bool __cdecl SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	
	//���ܽӿ�
public:
	//ɾ��ʱ��
	virtual bool __cdecl KillGameTimer(UINT nTimerID)=NULL;
	//����ʱ��
	virtual bool __cdecl SetGameTimer(UINT nTimerID, UINT nElapse)=NULL;
	//����׼��
	virtual bool __cdecl SendUserReady(VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool __cdecl SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)=NULL;

	//���ú���
public:
	//���ð汾
	virtual VOID __cdecl SetAdroidVersion( DWORD dwVersion )=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAndroidUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItemSink={0x366aba89,0xcca2,0x40d6,0x0087,0x95,0xb5,0xb2,0xb6,0x1d,0x29,0x80};
#else
	#define VER_IAndroidUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItemSink={0x4cbe7840,0x5068,0x42fb,0x0083,0x38,0x8a,0xb6,0x58,0x65,0xa5,0xdd};
#endif

//�����˽ӿ�
interface IAndroidUserItemSink : public IUnknownEx
{
	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool __cdecl InitUserItemSink(IUnknownEx * pIUnknownEx)=NULL;
	//���ýӿ�
	virtual bool __cdecl RepositUserItemSink()=NULL;

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool __cdecl OnEventTimer(UINT nTimerID)=NULL;
	//��Ϸ��Ϣ
	virtual bool __cdecl OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��Ϸ��Ϣ
	virtual bool __cdecl OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//������Ϣ
	virtual bool __cdecl OnEventGameScene(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)=NULL;

	//�û��¼�
public:
	//�û�����
	virtual VOID __cdecl OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//�û��뿪
	virtual VOID __cdecl OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//�û�����
	virtual VOID __cdecl OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//�û�״̬
	virtual VOID __cdecl OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//�û���λ
	virtual VOID __cdecl OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#endif