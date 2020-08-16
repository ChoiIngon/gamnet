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
	`item_seq` INT NOT NULL AUTO_INCREMENT COMMENT 'item unique identifier',
	`item_id`  INT NOT NULL,
	`user_seq` INT NOT NULL COMMENT 'owner user seq',
	`item_count` INT NOT NULL DEFAULT 1,
	`expire_date` TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00',
	`delete_date` TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00',
	`delete_yn`	varchar(1) not null default 'N',
	PRIMARY KEY(`item_seq`),
	INDEX `index_1` (`user_seq`, `expire_date`, `delete_yn`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `user_counter` (
	`counter_seq` INT NOT NULL AUTO_INCREMENT COMMENT 'counter unique identifier',
	`counter_id` INT NOT NULL,
	`user_seq`  INT NOT NULL,
	`counter` INT NOT NULL DEFAULT 0,
	`update_date` timestamp not null default '0000-00-00 00:00:00',
	PRIMARY KEY(`counter_seq`),
	INDEX `index_1` (`user_seq`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `user_mail` (
	`mail_seq` INT NOT NULL AUTO_INCREMENT,
	`user_seq`  INT NOT NULL,
	`expire_date` timestamp not null default '0000-00-00 00:00:00',
	`delete_yn` varchar(1) not null default 'N',
	`mail_message` VARCHAR(128) NOT NULL DEFAULT '',
	`item_id` INT NOT NULL DEFAULT 0,
	`item_count` INT NOT NULL DEFAULT 0,
	`delete_date` timestamp not null default '0000-00-00 00:00:00',

	PRIMARY KEY(`mail_seq`),
	INDEX `index_1` (`user_seq`, `expire_date`, `delete_yn`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `user_event` (
	`event_seq` INT NOT NULL AUTO_INCREMENT,
	`user_seq`  INT NOT NULL,
	`event_id` INT NOT NULL,
	`update_date` timestamp not null default '0000-00-00 00:00:00',
	PRIMARY KEY(`event_seq`),
	UNIQUE INDEX `unique_index_1` (`user_seq`, `event_id`)
)
DEFAULT CHARACTER SET=utf8mb4 COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

