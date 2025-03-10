#ifndef CONTROL_PACKET_HEAD_FILE
#define CONTROL_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//事件结果
#define ER_FAILURE					0									//失败标志
#define ER_SUCCESS					1									//成功标志

//////////////////////////////////////////////////////////////////////////
//界面数据包

#define UI_STORE_RESULT				1									//存储结果
#define UI_CENTER_RESULT			2									//中心结果
#define UI_SERVICE_RESULT			3									//服务结果
#define UI_SERVICE_CONFIG_RESULT	5									//配置结果
#define UI_SERVICE_WILLSTOP_RESULT	6									//预停止结果

//////////////////////////////////////////////////////////////////////////

//控制结果
struct CP_ControlResult
{
	BYTE							cbSuccess;							//成功标志
};

//////////////////////////////////////////////////////////////////////////
//控制数据包

#define CT_CONNECT_STORE			100									//连接存储
#define CT_CONNECT_CENTER			101									//连接中心
#define CT_CONNECT_SERVICE			102									//连接约战
#define CT_DEBUG_SERVICE			103									//调试服务
#define CT_SERVICE_WILLSTOP			104									//服务预停止
#define CT_LOAD_SERVICE_CONFIG		105									//加载配置
	

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#endif