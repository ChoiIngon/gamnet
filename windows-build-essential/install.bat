if not exist "C:\Program Files\7-Zip\" 7z1900-x64.exe
if not exist "C:\Program Files\MySQL\MySQL Connector C 6.1" mysql-connector-c-6.1.6-winx64.msi
if not exist "C:\OpenSSL-Win64" Win64OpenSSL-1_0_2c.exe
if not exist "C:\local" mkdir "C:\local"
if not exist "C:\local\curl-7.43.0-win64" "C:\Program Files\7-Zip\7z.exe" x curl-7.43.0-win64.zip -o"C:\local\"
setx path "C:\local\curl-7.43.0-win64\dlls;C:\Program Files\MySQL\MySQL Connector C 6.1\lib;%PATH%;" -m

set curdir=%cd%

SET INSTALL_PATH=C:\local
SET BOOST_VERSION=1.85.0

ECHO install directory: %INSTALL_PATH%
ECHO boost version: %BOOST_VERSION%

SET BOOST_PATH=boost_%BOOST_VERSION:.=_%

pushd .
if not exist .\%BOOST_PATH%.zip powershell -Command "Invoke-WebRequest https://boostorg.jfrog.io/artifactory/main/release/%BOOST_VERSION%/source/%BOOST_PATH%.zip -OutFile %BOOST_PATH%.zip"
if not exist %INSTALL_PATH%\%BOOST_PATH% "tar" -xf .\%BOOST_PATH%.zip -C %INSTALL_PATH%
cd /d "%INSTALL_PATH%\%BOOST_PATH%"
if not exist b2.exe call .\bootstrap.bat
.\b2
popd
