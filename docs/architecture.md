# WonChat 架构文档

## 整体架构

```
┌─────────────┐          ┌──────────────────────────┐          ┌──────────┐
│  Client     │   TCP    │  Server                   │  MySQL   │  MySQL   │
│  (Qt GUI)   │◄────────►│  (Qt Network + ThreadPool)│◄────────►│  DB      │
│             │   Frame  │                           │  SQL     │          │
└─────────────┘  protocol └──────────────────────────┘          └──────────┘
```

### 客户端
- Qt Widgets GUI，单例模式（`Client`、`Index`）
- 异步 socket 通信（`QSslSocket`），支持可选 TLS
- 状态机：`Disconnected → Connecting → Connected → LoggedIn`
- 15 秒心跳保活

### 服务端
- `QTcpServer` 监听，`QThreadPool` 处理并发连接（8 线程）
- 每连接一个 `MyTcpSocket` + `MsgHandler`（通过依赖注入获得 `OperateDB` 引用）
- 数据库连接池：`QThreadStorage<QSqlDatabase*>` 实现每线程独立连接
- 服务端 `OperateDB` 由 `Server` 创建并注入到 `MyTcpServer` → `MyTcpSocket` → `MsgHandler`

---

## 线程模型

```
Main Thread                Worker Thread 1..N
─────────                  ────────────────
QTcpServer (accept)        MyTcpSocket::recvMsg()
  │                        MyTcpSocket::handleMsg()
  ▼                        MsgHandler (业务逻辑)
MyTcpServer::              OperateDB (数据库操作)
  incomingConnection()       │
  ├─ new MyTcpSocket         └─ db() connection (per-thread)
  ├─ new ClientTask
  └─ threadPool->start()
```

- 主线程：接受连接、UI 事件
- Worker 线程：消息解析、业务处理、数据库访问
- `QThreadStorage` 确保每个 worker 线程获得独立的 MySQL 连接

---

## 协议分层

```
应用层: PDU (uiType + caData[64] + caMsg[])  ← 业务消息封装
传输层: Frame [4B len|1B ver|2B type|4B meta_len|4B payload_len|meta|payload]
传输层: TCP / TLS (QSslSocket)
```

- 新旧协议自动检测：Frame 的 `total_len` 最高位为 1，旧 PDU 无此标记
- 新协议（Frame）向后兼容旧协议（PDU）
- 错误码统一放在 `caData[0..1]`，具体响应数据从 `caData[2]` 开始

---

## 消息类型（ENUM_MSG_TYPE）

当前 40+ 消息类型，按功能分组：

| 分组 | 消息类型 | 方向 |
|------|---------|------|
| 认证 | REGISTER, LOGIN | C→S→C |
| 好友 | ADD_FRIEND, DELETE_FRIEND, REFRESH_FRIEND | C→S→C |
| 聊天 | CHAT, CHAT_HISTORY | C→S→C |
| 文件 | CREATE_FILE, REFRESH_FILE, UPLOAD_FILE, SHARE_FILE 等 | C→S→C |
| 心跳 | HEARTBEAT | C→S→C |

---

## 关键数据流：登录

```
Client                    Server                      MySQL
  │                         │                          │
  ├─ LOGIN_REQUEST ────────►│ (Frame 反序列化)          │
  │                         ├─ MsgHandler::login()     │
  │                         │  ├─ OperateDB::handleLogin()──► SELECT pwd,salt
  │                         │  │  ◄── stored hash + salt
  │                         │  ├─ PBKDF2(client_pwd, salt) ?= stored_hash
  │                         │  └─ UPDATE online=1
  │                         │                          │
  │◄── LOGIN_RESPOND ───────┤ (Frame 序列化)            │
  │  caData[0..1]=ERR_NONE  │                          │
  │  caData[2]=true         │                          │
  │                         │                          │
  ├─ 状态 → LoggedIn        │                          │
  ├─ 15s 心跳开始            │                          │
```

---

## 面试话术要点

1. **协议设计**："我设计了一个自研的 TLV 风格二进制协议，15 字节定长头 + 变长 meta/payload，通过最高位标记与旧格式自动区分，支持向前兼容。"

2. **安全**："密码存储用了 PBKDF2-HMAC-SHA256 + 16 字节随机盐，1000 次迭代，防止彩虹表攻击。路径操作做了沙箱检查防止目录穿越。"

3. **线程安全**："服务端用 QThreadStorage 实现 per-thread 数据库连接池，配合 QThreadPool 的 worker 线程，避免了全局锁竞争。"

4. **架构**："用依赖注入替代单例模式——OperateDB 由 Server 创建，通过 MyTcpServer → MyTcpSocket → MsgHandler 逐级注入。这样 MsgHandler 不依赖全局状态，方便单元测试。"

5. **工程化**："JSON 配置管理、结构化日志系统（按天轮转）、CMake 构建、Docker 一键部署、GitHub Actions CI。"

6. **错误处理**："统一的 ErrorCode 枚举，服务端所有响应报文前 2 字节携带错误码，客户端统一解析展示中文提示，消除了裸 exit(1) 的崩溃风险。"

---

## 技术栈总览

| 层面 | 技术 |
|------|------|
| 语言 | C++17 |
| GUI | Qt 5.14 Widgets |
| 网络 | QTcpServer / QSslSocket |
| 数据库 | MySQL 5.7，QSqlDatabase |
| 密码学 | PBKDF2-HMAC-SHA256 |
| 构建 | qmake + CMake |
| 部署 | Docker Compose |
| CI | GitHub Actions |
