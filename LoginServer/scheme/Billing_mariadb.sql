DROP DATABASE IF EXISTS `Billing`;
CREATE DATABASE `Billing` CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `Billing`;

CREATE TABLE `tbl_cashList`(
    `id` VARCHAR(7) NOT NULL,
    `name` VARCHAR(50) NOT NULL,
    `cost` CHAR(10) NOT NULL,
    CONSTRAINT `PK_tbl_cashList` PRIMARY KEY (`id`, `name`)
);

CREATE TABLE `tbl_CashLog`(
    `Serial` INT NOT NULL AUTO_INCREMENT,
    `Account` VARCHAR(20) NOT NULL,
    `Char` VARCHAR(30) NOT NULL,
    `ItemID` VARCHAR(7) NOT NULL,
    `Amount` INT NOT NULL,
    `Server` VARCHAR(32) NOT NULL,
    `UID` BIGINT NOT NULL,
    `Cost` INT NOT NULL,
    `Discount` DOUBLE NOT NULL,
    `Return` TINYINT NOT NULL,
    CONSTRAINT `PK_tbl_CashLog` PRIMARY KEY (`Serial`)
);

CREATE TABLE `tbl_catalog`(
    `idx` INT NOT NULL AUTO_INCREMENT,
    `tableCode` INT NOT NULL,
    `itemCode` VARCHAR(8) NOT NULL,
    `icon` INT NOT NULL,
    `name` VARCHAR(64) NOT NULL,
    `K` INT NOT NULL,
    `D` BIGINT NOT NULL,
    `U` INT NOT NULL,
    `T` INT NOT NULL,
    `Price` INT NOT NULL,
    CONSTRAINT `PK_tbl_catalog` PRIMARY KEY (`idx`)
);

CREATE TABLE `tbl_paylog`(
    `PayID` BIGINT NOT NULL,
    `UserID` VARCHAR(24) NULL,
    `sum` DOUBLE NOT NULL,
    `accept` TINYINT(1) NOT NULL DEFAULT 0,
    CONSTRAINT `PK_tbl_paylog` PRIMARY KEY (`PayID`)
);

CREATE TABLE `tbl_UserStatus`(
    `serial` INT NOT NULL AUTO_INCREMENT,
    `id` VARCHAR(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
    `Status` INT NOT NULL DEFAULT 1,
    `DTStartPrem` DATETIME NULL DEFAULT CURRENT_TIMESTAMP,
    `DTEndPrem` DATETIME NULL DEFAULT CURRENT_TIMESTAMP,
    `Cash` INT NOT NULL DEFAULT 0,
    CONSTRAINT `PK_tbl_UserStatus` PRIMARY KEY (`id`),
    CONSTRAINT `IX_tbl_UserStatus_serial` UNIQUE (`serial`)
);

DELIMITER $$

DROP PROCEDURE IF EXISTS `commit_pay`$$
CREATE PROCEDURE `commit_pay`(
    IN p_inv_id BIGINT,
    IN p_accid VARCHAR(24),
    IN p_summ INT,
    OUT p_rw CHAR(1)
)
proc: BEGIN
    DECLARE v_accept TINYINT DEFAULT NULL;
    DECLARE v_pay_id BIGINT DEFAULT NULL;

    SELECT `PayID`, `accept`
      INTO v_pay_id, v_accept
      FROM `tbl_paylog`
     WHERE `PayID` = p_inv_id
     LIMIT 1;

    UPDATE `tbl_paylog`
       SET `UserID` = p_accid
     WHERE `PayID` = p_inv_id;

    SET p_rw = 'A';

    IF v_accept = 0 AND v_pay_id IS NOT NULL THEN
        UPDATE `tbl_UserStatus`
           SET `Cash` = `Cash` + p_summ
         WHERE `id` = p_accid;

        UPDATE `tbl_paylog`
           SET `accept` = 1
         WHERE `PayID` = p_inv_id;

        SET p_rw = 'T';
        LEAVE proc;
    END IF;

    IF v_pay_id IS NULL THEN
        SET p_rw = 'F';
        LEAVE proc;
    END IF;

    IF v_accept IS NULL AND v_pay_id IS NOT NULL THEN
        SET p_rw = 'C';
        LEAVE proc;
    END IF;
END$$

DROP PROCEDURE IF EXISTS `prc_RFONLINE_AVG_EVENT`$$
CREATE PROCEDURE `prc_RFONLINE_AVG_EVENT`(
    OUT p_t_amount INT
)
BEGIN
    SET p_t_amount = 1000;
END$$

DROP PROCEDURE IF EXISTS `RF_ActivatePremium`$$
CREATE PROCEDURE `RF_ActivatePremium`(
    IN p_user_id VARCHAR(16),
    IN p_months TINYINT,
    OUT p_ret CHAR(1)
)
proc: BEGIN
    DECLARE v_serial INT DEFAULT 0;
    DECLARE v_plus_date DATETIME;

    SELECT `serial`
      INTO v_serial
      FROM `tbl_UserStatus`
     WHERE `id` = p_user_id
     LIMIT 1;

    IF v_serial IS NULL OR v_serial < 1 THEN
        SET p_ret = 'F';
        LEAVE proc;
    END IF;

    SET v_plus_date = DATE_ADD(NOW(), INTERVAL p_months MONTH);

    UPDATE `tbl_UserStatus`
       SET `Status` = 2,
           `DTStartPrem` = NOW(),
           `DTEndPrem` = v_plus_date
     WHERE `serial` = v_serial;

    IF ROW_COUNT() > 0 THEN
        SET p_ret = 'T';
    ELSE
        SET p_ret = 'F';
    END IF;
END$$

DROP PROCEDURE IF EXISTS `RF_ChargeBalance`$$
CREATE PROCEDURE `RF_ChargeBalance`(
    IN p_login VARCHAR(32),
    IN p_char VARCHAR(20),
    IN p_item_id VARCHAR(20),
    IN p_amount INT,
    IN p_server VARCHAR(20),
    IN p_uid BIGINT,
    IN p_cost INT,
    IN p_discount INT,
    OUT p_ret INT
)
proc: BEGIN
    DECLARE v_total_amount INT DEFAULT 0;
    DECLARE v_cash_balance INT DEFAULT NULL;

    INSERT INTO `tbl_CashLog` (`Account`, `Char`, `ItemID`, `Amount`, `Server`, `UID`, `Cost`, `Discount`, `Return`)
    VALUES (p_login, p_char, p_item_id, p_amount, p_server, p_uid, p_cost, p_discount, 0);

    IF p_login IS NULL OR RTRIM(p_login) = '' THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    IF p_cost IS NULL OR p_cost <= 0 THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    IF p_amount IS NULL OR p_amount <= 0 THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    IF p_discount IS NULL OR p_discount < 0 OR p_discount >= 100 THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    IF p_item_id IS NULL OR RTRIM(p_item_id) = '' THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    IF p_server IS NULL OR RTRIM(p_server) = '' THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    IF p_char IS NULL OR RTRIM(p_char) = '' THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    SELECT `Cash`
      INTO v_cash_balance
      FROM `tbl_UserStatus`
     WHERE `id` = p_login
     LIMIT 1;

    IF v_cash_balance IS NULL THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    IF p_discount = 0 THEN
        SET v_total_amount = p_cost * p_amount;
    ELSE
        SET v_total_amount = CAST((p_cost * p_amount) / p_discount AS SIGNED);
    END IF;

    IF v_total_amount > v_cash_balance THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    SET v_cash_balance = v_cash_balance - v_total_amount;

    UPDATE `tbl_UserStatus`
       SET `Cash` = v_cash_balance
     WHERE `id` = p_login;

    IF ROW_COUNT() = 0 THEN
        SET p_ret = 0;
        LEAVE proc;
    END IF;

    SET p_ret = 1;

    UPDATE `tbl_CashLog`
       SET `Return` = p_ret
     WHERE `Account` = p_login;
END$$

DROP PROCEDURE IF EXISTS `RF_CheckAccountStatus`$$
CREATE PROCEDURE `RF_CheckAccountStatus`(
    IN p_id VARCHAR(16),
    OUT p_status INT
)
BEGIN
    DECLARE v_end_dt DATETIME DEFAULT NULL;

    SELECT `DTEndPrem`
      INTO v_end_dt
      FROM `tbl_UserStatus`
     WHERE `id` = p_id
     LIMIT 1;

    IF v_end_dt IS NOT NULL AND v_end_dt > NOW() THEN
        SELECT `Status`
          INTO p_status
          FROM `tbl_UserStatus`
         WHERE `id` = p_id
         LIMIT 1;
    ELSE
        SET p_status = 1;

        UPDATE `tbl_UserStatus`
           SET `Status` = p_status
         WHERE `id` = p_id;
    END IF;
END$$

DROP PROCEDURE IF EXISTS `RF_CheckBalance`$$
CREATE PROCEDURE `RF_CheckBalance`(
    IN p_user_id VARCHAR(50),
    OUT p_amount INT
)
BEGIN
    SELECT `Cash`
      INTO p_amount
      FROM `tbl_UserStatus`
     WHERE `id` = p_user_id
     LIMIT 1;

    IF p_amount IS NULL THEN
        SET p_amount = 0;
    END IF;
END$$

DROP PROCEDURE IF EXISTS `RF_PING`$$
CREATE PROCEDURE `RF_PING`()
BEGIN
END$$

DROP PROCEDURE IF EXISTS `SP_RF_CHG_ITEM_GAMEON`$$
CREATE PROCEDURE `SP_RF_CHG_ITEM_GAMEON`(
    IN p_uid VARCHAR(32),
    IN p_charname VARCHAR(20),
    IN p_item_price INT,
    IN p_ccr_itemcode VARCHAR(7),
    IN p_world VARCHAR(20),
    IN p_ip_addr VARCHAR(16),
    OUT p_status INT,
    OUT p_amount INT
)
BEGIN
    SET p_amount = 10000;
    SET p_status = 0;
END$$

DELIMITER ;
