#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "redis++.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

using namespace redis;
using namespace lhchat;


//////////////////////////////////////////////////////////////////////////

//数据库钩子
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//组件变量
protected:
	CDataBaseAide					m_PlatformDBAide;					//平台数据库
	CDataBaseAide					m_AccountsDBAide;					//用户数据库
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库
	CDataBaseHelper					m_AccountsDBModule;					//用户数据库

	//辅助变量
protected:
	DBO_GP_LogonFailure				m_GPLogonFailure;					//登录失败
	DBO_GP_LogonSuccess				m_GPLogonSuccess;					//登录成功
	DBO_MB_LogonFailure				m_MBLogonFailure;					//登录失败
	DBO_MB_LogonSuccess				m_MBLogonSuccess;					//登录成功
	DBO_GP_UserIndividual			m_UserIndividual;					//用户资料

	//缓冲对象
protected:
	Redis *							m_pUserClient;						//缓冲对象
	Redis *							m_pFriendClient;					//缓冲对象
	Redis *							m_pTokenClient;						//缓冲对象
	Redis *							m_pMasterClient;					//缓冲对象

	//脚本变量
protected:
	string							m_sScriptDeleteFriend;				//删除好友
	string							m_sScriptAppendFriend;				//添加好友

	//组件变量
protected:
	tagServiceOption *				m_pServiceOption;					//服务配置
	CInitParameter *				m_pInitParameter;					//配置参数
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineStop(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//系统功能
protected:
	//加载配置
	bool OnRequestLoadConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//加载榜单
	bool OnRequestLoadRankingList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//重置数据
	bool OnRequestResetUserData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//在线信息
	bool OnRequestOnLineCountInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	

	//登录处理
protected:
	//帐号登录
	bool OnRequestPCLogonAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//帐号注册
	bool OnRequestPCRegisterAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//账号服务
protected:
	//锁定机器
	bool OnRequestPCBindMachine(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//解锁机器
	bool OnRequestPCUnbindMachine(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改帐号
	bool OnRequestPCModifyAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改密码
	bool OnRequestPCModifyLogonPass(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改密码
	bool OnRequestPCModifyInsurePass(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改资料
	bool OnRequestPCModifyIndividual(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//配置服务
protected:
	//查询配置
	bool OnRequestPCQueryGameConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//保存配置
	bool OnRequestPCUpdateBasicConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//保存配置
	bool OnRequestPCUpdateWeightConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//机器服务
protected:
	//获取参数
	bool OnRequestPCQueryParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//添加参数
	bool OnRequestPCAppendParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改参数
	bool OnRequestPCModifyParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//删除参数
	bool OnRequestPCDeleteParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//库存服务
protected:
	//查询库存
	bool OnRequestPCQueryStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//添加库存
	bool OnRequestPCAppendStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改库存
	bool OnRequestPCModifyStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//删除库存
	bool OnRequestPCDeleteStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);


	//查询服务
protected:
	//查询资料
	bool OnRequestPCQueryIndividual(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//手机登录
protected:
	//邮箱登陆
	bool OnRequestMBLogonEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//验证码登录
	bool OnRequestMBLogonEMailCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//验证码登录
	bool OnRequestMBLogonMobileCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//手机登陆
	bool OnRequestMBLogonMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//游客登录
	bool OnRequestMBLogonVisitor(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//帐号登录
	bool OnRequestMBLogonAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	
	//秘钥登录
	bool OnRequestMBLogonSecretKey(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//其他登录
	bool OnRequestMBLogonThirdParty(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	
	//邮箱注册 
	bool OnRequestMBRegisterEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//帐号注册 
	bool OnRequestMBRegisterAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	
	//手机注册 
	bool OnRequestMBRegisterMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	

	//手机登出
protected:
	//帐号登出
	bool OnRequestMBLogoutAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//账号服务
protected:
	//点赞玩家
	bool OnRequestMBLikePlayer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改密码
	bool OnRequestMBModifyLogonPass(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改资料
	bool OnRequestMBModifyIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改头像
	bool OnRequestMBModifySystemFace(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//修改头像
	bool OnRequestMBModifyCustomFace(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//实名认证
	bool OnRequestMBRequestRealAuthent(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//商品服务
protected:
	//兑换商品
	bool OnRequestMBExchangeMallGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//兑换礼包码
	bool OnRequestMBExchangeGiftbagCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//验证服务
protected:
	//绑定邮箱
	bool OnRequestMBBindEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//解绑邮箱
	bool OnRequestMBUnbindEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//获取验证码
	bool OnRequestMBAcquireAuthCode(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//绑定手机号
	bool OnRequestMBBindMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//解绑手机号
	bool OnRequestMBUnbindMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	
	//重置密码
	bool OnRequestMBResetLogonPasswd(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

    //请求处理
protected:
	//请求签到
	bool OnRequestMBRequestCheckIn(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
    //绑定邀请码
    bool OnRequestMBBindInviteCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//提交反馈
	bool OnRequestMBSubmitInformInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//绑定账号
	bool OnRequestMBBindExternalAccount(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//转账服务
protected:
	//货币转账
	bool OnRequestMBWealthTranser(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//佣金服务
protected:
	//取出佣金
	bool OnRequestMBDrawCommission(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//赠送礼物
protected:
    //赠送礼物
    bool OnRequestMBSendGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
    //加载礼物
    bool OnRequestMBLoadGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
    //礼物状态更新
    bool OnRequestMBGiftStatusUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//查询服务
protected:
	//查询财富
	bool OnRequestMBQueryWealth(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//查询排位
	bool OnRequestMBQueryRankingInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
    //查询魅力
    bool OnRequestMBQueryLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//查询低保
	bool OnRequestMBQuerySubsidy(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//转盘抽奖
protected:
	//查询抽奖
	bool OnRequestMBQueryTurntableLottery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//请求抽奖
	bool OnRequestMBRequestTurntableLottery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//银行功能
protected:
	//银行查询
	bool OnRequestMBRequestInsureQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//银行开通
	bool OnRequestMBRequestInsureEnable(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//银行存储
	bool OnRequestMBRequestInsureSave(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//银行提取
	bool OnRequestMBRequestInsureTake(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//银行转账
	bool OnRequestMBRequestInsureTransfer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//银行密码重置
	bool OnRequestMBInsureResetPassword(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//登录辅助
protected:
	//加载背包
	bool LoadUserPackGoods();
	//排位信息
	bool LoadUserRankingInfo();

	//加载函数
private:
	//游戏列表
	bool LoadGameList(DWORD dwContextID, DWORD dwTokenID);
	//敏感词汇
	bool LoadFilterWords(DWORD dwContextID, DWORD dwTokenID);
	//游戏版本
	bool LoadPlazaVersion(DWORD dwContextID, DWORD dwTokenID);
	//转账配置
	bool LoadTransferParemter(DWORD dwContextID, DWORD dwTokenID);
	//全局配置
	bool LoadGlobalOption(DWORD dwContextID, DWORD dwTokenID, DWORD dwOptionMask);
	//昵称配置
	bool LoadNickNameOption(DWORD dwContextID, DWORD dwTokenID);

	//辅助函数
protected:
	//清理信息
	bool CleanOnlineInfo(WORD wLogonID);
	//缓存信息
	bool CacheUserInfo(DWORD dwContextID, DWORD dwTokenID);	
	//更新缓存
	bool UpdateUserCache(DWORD dwContextID, DWORD dwTokenID, UserPublicInfo& PublicInfo);
	//生成Token
	bool BuildLogonToken(DWORD dwUserID, LPCTSTR pszAccounts, LPCTSTR pszSecretKey, TCHAR szUserToken[LEN_LOGON_TOKEN]);

	//辅助函数
protected:	
	//更新财富
	bool UpdateUserWealth(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID);
	//更新物品
	bool UpdateUserGoods(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID, tagGoodsItem GoodsItemList[], WORD wGoodsCount);
    //更新魅力
    bool UpdateUserLoveLiness(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID);

	//库存辅助
protected:
	//解析彩池配置	
	WORD ParseJackpotOption(LPCSTR szJackpotOption, tagJackpotItem JackpotItem[], WORD wBufferCount);
	//序列化彩池
	bool SerializeJackpotOption(tagJackpotItem JackpotItem[], WORD wItemCount, TCHAR szJackpotOption[], WORD wBufferSize);
	//序列化权重
	bool SerializeWeightConfig(tagTimesWeightItem WeightItem[], WORD wItemCount, TCHAR szWeightConfig[], WORD wBufferSize);

	//辅助函数
protected:		
	//输出异常
	void TraceDatabaseException(char *pFile, char *pFunction, int nLine, IDataBaseException * pIException);
	//库存结果
	VOID OnStockDisposeResult(DWORD dwContextID, DWORD dwTokenID, DWORD dwErrorCode, WORD wSubCommdID);
	//机器结果
	VOID OnRobotDisposeResult(DWORD dwContextID, DWORD dwTokenID, DWORD dwErrorCode,WORD wSubCommdID,WORD wServerID);	
	//登录结果
	VOID OnLogonDisposeResult(DWORD dwContextID, DWORD dwTokenID, LONG lResultCode,LPCTSTR pszErrorString, bool bMobileClient=false,bool isThirdLogon=false);
};

//////////////////////////////////////////////////////////////////////////

#endif