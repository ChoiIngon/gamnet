-- create database & tables for test

CREATE DATABASE If NOT EXISTS `game_global` character set = utf8mb4 collate = utf8mb4_unicode_ci;

USE `game_global`;

CREATE TABLE IF NOT EXISTS `Account` (
	`user_no` 		BIGINT 		 	NOT NULL AUTO_INCREMENT,
	`account_id` 	VARCHAR(128)	NOT NULL,
	`account_type` 	INT 			NOT NULL DEFAULT 0 COMMENT '0:invalid, 1:dev, 2:google, 3:ios',
	`account_level`	INT				NOT NULL DEFAULT 1 COMMENT '0:invalid, 1:normal, 2:admin',
	`account_state`	INT				NOT NULL DEFAULT 1 COMMENT '0:invalid, 1:normal, 2:penalty, 3:on delete, 4:deleted',
	`shard_index` 	INT 			NOT NULL DEFAULT 0,
	`create_time`	DATETIME		NOT NULL DEFAULT CURRENT_TIMESTAMP,
	`delete_time`	DATETIME		NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY(`account_id`, `account_type`),
	UNIQUE KEY `unique_user_no` (`user_no`)
) 
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

CREATE DATABASE If NOT EXISTS `game_shard` character set = utf8mb4 collate = utf8mb4_unicode_ci;

USE `game_shard`;

DROP TABLE IF EXISTS `UserData`;
CREATE TABLE IF NOT EXISTS `UserData` (
	`user_no` 		BIGINT 		NOT NULL,
	`user_name`		VARCHAR(16)	NOT NULL,
	`server_id`		INT 		NOT NULL DEFAULT '0',
	`offline_time` 	TIMESTAMP 	NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY(`user_no`),
	UNIQUE KEY `unique_user_name` (`user_name`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `UserCounter`;
CREATE TABLE IF NOT EXISTS `UserCounter` (
	`user_no` 		BIGINT 		NOT NULL,
	`counter_type`	INT		 	NOT NULL,
	`counter_value` BIGINT		NOT NULL DEFAULT '0',
	`update_time` 	TIMESTAMP 	NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY (`user_no`, `counter_type`),
	INDEX `index_user_no` (`user_no`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `UserItem`;
CREATE TABLE IF NOT EXISTS `UserItem` (
	`user_no`		INT NOT NULL COMMENT 'owner user seq',
	`item_no` 		INT NOT NULL COMMENT 'item unique identifier',
	`item_index` 	INT NOT NULL,
	`item_count` 	INT NOT NULL DEFAULT 1,
	`equip_part` 	INT NOT NULL DEFAULT 0,
	`delete_yn` VARCHAR(1) NOT NULL DEFAULT 'N' COLLATE 'utf8mb4_unicode_ci',
	PRIMARY KEY (`user_no`, `item_no`),
	INDEX `index_user_no` (`user_no`, `delete_yn`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;