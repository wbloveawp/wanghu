#ifndef MESSAGE_HEAD_FILE
#define MESSAGE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//数据库模块

#define MIM_DATABASE					1000									//消息索引

//////////////////////////////////////////////////////////////////////////

/*
* "您好，系统升级中。暂时停止注册。请稍后再尝试注册。谢谢"
*/
#define MIM_DATABASE_0001				(MIM_DATABASE+1)						//消息索引

/*
* "您好，系统升级中。暂时停止登录。请稍后再尝试注册。谢谢"
*/
#define MIM_DATABASE_0002				(MIM_DATABASE+2)						//消息索引

/*
* "抱歉地通知您，您所输入的帐号名含有限制字符串，请更换帐号名后再次申请帐号！"
*/
#define MIM_DATABASE_0003				(MIM_DATABASE+3)						//消息索引

/*
* "抱歉地通知您，您所输入的昵称含有限制字符串，请更换昵后再次申请昵称！"
*/
#define MIM_DATABASE_0004				(MIM_DATABASE+4)						//消息索引

/*
* "抱歉地通知您，系统禁止了您所在的 IP 地址的注册功能，请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0005				(MIM_DATABASE+5)						//消息索引

/*
* "抱歉地通知您，系统禁止了您的机器的注册功能，请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0006				(MIM_DATABASE+6)						//消息索引

/*
* "此帐号名已被注册，请换另一帐号名字尝试再次注册！"
*/
#define MIM_DATABASE_0007				(MIM_DATABASE+7)						//消息索引

/*
* "此昵称已被注册，请换另一昵称尝试再次注册！"
*/
#define MIM_DATABASE_0008				(MIM_DATABASE+8)						//消息索引

/*
* "帐号已存在，请换另一帐号名字尝试再次注册！"
*/
#define MIM_DATABASE_0009				(MIM_DATABASE+9)						//消息索引

/*
* "用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！"
*/
#define MIM_DATABASE_0010				(MIM_DATABASE+10)						//消息索引

/*
* "抱歉,您输入的手机号码不正确，请重新输入！"
*/
#define MIM_DATABASE_0011				(MIM_DATABASE+11)						//消息索引

/*
* "验证码错误，注册失败!"
*/
#define MIM_DATABASE_0012				(MIM_DATABASE+12)						//消息索引

/*
* "您的机器码发生异常，注册失败！"
*/
#define MIM_DATABASE_0013				(MIM_DATABASE+13)						//消息索引

/*
* "验证码已过期，继续注册请重新获取验证码！"
*/
#define MIM_DATABASE_0014				(MIM_DATABASE+14)						//消息索引

/*
* "此手机号码已被注册，请换另一手机号码尝试再次注册！"
*/
#define MIM_DATABASE_0015				(MIM_DATABASE+15)						//消息索引

/*
* "此手机号码已被其他游戏账号绑定，请换另一手机号码尝试再次注册！"
*/
#define MIM_DATABASE_0016				(MIM_DATABASE+16)						//消息索引

/*
* "抱歉，系统未查询到您绑定的推荐码，请查证后再次尝试！"
*/
#define MIM_DATABASE_0017				(MIM_DATABASE+17)						//消息索引

/*
* "手机号已存在，请换另一手机号码尝试再次注册！"
*/
#define MIM_DATABASE_0018				(MIM_DATABASE+18)						//消息索引

/*
* "抱歉地通知您，系统禁止了您所在的 IP 地址的登录功能，请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0019				(MIM_DATABASE+19)						//消息索引

/*
* "抱歉地通知您，系统禁止了您的机器的登录功能，请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0020				(MIM_DATABASE+20)						//消息索引

/*
* "您的帐号不存在或者密码输入有误，请查证后再次尝试！"
*/
#define MIM_DATABASE_0021				(MIM_DATABASE+21)						//消息索引

/*
* "您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0022				(MIM_DATABASE+22)						//消息索引

/*
* "您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！"
*/
#define MIM_DATABASE_0023				(MIM_DATABASE+23)						//消息索引

/*
* "您的帐号使用锁定机器登陆功能，您现所使用的机器不是所指定的机器！"
*/
#define MIM_DATABASE_0024				(MIM_DATABASE+24)						//消息索引

/*
* "您的帐号成功使用了锁定机器登陆功能！"
*/
#define MIM_DATABASE_0025				(MIM_DATABASE+25)						//消息索引

/*
* "很抱歉，目前的游客登录配额已经发放完毕，请稍后再次尝试！"
*/
#define MIM_DATABASE_0026				(MIM_DATABASE+26)						//消息索引

/*
* "验证码错误！"
*/
#define MIM_DATABASE_0027				(MIM_DATABASE+27)						//消息索引					

/*
* "您的机器码异常！"
*/
#define MIM_DATABASE_0028				(MIM_DATABASE+28)						//消息索引					

/*
* "验证码已过期，请重新获取验证码！"
*/
#define MIM_DATABASE_0029				(MIM_DATABASE+29)						//消息索引

/*
* "该手机号没有绑定的游戏账号，请认真核对输入的手机号码再次尝试！"
*/
#define MIM_DATABASE_0030				(MIM_DATABASE+30)						//消息索引

/*
* "您的帐号不存在或者密码输入有误，请查证后再次尝试！"
*/
#define MIM_DATABASE_0031				(MIM_DATABASE+31)						//消息索引

/*
* "抱歉地通知您，您所输入的昵称含有限制字符串，请更换昵称后尝试再次修改！"
*/
#define MIM_DATABASE_0032				(MIM_DATABASE+32)						//消息索引

/*
* "此昵称已被其他玩家注册了，请换另一昵称尝试再次修改！"
*/
#define MIM_DATABASE_0033				(MIM_DATABASE+33)						//消息索引

/*
* "服务器已经接受了您的新资料！"
*/
#define MIM_DATABASE_0034				(MIM_DATABASE+34)						//消息索引

/*
* "抱歉，转账功能已关闭。"
*/
#define MIM_DATABASE_0035				(MIM_DATABASE+35)						//消息索引						

/*
* "抱歉，目标帐号不存在。"
*/
#define MIM_DATABASE_0036				(MIM_DATABASE+36)						//消息索引						

/*
* "抱歉，同一帐号不允许进行赠送金币。"
*/
#define MIM_DATABASE_0037				(MIM_DATABASE+37)						//消息索引						

/*
* "抱歉！目标帐号暂时处于冻结状态，无法进行金币转账。"
*/
#define MIM_DATABASE_0038				(MIM_DATABASE+38)						//消息索引						

/*
* "抱歉！目标帐号使用了安全关闭功能，无法进行金币转账。"
*/
#define MIM_DATABASE_0039				(MIM_DATABASE+39)						//消息索引						

/*
* "抱歉！每笔转账金额不能小于%s。"
*/
#define MIM_DATABASE_0040				(MIM_DATABASE+40)						//消息索引

/*
* "抱歉！每笔转账金额不能大于%s。"
*/
#define MIM_DATABASE_0041				(MIM_DATABASE+41)						//消息索引

/*
* "抱歉！您所携带的金币不足以扣除服务费用。"
*/
#define MIM_DATABASE_0042				(MIM_DATABASE+42)						//消息索引

/*
* "抱歉！您身上的金币余额不足，转账失败。"
*/
#define MIM_DATABASE_0043				(MIM_DATABASE+43)						//消息索引							

/*
* "该邮箱没有绑定的游戏账号，请仔细核对邮箱后再次尝试！"
*/
#define MIM_DATABASE_0044				(MIM_DATABASE+44)						//消息索引							

/*
* "您的账号已绑定邮箱：%s，如需更换请先解除绑定！"
*/	
#define MIM_DATABASE_0045				(MIM_DATABASE+45)						//消息索引							

/*
* "解除绑定需使用原绑定邮箱，您输入的邮箱不匹配，请仔细核对后再次尝试！"
*/
#define MIM_DATABASE_0046				(MIM_DATABASE+46)						//消息索引							

/*
* "转账需使用游戏账号绑定的邮箱，您的邮箱不匹配，请仔细核对后再次尝试！"
*/
#define MIM_DATABASE_0047				(MIM_DATABASE+47)						//消息索引							

/*
* "%s已经跟游戏账号：%s绑定，是否重新将邮箱绑定到当前帐号上。"
*/
#define MIM_DATABASE_0048				(MIM_DATABASE+48)						//消息索引							

/*
* "抱歉，您已经绑定过邀请码，不能再次绑定！"
*/
#define MIM_DATABASE_0049				(MIM_DATABASE+49)						//消息索引

/*
* "抱歉，系统未查询到您绑定的邀请码，请查证后再次尝试！"
*/
#define MIM_DATABASE_0050				(MIM_DATABASE+50)						//消息索引

/*
* "操作成功！"
*/
#define MIM_DATABASE_0051				(MIM_DATABASE+51)						//消息索引

/*
* "系统检测到您的账号正在游戏房间中，请先退出房间再进行转账！"
*/
#define MIM_DATABASE_0052				(MIM_DATABASE+52)						//消息索引							

/*
* "转账需使用游戏账号绑定的手机号，您输入的手机号码不匹配，请仔细核对后再次尝试！"
*/
#define MIM_DATABASE_0053				(MIM_DATABASE+53)						//消息索引							

/*
* "抱歉,由于您权限不足,登录失败！"
*/
#define MIM_DATABASE_0054				(MIM_DATABASE+54)						//消息索引

/*
* "您的账号已绑定尾号为：%s 的手机号，如需更换手机号,请先解除绑定！"
*/
#define MIM_DATABASE_0055				(MIM_DATABASE+55)						//消息索引

/*
* "解除绑定需使用原手机号码，您输入的手机号码不匹配，请仔细核对后再次尝试！"
*/
#define MIM_DATABASE_0056				(MIM_DATABASE+56)						//消息索引

/*
* "%s 已经跟游戏账号：%s 绑定，是否重新将手机绑定到当前帐号上。"
*/
#define MIM_DATABASE_0057				(MIM_DATABASE+57)						//消息索引

/*
* "该手机号没有绑定的游戏账号，请仔细核对手机号再次尝试！"
*/
#define MIM_DATABASE_0058				(MIM_DATABASE+58)						//消息索引

/*
* "您的游戏账号今天重置密码的次数已达到%s次，为了您的帐号安全，已锁定验证。请明天再试！"
*/
#define MIM_DATABASE_0059				(MIM_DATABASE+59)						//消息索引

/*
* "该手机号没有绑定的游戏账号，请仔细核对手机号再次尝试！"
*/
#define MIM_DATABASE_0060				(MIM_DATABASE+60)						//消息索引

/*
* "您的手机号今天使用验证码登录的次数已达到%s次，为了您的帐号安全，已锁定验证。请明天再试！"
*/
#define MIM_DATABASE_0061				(MIM_DATABASE+61)						//消息索引

/*
* "该手机号今天的验证次数已超过%s次，为了您的帐号安全，已锁定验证。请明天再试！"
*/
#define MIM_DATABASE_0062				(MIM_DATABASE+62)						//消息索引

/*
* "该手机号今天的验证请求次数已超过%s次，为了您的帐号安全，已锁定验证。请更换手机号码或明天再试！"
*/
#define MIM_DATABASE_0063				(MIM_DATABASE+63)						//消息索引

/*
* "'手机验证码已成功发送至尾号为：%s 的手机。请留意短信通知。"
*/
#define MIM_DATABASE_0064				(MIM_DATABASE+64)						//消息索引

/*
* "该游戏账户今天的手机验证次数已超过%s次，为了您的帐号安全，已锁定验证。请明天再试！"
*/
#define MIM_DATABASE_0065				(MIM_DATABASE+65)						//消息索引

/*
* "该游戏账号今天的手机验证请求次数已超过%s次，为了您的帐号安全，已锁定验证。请明天再试！"
*/
#define MIM_DATABASE_0066				(MIM_DATABASE+66)						//消息索引

/*
* "抱歉，该手机号已经和其他游戏账号绑定，请更换新的手机号绑定！"
*/
#define MIM_DATABASE_0067				(MIM_DATABASE+67)						//消息索引

/*
* "验证码错误，手机号绑定失败！"
*/
#define MIM_DATABASE_0068				(MIM_DATABASE+68)						//消息索引

/*
* "您的机器码发生异常，操作失败！"
*/
#define MIM_DATABASE_0069				(MIM_DATABASE+69)						//消息索引

/*
* "手机号绑定成功！"
*/
#define MIM_DATABASE_0070				(MIM_DATABASE+70)						//消息索引

/*
* "验证码错误，手机号解绑失败！"
*/
#define MIM_DATABASE_0071				(MIM_DATABASE+71)						//消息索引

/*
* "你的钻石余额不足，请充值后再次尝试！"
*/
#define MIM_DATABASE_0072				(MIM_DATABASE+72)						//消息索引

/*
* "你的房卡余额不足，请充值后再次尝试！"
*/
#define MIM_DATABASE_0073				(MIM_DATABASE+73)						//消息索引

/*
* "手机号解绑成功"
*/
#define MIM_DATABASE_0074				(MIM_DATABASE+74)						//消息索引

/*
* "验证码错误，密码重置失败！"
*/
#define MIM_DATABASE_0075				(MIM_DATABASE+75)						//消息索引

/*
* "密码重置成功"
*/
#define MIM_DATABASE_0076				(MIM_DATABASE+76)						//消息索引

/*
* "该邮箱今天的验证次数已超过%s次，为了您的帐号安全，已锁定验证。请明天再试！"
*/
#define MIM_DATABASE_0077				(MIM_DATABASE+77)						//消息索引

/*
* "该邮箱今天的验证请求次数已超过%s次，为了您的帐号安全，已锁定验证。请更换手机号码或明天再试！"
*/
#define MIM_DATABASE_0078				(MIM_DATABASE+78)						//消息索引

/*
* "'验证码已成功发送至邮箱：%s 。请留意邮件通知。"
*/
#define MIM_DATABASE_0079				(MIM_DATABASE+79)						//消息索引

/*
* "该游戏账户今天的邮箱验证次数已超过%s次，为了您的帐号安全，已锁定验证。请明天再试！"
*/
#define MIM_DATABASE_0080				(MIM_DATABASE+80)						//消息索引

/*
* "该游戏账号今天的邮箱验证请求次数已超过%s次，为了您的帐号安全，已锁定验证。请明天再试！"
*/
#define MIM_DATABASE_0081				(MIM_DATABASE+81)						//消息索引

/*
* "您的邮箱今天使用验证码登录的次数已达到%s次，为了您的帐号安全，已锁定验证。请明天再试！"
*/
#define MIM_DATABASE_0082				(MIM_DATABASE+82)						//消息索引

/*
* "绑定银行账号需使用游戏账号绑定的手机号，您输入的手机号码不匹配，请仔细核对后再次尝试！"
*/
#define MIM_DATABASE_0083				(MIM_DATABASE+83)						//消息索引							

/*
* "绑定银行账号需使用游戏账号绑定的邮箱，您的邮箱不匹配，请仔细核对后再次尝试！"
*/
#define MIM_DATABASE_0084				(MIM_DATABASE+84)						//消息索引							

/*
* "抱歉，该邮箱已经和其他游戏账号绑定，邮箱绑定失败！"
*/
#define MIM_DATABASE_0085				(MIM_DATABASE+85)						//消息索引

/*
* "验证码错误，邮箱绑定失败！"
*/
#define MIM_DATABASE_0086				(MIM_DATABASE+86)						//消息索引

/*
* "抱歉，您的用户信息不存在，请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0091				(MIM_DATABASE+91)						//消息索引

/*
* "抱歉，系统检测到你当前正在游戏房间中，请退出房间再来领取！"
*/
#define MIM_DATABASE_0092				(MIM_DATABASE+92)						//消息索引

/*
* "抱歉，您当前身上的游戏币不符合赠送条件！"
*/
#define MIM_DATABASE_0093				(MIM_DATABASE+93)						//消息索引

/*
* "抱歉，您的低分赠送记录出现异常，请联系客服中心！"
*/
#define MIM_DATABASE_0094				(MIM_DATABASE+94)						//消息索引

/*
* "抱歉，您的机器出现异常，游戏币赠送失败！"
*/
#define MIM_DATABASE_0095				(MIM_DATABASE+95)						//消息索引

/*
* "抱歉，您今天已领取完系统赠送给您的游戏币，您还可以通过充值来获取游戏币！"
*/
#define MIM_DATABASE_0096				(MIM_DATABASE+96)						//消息索引

/*
* "抱歉，您今天已领取完系统赠送给您的游戏币，您还可以通过充值来获取游戏币！"
*/
#define MIM_DATABASE_0097				(MIM_DATABASE+97)						//消息索引

/*
* "抱歉，您所在的机器今天已领取完系统赠送的游戏币，您还可以通过充值来获取游戏币！"
*/
#define MIM_DATABASE_0098				(MIM_DATABASE+98)						//消息索引

/*
* "恭喜您，游戏币赠送成功！"
*/
#define MIM_DATABASE_0099				(MIM_DATABASE+99)						//消息索引

/*
* "您已经在金币游戏房间了，不能同时再进入此类游戏房间了！"
*/
#define MIM_DATABASE_0102				(MIM_DATABASE+102)						//消息索引

/*
* "抱歉地通知你，你的游戏币数据异常，请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0103				(MIM_DATABASE+103)						//消息索引

/*
* "抱歉地通知你，游戏币锁定操作失败，请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0104				(MIM_DATABASE+104)						//消息索引

/*
* "抱歉，您的福利赠送记录出现异常，请联系客服中心！"
*/
#define MIM_DATABASE_0105				(MIM_DATABASE+105)						//消息索引

/*
* "抱歉，您的机器码出现异常，游戏币赠送失败！"
*/
#define MIM_DATABASE_0106				(MIM_DATABASE+106)						//消息索引

/*
* "对不起，您今天的免费游戏次数已用完，请明天再来！"
*/
#define MIM_DATABASE_0107				(MIM_DATABASE+107)						//消息索引

/*
* "对不起，您所在的机器今天的免费游戏次数已用完，请明天再来！"
*/
#define MIM_DATABASE_0108				(MIM_DATABASE+108)						//消息索引

/*
* "抱歉，您的低分赠送记录出现异常，请联系客服中心！"
*/
#define MIM_DATABASE_0109				(MIM_DATABASE+109)						//消息索引

/*
* "抱歉,您报名的比赛不存在！"
*/
#define MIM_DATABASE_0110				(MIM_DATABASE+110)						//消息索引

/*
* "抱歉，您的会员等级达不到报名条件，报名失败！"
*/
#define MIM_DATABASE_0111				(MIM_DATABASE+111)						//消息索引

/*
* "抱歉，您的经验值达不到报名条件，报名失败！"
*/
#define MIM_DATABASE_0112				(MIM_DATABASE+112)						//消息索引

/*
* "抱歉，本场比赛已达到最大报名人数，报名失败！"
*/
#define MIM_DATABASE_0113				(MIM_DATABASE+113)						//消息索引

/*
* "报名失败，成功分享朋友圈后，即可免费报名本场比赛！"
*/
#define MIM_DATABASE_0114				(MIM_DATABASE+114)						//消息索引

/*
* "您已经报名本场比赛，不需要重复报名！"
*/
#define MIM_DATABASE_0115				(MIM_DATABASE+115)						//消息索引

/*
* "您身上的金币不足,无法报名！"
*/
#define MIM_DATABASE_0116				(MIM_DATABASE+116)						//消息索引

/*
* "您身上的奖牌不足,无法报名！"
*/
#define MIM_DATABASE_0117				(MIM_DATABASE+117)						//消息索引

/*
* "您身上的钻石不足,无法报名！"
*/
#define MIM_DATABASE_0118				(MIM_DATABASE+118)						//消息索引

/*
* "系统未查询到您的金币信息,请您与我们的客服人员联系！"
*/
#define MIM_DATABASE_0119				(MIM_DATABASE+119)						//消息索引

/*
* "系统未查询到您的奖牌信息,请您与我们的客服人员联系！"
*/
#define MIM_DATABASE_0120				(MIM_DATABASE+120)						//消息索引

/*
* "系统未查询到您的钻石信息,请您与我们的客服人员联系！"
*/
#define MIM_DATABASE_0121				(MIM_DATABASE+121)						//消息索引

/*
* "抱歉,您分享的比赛信息不存在！"
*/
#define MIM_DATABASE_0122				(MIM_DATABASE+122)						//消息索引

/*
* "抱歉,当前比赛不支持朋友圈分享免费报名！"
*/
#define MIM_DATABASE_0123				(MIM_DATABASE+123)						//消息索引

/*
* "分享成功！您今天的分享奖励报名次数已用完！"
*/
#define MIM_DATABASE_0124				(MIM_DATABASE+124)						//消息索引

/*
* "恭喜您，分享成功！奖励当前比赛的免费报名 1 次！"
*/
#define MIM_DATABASE_0125				(MIM_DATABASE+125)						//消息索引

/*
* "抱歉，本场比赛最多允许复活 %s 次，复活失败！"
*/
#define MIM_DATABASE_0126				(MIM_DATABASE+126)						//消息索引

/*
* "抱歉,您身上的游戏币不足,复活失败！"
*/
#define MIM_DATABASE_0127				(MIM_DATABASE+127)						//消息索引

/*
* "抱歉,您身上的奖牌不足,复活失败！"
*/
#define MIM_DATABASE_0128				(MIM_DATABASE+128)						//消息索引

/*
* "抱歉,您身上的钻石不足,复活失败！"
*/
#define MIM_DATABASE_0129				(MIM_DATABASE+129)						//消息索引

/*
* "抱歉，系统未查询到您的钻石信息，钻石返还失败！"
*/
#define MIM_DATABASE_0130				(MIM_DATABASE+130)						//消息索引

/*
* "抱歉，系统未查询到您的钻石信息，钻石返还失败！"
*/
#define MIM_DATABASE_0131				(MIM_DATABASE+131)						//消息索引

/*
* "抱歉，系统未查询到相关的茶社信息。 房间创建失败！"
*/
#define MIM_DATABASE_0132				(MIM_DATABASE+132)						//消息索引

/*
* "抱歉，系统未查询到相关的茶社信息。 房间创建失败！"
*/
#define MIM_DATABASE_0132				(MIM_DATABASE+132)						//消息索引

/*
* "抱歉，佣金提取功能已关闭。"
*/
#define MIM_DATABASE_0133				(MIM_DATABASE+133)						//消息索引

/*
* "抱歉, 每笔提取金额不能小于%s。"
*/
#define MIM_DATABASE_0134				(MIM_DATABASE+134)						//消息索引

/*
* "抱歉！您身上的佣金余额不足，提取失败。"
*/
#define MIM_DATABASE_0135				(MIM_DATABASE+135)						//消息索引

/*
当前帐号已被临时封禁至%s（解封时间）, 暂无法使用。如有其他疑问，请联系客服。
*/
#define MIM_DATABASE_0136				(MIM_DATABASE+136)						//消息索引



/*
* "抱歉，您身上携带的房卡不足%s, 茶社创建失败！"
*/
#define MIM_DATABASE_0150				(MIM_DATABASE+150)						//消息索引

/*
* "抱歉，当前帐号携带的房卡少于%s, 茶社创建失败！"
*/
#define MIM_DATABASE_0151				(MIM_DATABASE+151)						//消息索引

/*
* "抱歉，您创建的茶社数目已经达到上限，如有疑问，请联系客服！"
*/
#define MIM_DATABASE_0152				(MIM_DATABASE+152)						//消息索引

/*
* "系统中已经存在相同名字的茶社，创建失败！"
*/
#define MIM_DATABASE_0153				(MIM_DATABASE+153)						//消息索引

/*
* "您设置的支付类型与系统配置不符，创建失败！"
*/
#define MIM_DATABASE_0154				(MIM_DATABASE+154)						//消息索引

/*
* "由于系统原因, 茶社创建失败[%s]， 请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0155				(MIM_DATABASE+155)						//消息索引


/*
* "抱歉，您不是该茶社的掌柜，没有权限进行删除！"
*/
#define MIM_DATABASE_0157				(MIM_DATABASE+157)						//消息索引

/*
* "抱歉，您查找的茶社信息不存在，请查证后再次尝试！"
*/
#define MIM_DATABASE_0158				(MIM_DATABASE+158)						//消息索引

/*
* "您的帐号没有绑定手机号，请先绑定手机号再进行转让！"
*/
#define MIM_DATABASE_0159				(MIM_DATABASE+159)						//消息索引

/*
* "验证码错误，茶社转让失败[%s]！"
*/
#define MIM_DATABASE_0160				(MIM_DATABASE+160)						//消息索引

/*
* "茶社信息不存在，请查证后再次尝试！"
*/
#define MIM_DATABASE_0161				(MIM_DATABASE+161)						//消息索引

/*
* "抱歉，您不是该茶社的掌柜，没有权限进行转让！"
*/
#define MIM_DATABASE_0162				(MIM_DATABASE+162)						//消息索引

/*
* "茶社转让失败！新掌柜必须是该茶社的成员。"
*/
#define MIM_DATABASE_0163				(MIM_DATABASE+163)						//消息索引

/*
* "由于系统原因,茶社转让失败[%s]，请联系客服了解详细情况！"
*/
#define MIM_DATABASE_0164				(MIM_DATABASE+164)						//消息索引

/*
* "茶社转让成功！"
*/
#define MIM_DATABASE_0165				(MIM_DATABASE+165)						//消息索引

/*
* "抱歉，您没有权限修改茶社！"
*/
#define MIM_DATABASE_0166				(MIM_DATABASE+166)						//消息索引

/*
* "抱歉，茶社掌柜携带的房卡不足，茶社名称修改失败！"
*/
#define MIM_DATABASE_0167				(MIM_DATABASE+167)						//消息索引

/*
* "抱歉，您身上携带的房卡不足，茶社名称修改失败！"
*/
#define MIM_DATABASE_0168				(MIM_DATABASE+168)						//消息索引

/*
* "抱歉，茶社名称已被占用，请重新输入后再次尝试！"
*/
#define MIM_DATABASE_0169				(MIM_DATABASE+169)						//消息索引

/*
* "抱歉，您没有权限踢出该成员！"
*/
#define MIM_DATABASE_0170				(MIM_DATABASE+170)						//消息索引

/*
* "该成员不存在，或已经踢出了茶社！"
*/
#define MIM_DATABASE_0171				(MIM_DATABASE+171)						//消息索引

/*
* "系统未查询到您在该茶社的成员信息，操作失败！"
*/
#define MIM_DATABASE_0172				(MIM_DATABASE+172)						//消息索引

/*
* "系统未查询该成员在茶社里的成员信息，操作失败！"
*/
#define MIM_DATABASE_0173				(MIM_DATABASE+173)						//消息索引

/*
* "您不是茶社掌柜，没有权限设置管理员！"
*/
#define MIM_DATABASE_0174				(MIM_DATABASE+174)						//消息索引

/*
* "您不是茶社掌柜，没有权限取消管理员！"
*/
#define MIM_DATABASE_0175				(MIM_DATABASE+175)						//消息索引

/*
* "抱歉，您没有权限修改成员状态！"
*/
#define MIM_DATABASE_0176				(MIM_DATABASE+176)						//消息索引

/*
* "由于您在约战房间[%s]的游戏尚未完成，暂时不能加入其它约战房间！"
*/
#define MIM_DATABASE_0177				(MIM_DATABASE+177)						//消息索引

/*
* "您已被茶社掌柜冻结游戏权限，请联系掌柜解除！"
*/
#define MIM_DATABASE_0178				(MIM_DATABASE+178)						//消息索引

/*
* "约战玩法校验失败!错误代码[%s]"
*/
#define MIM_DATABASE_0179				(MIM_DATABASE+179)						//消息索引

/*
* "抱歉，您没有权限操作约战玩法！"
*/
#define MIM_DATABASE_0180				(MIM_DATABASE+180)						//消息索引

/*
* "抱歉，茶社内最多只能添加%s个约战玩法！"
*/
#define MIM_DATABASE_0181				(MIM_DATABASE+181)						//消息索引

/*
* "抱歉，玩法不存在！"
*/
#define MIM_DATABASE_0182				(MIM_DATABASE+182)						//消息索引

/*
* "您已经申请加入该茶社，不需要重复申请，请耐心等待茶社掌柜审核！"
*/
#define MIM_DATABASE_0183				(MIM_DATABASE+183)						//消息索引

/*
* "抱歉，该茶社已满员，您不能申请加入！"
*/
#define MIM_DATABASE_0184				(MIM_DATABASE+184)						//消息索引

/*
* "您已经是该茶社的成员！"
*/
#define MIM_DATABASE_0185				(MIM_DATABASE+185)						//消息索引

/*
* "抱歉，您同时可加入茶社数量为%s个，请退出多余茶社后再试！"
*/
#define MIM_DATABASE_0186				(MIM_DATABASE+186)						//消息索引

/*
* "您已成功加入茶社，祝您游戏愉快！"
*/
#define MIM_DATABASE_0187				(MIM_DATABASE+187)						//消息索引

/*
* "申请成功，请耐心等待茶社掌柜审核！"
*/
#define MIM_DATABASE_0188				(MIM_DATABASE+188)						//消息索引

/*
* "抱歉，您的茶社已经满员，成员添加失败！"
*/
#define MIM_DATABASE_0189				(MIM_DATABASE+189)						//消息索引

/*
* "抱歉，您没有权限审核用户申请！"
*/
#define MIM_DATABASE_0190				(MIM_DATABASE+190)						//消息索引

/*
* "系统未查询到相关的申请记录，该记录可能已被申请者撤销！"
*/
#define MIM_DATABASE_0191				(MIM_DATABASE+191)						//消息索引

/*
* "系统检测到申请记录异常，请联系客户服务中心了解详细情况！"
*/
#define MIM_DATABASE_0192				(MIM_DATABASE+192)						//消息索引

/*
* "抱歉，该消息未通过掌柜审核，无法删除！"
*/
#define MIM_DATABASE_0193				(MIM_DATABASE+193)						//消息索引

/*
* "抱歉，您不是申请人,无法删除消息！"
*/
#define MIM_DATABASE_0194				(MIM_DATABASE+194)						//消息索引

/*
* "抱歉，该消息已通过掌柜审核，无法撤销！"
*/
#define MIM_DATABASE_0195				(MIM_DATABASE+195)						//消息索引

/*
* "抱歉，您不是申请人,无法撤销该消息！"
*/
#define MIM_DATABASE_0196				(MIM_DATABASE+196)						//消息索引

/*
* "由于该馆员在约战房间[%s]中的游戏尚未完成，暂时不能进行清理操作！"
*/
#define MIM_DATABASE_0197				(MIM_DATABASE+197)						//消息索引

/*
* "由于当前茶社中部分成员还在进行约战，暂时不能进行清理操作！"
*/
#define MIM_DATABASE_0198				(MIM_DATABASE+198)						//消息索引

/*
* "抱歉，您不是该茶社的掌柜，没有权限进行结算！"
*/
#define MIM_DATABASE_0199				(MIM_DATABASE+199)						//消息索引

/*
* "抱歉，系统未查询到该成员的约战统计信息，结算失败！"
*/
#define MIM_DATABASE_0200				(MIM_DATABASE+200)						//消息索引

/*
* "抱歉，结算次数不能超过约战次数，结算失败！"
*/
#define MIM_DATABASE_0201				(MIM_DATABASE+201)						//消息索引

/*
* "系统未查询到改成员的用户信息，请查证后再次尝试登录！"
*/
#define MIM_DATABASE_0202				(MIM_DATABASE+202)						//消息索引

/*
* "抱歉，您没有权限增减积分！"
*/
#define MIM_DATABASE_0203				(MIM_DATABASE+203)						//消息索引

/*
* "由于该馆员在约战房间[%s]中的游戏尚未完成，暂时不能进行下分操作！"
*/
#define MIM_DATABASE_0204				(MIM_DATABASE+204)						//消息索引

/*
* "抱歉，由于系统原因，操作失败！"
*/
#define MIM_DATABASE_0205				(MIM_DATABASE+205)						//消息索引

/*
* "系统未查询到您的积分信息，请联系客户人员了解详情！"
*/
#define MIM_DATABASE_0206				(MIM_DATABASE+206)						//消息索引

/*
* "由于您的积分不足，请调整好积分额度后再次尝试！"
*/
#define MIM_DATABASE_0207				(MIM_DATABASE+207)						//消息索引

/*
* "系统未查询到该成员的积分信息，请联系客户人员了解详情！"
*/
#define MIM_DATABASE_0208				(MIM_DATABASE+208)						//消息索引

/*
* "您不是茶社的掌柜，没有权限修改公告！"
*/
#define MIM_DATABASE_0209				(MIM_DATABASE+209)						//消息索引

/*
* "您已退出"%s"茶社"
*/
#define MIM_DATABASE_0210				(MIM_DATABASE+210)						//消息索引

/*
* 您已被"%s"茶社的管理踢出茶社。
*/
#define MIM_DATABASE_0211				(MIM_DATABASE+211)						//消息索引

/*
* 您已被"%s"茶社设置为管理员。
*/
#define MIM_DATABASE_0212				(MIM_DATABASE+212)						//消息索引

/*
* 您已被"%s"茶社取消了管理员权限。
*/
#define MIM_DATABASE_0213				(MIM_DATABASE+213)						//消息索引

/*
* 您的"%s"茶社已解散。
*/
#define MIM_DATABASE_0214				(MIM_DATABASE+214)						//消息索引

/*
* 抱歉，您没有权限设置大赢家。
*/
#define MIM_DATABASE_0215				(MIM_DATABASE+215)						//消息索引

/*
* "抱歉，系统未查询到您的金币信息，金币返还失败！"
*/
#define MIM_DATABASE_0240				(MIM_DATABASE+240)						//消息索引

/*
* "抱歉，系统未查询到您的房卡信息，房卡返还失败！"
*/
#define MIM_DATABASE_0241				(MIM_DATABASE+241)						//消息索引

/*
* "抱歉，系统未查询到相关的茶社信息。 房间创建失败！"
*/
#define MIM_DATABASE_0242				(MIM_DATABASE+242)						//消息索引

/*
* "抱歉，当前茶社掌柜身上的房卡不足。房间创建失败，请联系掌柜。"
*/
#define MIM_DATABASE_0243				(MIM_DATABASE+243)						//消息索引

/*
* "抱歉！操作失败，如果您正在约战房间请先退出再进行操作！"
*/
#define MIM_DATABASE_0244				(MIM_DATABASE+244)						//消息索引

/*
* "您的金币携带量不足，房费扣除失败！"
*/
#define MIM_DATABASE_0245				(MIM_DATABASE+245)						//消息索引

/*
* "您的房卡数量不足，请获取更多房卡后重试！"
*/
#define MIM_DATABASE_0246				(MIM_DATABASE+246)						//消息索引

/*
* "抱歉，系统未查询到您的金币信息，金币返还失败！"
*/
#define MIM_DATABASE_0247				(MIM_DATABASE+247)						//消息索引

/*
* "验证码错误，收款账号设置失败！"
*/
#define MIM_DATABASE_0276				(MIM_DATABASE+276)						//消息索引

/*
* "抱歉，性别信息系统暂时只允许每个玩家修改一次，您已经修改过了！"
*/
#define MIM_DATABASE_0277				(MIM_DATABASE+277)						//消息索引

/*
* "您今天已经赞过该玩家！"
*/
#define MIM_DATABASE_0278				(MIM_DATABASE+278)						//消息索引

/*
* "点赞成功！"
*/
#define MIM_DATABASE_0279				(MIM_DATABASE+279)						//消息索引

/*
* "好友申请不存在！"
*/
#define MIM_DATABASE_0280				(MIM_DATABASE+280)						//消息索引

/*
* "好友申请已处理！"
*/
#define MIM_DATABASE_0281				(MIM_DATABASE+281)						//消息索引

/*
* "好友申请已过期！"
*/
#define MIM_DATABASE_0282				(MIM_DATABASE+282)						//消息索引

/*
* "您没有权限处理该好友申请！"
*/
#define MIM_DATABASE_0283				(MIM_DATABASE+283)						//消息索引

/*
* "已经是好友无需再申请！"
*/
#define MIM_DATABASE_0284				(MIM_DATABASE+284)						//消息索引

/*
* "抱歉，系统未查询到该商品信息，购买失败！"
*/
#define MIM_DATABASE_0285				(MIM_DATABASE+285)						//消息索引

/*
* "抱歉，该商品不能使用sawa point购买！"
*/
#define MIM_DATABASE_0286				(MIM_DATABASE+286)						//消息索引
/*
* "
抱歉，该商品需VIP%s才可购买！"
*/
#define MIM_DATABASE_0287				(MIM_DATABASE+287)						//消息索引

/*
* "系统未查询到相关的比赛信息，请查证后再次尝试！"
*/

#define MIM_DATABASE_0288				(MIM_DATABASE+288)						//消息索引

/*
* "您身上的钻石余额不足，比赛券购买失败！"
*/

#define MIM_DATABASE_0289				(MIM_DATABASE+289)						//消息索引

/*
* "您身上的比赛券不足, 无法参与比赛！"
*/

#define MIM_DATABASE_0290				(MIM_DATABASE+290)						//消息索引


/*
* "您点赞的用户信息不存在！"
*/

#define MIM_DATABASE_0291				(MIM_DATABASE+291)						//消息索引

/*
* "抱歉，您已经绑定了上级，绑定邀请码失败！"
*/

#define MIM_DATABASE_0292				(MIM_DATABASE+292)						//消息索引

/*
* "抱歉，该上级玩家不存在，绑定邀请码失败！"
*/

#define MIM_DATABASE_0293				(MIM_DATABASE+293)						//消息索引

/*
* "抱歉，您不能绑定自己，绑定邀请码失败！"
*/

#define MIM_DATABASE_0294				(MIM_DATABASE+294)						//消息索引

/*
* "抱歉，您不能绑定自己邀请的玩家，绑定邀请码失败！"
*/

#define MIM_DATABASE_0295				(MIM_DATABASE+295)						//消息索引

/*
* "抱歉，由于您身上的sawa point余额不足，兑换失败！"
*/

#define MIM_DATABASE_0296				(MIM_DATABASE+296)						//消息索引

/*
* "系统未查询到该邮件信息！"
*/

#define MIM_DATABASE_0297				(MIM_DATABASE+297)						//消息索引

/*
* "请先查阅邮件！"
*/

#define MIM_DATABASE_0298				(MIM_DATABASE+298)						//消息索引

/*
* "该邮件没有可领取的附件！"
*/

#define MIM_DATABASE_0299				(MIM_DATABASE+299)						//消息索引

/*
* "该邮件状态异常，请联系客服了解详细情况！"
*/

#define MIM_DATABASE_0300				(MIM_DATABASE+300)						//消息索引

/*
* "系统查询到您正在游戏房间中，附件领取失败！"
*/

#define MIM_DATABASE_0301				(MIM_DATABASE+301)						//消息索引

/*
* "部分物品堆叠数量已超上限，请扩容后再次尝试！"
*/

#define MIM_DATABASE_0302				(MIM_DATABASE+302)						//消息索引

/*
* "您已经绑定了当前账号, 无需重复绑定！"
*/

#define MIM_DATABASE_0303				(MIM_DATABASE+303)						//消息索引

/*
* "当前账号已经绑定了其他的游戏账号！"
*/

#define MIM_DATABASE_0304				(MIM_DATABASE+304)						//消息索引

/*
* "账号绑定成功！"
*/

#define MIM_DATABASE_0305				(MIM_DATABASE+305)						//消息索引

/*
* "您投诉的用户信息不存在！"
*/

#define MIM_DATABASE_0306				(MIM_DATABASE+306)						//消息索引

/*
* "礼包码无效，请重新输入！"
*/

#define MIM_DATABASE_0307				(MIM_DATABASE+307)						//消息索引

/*
* "礼包码不正确，请重新输入！"
*/

#define MIM_DATABASE_0308				(MIM_DATABASE+308)						//消息索引

/*
* "礼包码已失效，兑换失败！"
*/

#define MIM_DATABASE_0309				(MIM_DATABASE+309)						//消息索引

/*
* "此礼包码已兑换完！"
*/

#define MIM_DATABASE_0310				(MIM_DATABASE+310)						//消息索引

/*
* "此礼包码您已兑换完！"
*/

#define MIM_DATABASE_0311				(MIM_DATABASE+311)						//消息索引

/*
* "登录授权已过期，请重启APP！"
*/

#define MIM_DATABASE_0312				(MIM_DATABASE+312)						//消息索引

/*
*  "抱歉，系统未查询到该礼物信息，赠送失败！"
*/

#define MIM_DATABASE_0313				(MIM_DATABASE+313)						//消息索引

/*
*  "抱歉，您要赠送的玩家不存在，赠送失败！"
*/

#define MIM_DATABASE_0314				(MIM_DATABASE+314)						//消息索引

/*
*  "抱歉，您的金币不足，赠送失败！"
*/

#define MIM_DATABASE_0315				(MIM_DATABASE+315)						//消息索引

/*
*  "抱歉，您的金币不足，赠送失败！"
*/

#define MIM_DATABASE_0316				(MIM_DATABASE+316)						//消息索引

/*
*  "您需要保留'%s'的金币，由于金币不足，赠送失败！"
*/

#define MIM_DATABASE_0317				(MIM_DATABASE+317)						//消息索引

/*
*  "不支持的支付方式，请联系管理员！"
*/

#define MIM_DATABASE_0318				(MIM_DATABASE+318)						//消息索引

/*
*  "抱歉，您不能给自己送礼物，赠送失败！"
*/

#define MIM_DATABASE_0319				(MIM_DATABASE+319)	

/*
* "您身上的金币余额不足，比赛券购买失败！"
*/

#define MIM_DATABASE_0320				(MIM_DATABASE+320)						//消息索引

/*
* "您身上的金币余额不足%s，修改昵称失败！"
*/

#define MIM_DATABASE_0321				(MIM_DATABASE+321)						//消息索引

/*
* "您身上的金币余额不足%s，修改头像失败！"
*/

#define MIM_DATABASE_0322				(MIM_DATABASE+322)						//消息索引

/*
* "您尚未拥有该配件，更换失败！"
*/

#define MIM_DATABASE_0323				(MIM_DATABASE+323)						//消息索引

/*
* "转盘抽奖活动已关闭！"
*/

#define MIM_DATABASE_0324				(MIM_DATABASE+324)						//消息索引

/*
* "抽奖机会已用完！"
*/

#define MIM_DATABASE_0325				(MIM_DATABASE+325)						//消息索引

/*
* "当前账号存在风险，暂时无法游戏，详情咨请询客服"
*/

#define MIM_DATABASE_0326				(MIM_DATABASE+326)						//消息索引

/*
* "已认证成功，请勿重复认证"
*/

#define MIM_DATABASE_0327				(MIM_DATABASE+327)						//消息索引

/*
* "认证成功"
*/

#define MIM_DATABASE_0328				(MIM_DATABASE+328)						//消息索引

/*
* "修改次数已达上限"
*/

#define MIM_DATABASE_0329				(MIM_DATABASE+329)						//消息索引

/*
* "系统检测到您的账号正在游戏房间中，请先退出房间再进行银行操作！"
*/

#define MIM_DATABASE_0330				(MIM_DATABASE+330)						//消息索引

/*
* "抱歉，银行功能已关闭！"
*/

#define MIM_DATABASE_0331				(MIM_DATABASE+331)						//消息索引

/*
* "抱歉, 每笔操作金额不能小于%s。"
*/

#define MIM_DATABASE_0332				(MIM_DATABASE+332)						//消息索引

/*
* "抱歉, 每笔操作金额不能大于%s。"
*/

#define MIM_DATABASE_0333				(MIM_DATABASE+333)						//消息索引

/*
* "抱歉, 每日累计操作金额不能大于%s。"
*/

#define MIM_DATABASE_0334				(MIM_DATABASE+334)						//消息索引

/*
* "抱歉, 银行密码错误！"
*/

#define MIM_DATABASE_0335				(MIM_DATABASE+335)						//消息索引

/*
* "抱歉, 银行余额不足！"
*/

#define MIM_DATABASE_0336				(MIM_DATABASE+336)						//消息索引

/*
* "抱歉, 金币不足！"
*/

#define MIM_DATABASE_0337				(MIM_DATABASE+337)						//消息索引

/*
* "抱歉，您所在的IP今天已领取完系统赠送的游戏币，您还可以通过充值来获取游戏币！"
*/

#define MIM_DATABASE_0338				(MIM_DATABASE+338)						//消息索引

//////////////////////////////////////////////////////////////////////////
//网关模块
#define MIM_GATE						2000									//消息索引

//////////////////////////////////////////////////////////////////////////

/*
* "当前服务模块链接状态异常，请重新登录再次进入！"
*/
#define MIM_GATE_0001					(MIM_GATE+1)							//消息索引

/*
* "服务器繁忙，请您稍后再次尝试！"
*/
#define MIM_GATE_0002					(MIM_GATE+2)							//消息索引

/*
* "您请求的服务器不可达！"
*/
#define MIM_GATE_0003					(MIM_GATE+3)							//消息索引

/*
* "登录令牌验证失败！"
*/
#define MIM_GATE_0004					(MIM_GATE+4)							//消息索引

//////////////////////////////////////////////////////////////////////////
//登录模块

#define MIM_LOGON						2500									//消息索引

//////////////////////////////////////////////////////////////////////////

/*
* "抱歉，系统未查询到您的验证码信息，请重新输入新的验证码！"
*/
#define MIM_LOGON_0001					(MIM_LOGON+1)							//消息索引

/*
* "抱歉，您的验证码已过期！"
*/
#define MIM_LOGON_0002					(MIM_LOGON+2)							//消息索引

/*
* "抱歉，您输入的验证码有误！"
*/
#define MIM_LOGON_0003					(MIM_LOGON+3)							//消息索引

/*
* "由于数据库操作异常，请您稍后重试或选择另一服务器登录！"
*/
#define MIM_LOGON_0004					(MIM_LOGON+4)							//消息索引

/*
* "由于系统繁忙，请稍后再试！"
*/
#define MIM_LOGON_0005					(MIM_LOGON+5)							//消息索引


/*
* "由于数据库异常，请稍后再次尝试！"
*/
#define MIM_LOGON_0006					(MIM_LOGON+6)							//消息索引

/*
* "由于您的账号在别处登录，您被迫下线！"
*/
#define MIM_LOGON_0007					(MIM_LOGON+7)							//消息索引

/*
* "由于您的登录令牌已过期！"
*/
#define MIM_LOGON_0008					(MIM_LOGON+8)							//消息索引

/*
* "抱歉，您所输入的昵称含有限制字符串，请更换昵称后尝试再次修改！"
*/
#define MIM_LOGON_0009					(MIM_LOGON+9)							//消息索引

/*
* "抱歉，您所输入的个性签名含有限制字符串，请更换后尝试再次修改！"
*/
#define MIM_LOGON_0010					(MIM_LOGON+10)							//消息索引

/*
* "抱歉，您的账号已被冻结！"
*/
#define MIM_LOGON_0011					(MIM_LOGON+11)							//消息索引

/*
* "抱歉，您已被迫下线！"
*/
#define MIM_LOGON_0012					(MIM_LOGON+12)							//消息索引

//////////////////////////////////////////////////////////////////////////
//房间模块

#define MIM_SERVER						3000									//消息索引

//////////////////////////////////////////////////////////////////////////

/*
* "抱歉，您被管理员踢出房间，需在被踢后一小时才能进入，若有任何疑问，请联系游戏客服！"
*/

#define MIM_SERVER_0001					(MIM_SERVER+1)						//消息索引

/*
* "抱歉，比赛解散失败！"
*/

#define MIM_SERVER_0002					(MIM_SERVER+2)						//消息索引

/*
* "比赛解散成功"
*/

#define MIM_SERVER_0003					(MIM_SERVER+3)						//消息索引

/*
* "服务器房间配置参数选项设置成功"
*/

#define MIM_SERVER_0004					(MIM_SERVER+4)						//消息索引

/*
* "用户聊天权限配置成功"
*/

#define MIM_SERVER_0005					(MIM_SERVER+5)						//消息索引

/*
* "抱歉！您被会员玩家请出了游戏桌!"
*/

#define MIM_SERVER_0006					(MIM_SERVER+6)						//消息索引


/*
* "很抱歉，您的约战密码错误，不允许进入约战房间！"
*/

#define MIM_SERVER_0007					(MIM_SERVER+7)						//消息索引


/*
* "该帐号已在此房间游戏，且不允许踢出，请咨询管理员！"
*/

#define MIM_SERVER_0008					(MIM_SERVER+8)						//消息索引


/*
* "很抱歉，您的登录密码错误，不允许继续进入！"
*/

#define MIM_SERVER_0009					(MIM_SERVER+9)						//消息索引


/*
* "您正在游戏中，暂时不能离开，请先结束当前游戏！"
*/

#define MIM_SERVER_0010					(MIM_SERVER+10)						//消息索引


/*
* "抱歉，比赛房间不允许旁观！"
*/

#define MIM_SERVER_0011					(MIM_SERVER+11)						//消息索引


/*
* "抱歉，此房间不允许旁观游戏！"
*/

#define MIM_SERVER_0012					(MIM_SERVER+12)						//消息索引

/*
* "旁观过于频繁，请您稍后再试！"
*/

#define MIM_SERVER_0013					(MIM_SERVER+13)						//消息索引

/*
* "您正在游戏中，暂时不能离开，请先结束当前游戏！"
*/

#define MIM_SERVER_0014					(MIM_SERVER+14)						//消息索引

/*
* "您的帐号总游戏币（包含保险柜内游戏币）未低于%s，不符合免费游戏条件！"
*/

#define MIM_SERVER_0015					(MIM_SERVER+15)						//消息索引

/*
* "对不起，您今天的免费游戏次数已用完，请明天再来！"
*/

#define MIM_SERVER_0016					(MIM_SERVER+16)						//消息索引

/*
* "您好，本房间只有游戏币为 %s 以下的玩家才能参加游戏，请您到别的房间游戏！"
*/

#define MIM_SERVER_0017					(MIM_SERVER+17)						//消息索引

/*
* "您好，本房间只有茶社积分为 %s 以下的玩家才能参加游戏，请您到别的房间游戏！"
*/

#define MIM_SERVER_0018					(MIM_SERVER+18)						//消息索引

/*
* "您好，本房间只有游戏积分为 %s 以下的玩家才能参加游戏，请您到别的房间游戏！"
*/

#define MIM_SERVER_0019					(MIM_SERVER+19)						//消息索引

/*
* "没有找到可用的桌子，请稍后再试！"
*/

#define MIM_SERVER_0020					(MIM_SERVER+20)						//消息索引

/*
* "对不起，您所查找的约战房间人数已满！"
*/

#define MIM_SERVER_0021					(MIM_SERVER+21)						//消息索引

/*
* "没有找到可用的椅子，请稍后再试！"
*/

#define MIM_SERVER_0022					(MIM_SERVER+22)						//消息索引

/*
* "抱歉，此游戏房间不允许大厅公聊！"
*/

#define MIM_SERVER_0023					(MIM_SERVER+23)						//消息索引

/*
* "抱歉，您没有大厅公聊的权限，若需要帮助，请联系游戏客服咨询！"
*/

#define MIM_SERVER_0024					(MIM_SERVER+24)						//消息索引

/*
* "抱歉，此房间不允许发送喇叭！"
*/

#define MIM_SERVER_0025					(MIM_SERVER+25)						//消息索引

/*
* "抱歉，您没有发送喇叭的权限，若需要帮助，请联系游戏客服咨询！"
*/

#define MIM_SERVER_0026					(MIM_SERVER+26)						//消息索引

/*
* "您好，此功能暂时不开放！"
*/

#define MIM_SERVER_0027					(MIM_SERVER+27)						//消息索引

/*
* "抱歉，比赛正常进行，不允许使用喇叭功能！"
*/

#define MIM_SERVER_0028					(MIM_SERVER+28)						//消息索引

/*
* "您的发送速度太快了，%s秒允许发送一次，请坐下来喝杯茶先休息下吧！，请坐下来喝杯茶休息下吧！"
*/

#define MIM_SERVER_0029					(MIM_SERVER+29)						//消息索引

/*
* "发送失败，您的帐号保险柜游戏币余额不够，请先存入游戏币。需要充值请点击确定。"
*/

#define MIM_SERVER_0030					(MIM_SERVER+30)						//消息索引

/*
* "抱歉，由于此房间已经人满，普通玩家不能继续进入了！"
*/

#define MIM_SERVER_0031					(MIM_SERVER+31)						//消息索引

/*
* "抱歉，由于系统维护的原因，此游戏房间暂时不允许用户进入！"
*/

#define MIM_SERVER_0032					(MIM_SERVER+32)						//消息索引

/*
* "您的帐号总游戏币（包含保险柜内游戏币）未低于%s，不符合免费游戏条件！"
*/

#define MIM_SERVER_0033					(MIM_SERVER+33)						//消息索引

/*
* "您当前游戏币低于 %s，系统已自动补足至 %s，您今天的低分赠送次数还有 %s 次！"
*/

#define MIM_SERVER_0034					(MIM_SERVER+34)						//消息索引


/*
* "亲爱的%s,您的游戏币少于 %s 个，不能继续游戏！"
*/

#define MIM_SERVER_0035					(MIM_SERVER+35)						//消息索引

/*
* "抱歉，本桌座位已坐满！"
*/

#define MIM_SERVER_0036					(MIM_SERVER+36)						//消息索引

/*
* "欢迎您进入游戏房间，祝您游戏愉快！"
*/

#define MIM_SERVER_0037					(MIM_SERVER+37)						//消息索引

/*
* "请注意，您的帐号在另一地方进入此游戏房间了，您被迫离开！"
*/

#define MIM_SERVER_0038					(MIM_SERVER+38)						//消息索引

/*
* "没找到可进入的游戏桌！"
*/

#define MIM_SERVER_0039					(MIM_SERVER+39)						//消息索引

/*
* "抱歉，此游戏房间不允许用户私聊！"
*/

#define MIM_SERVER_0040					(MIM_SERVER+40)						//消息索引

/*
* "抱歉，您没有发送私聊的权限，若需要帮助，请联系游戏客服咨询！"
*/

#define MIM_SERVER_0041					(MIM_SERVER+41)						//消息索引

/*
* "抱歉，此游戏房间不允许在游戏中与同桌的玩家私聊！"
*/

#define MIM_SERVER_0042					(MIM_SERVER+42)						//消息索引

/*
* "抱歉，没找到可用的房间, 请稍后再试！"
*/

#define MIM_SERVER_0043					(MIM_SERVER+43)						//消息索引

/*
* "抱歉，房间创建失败, 请稍后再试！""
*/

#define MIM_SERVER_0044					(MIM_SERVER+44)						//消息索引

/*
* "抱歉，系统未查询到房间信息,配置更新失败！"
*/

#define MIM_SERVER_0045					(MIM_SERVER+45)						//消息索引

/*
* "抱歉，配置更新失败！"
*/

#define MIM_SERVER_0046					(MIM_SERVER+46)						//消息索引

/*
* "房间已被解散！"
*/

#define MIM_SERVER_0047					(MIM_SERVER+47)						//消息索引

/*
* "此桌有玩家设置了不与您同桌游戏！"
*/

#define MIM_SERVER_0048					(MIM_SERVER+48)						//消息索引

/*
* "您没有权限解散房间!"
*/

#define MIM_SERVER_0049					(MIM_SERVER+49)						//消息索引

/*
* "房间解散成功!"
*/

#define MIM_SERVER_0050					(MIM_SERVER+50)						//消息索引

/*
* "房间解散失败!"
*/

#define MIM_SERVER_0051					(MIM_SERVER+51)						//消息索引

/*
* "您的积分少于 " %s " 个，无法继续游戏！"
*/

#define MIM_SERVER_0057					(MIM_SERVER+57)						//消息索引

/*
* "您的游戏币少于 " %s " 个，无法继续游戏！"
*/

#define MIM_SERVER_0058					(MIM_SERVER+58)						//消息索引

/*
* "您的游戏币少于 " %s " 个，无法继续游戏,牌局结束！"
*/

#define MIM_SERVER_0059					(MIM_SERVER+59)						//消息索引

/*
* "请注意，游戏房间即将关闭或者不允许玩家进入，请您离开游戏桌子！"
*/

#define MIM_SERVER_0060					(MIM_SERVER+60)						//消息索引

/*
* "您好，您的积分已经高于 " %s "，请您到别的房间游戏！"
*/

#define MIM_SERVER_0061					(MIM_SERVER+61)						//消息索引

/*
* "您好，您的游戏币已经高于 " %s "，请您到别的房间游戏！"
*/

#define MIM_SERVER_0062					(MIM_SERVER+62)						//消息索引

/*
* "您好，您的游戏积分已经高于 " %s "，请您到别的房间游戏！"
*/

#define MIM_SERVER_0063					(MIM_SERVER+63)						//消息索引

/*
* "当前有玩家积分不足，无法继续游戏，牌局结束！"
*/

#define MIM_SERVER_0064					(MIM_SERVER+64)						//消息索引

/*
* "游戏超时时间已到，当前有玩家超时或断线超过规定时间；约战自动结束。"
*/

#define MIM_SERVER_0065					(MIM_SERVER+65)						//消息索引

/*
* "该房间经玩家协商，一致同意解散，请重新约战。"
*/

#define MIM_SERVER_0066					(MIM_SERVER+66)						//消息索引

/*
* "房主已解散当前房间，请重新约战。"
*/

#define MIM_SERVER_0067					(MIM_SERVER+67)						//消息索引

/*
* "由于您长时间没有开始游戏，系统请您重新选择游戏桌！"
*/

#define MIM_SERVER_0068					(MIM_SERVER+68)						//消息索引

/*
* "本桌的服务时间已到，系统将在%s分%s秒之后强制解散，请尽快完成游戏！"
*/

#define MIM_SERVER_0069					(MIM_SERVER+69)						//消息索引

/*
* "房间等待时间已到，当前游戏人数不够！房间自动解散，服务费将退还房主！"
*/

#define MIM_SERVER_0070					(MIM_SERVER+70)						//消息索引

/*
* "请注意，本桌有玩家IP地址相同！"
*/

#define MIM_SERVER_0071					(MIM_SERVER+71)						//消息索引

/*
* "系统重新分配桌子，请稍后！"
*/

#define MIM_SERVER_0072					(MIM_SERVER+72)						//消息索引

/*
* "抱歉，为了避免影响游戏的正常进行，系统禁止玩家在旁观过程中发言！"
*/

#define MIM_SERVER_0073					(MIM_SERVER+73)						//消息索引

/*
* "抱歉！此房间旁观聊天功能只对会员开放"
*/

#define MIM_SERVER_0074					(MIM_SERVER+74)						//消息索引

/*
* "抱歉，此游戏房间不允许游戏聊天！"
*/

#define MIM_SERVER_0075					(MIM_SERVER+75)						//消息索引

/*
* "抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"
*/

#define MIM_SERVER_0076					(MIM_SERVER+76)						//消息索引

/*
* "抱歉，本房间未开放语音聊天功能！"
*/

#define MIM_SERVER_0077					(MIM_SERVER+77)						//消息索引

/*
* "系统即将维护，约战房间暂时停止服务。请在维护完成后重试！"
*/

#define MIM_SERVER_0078					(MIM_SERVER+78)						//消息索引

/*
* "系统即将维护，本局游戏完成后，暂时无法进入房间，请在维护完成后重试！"
*/

#define MIM_SERVER_0079					(MIM_SERVER+79)						//消息索引

/*
* "茶社钻石不足，请联系掌柜！"
*/

#define MIM_SERVER_0080					(MIM_SERVER+80)						//消息索引

/*
* "茶社钻石不足，请及时充值！"
*/

#define MIM_SERVER_0081					(MIM_SERVER+81)						//消息索引

/*
* "本桌有玩家金币不足，游戏无法开始！"
*/

#define MIM_SERVER_0082					(MIM_SERVER+82)						//消息索引

/*
* "由于您身上金币不足，无法参与游戏！"
*/

#define MIM_SERVER_0083					(MIM_SERVER+83)						//消息索引

/*
* "由于玩家离开，系统重新分配桌子，请稍后！"
*/

#define MIM_SERVER_0084					(MIM_SERVER+84)						//消息索引

/*
* "游戏还没有开始，暂时不能旁观此游戏桌！"
*/

#define MIM_SERVER_0085					(MIM_SERVER+85)						//消息索引

/*
* "您所请求的位置没有游戏玩家，无法旁观此游戏桌！"
*/

#define MIM_SERVER_0086					(MIM_SERVER+86)						//消息索引

/*
* "抱歉，此游戏桌暂时不允许用户进入！"
*/

#define MIM_SERVER_0087					(MIM_SERVER+87)						//消息索引

/*
* "约战已经开始了，暂时不能进入游戏桌！"
*/

#define MIM_SERVER_0088					(MIM_SERVER+88)						//消息索引

/*
* "系统维护中，约战房间暂时停止服务。请在维护完成后重试！"
*/

#define MIM_SERVER_0089					(MIM_SERVER+89)						//消息索引

/*
* "桌子密码错误，不能加入游戏！"
*/

#define MIM_SERVER_0090					(MIM_SERVER+90)						//消息索引

/*
* "抱歉，当前约战房间不允许旁观！"
*/

#define MIM_SERVER_0091					(MIM_SERVER+91)						//消息索引

/*
* "游戏已经开始了，暂时不能进入游戏桌！"
*/

#define MIM_SERVER_0092					(MIM_SERVER+92)						//消息索引

/*
* "椅子已经被 [ %s ] 捷足先登了，下次动作要快点了！"
*/

#define MIM_SERVER_0093					(MIM_SERVER+93)						//消息索引

/*
* "您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！"
*/

#define MIM_SERVER_0094					(MIM_SERVER+94)						//消息索引

/*
* "您的逃跑率太高，与 %s 设置的设置不符，不能加入游戏！"
*/

#define MIM_SERVER_0095					(MIM_SERVER+95)						//消息索引

/*
* "您的胜率太低，与 %s 设置的设置不符，不能加入游戏！"
*/

#define MIM_SERVER_0096					(MIM_SERVER+96)						//消息索引

/*
* "您的积分太高，与 %s 设置的设置不符，不能加入游戏！"
*/

#define MIM_SERVER_0097					(MIM_SERVER+97)						//消息索引

/*
* "您的积分太低，与 %s 设置的设置不符，不能加入游戏！"
*/

#define MIM_SERVER_0098					(MIM_SERVER+98)						//消息索引

/*
* "加入游戏至少需要 " %s " 个贝壳，您的贝壳不足不能加入！请联系茶社掌柜添加。"
*/

#define MIM_SERVER_0099					(MIM_SERVER+99)						//消息索引

/*
* "加入游戏至少需要 " %s " 个游戏币，您的游戏币不足不能加入！"
*/

#define MIM_SERVER_0100					(MIM_SERVER+100)					//消息索引

/*
* "加入游戏至少需要 " %s "积分，您的积分不足不能加入！请联系茶社掌柜添加。"
*/

#define MIM_SERVER_0101					(MIM_SERVER+101)					//消息索引

/*
* "加入游戏至少需要 %s 个游戏币，您的游戏币不够，不能加入！需要充值请点击确定！"
*/

#define MIM_SERVER_0102					(MIM_SERVER+102)					//消息索引

/*
* "本桌设置了只允许 %s 人游戏，您不能加入！"
*/

#define MIM_SERVER_0103					(MIM_SERVER+103)					//消息索引

/*
* "抱歉，本房间尚未创建，您不能加入！"
*/

#define MIM_SERVER_0104					(MIM_SERVER+104)					//消息索引

/*
* "抱歉，该茶社已打烊，暂时不能加入！"
*/

#define MIM_SERVER_0105					(MIM_SERVER+105)					//消息索引

/*
* "抱歉，您身上的金币不够支付入场费，不能加入！"
*/

#define MIM_SERVER_0106					(MIM_SERVER+106)						//消息索引

/*
* "抱歉，您身上的房卡不够，不能加入！"
*/

#define MIM_SERVER_0107					(MIM_SERVER+107)						//消息索引

/*
* "由于您的IP跟已在桌的玩家相同，无法参与本次约战。"
*/

#define MIM_SERVER_0108					(MIM_SERVER+108)						//消息索引

/*
* "由于您的地理位置获取异常，无法参与本次约战，请打开GPS定位后再次尝试！"
*/

#define MIM_SERVER_0109					(MIM_SERVER+109)						//消息索引

/*
* "由于您的地理位置跟已在桌玩家的距离小于%s米，无法参与本次游戏。"
*/

#define MIM_SERVER_0110					(MIM_SERVER+110)						//消息索引

/*
* "很抱歉，由于数据库处理繁忙，请您稍后再次尝试或者进去其他游戏房间！"
*/

#define MIM_SERVER_0111					(MIM_SERVER+111)						//消息索引

/*
* "由于数据库异常，房间创建失败！"
*/

#define MIM_SERVER_0112					(MIM_SERVER+112)						//消息索引

/*
* "由于数据库操作异常，请您稍后重试！"
*/

#define MIM_SERVER_0113					(MIM_SERVER+113)						//消息索引

/*
* "游戏等待时间已到，桌子自动解散！"
*/

#define MIM_SERVER_0114					(MIM_SERVER+114)						//消息索引

/*
* "本桌有玩家爱心不足，游戏无法开始！"
*/

#define MIM_SERVER_0115					(MIM_SERVER+115)						//消息索引

/*
* "由于您身上爱心不足，无法参与游戏！"
*/

#define MIM_SERVER_0116					(MIM_SERVER+116)						//消息索引

/*
* "很抱歉，您的排位密码错误，不允许进入排位房间！"
*/

#define MIM_SERVER_0117					(MIM_SERVER+117)						//消息索引

/*
* "很抱歉，您已被管理员禁言，请联系客服了解详细情况！"
*/

#define MIM_SERVER_0118					(MIM_SERVER+118)						//消息索引

/*
* "抱歉，由于此房间即将关闭，为了保障您的帐号安全，服务器主动与您断开连接！ "
*/

#define MIM_SERVER_0119					(MIM_SERVER+119)						//消息索引


/*
* "抱歉，游戏尚未完成，您暂时不能离开！ "
*/

#define MIM_SERVER_0120					(MIM_SERVER+120)						//消息索引

/*
* "抱歉，约战尚未完成，您暂时不能退出！ "
*/

#define MIM_SERVER_0121					(MIM_SERVER+121)						//消息索引

/*
* "当前所旁观的房间已解散。 "
*/

#define MIM_SERVER_0122					(MIM_SERVER+122)						//消息索引


//////////////////////////////////////////////////////////////////////////
//茶社广场模块

#define MIM_CLUBPLAZA					3500									//消息索引


//////////////////////////////////////////////////////////////////////////

/*
* "系统维护中，茶社暂时停止服务。请稍后再次重试！"
*/

#define MIM_CLUBPLAZA_0001					(MIM_CLUBPLAZA+1)					//消息索引

/*
* "抱歉, 茶社名称中包含敏感词汇, 茶社创建失败！"
*/

#define MIM_CLUBPLAZA_0002					(MIM_CLUBPLAZA+2)					//消息索引

/*
* "抱歉,茶社介绍中包含敏感词汇,茶社创建失败！"
*/

#define MIM_CLUBPLAZA_0003					(MIM_CLUBPLAZA+3)					//消息索引

/*
* "抱歉,茶社名称中包含敏感词汇,修改失败！"
*/

#define MIM_CLUBPLAZA_0004					(MIM_CLUBPLAZA+4)					//消息索引

/*
* "抱歉,茶社介绍中包含敏感词汇,修改失败！"
*/

#define MIM_CLUBPLAZA_0005					(MIM_CLUBPLAZA+5)					//消息索引

/*
* "抱歉,成员类型异常,操作失败！"
*/

#define MIM_CLUBPLAZA_0006					(MIM_CLUBPLAZA+6)					//消息索引

/*
* "抱歉,成员状态异常,操作失败！"
*/

#define MIM_CLUBPLAZA_0007					(MIM_CLUBPLAZA+7)					//消息索引

/*
* "茶社名称包含敏感词汇！"
*/

#define MIM_CLUBPLAZA_0008					(MIM_CLUBPLAZA+8)					//消息索引

/*
* "由于数据库操作异常，请稍后重试！"
*/

#define MIM_CLUBPLAZA_0009					(MIM_CLUBPLAZA+9)					//消息索引

/*
* "抱歉，您已被移出[%s]的茶社。"
*/

#define MIM_CLUBPLAZA_0010					(MIM_CLUBPLAZA+10)					//消息索引

/*
* "您已退出[%s]的茶社。"
*/

#define MIM_CLUBPLAZA_0011					(MIM_CLUBPLAZA+11)					//消息索引

/*
* "由于系统原因，茶社创建失败！"
*/

#define MIM_CLUBPLAZA_0012					(MIM_CLUBPLAZA+12)					//消息索引

/*
* "恭喜您，茶社创建成功！"
*/

#define MIM_CLUBPLAZA_0013					(MIM_CLUBPLAZA+13)					//消息索引

/*
* "操作成功！"
*/

#define MIM_CLUBPLAZA_0014					(MIM_CLUBPLAZA+14)					//消息索引

/*
* "当前茶社名称已被占用！"
*/

#define MIM_CLUBPLAZA_0015					(MIM_CLUBPLAZA+15)					//消息索引

/*
* "抱歉,公告内容中包含敏感词汇,修改失败！"
*/

#define MIM_CLUBPLAZA_0016					(MIM_CLUBPLAZA+4)					//消息索引

////////////////////////////////////////////////////////////////////////////
//群组模块

#define MIM_CLUB						4000									//消息索引


////////////////////////////////////////////////////////////////////////////

/*
* "系统维护中，茶社暂时停止服务。请稍后再次重试！"
*/

#define MIM_CLUB_0001					(MIM_CLUB+1)							//消息索引

/*
* "系统未查询到该茶社的信息！"
*/

#define MIM_CLUB_0002					(MIM_CLUB+2)							//消息索引

/*
* "系统未查询到您的用户信息！"
*/

#define MIM_CLUB_0003					(MIM_CLUB+3)							//消息索引

/*
* "抱歉，你不是该茶社的成员，进入失败！"
*/

#define MIM_CLUB_0004					(MIM_CLUB+4)							//消息索引

/*
* "系统未查询到您的用户信息，创建失败！"
*/

#define MIM_CLUB_0005					(MIM_CLUB+5)							//消息索引

/*
* "抱歉，您没有权限创建房间！"
*/

#define MIM_CLUB_0006					(MIM_CLUB+6)							//消息索引

/*
* "抱歉，您没有权限修改房间！"
*/

#define MIM_CLUB_0007					(MIM_CLUB+7)							//消息索引

/*
* "抱歉，您没有权限解散房间！"
*/

#define MIM_CLUB_0008					(MIM_CLUB+8)							//消息索引

/*
* "抱歉，您没有权限添加约战玩法！"
*/

#define MIM_CLUB_0009					(MIM_CLUB+9)							//消息索引

/*
* "抱歉，您没有权限修改约战玩法！"
*/

#define MIM_CLUB_0010					(MIM_CLUB+10)							//消息索引

/*
* "抱歉，您没有权限删除约战玩法！"
*/

#define MIM_CLUB_0011					(MIM_CLUB+11)							//消息索引

/*
* "您已解散茶社[%s]！"
*/

#define MIM_CLUB_0012					(MIM_CLUB+12)							//消息索引

/*
* "您加入的茶社[%s]已被掌柜解散！"
*/

#define MIM_CLUB_0013					(MIM_CLUB+13)							//消息索引

/*
* "茶社掌柜,给您%s%s分"
*/

#define MIM_CLUB_0014					(MIM_CLUB+14)							//消息索引

/*
* "您的积分已被清零"
*/

#define MIM_CLUB_0015					(MIM_CLUB+15)							//消息索引

/*
* "您不是该茶社的成员不能加入房间！"
*/

#define MIM_CLUB_0016					(MIM_CLUB+16)							//消息索引

/*
* "您已被茶社掌柜冻结游戏权限，请联系掌柜解除！"
*/

#define MIM_CLUB_0017					(MIM_CLUB+17)							//消息索引

/*
* "抱歉，您没有权限设置大赢家最低赢分！"
*/

#define MIM_CLUB_0018					(MIM_CLUB+18)							//消息索引

//////////////////////////////////////////////////////////////////////////
//比赛模块

#define MIM_MATCH						4500									//消息索引

/*
* "抱歉，同一游戏的同类型比赛只能报名一场，请完成比赛再来参加！"
*/

#define MIM_MATCH_0001					(MIM_MATCH+1)							//消息索引


/*
* "抱歉，比赛分组获取失败，请稍后再试！"
*/

#define MIM_MATCH_0002					(MIM_MATCH+2)							//消息索引

/*
* "比赛报名时间 %s%s:%s， 请您准时报名参加！"
*/

#define MIM_MATCH_0003					(MIM_MATCH+3)							//消息索引

/*
* "比赛已经开始,报名失败,请重新报名下一场比赛！"
*/

#define MIM_MATCH_0004					(MIM_MATCH+4)							//消息索引

/*
* "报名失败！"
*/

#define MIM_MATCH_0005					(MIM_MATCH+5)							//消息索引

/*
* "退赛成功，退还朋友圈分享免费报名一次！"
*/

#define MIM_MATCH_0006					(MIM_MATCH+6)							//消息索引

/*
* "退赛成功，退还报名费%s%s！"
*/

#define MIM_MATCH_0007					(MIM_MATCH+7)							//消息索引

/*
* "抱歉，由于本轮比赛已达到最大淘汰人数即将结束，复活失败！"
*/

#define MIM_MATCH_0008					(MIM_MATCH+8)							//消息索引

/*
* "抱歉，由于本场比赛未达到最小开赛人数，系统自动取消。您的报名费用将如数退还！"
*/

#define MIM_MATCH_0009					(MIM_MATCH+9)							//消息索引

/*
* "抱歉，您的比赛已被系统解散。报名费用将如数退还！"
*/

#define MIM_MATCH_0010					(MIM_MATCH+10)							//消息索引

/*
* "抱歉，本场比赛已被系统解散。请前往比赛中心报名其他比赛吧！"
*/

#define MIM_MATCH_0011					(MIM_MATCH+11)							//消息索引

/*
* "抱歉，您报名的比赛已经开始，请报名下一场比赛！"
*/

#define MIM_MATCH_0012					(MIM_MATCH+12)							//消息索引

/*
* "抱歉，您报名的比赛场次已满员,请报名下一场比赛！"
*/

#define MIM_MATCH_0013					(MIM_MATCH+13)							//消息索引

/*
* "您已经成功报名，不能重复报名！"
*/

#define MIM_MATCH_0014					(MIM_MATCH+14)							//消息索引

/*
* "对不起，您当前的状态不允许参加比赛！"
*/

#define MIM_MATCH_0015					(MIM_MATCH+15)							//消息索引

/*
* "比赛报名已于 %s%s:%s 截止！"
*/

#define MIM_MATCH_0016					(MIM_MATCH+16)							//消息索引

/*
* "抱歉，您的会员等级达不到报名条件，报名失败！"
*/

#define MIM_MATCH_0017					(MIM_MATCH+17)							//消息索引

/*
* "抱歉，您的经验值达不到报名条件，报名失败！"
*/

#define MIM_MATCH_0018					(MIM_MATCH+18)							//消息索引

/*
* "抱歉，您要退赛的场次与当前比赛不匹配，退赛失败！"
*/

#define MIM_MATCH_0019					(MIM_MATCH+19)							//消息索引

/*
* "抱歉，本场比赛已经结束，复活失败！"
*/

#define MIM_MATCH_0020					(MIM_MATCH+20)							//消息索引

/*
* "抱歉，由于系统原因新的比赛尚未开启，分享失败！"
*/

#define MIM_MATCH_0021					(MIM_MATCH+21)							//消息索引

/*
* "比赛已经开始,您可以回到比赛中心报名其他比赛！"
*/

#define MIM_MATCH_0022					(MIM_MATCH+22)							//消息索引

/*
* "抱歉，由于本场比赛未达到最小开赛人数，系统自动取消比赛！"
*/

#define MIM_MATCH_0023					(MIM_MATCH+23)							//消息索引

/*
* "系统正在努力为您匹配用户，请耐心等待！"
*/

#define MIM_MATCH_0024					(MIM_MATCH+24)							//消息索引

/*
* "恭喜您,本轮已达到淘汰人数,等其他桌完成后系统将进行晋级排名,请您耐心等待！"
*/

#define MIM_MATCH_0025					(MIM_MATCH+25)							//消息索引

/*
* "当前还有%s桌正在游戏中，请耐心等待！"
*/

#define MIM_MATCH_0026					(MIM_MATCH+26)							//消息索引

/*
* "恭喜您,您已玩满%s局游戏,等其他桌完成后系统将进行晋级排名,请您耐心等待！"
*/

#define MIM_MATCH_0027					(MIM_MATCH+27)							//消息索引

/*
* "恭喜您,您已玩满%s局游戏,等其他桌完成后系统将进行晋级排名,请您耐心等待！"
*/

#define MIM_MATCH_0028					(MIM_MATCH+28)							//消息索引

/*
* "恭喜您，您在本轮比赛中获得了第%s名，成功晋级到下轮比赛！"
*/

#define MIM_MATCH_0029					(MIM_MATCH+29)							//消息索引

/*
* "很遗憾，您当前的比赛积分为 %s，低于淘汰分 %s，已被淘汰！"
*/

#define MIM_MATCH_0030					(MIM_MATCH+30)							//消息索引

/*
* "很遗憾，您在本轮比赛中未能晋级，已被淘汰！"
*/

#define MIM_MATCH_0031					(MIM_MATCH+31)							//消息索引

/*
* "抱歉,您当前状态不具备复活条件！"
*/

#define MIM_MATCH_0032					(MIM_MATCH+32)							//消息索引

/*
* "抱歉，本轮比赛已经结束，复活失败！"
*/

#define MIM_MATCH_0033					(MIM_MATCH+33)							//消息索引

/*
* "抱歉，本场比赛您已复活%s次，复活失败！"
*/

#define MIM_MATCH_0034					(MIM_MATCH+34)							//消息索引

/*
* "抱歉，本轮比赛已达到最大淘汰人数即将结束，复活失败！"
*/

#define MIM_MATCH_0035					(MIM_MATCH+35)							//消息索引

/*
* "恭喜您，您在本轮比赛中获得了第%s名，成功晋级到下轮比赛！"
*/

#define MIM_MATCH_0036					(MIM_MATCH+36)							//消息索引

/*
* "很遗憾，您当前的比赛积分为 %I64d，低于淘汰分 %s，已被淘汰！"
*/

#define MIM_MATCH_0037					(MIM_MATCH+37)							//消息索引

/*
* "很遗憾，您在本轮比赛中未能晋级，已被淘汰！"
*/

#define MIM_MATCH_0038					(MIM_MATCH+38)							//消息索引

/*
* "退赛成功，退还比赛券%s张！"
*/

#define MIM_MATCH_0039					(MIM_MATCH+39)							//消息索引

/*
* "由于数据库异常，请稍后再次尝试！"
*/

#define MIM_MATCH_0040					(MIM_MATCH+40)							//消息索引

/*
* "由于您比赛券不足, 无法参与比赛！"
*/

#define MIM_MATCH_0041					(MIM_MATCH+41)							//消息索引

/*
* "本场比赛到此结束, 非常感谢您的参与！"
*/

#define MIM_MATCH_0042					(MIM_MATCH+42)							//消息索引

/*
* "未查询到相关比赛信息，比赛券购买失败！"
*/

#define MIM_MATCH_0043					(MIM_MATCH+43)							//消息索引

//////////////////////////////////////////////////////////////////////////
//约战模块

#define MIM_BATTLE						5000									//消息索引

/*
* "抱歉，系统没有查询到您进入的房间，请确认房间号再次尝试！"
*/

#define MIM_BATTLE_0001					(MIM_BATTLE+1)							//消息索引

/*
* "系统维护中，约战房间暂时停止服务。请在维护完成后重试！"
*/

#define MIM_BATTLE_0002					(MIM_BATTLE+2)							//消息索引

/*
* "房间创建失败！"
*/

#define MIM_BATTLE_0003					(MIM_BATTLE+3)							//消息索引

/*
* "抱歉，您创建的桌子数目已经达到上限，如有疑问，请联系客服！"
*/

#define MIM_BATTLE_0004					(MIM_BATTLE+4)							//消息索引

/*
* "抱歉，房间创建失败[%s]，请稍后再次尝试！"
*/

#define MIM_BATTLE_0005					(MIM_BATTLE+5)							//消息索引


/*
* "抱歉，茶社成员没有权限创建房间！"
*/

#define MIM_BATTLE_0008					(MIM_BATTLE+8)							//消息索引

/*
* "抱歉，系统未查询到您要修改的房间，请刷新后再次尝试！"
*/

#define MIM_BATTLE_0009					(MIM_BATTLE+9)							//消息索引

/*
* "抱歉，您不是该茶社的掌柜，没有权限修改房间配置！"
*/

#define MIM_BATTLE_0010					(MIM_BATTLE+10)							//消息索引

/*
* "房间修改失败！"
*/

#define MIM_BATTLE_0011					(MIM_BATTLE+11)							//消息索引
/*
* "抱歉，系统未查询到您要解散的房间，请刷新后再次尝试！"
*/

#define MIM_BATTLE_0012					(MIM_BATTLE+12)							//消息索引

/*
* "抱歉，系统没有查询到您进入的房间，请确认房间号再次尝试！"
*/

#define MIM_BATTLE_0013					(MIM_BATTLE+13)							//消息索引

/*
* "抱歉，您不是该房间的房主，没有权限解散！"
*/

#define MIM_BATTLE_0014					(MIM_BATTLE+14)							//消息索引

/*
* "约战玩法校验失败，请认真核对后再次尝试！"
*/

#define MIM_BATTLE_0015					(MIM_BATTLE+15)							//消息索引

/*
* "房间创建成功！"
*/

#define MIM_BATTLE_0016					(MIM_BATTLE+16)							//消息索引

/*
* "房间配置修改成功！"
*/

#define MIM_BATTLE_0017					(MIM_BATTLE+17)							//消息索引

/*
* "游戏已经开始，房间配置修改失败！"
*/

#define MIM_BATTLE_0018					(MIM_BATTLE+18)							//消息索引


//////////////////////////////////////////////////////////////////////////
//排位模块

#define MIM_RANKING						5500									//消息索引

/*
* "系统维护中，排位系统暂时停止服务。请在维护完成后重试！"
*/

#define MIM_RANKING_0001				(MIM_RANKING+1)							//消息索引

/*
* "您的爱心数量不足，无法加入排位游戏！"
*/

#define MIM_RANKING_0002				(MIM_RANKING+2)							//消息索引

/*
* "服务器繁忙，请稍后再次尝试！"
*/

#define MIM_RANKING_0003				(MIM_RANKING+3)							//消息索引

/*
* "您的报名费不足，无法加入排位游戏！"
*/

#define MIM_RANKING_0004				(MIM_RANKING+4)							//消息索引


//////////////////////////////////////////////////////////////////////////
//消息模块

//#define MIM_MESSAGE						6500									//消息索引

//////////////////////////////////////////////////////////////////////////
//游戏模块
#define MIM_GAME						10000									//消息索引
#define MIM_GAME_WINGO					10100									//消息索引
#define MIM_GAME_TEENPATTI				10200									//消息索引
#define MIM_GAME_DEALRUMMY				10300									//消息索引


//////////////////////////////////////////////////////////////////////////
//龙虎斗
#define MIM_GAME_DRAGON_TIGER			10400									//消息索引

/*
* "由于庄家%s强退，游戏提前结束！"
*/

#define MIM_GAME_DRAGON_TIGER_0001		(MIM_GAME_DRAGON_TIGER+1)				//消息索引

/*
* "庄家[%s]下庄成功!"
*/

#define MIM_GAME_DRAGON_TIGER_0002		(MIM_GAME_DRAGON_TIGER+2)				//消息索引

/*
* "请下一轮下注！"
*/

#define MIM_GAME_DRAGON_TIGER_0003		(MIM_GAME_DRAGON_TIGER+3)				//消息索引

/*
* "携带金币为 %s, 已下注金币为 %s, 当前下注金币为 %s, 下注失败!"
*/

#define MIM_GAME_DRAGON_TIGER_0004		(MIM_GAME_DRAGON_TIGER+4)				//消息索引

/*
* "个人限制为%s, 已下注金币为%s, 当前下注金币为%s, 下注失败!"
*/

#define MIM_GAME_DRAGON_TIGER_0005		(MIM_GAME_DRAGON_TIGER+5)				//消息索引

/*
* "你的金币不足以申请庄家，申请失败！"
*/

#define MIM_GAME_DRAGON_TIGER_0006		(MIM_GAME_DRAGON_TIGER+6)				//消息索引

/*
* "你已经申请了庄家，不需要再次申请！"
*/

#define MIM_GAME_DRAGON_TIGER_0007		(MIM_GAME_DRAGON_TIGER+7)				//消息索引

/*
* "申请成功，等待游戏结束后主动下庄!"
*/

#define MIM_GAME_DRAGON_TIGER_0008		(MIM_GAME_DRAGON_TIGER+8)				//消息索引


/*
* "[ %s ]分数少于(%s)，强行换庄!"
*/

#define MIM_GAME_DRAGON_TIGER_0009		(MIM_GAME_DRAGON_TIGER+9)				//消息索引


/*
* "[%s]做庄次数达到(%s)，强行换庄!"
*/

#define MIM_GAME_DRAGON_TIGER_0010		(MIM_GAME_DRAGON_TIGER+10)				//消息索引

/*
* "%s上庄了,让我们给他点颜色看看"
*/

#define MIM_GAME_DRAGON_TIGER_0011		(MIM_GAME_DRAGON_TIGER+11)				//消息索引

/*
* "%s上庄了,掌声欢迎"
*/

#define MIM_GAME_DRAGON_TIGER_0012		(MIM_GAME_DRAGON_TIGER+12)				//消息索引

/*
* "%s上庄了,大家尽情游戏吧"
*/

#define MIM_GAME_DRAGON_TIGER_0013		(MIM_GAME_DRAGON_TIGER+13)				//消息索引

//////////////////////////////////////////////////////////////////////////
//飞禽走兽

#define MIM_GAME_ANIMAL_BATTLE			10500									//消息索引

/*
* "你的金币不足以申请庄家，申请失败！"
*/

#define MIM_GAME_ANIMAL_BATTLE_0001		(MIM_GAME_ANIMAL_BATTLE+1)				//消息索引

/*
* "你已经申请了庄家，不需要再次申请！"
*/

#define MIM_GAME_ANIMAL_BATTLE_0002		(MIM_GAME_ANIMAL_BATTLE+2)				//消息索引

/*
* "游戏已经开始，不可以取消当庄！"
*/

#define MIM_GAME_ANIMAL_BATTLE_0003		(MIM_GAME_ANIMAL_BATTLE+3)				//消息索引

/*
* "[%s]的分数超过其他申请上庄玩家，获得%s次额外坐庄次数!"
*/

#define MIM_GAME_ANIMAL_BATTLE_0004		(MIM_GAME_ANIMAL_BATTLE+4)				//消息索引

/*
* "[%s]的分数超过[%s]，获得%s次额外坐庄次数!"
*/

#define MIM_GAME_ANIMAL_BATTLE_0005		(MIM_GAME_ANIMAL_BATTLE+5)				//消息索引

/*
* "[%s]的分数超过[%s]且超过其他申请上庄玩家，获得%s次额外坐庄次数!"
*/

#define MIM_GAME_ANIMAL_BATTLE_0006		(MIM_GAME_ANIMAL_BATTLE+6)				//消息索引

/*
* "[%s]分数少于(%s)，强行换庄!"
*/

#define MIM_GAME_ANIMAL_BATTLE_0007		(MIM_GAME_ANIMAL_BATTLE+7)				//消息索引

/*
* "[ %s ]做庄次数达到(%s)，强行换庄!"
*/

#define MIM_GAME_ANIMAL_BATTLE_0008		(MIM_GAME_ANIMAL_BATTLE+8)				//消息索引


/*
* "[ %s ]上庄了，%s"
*/

#define MIM_GAME_ANIMAL_BATTLE_0009		(MIM_GAME_ANIMAL_BATTLE+9)				//消息索引

/*
* "由于你的金币数少于坐庄必须金币数（%s）, 你自动下庄！"
*/

#define MIM_GAME_ANIMAL_BATTLE_0010		(MIM_GAME_ANIMAL_BATTLE+10)				//消息索引

//////////////////////////////////////////////////////////////////////////

#endif