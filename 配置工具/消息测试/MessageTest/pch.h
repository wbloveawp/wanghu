// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"

//平台文件
#include "..\..\..\系统模块\全局定义\Platform.h"
#include "..\..\..\系统模块\全局定义\RightDefine.h"
#include "..\..\..\系统模块\全局定义\ServerRule.h"
#include "..\..\..\系统模块\全局定义\Module.h"

//网络命令
#include "..\..\..\系统模块\消息定义\CMD_Commom.h"
#include "..\..\..\系统模块\消息定义\CMD_GameServer.h"
#include "..\..\..\系统模块\消息定义\CMD_LogonServer.h"
#include "..\..\..\系统模块\消息定义\CMD_GateServer.h"
#include "..\..\..\系统模块\消息定义\CMD_MessageServer.h"

//进程命令
//#include "..\..\消息定义\IPC_GameFrame.h"

//类头文件
#include "..\..\..\系统模块\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\..\系统模块\客户端组件\调试组件\ClientDebugHead.h"
#include "..\..\..\系统模块\客户端组件\平台数据\PlatformDataHead.h"
#include "..\..\..\系统模块\客户端组件\界面控件\SkinControlsHead.h"
#include "..\..\..\系统模块\客户端组件\广场控件\PlazaControlsHead.h"
#include "..\..\..\系统模块\客户端组件\游戏控件\ClientControlsHead.h"

//组件文件
#include "..\..\..\系统模块\公共组件\进程通讯\ProcessTradeHead.h"
#include "..\..\..\系统模块\客户端组件\界面资源\SkinResourceHead.h"
//#include "..\...\系统模块\.\客户端组件\形象组件\AvatarControlHead.h"
#include "..\..\..\系统模块\客户端组件\服务组件\ClientServiceHead.h"
#include "..\..\..\系统模块\客户端组件\更新服务\UpdateServiceHead.h"
#include "..\..\..\系统模块\客户端组件\网络服务\NetworkServiceHead.h"
//////////////////////////////////////////////////////////////////////////
//链接代码

//平台库文件
#ifndef _DEBUG
#pragma comment (lib,"../../../系统模块/链接库/KernelEngine.lib")
#pragma comment (lib,"../../../系统模块/链接库/ServiceFrame.lib")
#pragma comment (lib,"../../../系统模块/链接库/CommonService.lib")
#pragma comment (lib,"../../../系统模块/链接库/ModuleManager.lib")
#pragma comment (lib,"../../../系统模块/链接库/ConfigService.lib")
#else
#pragma comment (lib,"../../../系统模块/链接库/KernelEngineD.lib")
#pragma comment (lib,"../../../系统模块/链接库/ServiceFrameD.lib")
#pragma comment (lib,"../../../系统模块/链接库/CommonServiceD.lib")
#pragma comment (lib,"../../../系统模块/链接库/ModuleManagerD.lib")
#pragma comment (lib,"../../../系统模块/链接库/ConfigServiceD.lib")
#endif

//第三方库
#ifndef _DEBUG
#pragma comment (lib,"../../../第三方库/redis/lib/release/redis.lib")	
#pragma comment (lib,"../../../第三方库/redis/lib/release/hiredis.lib")	
#pragma comment (lib,"../../../第三方库/protobuf/lib/release/libprotoc.lib")
#pragma comment (lib,"../../../第三方库/protobuf/lib/release/libprotobuf.lib")
#else
#pragma comment (lib,"../../../第三方库/redis/lib/debug/redis.lib")	
#pragma comment (lib,"../../../第三方库/redis/lib/debug/hiredis.lib")	
#pragma comment (lib,"../../../第三方库/protobuf/lib/debug/libprotoc.lib")
#pragma comment (lib,"../../../第三方库/protobuf/lib/debug/libprotobuf.lib")
#endif

//////////////////////////////////////////////////////////////////////////
#endif //PCH_H

