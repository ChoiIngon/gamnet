-- create database & tables for test

CREATE DATABASE If NOT EXISTS gamnet;

USE gamnet;
DROP TABLE IF EXISTS user_data;
CREATE TABLE `user_data` (
	`user_seq` INT NOT NULL AUTO_INCREMENT,
	`user_id` VARCHAR(32) NOT NULL DEFAULT '',
	PRIMARY KEY(`user_seq`),
	UNIQUE INDEX `UNIQUE_IDX` (`user_id`)
);

DROP TABLE IF EXISTS item_data;
CREATE TABLE `item_data` (
	`item_seq` INT NOT NULL AUTO_INCREMENT COMMENT 'item unique identifier',
	`item_id`  INT NOT NULL,
	`user_seq` INT NOT NULL COMMENT 'owner user seq',
	`item_count` INT NOT NULL DEFAULT 1,
	PRIMARY KEY(`item_seq`)
);

DROP TABLE IF EXISTS user_counter;
CREATE TABLE `user_counter` (
	`counter_seq` INT NOT NULL AUTO_INCREMENT COMMENT 'counter unique identifier',
	`counter_id` INT NOT NULL,
	`user_seq`  INT NOT NULL,
	`counter` INT NOT NULL DEFAULT 0,
	PRIMARY KEY(`counter_seq`),
	UNIQUE INDEX `UNIQUE_IDX` (`user_seq`, `counter_id`)
);