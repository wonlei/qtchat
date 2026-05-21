# WonChat 简历项目描述

## 一句话简介
基于 Qt5/C++17 的即时通讯系统，自研 TLV 二进制协议，MySQL 持久化，支持好友管理、文件共享和离线消息。

## 6 条 Bullet Points

1. **自研应用层通信协议**：设计 TLV 帧格式（长度前缀 + 版本协商），支持新旧协议自动检测（0x80000000 魔数标记），定义 40 种消息类型和统一错误码体系，响应报文首部携带 uint16 错误码，客户端解析为中文提示

2. **安全加固**：PBKDF2-HMAC-SHA256 随机盐密码存储、文件路径穿越防护（canonical path 校验）、SQL 参数化查询、心跳保活 30 秒超时踢人

3. **服务端架构设计**：消息分发器（策略模式 + unordered_map 注册表）替代巨型 switch-case，QThreadPool 8 线程并发处理，QThreadStorage 每线程一个数据库连接避免锁竞争

4. **依赖注入去单例**：OperateDB / AppConfig 通过构造函数注入 MsgHandler → MyTcpSocket → MyTcpServer → Server，提升可测试性和代码质量

5. **工程化与测试**：Google Test 24 项单元测试（协议帧、密码哈希、分发器、路径安全），Python 端到端集成测试 32 项（覆盖注册→登录→好友→聊天→文件→离线消息全流程），GitHub Actions CI 自动编译+测试

6. **客户端状态机**：ConnectionState（Disconnected → Connecting → Connected → LoggedIn）四状态转换，关键操作入口状态校验，防止未登录越权操作

## 技术栈关键词
Qt 5.14, C++17, TCP, MySQL, PBKDF2, 连接池, 策略模式, Google Test, Python, GitHub Actions
