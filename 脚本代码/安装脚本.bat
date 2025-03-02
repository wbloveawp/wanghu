@echo off
echo *************************************************************************
echo                              领航平台数据库一键安装脚本
echo 
echo 自动创建领航平台初始数据库。请按任意键继续...
echo *************************************************************************
pause
echo.
echo 正在清理数据库
Rem 数据库重置
Rem net stop mssqlserver
Rem set Ymd=%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%%TIME:~0,2%%TIME:~3,2%%TIME:~6,2% 
Rem md D:\数据库\领航平台备份\%ymd%
Rem XCOPY D:\数据库\领航平台\* D:\数据库\领航平台备份\%ymd% /s  /e
Rem if exist D:\数据库\领航平台 (echo y|cacls D:\数据库\领航平台 /p everyone:f >nul 2>nul &&rd /s /q D:\数据库\领航平台) 
md D:\数据库\领航平台
echo.
echo *************************************************************************echo 
echo 
echo   即将删除现有数据库，继续请按任意键, 退出请直接关闭
echo *************************************************************************
pause

Rem net start mssqlserver
set rootPath=数据库脚本\
osql -E -i "%rootPath%数据库删除.sql"

echo.
echo 建立数据库
set rootPath=数据库脚本\
osql -E -i "%rootPath%1_1_用户库脚本.sql"
osql -E -i "%rootPath%1_2_平台库脚本.sql"
osql -E -i "%rootPath%1_3_金币库脚本.sql"
osql -E -i "%rootPath%1_4_积分库脚本.sql"
osql -E -i "%rootPath%1_5_比赛库脚本.sql"
osql -E -i "%rootPath%1_6_排位库脚本.sql"
osql -E -i "%rootPath%1_7_约战库脚本.sql"
osql -E -i "%rootPath%1_8_约战记录库脚本.sql"
osql -E -i "%rootPath%1_9_记录库脚本.sql"
osql -E -i "%rootPath%1_10_银行库脚本.sql"
osql -E -i "%rootPath%1_11_后台库脚本.sql"
osql -E -i "%rootPath%1_12_本地库脚本.sql"

osql -E -i "%rootPath%2_1_用户库脚本.sql"
osql -E -i "%rootPath%2_2_平台库脚本.sql"
osql -E -i "%rootPath%2_3_金币库脚本.sql"
osql -E -i "%rootPath%2_4_积分库脚本.sql"
osql -E -i "%rootPath%2_5_比赛库脚本.sql"
osql -E -i "%rootPath%2_6_排位库脚本.sql"
osql -E -i "%rootPath%2_7_约战库脚本.sql"
osql -E -i "%rootPath%2_8_约战记录库脚本.sql"
osql -E -i "%rootPath%2_9_记录库脚本.sql"
osql -E -i "%rootPath%2_10_银行库脚本.sql"
osql -E -i "%rootPath%2_11_后台库脚本.sql"
osql -E -i "%rootPath%2_12_本地库脚本.sql"

echo.
echo 建立链接服务器
set rootPath=链接脚本\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ******************************创建游戏脚本********************************echo 

echo 函数定义
set rootPath=游戏脚本\函数定义
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 平台库
set rootPath=游戏脚本\存储过程\平台库\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 用户库
set rootPath=游戏脚本\存储过程\用户库\
osql -E -i "%rootPath%用户财富.sql"
osql -E -i "%rootPath%背包管理.sql"
osql -E -i "%rootPath%邮件管理.sql"
osql -E -i "%rootPath%绑定邀请码.sql"
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 金币库
set rootPath=游戏脚本\存储过程\金币库\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 积分库
set rootPath=游戏脚本\存储过程\积分库\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 比赛库
set rootPath=游戏脚本\存储过程\比赛库\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 约战库
set rootPath=游戏脚本\存储过程\约战库\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 排位库
set rootPath=游戏脚本\存储过程\排位库\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 记录库
set rootPath=游戏脚本\存储过程\记录库\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ******************************创建网站脚本********************************echo 

echo 前台
set rootPath=网站脚本\前台\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 后台
set rootPath=网站脚本\后台
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo 作业
set rootPath=网站脚本\新作业创建\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a


echo ******************************初始脚本********************************echo 

echo 插入初始数据
set rootPath=初始数据\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ******************************脚本创建完成********************************echo 

echo.
echo *************************************************************************
echo 安装脚本已经建立领航平台数据库
echo 无需生成游戏标识，请直接关闭
echo 需要生成游戏标识，请按任意键继续
echo *************************************************************************
pause

CLS
echo.
@echo 建立游戏标识
osql -E  -i "游戏标识.sql"

CLS
@echo off
echo *************************************************************************
echo.
echo 领航平台数据库安装脚本执行完毕
echo.
echo.
echo *************************************************************************

pause


