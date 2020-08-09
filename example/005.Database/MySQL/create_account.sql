-- create mysql account
USE mysql;
CREATE USER 'gamnet'@'%' identified by 'P@ssword!';
GRANT ALL PRIVILEGES ON *.* to 'gamnet'@'%';
CREATE USER 'gamnet'@'localhost' identified by 'P@ssword!';
GRANT ALL PRIVILEGES ON *.* to 'gamnet'@'localhost';
FLUSH PRIVILEGES;
