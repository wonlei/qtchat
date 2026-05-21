# WonChat Protocol Specification v2

## Overview

WonChat uses a custom binary protocol over TCP. Version 2 introduced a TLV-inspired frame format with version negotiation, variable-length metadata, and length-prefix framing to replace the fixed-size struct serialization of v1.

## Frame Format (v2)

```
Byte offset  Size    Field         Description
─────────────────────────────────────────────────────
 0           4       total_len     Total frame size (MSB = 1 marks v2)
 4           1       version       Protocol version (= 2)
 5           2       msg_type      Message type (ENUM_MSG_TYPE)
 7           4       meta_len      Metadata section length
11           4       payload_len   Payload section length
15          N        meta          Metadata (up to 4 GB)
15+N        M        payload       Message body
─────────────────────────────────────────────────────
HEADER = 15 bytes
```

**Format detection**: v2 frames set the MSB of `total_len` (bit 31). Legacy v1 PDUs have `total_len < 2GB`, so the MSB is never set. Receivers read the first 4 bytes and check this bit.

## Message Types

40 message types organized as request/response pairs:

| Type | Value | Description |
|------|-------|-------------|
| `REGISTER_REQUEST` | 1 | User registration |
| `REGISTER_RESPOND` | 2 | Registration result |
| `LOGIN_REQUEST` | 3 | User login |
| `LOGIN_RESPOND` | 4 | Login result |
| `FIND_USER_REQUEST` | 5 | Search for user |
| `FIND_USER_RESPOND` | 6 | Search result (0=offline,1=online,2=not found) |
| `ONLINE_USER_REQUEST` | 7 | List online users |
| `ONLINE_USER_RESPOND` | 8 | Online user name list |
| `ADD_FRIEND_REQUEST` | 9 | Friend request |
| `ADD_FRIEND_RESPOND` | 10 | Friend request ack |
| `ADD_FRIEND_ACCEPT_REQUEST` | 11 | Accept friend request |
| `ADD_FRIEND_ACCEPT_RESPOND` | 12 | Accept result |
| `REFRESH_FRIEND_REQUEST` | 13 | Refresh friend list |
| `REFRESH_FRIEND_RESPOND` | 14 | Friend list data |
| `DELETE_FRIEND_REQUEST` | 15 | Remove friend |
| `DELETE_FRIEND_RESPOND` | 16 | Remove result |
| `CHAT_REQUEST` | 17 | Send chat message |
| `CHAT_RESPOND` | 18 | - (reserved) |
| `CHAT_HISTORY_REQUEST` | 19 | Request chat history |
| `CHAT_HISTORY_RESPOND` | 20 | Chat history data |
| `CREATE_FILE_REQUEST` | 21 | Create folder |
| `CREATE_FILE_RESPOND` | 22 | Create result |
| `REFRESH_FILE_REQUEST` | 23 | List directory |
| `REFRESH_FILE_RESPOND` | 24 | File info list |
| `DELETE_FILE_REQUEST` | 25 | Delete file/folder |
| `DELETE_FILE_RESPOND` | 26 | Delete result |
| `RENAME_FILE_REQUEST` | 27 | Rename file/folder |
| `RENAME_FILE_RESPOND` | 28 | Rename result |
| `MOVE_FILE_REQUEST` | 29 | Move file/folder |
| `MOVE_FILE_RESPOND` | 30 | Move result |
| `UPLOAD_FILE_INIT_REQUEST` | 31 | Initiate file upload |
| `UPLOAD_FILE_INIT_RESPOND` | 32 | Upload ack |
| `UPLOAD_FILE_DEAL_REQUEST` | 33 | Upload data chunk |
| `UPLOAD_FILE_DEAL_RESPOND` | 34 | Chunk ack |
| `SHARE_FILE_REQUEST` | 35 | Share file to friends |
| `SHARE_FILE_RESPOND` | 36 | Share ack |
| `SHARE_FILE_ACCEPT_REQUEST` | 37 | Accept shared file |
| `SHARE_FILE_ACCEPT_RESPOND` | 38 | Accept result |
| `HEARTBEAT_REQUEST` | 39 | Keepalive ping (every 15s) |
| `HEARTBEAT_RESPOND` | 40 | Keepalive pong |

## Metadata Convention

Metadata in v2 frames carries the same data as v1's `caData[64]` but without the 64-byte limit:

- Auth messages: `meta[0:32]` = username, `meta[32:64]` = password
- Friend messages: `meta[0:32]` = sender, `meta[32:64]` = target
- Chat messages: `meta[0:32]` = sender, `meta[32:64]` = receiver
- File messages: `meta` contains path lengths (int32) + path data

## Flow: Registration

```
Client                          Server
  |                                |
  |── REGISTER_REQUEST ──────────>|
  |   meta: username + password   |
  |                                |── INSERT user_info (PBKDF2 hash)
  |                                |── mkdir user folder
  |<── REGISTER_RESPOND ──────────|
  |   meta: bool success          |
```

## Flow: Login

```
Client                          Server
  |                                |
  |── LOGIN_REQUEST ─────────────>|
  |   meta: username + password   |
  |                                |── SELECT salt FROM user_info
  |                                |── PBKDF2 verify
  |                                |── UPDATE online=1
  |<── LOGIN_RESPOND ─────────────|
  |   meta: bool success          |
```

## Flow: Chat

```
Client A                        Server                        Client B
  |                                |                              |
  |── CHAT_REQUEST ──────────────>|                              |
  |   meta: A + B                 |                              |
  |   payload: message text       |── INSERT message             |
  |                                |── CHAT_REQUEST ────────────>|
  |                                |   (forwarded to B)          |
```

## Flow: File Upload

```
Client                          Server
  |                                |
  |── UPLOAD_FILE_INIT_REQUEST ──>|
  |   meta: filename + filesize   |
  |   payload: target path        |
  |<── UPLOAD_FILE_INIT_RESPOND ──|
  |   meta: bool ack              |
  |                                |
  |── UPLOAD_FILE_DEAL_REQUEST ──>|  (repeated for each 4KB chunk)
  |   payload: file chunk         |
  |<── UPLOAD_FILE_DEAL_RESPOND ──|
  |   (sent after final chunk)    |
```

## Heartbeat

- Client sends `HEARTBEAT_REQUEST` every 15 seconds
- Server resets 30-second timer on each received message
- If timer expires: server disconnects client

## Security

- Passwords hashed with PBKDF2-HMAC-SHA256 (1000 iterations, 16-byte random salt)
- Directory traversal prevented via `canonicalFilePath()` validation on all file operations
- TLS support available via QSslSocket (optional, enabled in config)
