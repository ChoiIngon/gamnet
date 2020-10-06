SET Year=%DATE:~0,4%
SET Month=%DATE:~5,2%
SET Day=%DATE:~8,2%
SET Hour=%TIME:~0,2%
IF "%Hour:~0,1%"==" " (SET Hour=0%Hour:~1%)
SET Minute=%TIME:~3,2%
SET Second=%TIME:~6,2%
SET TimeStamp=%DATE% %TIME%


SET MySqlPath="C:\Program Files\MySQL\MySQL Server 8.0\bin\mysql.exe"

@echo off
setlocal
chcp 65001

set WorkDirectory=%~dp0

echo start timestamp %TimeStamp%
echo .

cd /d %WorkDirectory%

set MySqlCommand=%MySqlPath% --login-path=localserver_sa --host=127.0.0.1 -P3306 ^<

%MySqlCommand% account.sql
if not "%ERRORLEVEL%" == "0" goto Fail

%MySqlCommand% user_data.sql
if not "%ERRORLEVEL%" == "0" goto Fail

echo .
echo end timestamp  %TimeStamp%

:Finish
ECHO Sucess Finish 작업을 모두 마쳤습니다.
GOTO Exit

:Fail
COLOR 4f
ECHO ERRORLEVEL " % ERRORLEVEL % " Error 에러가 발생하였습니다.
PAUSE > nul
GOTO Exit

:Exit
ECHO =================================================

ENDLOCAL
COLOR
TIMEOUT 10
