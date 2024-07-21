CREATE TABLE `user_counter` (
	`user_no` INT(10) NOT NULL,
	`type` INT(10) NOT NULL,
	`value` BIGINT(10) NOT NULL DEFAULT '0',
	`update_time` TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00',
	PRIMARY KEY (`user_no`, `type`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB
;
