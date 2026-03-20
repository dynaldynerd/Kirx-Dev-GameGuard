CREATE DATABASE IF NOT EXISTS `Billing`;
USE `Billing`;

CREATE TABLE IF NOT EXISTS `tbl_UserStatus`(
    `id` VARCHAR(16) NOT NULL,
    `Status` INT NOT NULL DEFAULT 1,
    `DTEndPrem` DATETIME NULL,
    CONSTRAINT `PK_tbl_UserStatus` PRIMARY KEY (`id`)
);
