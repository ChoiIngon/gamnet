/* =================================================================
 *
 *  MySQL 스키마 install/uninstall(Window MySQL 8.0 기준)
 *
 * ================================================================= */
 
# MySQL 서버를 설치한 후 설치 패스를 확인한다.(C:\Program Files\MySQL\MySQL Server 8.0)

# MySQL 로컬 로그인 패스 설정하기

- 보안 문제로 인하여 커맨드 라인에서 mysql 명령을 바로 실행 할때마다 'Warning: using a password on the command line interface can be insecure.' 경고를 발생 한다. 이를 해결 하기 위해 로컬 로그인 패스를 만들어 두는 작업이 필요하다. (참고 : http://www.irgroup.org/mysql-login-path/) 
- 로컬 데이터 베이스에 편의상 모든 Administrative roles를 가지고 있는 계정을 추가한 후 아래와 같이 localserver sa config를 만들어 준다. (mysql_account.sql 중 gamnet 계정)

 C:\Program Files\MySQL\MySQL Server 8.0\bin> mysql_config_editor set --login-path=localserver_sa --host=localhost --user=gamnet --password
 C:\Program Files\MySQL\MySQL Server 8.0\bin> password : P@ssword!

# MySQL 데이터베이스 및 테이블 install하기

- install : $GIT_PATH\example\MySQL\install.bat
- uninstall : $GIT_PATH\example\MySQL\uninstall.bat
