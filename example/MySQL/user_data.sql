CREATE DATABASE If NOT EXISTS `user_data` character set = utf8mb4 collate = utf8mb4_unicode_ci;

USE `user_data`;

CREATE TABLE IF NOT EXISTS `user_data` (
	`user_seq` 		INT 		NOT NULL,
	`user_name`		VARCHAR(16)	NOT NULL,
	`server_id`		INT UNSIGNED NOT NULL DEFAULT 0,
	`create_date` 	TIMESTAMP 	NOT NULL DEFAULT CURRENT_TIMESTAMP,
	`offline_date` 	TIMESTAMP 	NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY(`user_seq`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `user_item` (
	`user_seq` INT(11) NOT NULL COMMENT 'owner user seq',
	`item_seq` INT(11) NOT NULL COMMENT 'item unique identifier',
	`item_index` INT(11) NOT NULL,
	`item_count` INT(11) NOT NULL DEFAULT 1,
	`equip_part` INT(11) NOT NULL DEFAULT 0,
	`expire_date` TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00',
	`delete_date` TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00',
	`delete_yn` VARCHAR(1) NOT NULL DEFAULT 'N' COLLATE 'utf8mb4_unicode_ci',
	PRIMARY KEY (`user_seq`, `item_seq`),
	INDEX `user_item_index_1` (`user_seq`, `expire_date`, `delete_yn`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `user_counter` (
	`user_seq`  INT NOT NULL,
	`counter_type` INT NOT NULL,
	`counter_value` INT NOT NULL DEFAULT 0,
	`update_date` timestamp not null default '0000-00-00 00:00:00',
	PRIMARY KEY(`user_seq`, `counter_type`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `user_mail` (
	`mail_seq` INT NOT NULL AUTO_INCREMENT,
	`user_seq`  INT NOT NULL,
	`expire_date` timestamp not null default '0000-00-00 00:00:00',
	`delete_yn` varchar(1) not null default 'N',
	`mail_message` VARCHAR(128) NOT NULL DEFAULT '',
	`item_index` INT NOT NULL DEFAULT 0,
	`item_count` INT NOT NULL DEFAULT 0,
	`delete_date` timestamp not null default '0000-00-00 00:00:00',
	PRIMARY KEY(`mail_seq`),
	INDEX `user_mail_index_1` (`user_seq`, `expire_date`, `delete_yn`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `user_event` (
	`event_seq` INT NOT NULL AUTO_INCREMENT,
	`user_seq`  INT NOT NULL,
	`event_index` INT NOT NULL,
	`update_date` timestamp not null default '0000-00-00 00:00:00',
	PRIMARY KEY(`event_seq`),
	UNIQUE INDEX `user_event_unique_index_1` (`user_seq`, `event_index`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

