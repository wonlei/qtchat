# WonChat

基于 Qt5/C++17 的即时通讯系统，自研 TLV 二进制协议，MySQL 持久化，PBKDF2 密码加盐。

## 功能

- 用户注册 / 登录（PBKDF2-HMAC-SHA256 加盐哈希）
- 好友管理（查找、添加、接受、删除、刷新）
- 一对一聊天（消息历史记录）
- 离线消息（接收方不在线时暂存，登录后推送）
- 在线用户列表（心跳保活，30s 超时）
- 文件管理（创建目录、上传、下载、分享、重命名、删除、移动）
- 路径穿越防护（路径安全校验）
- 统一错误码（15 种错误类型，客户端中文提示）
- 日志系统（按天轮转，线程 ID 标注）

## 技术栈

| 技术 | 说明 |
|------|------|
| Qt 5.14.2 | Widgets, Network, SQL, Core |
| C++17 | 智能指针、lambda、auto |
| 通信协议 | 自研 TLV Frame + 新旧格式自动检测 |
| 数据库 | MySQL 8.0（参数化查询，连接池） |
| 并发 | QThreadPool（8 线程）+ QThreadStorage |
| 密码 | PBKDF2-HMAC-SHA256（1000 次迭代，随机盐） |
| 构建 | qmake (Win) + CMake (CI/Docker) |
| 测试 | Google Test 单元测试 + Python 集成测试 |
| CI | GitHub Actions (Ubuntu 22.04 + Qt 5.15) |

## 快速开始

```bash
# 1. 初始化数据库
mysql -u root -p < schema.sql

# 2. 配置 Server/config.json（数据库连接信息）
# 3. 配置 Client/config.json（服务器地址）

# 4. 编译运行
# 用 Qt Creator 分别打开 Server/Server.pro 和 Client/Client.pro 构建
# 或使用 CMake:
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/qt5 -DBUILD_TESTS=ON
cmake --build build -j$(nproc)
```

## 测试

```bash
# 单元测试（24 项）
cd build && ctest --output-on-failure

# 集成测试（32 项，需服务端运行）
python tools/integration_test.py --host 127.0.0.1 --port 5000

# 压力测试
python tools/stress_test.py --clients 50 --rounds 10
```

## 项目结构

```
├── common/              # 共享协议与工具库
│   ├── protocol.h/cpp   # PDU 结构、消息类型、错误码
│   ├── packet.h/cpp     # TLV Frame 序列化/反序列化
│   ├── messagedispatcher.h/cpp  # 消息分发器（注册表模式）
│   ├── password.h/cpp   # PBKDF2 加盐哈希
│   ├── config.h/cpp     # JSON 配置管理
│   └── pathsafety.h/cpp # 路径穿越防护
├── Server/              # 服务端
│   ├── msghandler.cpp   # 消息处理（注册、登录、好友、聊天、文件）
│   ├── operatedb.cpp    # 数据库操作（连接池、CRUD）
│   ├── mytcpserver.cpp  # TCP 服务器（TLS 可选）
│   ├── mytcpsocket.cpp  # Socket 封装（心跳、加密）
│   ├── logmanager.cpp   # 日志系统（按天轮转）
│   └── config.json      # 服务端配置
├── Client/              # 客户端
│   ├── client.cpp       # 主窗口 + 状态机
│   ├── msghandle.cpp    # 客户端消息分发 + 错误码处理
│   ├── file.cpp         # 文件管理界面
│   └── config.json      # 客户端配置
├── tests/                # 单元测试
│   ├── test_frame.cpp
│   ├── test_password.cpp
│   ├── test_dispatcher.cpp
│   └── test_path.cpp
├── tools/                # 测试脚本
│   ├── integration_test.py
│   └── stress_test.py
├── docs/                 # 文档
│   ├── architecture.md   # 架构文档
│   └── protocol.md       # 协议文档
├── schema.sql            # 数据库建表脚本
└── .github/workflows/    # CI/CD
```

## 协议要点

- **帧格式**: `[total_len:4][version:1][msg_type:2][meta_len:4][payload_len:4][meta][payload]`
- 首字高位置位（0x80000000）区分新旧格式，向后兼容旧 PDU
- 40 种消息类型，15 种错误码
- 响应统一在 caData[0..1] 放置 uint16 ErrorCode

## 面试亮点

- 自研协议：TLV 帧格式 + 新旧协议自动检测 + Protobuf 对比方案
- 安全：PBKDF2 加盐、路径穿越防护、参数化查询、TLS 代码就绪
- 工程：消息分发器（策略模式）、依赖注入（去单例）、连接池、状态机
- 测试：Google Test 单元测试 + Python 端到端集成测试 + 压测
- 性能：8 线程并发，实测 437 ops/s（单机 20 客户端）
