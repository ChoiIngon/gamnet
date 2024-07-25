-- create database & tables for test

CREATE DATABASE If NOT EXISTS `GameGlobal` character set = utf8mb4 collate = utf8mb4_unicode_ci;

USE `GameGlobal`;

CREATE TABLE IF NOT EXISTS `Account` (
	`account_id` 	VARCHAR(128)	NOT NULL,
	`account_type` 	INT 			NOT NULL DEFAULT 0 COMMENT '0:invalid, 1:dev, 2:google, 3:ios',
	`account_level`	INT				NOT NULL DEFAULT 0 COMMENT '0:invalid, 1:normal, 2:admin',
	`account_state`	INT				NOT NULL DEFAULT 0 COMMENT '0:invalid, 1:normal, 2:penalty, 3:on delete, 4:deleted',
	`user_no` 		BIGINT 		 	NOT NULL AUTO_INCREMENT,
	`user_name`		VARCHAR(16)		NOT NULL,
	`shard_index` 	INT 			NOT NULL DEFAULT 0,
	`delete_time`	DATETIME		CURRENT_TIMESTAMP,
	PRIMARY KEY(`account_id`, `account_type`),
	UNIQUE KEY `unique_user_name` (`user_name`),
	UNIQUE INDEX `index_user_no` (`user_no`)
) 
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

DELIMITER $$
DROP PROCEDURE IF EXISTS `SP_ACCOUNT_DELETE`;
CREATE PROCEDURE `SP_ACCOUNT_DELETE`(
	IN `p_account_id` VARCHAR(128),
	IN `p_account_type` INT(10),
	IN `p_delete_day` INT(10)
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
		IF (SELECT COUNT(`user_no`) FROM Account WHERE `account_id` = p_account_id AND `account_type` = p_account_type) <= 0 THEN
			SIGNAL SQLSTATE '02000' SET message_text = 'not_found_account_id', mysql_errno=1329;
			-- https://dev.mysql.com/doc/refman/5.7/en/signal.html
		END IF;
        
        UPDATE Account SET account_state=3, delete_time=DATE_ADD(NOW(), INTERVAL p_delete_day DAY) WHERE `account_id`=p_account_id AND `account_type`=p_account_type;
    COMMIT;
    
    select 'Error' Level, '0' Code, 'OK' Message;
END $$
DELIMITER ;

