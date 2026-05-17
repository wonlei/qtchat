-- WonChat Database Schema
-- MySQL 5.7+

CREATE DATABASE IF NOT EXISTS wonchat
  DEFAULT CHARACTER SET utf8mb4
  COLLATE utf8mb4_unicode_ci;

USE wonchat;

-- 用户表
CREATE TABLE IF NOT EXISTS user_info (
    id      INT AUTO_INCREMENT PRIMARY KEY,
    name    VARCHAR(32)  NOT NULL UNIQUE,
    pwd     CHAR(32)     NOT NULL,            -- SHA256 原始 32 字节
    online  TINYINT(1)   NOT NULL DEFAULT 0
) ENGINE=InnoDB;

-- 好友关系表（双向关系存一条记录，约定 user_id < friend_id 或按添加方向）
CREATE TABLE IF NOT EXISTS friend (
    id          INT AUTO_INCREMENT PRIMARY KEY,
    user_id     INT NOT NULL,
    friend_id   INT NOT NULL,
    FOREIGN KEY (user_id)   REFERENCES user_info(id) ON DELETE CASCADE,
    FOREIGN KEY (friend_id) REFERENCES user_info(id) ON DELETE CASCADE,
    UNIQUE KEY uq_friendship (user_id, friend_id)
) ENGINE=InnoDB;

-- 聊天消息表
CREATE TABLE IF NOT EXISTS message (
    id          INT AUTO_INCREMENT PRIMARY KEY,
    sender      VARCHAR(32)   NOT NULL,
    receiver    VARCHAR(32)   NOT NULL,
    content     TEXT          NOT NULL,
    created_at  DATETIME      NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_sender_receiver (sender, receiver),
    INDEX idx_created_at (created_at)
) ENGINE=InnoDB;
