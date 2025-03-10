USE LHPlatformDB

DELETE FROM [dbo].[SystemStatusInfo]

INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'AgentOption','AgentLevel_0','{"NeedCommissionAmount":0,"LowerBetAwardRate":10,"LLowerBetAwardRate":1,"LowerPayAwardAmount":20000,"LLowerPayAwardAmount":1000}','初级代理配置')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'AgentOption','AgentLevel_1','{"NeedCommissionAmount":1000,"LowerBetAwardRate":10,"LLowerBetAwardRate":1,"LowerPayAwardAmount":20000,"LLowerPayAwardAmount":1000}','高级代理配置')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'CommissionOption','DrawEnabled','1','提取佣金开关')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'CommissionOption','MinDrawAmount','1000','最小提取金额（放大1000倍）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'CommissionOption','ServiceFeeRate','10','提取佣金手续费(千分比)')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','BenchmarkValue','10000','用于计算胜率时统一分母值，避免胜率波动过大频繁进出黑白名单')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','BLControlRate','70','进入黑名单后要保持的游戏胜率')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','ControlEnabled','1','自动控制开关（0标识关闭  其他开启）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','InBlackListRate','30','进入黑名单要达到的游戏胜率')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','InWhiteListRate','30','进入白名单要达到的游戏胜率')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','OutBlackListRate','50','退出黑名单要达到的游戏胜率')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','OutWhiteListRate','50','退出白名单要达到的游戏胜率')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','StartPlayCount','10','起始游戏局数；低于该值不受控制')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','SupportControlMode','3','系统支持的控制模式（1、自动控制  2、手动控制）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'ControlOption','WLControlRate','30','进入白名单后要保持的游戏胜率')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'DowloadOption','AndroidDownloadUrl','http://update.zz.foxuc.net/Supreme/Test/Supreme.apk','官方包安卓下载地址')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'MarqueeOption','NotifyOption','{"DurationTime":300,"LogonNotice":{"ListType":[1,2,3],"ReachRankID":3},"PrizePoolNotice":{"ReachAwardAmount":100},"GameWinNotice":{"ReachWinScore":1000}}','跑马灯通知配置')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'MarqueeOption','RollOption','[{"NoticeKind":1,"Priority":1,"Exclusive":false,"RollTimes":3,"ViewPlace":3},{"NoticeKind":2,"Priority":1,"Exclusive":false,"RollTimes":3,"ViewPlace":3},{"NoticeKind":3,"Priority":1,"Exclusive":false,"RollTimes":3,"ViewPlace":3}]','跑马灯滚动配置')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'RegisterOption','PresentLimit','192','赠送限制（0x01、游客账号注册不赠送  0x02、邮箱账号注册不赠送 0x04、普通账号注册不赠送 0x08、第三方账号注册不赠送 0x10、手机账号注册不赠送 、0x40、限制IP  0x80、限制机器码  ）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SubsidyOption','SubsidyCondition','1888','低保领取条件（身上金币低于该值可领取）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SubsidyOption','SubsidyGold','1888','低保领取金币数量')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SubsidyOption','SubsidyTimesPerAccounts','3','每个游戏账号每天赠送低保次数')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SubsidyOption','SubsidyTimesPerMachine','3','每个机器码每天赠送低保次数')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SubsidyOption','SubsidyTimesPerIP','3','每个IP每天赠送低保次数')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SubsidyOption','EnjoinSubsidy','0','低保开关 0：开启 1：关闭')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SystemOption','BindInviteCodeReward','10000','绑定邀请码获得的金币奖励')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SystemOption','EnjoinLogon','0','禁止登录开关（0表示允许，其他禁止）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SystemOption','EnjoinRegister','0','禁止登录开关（0表示允许，其他禁止）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TransferOption','MaxTransferAmount','10000000','最大转账金额（放大1000倍）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TransferOption','MinTransferAmount','10000','最小转账金额（放大1000倍）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TransferOption','ReservedAmount','100000','转账后身上需保留的金额（放大1000倍）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TransferOption','ServiceFeeRate','30','转账手续费率(千分比)')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TransferOption','TransferEnabled','1','转账开关（0标识关闭，其他开启）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TurntableOption','ConfineClientIP','0','转盘每日免费抽奖次数赠送是否限制连接地址（0不限制，其他限制）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TurntableOption','ConfineMachine','0','转盘每日免费抽奖次数赠送是否限制机器码（0不限制，其他限制）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TurntableOption','DailyFreeLotteryTimes','3','转盘每日免费抽奖次数')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TurntableOption','DailyMayLotteryTimes',3,'转盘每日最大可抽奖次数') 
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TurntableOption','SliceGameCount','50','每增加多少游戏局数可获得一次抽奖次数')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TurntableOption','SliceGameExpend','10000','每增加多少游戏消耗可获得一次抽奖次数')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TurntableOption','SliceGameTime','300','每增加多少游戏时长可获得一次抽奖次数')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'TurntableOption','TurantableEnabled','1','转盘开关（0表示关闭，其他开启）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'WarningOption','WarningReceiveEmail','494030492@qq.com,184480463@qq.com','预警收件邮箱，多个用英文的","号分割')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'WithdrawOption','EnjoinWithdraw','0','提现状态 0：开启 1：关闭')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'WithdrawOption','WithdrawDailyNum','1','提现每日次数')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'WithdrawOption','WithdrawFeeRate','15','提现手续费，千分比')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'WithdrawOption','WithdrawMaxAmount','10000000','最大提现金额（放大1000倍）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'WithdrawOption','WithdrawMinAmount','100000','最小提现金额（放大1000倍）')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'SystemOption','ModifyNickNameOption','{"MoneyID":1,"MoneyAmount":400,"FreeTimesPerAccounts":1,"MaxTimesPerDay":1}','昵称修改配置')
INSERT SystemStatusInfo (StatusKind,StatusName,StatusValue,StatusString)  VALUES ( 'InsureOption','Insure','{"Enable":1,"SaveAndTake":{"Rate":50,"ReserveAmount":0,"DealMin":100,"DealMax":5000,"DailyMax":10000},"Transfer":{"Rate":50,"ReserveAmount":0,"DealMin":100,"DealMax":5000,"DailyMax":10000}}','银行配置')


