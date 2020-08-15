-- create database & tables for test

CREATE DATABASE If NOT EXISTS account character set = utf8mb4 collate = utf8mb4_unicode_ci;

USE account;
CREATE TABLE IF NOT EXISTS `account` (
	`user_seq` 		BIGINT 		 	NOT NULL AUTO_INCREMENT,
	`user_name`		VARCHAR(16)		NOT NULL,
	`account_id` 	VARCHAR(128)	NOT NULL,
	`account_type` 	INT 			NOT NULL DEFAULT 0 COMMENT '0:invalid, 1:dev, 2:google, 3:ios',
	`account_level`	INT				NOT NULL DEFAULT 0 COMMENT '0:invalid, 1:normal, 2:admin',
	`account_state`	INT				NOT NULL DEFAULT 0 COMMENT '0:invalid, 1:normal, 2:penalty, 3:on delete, 4:deleted',
	`shard_index` 	INT 			NOT NULL DEFAULT 0,
	`create_date` 	TIMESTAMP 		NOT NULL DEFAULT CURRENT_TIMESTAMP,
	`delete_date` 	TIMESTAMP 		NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY(`user_seq`),
	UNIQUE KEY `unique_key_1` (`user_name`),
	UNIQUE INDEX `unique_index_1` (`account_id`, `account_type`)
) 
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

DELIMITER //
DROP PROCEDURE IF EXISTS `sp_account_delete`;
CREATE PROCEDURE `sp_account_delete`(
	IN `p_account_id` VARCHAR(128),
	IN `p_account_type` INT(10)
)
BEGIN
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        SHOW ERRORS LIMIT 1;
        ROLLBACK;
    END;
    
    DECLARE EXIT HANDLER FOR SQLWARNING
    BEGIN
        SHOW WARNINGS LIMIT 1;
        ROLLBACK;
    END;
    
    START TRANSACTION;
		IF (SELECT COUNT(user_seq) FROM account WHERE account_id=p_account_id AND account_type=p_account_type) <= 0 THEN
			SIGNAL SQLSTATE '02000' SET message_text = 'not_found_account_id', mysql_errno=1329;
			-- https://dev.mysql.com/doc/refman/5.7/en/signal.html
		END IF;
        
        UPDATE account SET account_state=3, delete_date=NOW() WHERE account_id=p_account_id AND account_type=p_account_type;
    COMMIT;
    
    select 'Error' Level, '0' Code, 'OK' Message;
END//
DELIMITER ;

