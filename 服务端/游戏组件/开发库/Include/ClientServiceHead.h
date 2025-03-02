#ifndef CLIENT_SERVICE_HEAD_HEAD_FILE
#define CLIENT_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨ͷ�ļ�
#include "Platform.h"           

//���ͷ�ļ�
#include "CommonServiceHead.h"                

//////////////////////////////////////////////////////////////////////////
//�����궨��

//ģ�鶨��
#ifndef _DEBUG
	#define CLIENT_SERVICE_DLL_NAME	TEXT("ClientService.dll")			//��� DLL ����
#else
	#define CLIENT_SERVICE_DLL_NAME	TEXT("ClientServiceD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////

//���ȶ���
#define LEN_NOTE					32									//��ע����

//��ϵ��Ϣ
struct tagCompanionInfo
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	BYTE							cbCompanion;						//�û���ϵ
	WCHAR							szUserNote[LEN_NOTE];				//�û���ע
	WCHAR							szAccounts[LEN_ACCOUNTS];			//�û��˺�
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IMemberParser INTERFACE_VERSION(1,1)
	static const GUID IID_IMemberParser={0xaa35af14,0xef68,0x4a34,0x00bd,0x55,0x59,0x74,0x88,0x69,0x01,0x62};
#else
	#define VER_IMemberParser INTERFACE_VERSION(1,1)
	static const GUID IID_IMemberParser={0xb45e8c41,0x83c1,0x4e45,0x00ad,0x36,0xc3,0xf2,0xe8,0x87,0xcf,0xc2};
#endif

//��Ա�ӿ�
interface IMemberParser : public IUnknownEx
{
	//���ýӿ�
public:
	//������Ŀ
	virtual BYTE __cdecl GetMemberParserItemCount()=NULL;
	//������Ŀ
	virtual BYTE __cdecl GetMasterParserItemCount()=NULL;
	//���ý���
	virtual bool __cdecl SetMemberParser(tagMemberItem MemberItem[], WORD wItemCount)=NULL;
	//���ý���
	virtual bool __cdecl SetMasterParser(tagMasterItem MasterItem[], WORD wItemCount)=NULL;

	//���ܽӿ�
public:
	//��ȡ����
	virtual LPCTSTR __cdecl GetMemberDescribe(BYTE cbMemberOrder)=NULL;
	//��ȡ����
	virtual LPCTSTR __cdecl GetMasterDescribe(BYTE cbMasterOrder)=NULL;
	//��ȡ����
	virtual BYTE __cdecl GetMemberParser(tagMemberItem MemberItem[], WORD wItemCount)=NULL;
	//��ȡ����
	virtual BYTE __cdecl GetMasterParser(tagMasterItem MasterItem[], WORD wItemCount)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IClientUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IClientUserItem={0x7c3908c4,0xa275,0x4abb,0x008b,0x78,0x09,0x4d,0xc9,0xf1,0x29,0x56};
#else
	#define VER_IClientUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IClientUserItem={0x22dfa4e3,0xa6b9,0x4716,0x0086,0x46,0xf8,0xb1,0x3b,0x51,0x2d,0x7b};
#endif

//�û��ӿ�
interface IClientUserItem : public IUnknownEx
{
	//���Խӿ�
public:
	//��Ҫ�ȼ�
	virtual LONG __cdecl GetImportOrder()=NULL;
	//�û���Ϣ
	virtual tagClientUserInfo* __cdecl GetUserInfo()=NULL;
	//�Զ�ͷ��
	virtual tagCustomFaceInfo * GetCustomFaceInfo()=NULL;

	//ͷ����Ϣ
public:
	//ͷ������
	virtual WORD __cdecl GetFaceID()=NULL;
	//�Զ�����
	virtual DWORD __cdecl GetCustomID()=NULL;

	//������Ϣ
public:
	//������ʾ
	virtual bool __cdecl IsRobot()=NULL;
	//�û�����
	virtual BYTE __cdecl GetAge()=NULL;
	//�û��Ա�
	virtual BYTE __cdecl GetGender()=NULL;
	//�û���ʶ
	virtual DWORD __cdecl GetUserID()=NULL;
	//��Ϸ��ʶ
	virtual DWORD __cdecl GetGameID()=NULL;
	//���ű�ʶ
	virtual DWORD __cdecl GetGroupID()=NULL;	
	//�û�����
	virtual LPCWSTR __cdecl GetAccounts()=NULL;
	//����ǩ��
	virtual LPCWSTR __cdecl GetUnderWrite()=NULL;

	//�ȼ���Ϣ
public:
	//��Ա�ȼ�
	virtual BYTE __cdecl GetMemberOrder()=NULL;
	//����ȼ�
	virtual BYTE __cdecl GetMasterOrder()=NULL;

	//������Ϣ
public:
	//������Ϣ
	virtual LPCWSTR __cdecl GetUserArea()=NULL;
	//������Ϣ
	virtual LPCWSTR __cdecl GetUserCity()=NULL;
	//ʡ����Ϣ
	virtual LPCWSTR __cdecl GetUserProvince()=NULL;

	//�û�״̬
public:
	//�û�����
	virtual WORD __cdecl GetTableID()=NULL;
	//�û�����
	virtual WORD __cdecl GetChairID()=NULL;
	//�û�״̬
	virtual BYTE __cdecl GetUserStatus()=NULL;

	//����ȼ�
public:
	//������ֵ
	virtual LONG __cdecl GetUserExperience()=NULL;
	//��ȡ�ȼ�
	virtual WORD __cdecl GetExperienceLevel()=NULL;
	//�ȼ���Ϣ
	virtual WORD __cdecl GetExperienceIndex(BYTE cbOrderIndex[], WORD wMaxCount)=NULL;

//	//��λ��Ϣ
//public:
//	//��λģʽ
//	virtual WORD __cdecl GetSegmentMode()=NULL;
//	//ҵ���λ
//	virtual WORD __cdecl GetGameSegment()=NULL;
//	//���ܶ�λ
//	virtual BYTE __cdecl GetMasterSegment()=NULL;

	//��Ϸ��Ϣ
public:
	//������ֵ
	virtual SCORE __cdecl GetUserScore()=NULL;
	//�����Ϣ
	virtual SCORE __cdecl GetUserGold() = NULL;
	//������Ϣ
	virtual SCORE __cdecl GetInsureGold() = NULL;

	//��Ϸ��Ϣ
public:
	//ʤ������
	virtual LONG __cdecl GetUserWinCount()=NULL;
	//ʧ������
	virtual LONG __cdecl GetUserLostCount()=NULL;
	//�;�����
	virtual LONG __cdecl GetUserDrawCount()=NULL;
	//��������
	virtual LONG __cdecl GetUserFleeCount()=NULL;
	//��Ϸ����
	virtual LONG __cdecl GetUserPlayCount()=NULL;

	//������Ϣ
public:
	//�û�ʤ��
	virtual float __cdecl GetUserWinRate()=NULL;
	//�û�����
	virtual float __cdecl GetUserLostRate()=NULL;
	//�û�����
	virtual float __cdecl GetUserDrawRate()=NULL;
	//�û�����
	virtual float __cdecl GetUserFleeRate()=NULL;

	//�û���ϵ
public:
	//��ȡ��ϵ
	virtual BYTE __cdecl GetUserCompanion()=NULL;
	//���ù�ϵ
	virtual VOID __cdecl SetUserCompanion(BYTE cbCompanion)=NULL;

	//�û���ע
public:
	//��ȡ��ע
	virtual LPCWSTR __cdecl GetUserNoteInfo()=NULL;
	//���ñ�ע
	virtual VOID __cdecl SetUserNoteInfo(LPCWSTR pszUserNote)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGlobalUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IGlobalUserItem={0x6b8832af,0x7d1b,0x48db,0x008f,0x7b,0xa2,0xab,0xb,0xd0,0x3b,0xb7};
#else
#define VER_IGlobalUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IGlobalUserItem={0x7b48f53c,0xe9a4,0x471f,0x009d,0x9,0x5e,0x89,0xcb,0xfa,0x2,0xef};
#endif

//�û��ӿ�
interface IGlobalUserItem : public IUnknownEx
{
	//������Ϣ
public:
	//�û���ʶ
	virtual DWORD __cdecl GetUserID()=NULL;
	//��Ϸ��ʶ
	virtual DWORD __cdecl GetGameID()=NULL;
	//�û�����
	virtual LPCWSTR __cdecl GetAccounts()=NULL;

	//������Ϣ
public:
	//��������
	virtual VOID * __cdecl GetExtraInfo(WORD &wDataSize,WORD wDataType)=NULL;

	//���ú���
public:
	//�����Ϣ
	virtual bool __cdecl AddExtraInfo(VOID * pData, WORD wDataSize, WORD wDataType)=NULL;
	//ɾ����Ϣ
	virtual bool __cdecl RemoveExtraInfo(WORD wDataType)=NULL;
	//�����û�
	virtual bool __cdecl ResetUserItem()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IUserManagerSink INTERFACE_VERSION(1,1)
	static const GUID IID_IUserManagerSink={0x28e4435e,0xdcb8,0x4ad1,0x008a,0x58,0x60,0xe8,0xc4,0xd4,0x94,0xc7};
#else
	#define VER_IUserManagerSink INTERFACE_VERSION(1,1)
	static const GUID IID_IUserManagerSink={0x6b48deaf,0x28f0,0x4cd4,0x00bf,0x08,0x0d,0x99,0x93,0x4e,0x5d,0x37};
#endif

//����֪ͨ
interface IUserManagerSink : public IUnknownEx
{
	//�û�����
	virtual VOID __cdecl OnUserItemAcitve(IClientUserItem * pIClientUserItem)=NULL;
	//�û�ɾ��
	virtual VOID __cdecl OnUserItemDelete(IClientUserItem * pIClientUserItem)=NULL;
	//�û�����
	virtual VOID __cdecl OnUserFaceUpdate(IClientUserItem * pIClientUserItem)=NULL;
	//�û�����
	virtual VOID __cdecl OnUserItemUpdate(IClientUserItem * pIClientUserItem, const tagClientUserScore& LastScore)=NULL;
	//�û�����
	virtual VOID __cdecl OnUserItemUpdate(IClientUserItem * pIClientUserItem, const tagUserStatus & LastStatus)=NULL;
	//�û�����
	//virtual VOID __cdecl OnUserItemUpdate(IClientUserItem * pIClientUserItem, const tagUserSegment & LastSegment)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGlobalUserManagerSink INTERFACE_VERSION(1,1)
static const GUID IID_IGlobalUserManagerSink={0xff21573,0xa4be,0x4801,0x00bd,0x83,0xa6,0xc2,0x4c,0xe4,0xfe,0xb4};
#else
#define VER_IGlobalUserManagerSink INTERFACE_VERSION(1,1)
static const GUID IID_IGlobalUserManagerSink={0x922d9c66,0xd417,0x4ac1,0x0091,0xc3,0x55,0x8d,0x91,0x13,0x1e,0x32};
#endif

//����֪ͨ
interface IGlobalUserManagerSink : public IUnknownEx
{
	//�û�����
	virtual VOID __cdecl OnUserItemAcitve(IGlobalUserItem * pIGlobalUserItem)=NULL;
	//�û�ɾ��
	virtual VOID __cdecl OnUserItemDelete(IGlobalUserItem * pIGlobalUserItem)=NULL;
	//�û�������Ϣ����
	virtual VOID __cdecl OnUserItemUpdate(IGlobalUserItem * pIGlobalUserItem)=NULL;
	//�û�������Ϣ����
	virtual VOID __cdecl OnUserExtraInfoUpdate(IGlobalUserItem *pIGlobalUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IPlazaUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IPlazaUserManager={0xa2992ab9,0x2fdd,0x4f9f,0x00bb,0xac,0xcf,0x8b,0x60,0x70,0x80,0xcd};
#else
	#define VER_IPlazaUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IPlazaUserManager={0xe733fb10,0x4b3c,0x442e,0x008c,0xc2,0x37,0x42,0x8b,0x67,0x35,0x0b};
#endif

//�û�����
interface IPlazaUserManager : public IUnknownEx
{
	//���ýӿ�
public:
	//���ýӿ�
	virtual bool __cdecl SetUserInformation(IUnknownEx * pIUnknownEx)=NULL;
	//���ýӿ�
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx)=NULL;

	//����ӿ�
public:
	//�����û�
	virtual IClientUserItem * __cdecl ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo)=NULL;
	//ɾ���û�
	virtual bool __cdecl DeleteUserItem(IClientUserItem * pIClientUserItem)=NULL;

	//���½ӿ�
public:
	//���»���
	virtual bool __cdecl UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore* const pUserScore)=NULL;
	//����״̬
	virtual bool __cdecl UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus)=NULL;
	//���¶�λ
	//virtual bool __cdecl UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment)=NULL;
	//����ͷ��
	virtual bool __cdecl UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo)=NULL;

	//���ҽӿ�
public:
	//ö���û�
	virtual IClientUserItem * __cdecl EnumUserItem(WORD wEnumIndex)=NULL;
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByUserID(DWORD dwUserID)=NULL;
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByGameID(DWORD dwGameID)=NULL;
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByAccounts(LPCTSTR pszAccounts)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGlobalUserManager INTERFACE_VERSION(1,1)
static const GUID IID_IGlobalUserManager={0x2dc88d28,0x1e50,0x4f2e,0x0096,0x60,0x97,0x94,0xf0,0xb2,0x38,0x56};
#else
#define VER_IGlobalUserManager INTERFACE_VERSION(1,1)
static const GUID IID_IGlobalUserManager={0xdcf4c5fe,0x5f9a,0x4811,0x00a3,0xbd,0x48,0x5f,0x9,0x84,0x3b,0xcb};
#endif

//�û�����
interface IGlobalUserManager : public IUnknownEx
{
	//���ýӿ�
public:
	//���ýӿ�
	virtual bool __cdecl AddUserManagerSink(IUnknownEx * pIUnknownEx)=NULL;
	//ɾ���ӿ�
	virtual bool __cdecl RemoveUserManagerSink(IUnknownEx *pIUnkownEx)=NULL;
	//ɾ���ӿ�
	virtual bool __cdecl RemoveAllUserManagerSink()=NULL;

	//����ӿ�
public:
	//�����û�
	virtual IGlobalUserItem * __cdecl ActiveUserItem(const tagGlobalUserInfo & GlobalUserInfo)=NULL;
	//ɾ���û�
	virtual bool __cdecl DeleteUserItem(IGlobalUserItem * pIGlobalUserItem)=NULL;

	//���½ӿ�
public:
	//���»�����Ϣ
	virtual bool __cdecl UpdateUserItem(IGlobalUserItem * pIGlobalUserItem, tagGlobalUserInfo * const pGlobalUserInfo)=NULL;
	//��Ӹ�����Ϣ
	virtual bool __cdecl AddUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, VOID * pData, WORD wDataSize, WORD wDataType)=NULL;
	//ɾ��������Ϣ
	virtual bool __cdecl RemoveUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, WORD wDataType)=NULL;

	//���ҽӿ�
public:
	//ö���û�
	virtual IGlobalUserItem * __cdecl EnumUserItem(WORD wEnumIndex)=NULL;
	//�����û�
	virtual IGlobalUserItem * __cdecl SearchUserByUserID(DWORD dwUserID)=NULL;
	//�����û�
	virtual IGlobalUserItem * __cdecl SearchUserByGameID(DWORD dwGameID)=NULL;
	//�����û�
	virtual IGlobalUserItem * __cdecl SearchUserByAccounts(LPCTSTR pszAccounts)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IGameUserManager={0xb0e334b9,0xf693,0x4c5f,0x0087,0x0f,0x66,0x77,0x0a,0xb9,0x93,0xb2};
#else
	#define VER_IGameUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IGameUserManager={0x2e5d87ff,0x3150,0x4897,0x008c,0x1f,0xbb,0x3d,0x29,0xfc,0x51,0x2e};
#endif

//�û�����
interface IGameUserManager : public IUnknownEx
{
	//���ýӿ�
public:
	//���ýӿ�
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx)=NULL;
	//��ȡ�ӿ�
	virtual VOID * __cdecl GetUserManagerSink(REFGUID Guid, DWORD dwQueryVer)=NULL;

	//����ӿ�
public:
	//�����û�
	virtual IClientUserItem * __cdecl ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo, BYTE cbCompanion, LPCWSTR pszUserNote)=NULL;
	//ɾ���û�
	virtual bool __cdecl DeleteUserItem(IClientUserItem * pIClientUserItem)=NULL;
	//�����û�
	virtual bool __cdecl DeleteAllUserItem()=NULL;

	//���½ӿ�
public:
	//���»���
	virtual bool __cdecl UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore* const pUserScore)=NULL;
	//����״̬
	virtual bool __cdecl UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus)=NULL;
	//���¶�λ
	//virtual bool __cdecl UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment)=NULL;
	//����ͷ��
	virtual bool __cdecl UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo)=NULL;

	//��ѯ�ӿ�
public:
	//��Ϸ�û�
	virtual IClientUserItem * __cdecl GetTableUserItem(WORD wChariID)=NULL;
	//�Թ��û�
	virtual IClientUserItem * __cdecl EnumLookonUserItem(WORD wEnumIndex)=NULL;

	//���ҽӿ�
public:
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByUserID(DWORD dwUserID)=NULL;
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByGameID(DWORD dwGameID)=NULL;
	//�����û�
	virtual IClientUserItem * __cdecl SearchUserByAccounts(LPCTSTR pszAccounts)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameLevelParser INTERFACE_VERSION(1,1)
	static const GUID IID_IGameLevelParser={0x12823198,0x27b6,0x46c5,0x009c,0x2c,0x9c,0x37,0x08,0x58,0xbf,0xfa};
#else
	#define VER_IGameLevelParser INTERFACE_VERSION(1,1)
	static const GUID IID_IGameLevelParser={0x05d71a6a,0x4378,0x4213,0x00a1,0x51,0x0f,0x96,0x85,0x22,0x71,0x58};
#endif

//�ȼ�����
interface IGameLevelParser : public IUnknownEx
{
	//��Ϣ�ӿ�
public:
	//�����ж�
	virtual bool __cdecl IsLoadLevel()=NULL;
	//�ȼ���Ŀ
	virtual WORD __cdecl GetGameLevelCount()=NULL;

	//���ýӿ�
public:
	//���صȼ�
	virtual bool __cdecl LoadGameLevel(LPCTSTR pszDirectory, WORD wGameGenre)=NULL;
	//�ͷŵȼ�
	virtual bool __cdecl UnLoadGameLevel()=NULL;

	//�����ӿ�
public:
	//�ȼ�����
	virtual LPCTSTR __cdecl GetLevelDescribe(LONGLONG lScore)=NULL;
	//�ȼ�����
	virtual LPCTSTR __cdecl GetLevelDescribe(IClientUserItem * pIClientUserItem)=NULL;

	//���ݽӿ�
public:
	//��ȡ�ȼ�
	virtual WORD __cdecl GetGameLevelItem(tagLevelItem LevelItem[], WORD wMaxCount)=NULL;
	//���õȼ�
	virtual bool __cdecl SetGameLevelItem(tagLevelItem LevelItem[], WORD wItemCount)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IUserInformation INTERFACE_VERSION(1,1)
	static const GUID IID_IUserInformation={0xb3bcac27,0x7e21,0x479a,0x00b7,0x49,0xa3,0xdc,0x79,0xfe,0x8d,0xd1};
#else
	#define VER_IUserInformation INTERFACE_VERSION(1,1)
	static const GUID IID_IUserInformation={0x77084681,0xf172,0x4352,0x00a3,0x27,0x11,0x22,0x17,0x03,0x3c,0x47};
#endif

//�û���Ϣ
interface IUserInformation : public IUnknownEx
{
	//����ӿ�
public:
	//������Ϣ
	virtual bool __cdecl LoadInformation()=NULL;
	//���ýӿ�
	virtual bool __cdecl SetUserCompanionSink(IUnknownEx * pIUnknownEx)=NULL;

	//��ϵ�ӿ�
public:
	//ö�ٹ�ϵ
	virtual tagCompanionInfo * __cdecl EmunCompanionInfo(WORD wIndex)=NULL;
	//Ѱ�ҹ�ϵ
	virtual tagCompanionInfo * __cdecl SearchCompanionInfo(DWORD dwUserID)=NULL;
	//�����ϵ
	virtual tagCompanionInfo * __cdecl InsertCompanionInfo(IClientUserItem * pIClientUserItem, BYTE cbCompanion)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IUserCompanionSink INTERFACE_VERSION(1,1)
	static const GUID IID_IUserCompanionSink={0xd145ece3,0xe8be,0x4178,0x0083,0xa6,0x8f,0x5b,0x12,0x33,0x4c,0x82};
#else
	#define VER_IUserCompanionSink INTERFACE_VERSION(1,1)
	static const GUID IID_IUserCompanionSink={0xd687ffa9,0x21ca,0x47ec,0x008a,0x99,0x7d,0xb9,0xec,0xf0,0x6b,0x96};
#endif

//��ϵ�ص�
interface IUserCompanionSink : public IUnknownEx
{
	//����֪ͨ
	virtual bool __cdecl OnCompanionInsert(tagCompanionInfo * pCompanionInfo)=NULL;
	//ɾ��֪ͨ
	virtual bool __cdecl OnCompanionDelete(tagCompanionInfo * pCompanionInfo)=NULL;
	//����֪ͨ
	virtual bool __cdecl OnCompanionUpdate(tagCompanionInfo * pCompanionInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//���������

DECLARE_MODULE_HELPER(MemberParser,MemberParser,CLIENT_SERVICE_DLL_NAME,"CreateMemberParser")
DECLARE_MODULE_HELPER(UserInformation, UserInformation,CLIENT_SERVICE_DLL_NAME,"CreateUserInformation")
DECLARE_MODULE_HELPER(GameLevelParser, GameLevelParser,CLIENT_SERVICE_DLL_NAME,"CreateGameLevelParser")
DECLARE_MODULE_HELPER(GameUserManager, GameUserManager,CLIENT_SERVICE_DLL_NAME,"CreateGameUserManager")
DECLARE_MODULE_HELPER(PlazaUserManager, PlazaUserManager,CLIENT_SERVICE_DLL_NAME,"CreatePlazaUserManager")
DECLARE_MODULE_HELPER(GlobalUserManager, GlobalUserManager,CLIENT_SERVICE_DLL_NAME,"CreateGlobalUserManager")

//////////////////////////////////////////////////////////////////////////

#endif