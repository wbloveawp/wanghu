
REM /////////////////////////////////////////////////////////////
REM
REM 游戏组件工程编译处理
REM
REM /////////////////////////////////////////////////////////////

DEL 编译日志.txt
SET PATH=PATH;"C:\Program Files (x86)\Microsoft Visual Studio .NET 2003\Common7\IDE"

copy /y res\奉化图标.ico res\GamePlaza.ico
"Devenv" /ReBuild Release_Unicode /out 编译日志.txt "GamePlaza.sln"
rename  ..\..\发布组件\客户端组件\Release\Unicode\GamePlaza.exe GamePlaza-奉化.exe

copy /y res\沭阳图标.ico res\gameplaza.ico
"Devenv" /ReBuild Release_Unicode /out 编译日志.txt "GamePlaza.sln"
rename  ..\..\发布组件\客户端组件\Release\Unicode\GamePlaza.exe GamePlaza-沭阳.exe

copy /y res\泗洪图标.ico res\gameplaza.ico
"Devenv" /ReBuild Release_Unicode /out 编译日志.txt "GamePlaza.sln"
rename  ..\..\发布组件\客户端组件\Release\Unicode\GamePlaza.exe GamePlaza-泗洪.exe

copy /y res\泗阳图标.ico res\gameplaza.ico
"Devenv" /ReBuild Release_Unicode /out 编译日志.txt "GamePlaza.sln"
rename  ..\..\发布组件\客户端组件\Release\Unicode\GamePlaza.exe GamePlaza-泗阳.exe

copy /y res\宿迁图标.ico res\gameplaza.ico
"Devenv" /ReBuild Release_Unicode /out 编译日志.txt "GamePlaza.sln"
rename  ..\..\发布组件\客户端组件\Release\Unicode\GamePlaza.exe GamePlaza-宿迁.exe

copy /y res\盐城图标.ico res\gameplaza.ico
"Devenv" /ReBuild Release_Unicode /out 编译日志.txt "GamePlaza.sln"
rename  ..\..\发布组件\客户端组件\Release\Unicode\GamePlaza.exe GamePlaza-盐城.exe

copy /y res\网狐图标.ico res\gameplaza.ico
"Devenv" /ReBuild Release_Unicode /out 编译日志.txt "GamePlaza.sln"
rename  ..\..\发布组件\客户端组件\Release\Unicode\GamePlaza.exe GamePlaza-网狐.exe
