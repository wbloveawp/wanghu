#ifndef CLIENT_SERVICE_HEAD_HEAD_FILE
#define CLIENT_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "Platform.h"           

//类库头文件
#include "CommonServiceHead.h"                

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//模块定义
#ifndef _DEBUG
	#define CLIENT_SERVICE_DLL_NAME	TEXT("ClientService.dll")			//组件 DLL 名字
#else
	#define CLIENT_SERVICE_DLL_NAME	TEXT("ClientServiceD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

//长度定义
#define LEN_NOTE					32									//备注长度

//关系信息
struct tagCompanionInfo
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	BYTE							cbCompanion;						//用户关系
	WCHAR							szUserNote[LEN_NOTE];				//用户备注
	WCHAR							szAccounts[LEN_ACCOUNTS];			//用户账号
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IMemberParser INTERFACE_VERSION(1,1)
	static const GUID IID_IMemberParser={0xaa35af14,0xef68,0x4a34,0x00bd,0x55,0x59,0x74,0x88,0x69,0x01,0x62};
#else
	#define VER_IMemberParser INTERFACE_VERSION(1,1)
	static const GUID IID_IMemberParser={0xb45e8c41,0x83c1,0x4e45,0x00ad,0x36,0xc3,0xf2,0xe8,0x87,0xcf,0xc2};
#endif

//会员接口
interface IMemberParser : public IUnknownEx
{
	//配置接口
public:
	//解释数目
	virtual BYTE __cdecl GetMemberParserItemCount()=NULL;
	//解释数目
	virtual BYTE __cdecl GetMasterParserItemCount()=NULL;
	//设置解释
	virtual bool __cdecl SetMemberParser(tagMemberItem MemberItem[], WORD wItemCount)=NULL;
	//设置解释
	virtual bool __cdecl SetMasterParser(tagMasterItem MasterItem[], WORD wItemCount)=NULL;

	//功能接口
public:
	//获取描述
	virtual LPCTSTR __cdecl GetMemberDescribe(BYTE cbMemberOrder)=NULL;
	//获取描述
	virtual LPCTSTR __cdecl GetMasterDescribe(BYTE cbMasterOrder)=NULL;
	//获取解释
	virtual BYTE __cdecl GetMemberParser(tagMemberItem MemberItem[], WORD wItemCount)=NULL;
	//获取解释
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

//用户接口
interface IClientUserItem : public IUnknownEx
{
	//属性接口
public:
	//重要等级
	virtual LONG __cdecl GetImportOrder()=NULL;
	//用户信息
	virtual tagClientUserInfo* __cdecl GetUserInfo()=NULL;
	//自定头像
	virtual tagCustomFaceInfo * GetCustomFaceInfo()=NULL;

	//头像信息
public:
	//头像索引
	virtual WORD __cdecl GetFaceID()=NULL;
	//自定索引
	virtual DWORD __cdecl GetCustomID()=NULL;

	//属性信息
public:
	//机器标示
	virtual bool __cdecl IsRobot()=NULL;
	//用户年龄
	virtual BYTE __cdecl GetAge()=NULL;
	//用户性别
	virtual BYTE __cdecl GetGender()=NULL;
	//用户标识
	virtual DWORD __cdecl GetUserID()=NULL;
	//游戏标识
	virtual DWORD __cdecl GetGameID()=NULL;
	//社团标识
	virtual DWORD __cdecl GetGroupID()=NULL;	
	//用户名字
	virtual LPCWSTR __cdecl GetAccounts()=NULL;
	//个性签名
	virtual LPCWSTR __cdecl GetUnderWrite()=NULL;

	//等级信息
public:
	//会员等级
	virtual BYTE __cdecl GetMemberOrder()=NULL;
	//管理等级
	virtual BYTE __cdecl GetMasterOrder()=NULL;

	//地区信息
public:
	//区域信息
	virtual LPCWSTR __cdecl GetUserArea()=NULL;
	//城市信息
	virtual LPCWSTR __cdecl GetUserCity()=NULL;
	//省份信息
	virtual LPCWSTR __cdecl GetUserProvince()=NULL;

	//用户状态
public:
	//用户桌子
	virtual WORD __cdecl GetTableID()=NULL;
	//用户椅子
	virtual WORD __cdecl GetChairID()=NULL;
	//用户状态
	virtual BYTE __cdecl GetUserStatus()=NULL;

	//经验等级
public:
	//经验数值
	virtual LONG __cdecl GetUserExperience()=NULL;
	//获取等级
	virtual WORD __cdecl GetExperienceLevel()=NULL;
	//等级信息
	virtual WORD __cdecl GetExperienceIndex(BYTE cbOrderIndex[], WORD wMaxCount)=NULL;

//	//段位信息
//public:
//	//段位模式
//	virtual WORD __cdecl GetSegmentMode()=NULL;
//	//业余段位
//	virtual WORD __cdecl GetGameSegment()=NULL;
//	//网管段位
//	virtual BYTE __cdecl GetMasterSegment()=NULL;

	//游戏信息
public:
	//积分数值
	virtual SCORE __cdecl GetUserScore()=NULL;
	//金币信息
	virtual SCORE __cdecl GetUserGold() = NULL;
	//银行信息
	virtual SCORE __cdecl GetInsureGold() = NULL;

	//游戏信息
public:
	//胜利盘数
	virtual LONG __cdecl GetUserWinCount()=NULL;
	//失败盘数
	virtual LONG __cdecl GetUserLostCount()=NULL;
	//和局盘数
	virtual LONG __cdecl GetUserDrawCount()=NULL;
	//逃跑盘数
	virtual LONG __cdecl GetUserFleeCount()=NULL;
	//游戏局数
	virtual LONG __cdecl GetUserPlayCount()=NULL;

	//比率信息
public:
	//用户胜率
	virtual float __cdecl GetUserWinRate()=NULL;
	//用户输率
	virtual float __cdecl GetUserLostRate()=NULL;
	//用户和率
	virtual float __cdecl GetUserDrawRate()=NULL;
	//用户逃率
	virtual float __cdecl GetUserFleeRate()=NULL;

	//用户关系
public:
	//获取关系
	virtual BYTE __cdecl GetUserCompanion()=NULL;
	//设置关系
	virtual VOID __cdecl SetUserCompanion(BYTE cbCompanion)=NULL;

	//用户备注
public:
	//获取备注
	virtual LPCWSTR __cdecl GetUserNoteInfo()=NULL;
	//设置备注
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

//用户接口
interface IGlobalUserItem : public IUnknownEx
{
	//属性信息
public:
	//用户标识
	virtual DWORD __cdecl GetUserID()=NULL;
	//游戏标识
	virtual DWORD __cdecl GetGameID()=NULL;
	//用户名字
	virtual LPCWSTR __cdecl GetAccounts()=NULL;

	//其他信息
public:
	//额外属性
	virtual VOID * __cdecl GetExtraInfo(WORD &wDataSize,WORD wDataType)=NULL;

	//设置函数
public:
	//添加信息
	virtual bool __cdecl AddExtraInfo(VOID * pData, WORD wDataSize, WORD wDataType)=NULL;
	//删除信息
	virtual bool __cdecl RemoveExtraInfo(WORD wDataType)=NULL;
	//重置用户
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

//管理通知
interface IUserManagerSink : public IUnknownEx
{
	//用户激活
	virtual VOID __cdecl OnUserItemAcitve(IClientUserItem * pIClientUserItem)=NULL;
	//用户删除
	virtual VOID __cdecl OnUserItemDelete(IClientUserItem * pIClientUserItem)=NULL;
	//用户更新
	virtual VOID __cdecl OnUserFaceUpdate(IClientUserItem * pIClientUserItem)=NULL;
	//用户更新
	virtual VOID __cdecl OnUserItemUpdate(IClientUserItem * pIClientUserItem, const tagClientUserScore& LastScore)=NULL;
	//用户更新
	virtual VOID __cdecl OnUserItemUpdate(IClientUserItem * pIClientUserItem, const tagUserStatus & LastStatus)=NULL;
	//用户更新
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

//管理通知
interface IGlobalUserManagerSink : public IUnknownEx
{
	//用户激活
	virtual VOID __cdecl OnUserItemAcitve(IGlobalUserItem * pIGlobalUserItem)=NULL;
	//用户删除
	virtual VOID __cdecl OnUserItemDelete(IGlobalUserItem * pIGlobalUserItem)=NULL;
	//用户基本信息更新
	virtual VOID __cdecl OnUserItemUpdate(IGlobalUserItem * pIGlobalUserItem)=NULL;
	//用户附加信息更新
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

//用户管理
interface IPlazaUserManager : public IUnknownEx
{
	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetUserInformation(IUnknownEx * pIUnknownEx)=NULL;
	//设置接口
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx)=NULL;

	//管理接口
public:
	//增加用户
	virtual IClientUserItem * __cdecl ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo)=NULL;
	//删除用户
	virtual bool __cdecl DeleteUserItem(IClientUserItem * pIClientUserItem)=NULL;

	//更新接口
public:
	//更新积分
	virtual bool __cdecl UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore* const pUserScore)=NULL;
	//更新状态
	virtual bool __cdecl UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus)=NULL;
	//更新段位
	//virtual bool __cdecl UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment)=NULL;
	//更新头像
	virtual bool __cdecl UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo)=NULL;

	//查找接口
public:
	//枚举用户
	virtual IClientUserItem * __cdecl EnumUserItem(WORD wEnumIndex)=NULL;
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByUserID(DWORD dwUserID)=NULL;
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByGameID(DWORD dwGameID)=NULL;
	//查找用户
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

//用户管理
interface IGlobalUserManager : public IUnknownEx
{
	//配置接口
public:
	//设置接口
	virtual bool __cdecl AddUserManagerSink(IUnknownEx * pIUnknownEx)=NULL;
	//删除接口
	virtual bool __cdecl RemoveUserManagerSink(IUnknownEx *pIUnkownEx)=NULL;
	//删除接口
	virtual bool __cdecl RemoveAllUserManagerSink()=NULL;

	//管理接口
public:
	//增加用户
	virtual IGlobalUserItem * __cdecl ActiveUserItem(const tagGlobalUserInfo & GlobalUserInfo)=NULL;
	//删除用户
	virtual bool __cdecl DeleteUserItem(IGlobalUserItem * pIGlobalUserItem)=NULL;

	//更新接口
public:
	//更新基本信息
	virtual bool __cdecl UpdateUserItem(IGlobalUserItem * pIGlobalUserItem, tagGlobalUserInfo * const pGlobalUserInfo)=NULL;
	//添加附加信息
	virtual bool __cdecl AddUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, VOID * pData, WORD wDataSize, WORD wDataType)=NULL;
	//删除附加信息
	virtual bool __cdecl RemoveUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, WORD wDataType)=NULL;

	//查找接口
public:
	//枚举用户
	virtual IGlobalUserItem * __cdecl EnumUserItem(WORD wEnumIndex)=NULL;
	//查找用户
	virtual IGlobalUserItem * __cdecl SearchUserByUserID(DWORD dwUserID)=NULL;
	//查找用户
	virtual IGlobalUserItem * __cdecl SearchUserByGameID(DWORD dwGameID)=NULL;
	//查找用户
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

//用户管理
interface IGameUserManager : public IUnknownEx
{
	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx)=NULL;
	//获取接口
	virtual VOID * __cdecl GetUserManagerSink(REFGUID Guid, DWORD dwQueryVer)=NULL;

	//管理接口
public:
	//增加用户
	virtual IClientUserItem * __cdecl ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo, BYTE cbCompanion, LPCWSTR pszUserNote)=NULL;
	//删除用户
	virtual bool __cdecl DeleteUserItem(IClientUserItem * pIClientUserItem)=NULL;
	//重置用户
	virtual bool __cdecl DeleteAllUserItem()=NULL;

	//更新接口
public:
	//更新积分
	virtual bool __cdecl UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore* const pUserScore)=NULL;
	//更新状态
	virtual bool __cdecl UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus)=NULL;
	//更新段位
	//virtual bool __cdecl UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment)=NULL;
	//更新头像
	virtual bool __cdecl UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo)=NULL;

	//查询接口
public:
	//游戏用户
	virtual IClientUserItem * __cdecl GetTableUserItem(WORD wChariID)=NULL;
	//旁观用户
	virtual IClientUserItem * __cdecl EnumLookonUserItem(WORD wEnumIndex)=NULL;

	//查找接口
public:
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByUserID(DWORD dwUserID)=NULL;
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByGameID(DWORD dwGameID)=NULL;
	//查找用户
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

//等级解释
interface IGameLevelParser : public IUnknownEx
{
	//信息接口
public:
	//加载判断
	virtual bool __cdecl IsLoadLevel()=NULL;
	//等级数目
	virtual WORD __cdecl GetGameLevelCount()=NULL;

	//配置接口
public:
	//加载等级
	virtual bool __cdecl LoadGameLevel(LPCTSTR pszDirectory, WORD wGameGenre)=NULL;
	//释放等级
	virtual bool __cdecl UnLoadGameLevel()=NULL;

	//描述接口
public:
	//等级描述
	virtual LPCTSTR __cdecl GetLevelDescribe(LONGLONG lScore)=NULL;
	//等级描述
	virtual LPCTSTR __cdecl GetLevelDescribe(IClientUserItem * pIClientUserItem)=NULL;

	//数据接口
public:
	//获取等级
	virtual WORD __cdecl GetGameLevelItem(tagLevelItem LevelItem[], WORD wMaxCount)=NULL;
	//设置等级
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

//用户信息
interface IUserInformation : public IUnknownEx
{
	//管理接口
public:
	//加载信息
	virtual bool __cdecl LoadInformation()=NULL;
	//设置接口
	virtual bool __cdecl SetUserCompanionSink(IUnknownEx * pIUnknownEx)=NULL;

	//关系接口
public:
	//枚举关系
	virtual tagCompanionInfo * __cdecl EmunCompanionInfo(WORD wIndex)=NULL;
	//寻找关系
	virtual tagCompanionInfo * __cdecl SearchCompanionInfo(DWORD dwUserID)=NULL;
	//插入关系
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

//关系回调
interface IUserCompanionSink : public IUnknownEx
{
	//插入通知
	virtual bool __cdecl OnCompanionInsert(tagCompanionInfo * pCompanionInfo)=NULL;
	//删除通知
	virtual bool __cdecl OnCompanionDelete(tagCompanionInfo * pCompanionInfo)=NULL;
	//更新通知
	virtual bool __cdecl OnCompanionUpdate(tagCompanionInfo * pCompanionInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//组件辅助类

DECLARE_MODULE_HELPER(MemberParser,MemberParser,CLIENT_SERVICE_DLL_NAME,"CreateMemberParser")
DECLARE_MODULE_HELPER(UserInformation, UserInformation,CLIENT_SERVICE_DLL_NAME,"CreateUserInformation")
DECLARE_MODULE_HELPER(GameLevelParser, GameLevelParser,CLIENT_SERVICE_DLL_NAME,"CreateGameLevelParser")
DECLARE_MODULE_HELPER(GameUserManager, GameUserManager,CLIENT_SERVICE_DLL_NAME,"CreateGameUserManager")
DECLARE_MODULE_HELPER(PlazaUserManager, PlazaUserManager,CLIENT_SERVICE_DLL_NAME,"CreatePlazaUserManager")
DECLARE_MODULE_HELPER(GlobalUserManager, GlobalUserManager,CLIENT_SERVICE_DLL_NAME,"CreateGlobalUserManager")

//////////////////////////////////////////////////////////////////////////

#endif