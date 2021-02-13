if not exist "C:\Program Files\7-Zip\" 7z1900-x64.exe
if not exist "C:\Program Files\MySQL\MySQL Connector C 6.1" mysql-connector-c-6.1.6-winx64.msi
if not exist "C:\OpenSSL-Win64" Win64OpenSSL-1_0_2c.exe
if not exist "C:\local" mkdir "C:\local"
if not exist "C:\local\curl-7.43.0-win64" "C:\Program Files\7-Zip\7z.exe" x curl-7.43.0-win64.zip -o"C:\local\"
if not exist boost_1_71_0.zip powershell -Command "Invoke-WebRequest https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.zip -OutFile boost_1_71_0.zip"
if not exist "C:\local\boost_1_71_0" "C:\Program Files\7-Zip\7z.exe" x boost_1_71_0.zip -o"C:\local\"
cd /d "C:\local\boost_1_71_0"
if not exist b2.exe call .\bootstrap.bat
.\b2