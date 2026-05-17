# WonChat

基于 Qt5/C++ 的即时通讯系统，TCP 自定义二进制协议，MySQL 持久化。

## 功能

- 用户注册 / 登录（SHA256 密码哈希）
- 好友管理（查找、添加、删除）
- 一对一聊天（消息历史记录）
- 在线用户列表
- 文件共享（上传、下载、分享、重命名、删除、移动）
- 服务端线程池并发处理

## 技术栈

| 技术 | 说明 |
|------|------|
| Qt 5.14.2 | Widgets, Network, SQL |
| 通信协议 | TCP 自定义二进制 PDU |
| 数据库 | MySQL（参数化查询） |
| 并发 | QThreadPool（8 线程） |
| 密码 | 客户端 SHA256 哈希 |

## 构建

```bash
# 1. 初始化数据库
mysql -u root -p < schema.sql

# 2. 配置环境变量（或使用默认值）
#   DB_HOST  DB_PORT  DB_USER  DB_PASS  DB_NAME

# 3. 编译
#   用 Qt Creator 分别打开 Client/Client.pro 和 Server/Server.pro 构建
```

## 目录结构

```
├── common/         # 共享协议定义（PDU 结构、消息类型枚举）
├── Client/         # 客户端代码
├── Server/         # 服务端代码
├── schema.sql      # 数据库建表脚本
└── .env.example    # 环境变量模板
```
