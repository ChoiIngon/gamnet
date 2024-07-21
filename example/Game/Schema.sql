CREATE DATABASE `game_global`;

USE `game_global`;

CREATE TABLE `account` (
	`account_id` VARCHAR(128) NOT NULL COLLATE 'utf8mb4_unicode_ci',
	`account_type` INT(10) NOT NULL DEFAULT '0' COMMENT '0:invalid, 1:dev, 2:google, 3:ios',
	`account_level` INT(10) NOT NULL DEFAULT '0' COMMENT '0:invalid, 1:normal, 2:admin',

	`user_no` BIGINT(19) NOT NULL AUTO_INCREMENT,
	`user_name` VARCHAR(16) NOT NULL COLLATE 'utf8mb4_unicode_ci',
	
	`shard_index` INT(10) NOT NULL DEFAULT '0',
	`create_date` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
	`delete_date` TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY (`account_id`, `account_type`) USING BTREE,
	UNIQUE INDEX `unique_key_1` (`user_name`) USING BTREE,
	UNIQUE INDEX `unique_index_1` (`user_no`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB
AUTO_INCREMENT=6
;

CREATE DATABASE `game_shard`;

USE `game_shard`;

CREATE TABLE `user_count` (
	`user_no` BIGINT(10) NOT NULL,
	`type` INT(10) NOT NULL,
	`value` BIGINT(10) NOT NULL DEFAULT '0',
	`update_time` TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY (`user_no`, `type`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB
;
