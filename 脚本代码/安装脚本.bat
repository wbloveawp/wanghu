@echo off
echo *************************************************************************
echo                              �캽ƽ̨���ݿ�һ����װ�ű�
echo 
echo �Զ������캽ƽ̨��ʼ���ݿ⡣�밴���������...
echo *************************************************************************
pause
echo.
echo �����������ݿ�
Rem ���ݿ�����
Rem net stop mssqlserver
Rem set Ymd=%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%%TIME:~0,2%%TIME:~3,2%%TIME:~6,2% 
Rem md D:\���ݿ�\�캽ƽ̨����\%ymd%
Rem XCOPY D:\���ݿ�\�캽ƽ̨\* D:\���ݿ�\�캽ƽ̨����\%ymd% /s  /e
Rem if exist D:\���ݿ�\�캽ƽ̨ (echo y|cacls D:\���ݿ�\�캽ƽ̨ /p everyone:f >nul 2>nul &&rd /s /q D:\���ݿ�\�캽ƽ̨) 
md D:\���ݿ�\�캽ƽ̨
echo.
echo *************************************************************************echo 
echo 
echo   ����ɾ���������ݿ⣬�����밴�����, �˳���ֱ�ӹر�
echo *************************************************************************
pause

Rem net start mssqlserver
set rootPath=���ݿ�ű�\
osql -E -i "%rootPath%���ݿ�ɾ��.sql"

echo.
echo �������ݿ�
set rootPath=���ݿ�ű�\
osql -E -i "%rootPath%1_1_�û���ű�.sql"
osql -E -i "%rootPath%1_2_ƽ̨��ű�.sql"
osql -E -i "%rootPath%1_3_��ҿ�ű�.sql"
osql -E -i "%rootPath%1_4_���ֿ�ű�.sql"
osql -E -i "%rootPath%1_5_������ű�.sql"
osql -E -i "%rootPath%1_6_��λ��ű�.sql"
osql -E -i "%rootPath%1_7_Լս��ű�.sql"
osql -E -i "%rootPath%1_8_Լս��¼��ű�.sql"
osql -E -i "%rootPath%1_9_��¼��ű�.sql"
osql -E -i "%rootPath%1_10_���п�ű�.sql"
osql -E -i "%rootPath%1_11_��̨��ű�.sql"
osql -E -i "%rootPath%1_12_���ؿ�ű�.sql"

osql -E -i "%rootPath%2_1_�û���ű�.sql"
osql -E -i "%rootPath%2_2_ƽ̨��ű�.sql"
osql -E -i "%rootPath%2_3_��ҿ�ű�.sql"
osql -E -i "%rootPath%2_4_���ֿ�ű�.sql"
osql -E -i "%rootPath%2_5_������ű�.sql"
osql -E -i "%rootPath%2_6_��λ��ű�.sql"
osql -E -i "%rootPath%2_7_Լս��ű�.sql"
osql -E -i "%rootPath%2_8_Լս��¼��ű�.sql"
osql -E -i "%rootPath%2_9_��¼��ű�.sql"
osql -E -i "%rootPath%2_10_���п�ű�.sql"
osql -E -i "%rootPath%2_11_��̨��ű�.sql"
osql -E -i "%rootPath%2_12_���ؿ�ű�.sql"

echo.
echo �������ӷ�����
set rootPath=���ӽű�\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ******************************������Ϸ�ű�********************************echo 

echo ��������
set rootPath=��Ϸ�ű�\��������
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ƽ̨��
set rootPath=��Ϸ�ű�\�洢����\ƽ̨��\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo �û���
set rootPath=��Ϸ�ű�\�洢����\�û���\
osql -E -i "%rootPath%�û��Ƹ�.sql"
osql -E -i "%rootPath%��������.sql"
osql -E -i "%rootPath%�ʼ�����.sql"
osql -E -i "%rootPath%��������.sql"
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ��ҿ�
set rootPath=��Ϸ�ű�\�洢����\��ҿ�\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ���ֿ�
set rootPath=��Ϸ�ű�\�洢����\���ֿ�\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ������
set rootPath=��Ϸ�ű�\�洢����\������\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo Լս��
set rootPath=��Ϸ�ű�\�洢����\Լս��\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ��λ��
set rootPath=��Ϸ�ű�\�洢����\��λ��\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ��¼��
set rootPath=��Ϸ�ű�\�洢����\��¼��\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ******************************������վ�ű�********************************echo 

echo ǰ̨
set rootPath=��վ�ű�\ǰ̨\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ��̨
set rootPath=��վ�ű�\��̨
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ��ҵ
set rootPath=��վ�ű�\����ҵ����\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a


echo ******************************��ʼ�ű�********************************echo 

echo �����ʼ����
set rootPath=��ʼ����\
for /r %rootPath% %%a in (*.sql) do osql -E  -i %%a

echo ******************************�ű��������********************************echo 

echo.
echo *************************************************************************
echo ��װ�ű��Ѿ������캽ƽ̨���ݿ�
echo ����������Ϸ��ʶ����ֱ�ӹر�
echo ��Ҫ������Ϸ��ʶ���밴���������
echo *************************************************************************
pause

CLS
echo.
@echo ������Ϸ��ʶ
osql -E  -i "��Ϸ��ʶ.sql"

CLS
@echo off
echo *************************************************************************
echo.
echo �캽ƽ̨���ݿⰲװ�ű�ִ�����
echo.
echo.
echo *************************************************************************

pause


