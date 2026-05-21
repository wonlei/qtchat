#!/usr/bin/env python3
"""
WonChat Integration Test Suite
Tests the full protocol end-to-end: register -> login -> friends -> chat -> file ops.

Usage: python tools/integration_test.py [--host 127.0.0.1] [--port 5000]

Requires the WonChat server to be running and MySQL to be accessible.
"""

import socket
import struct
import argparse
import time
import sys
import random
import string

# Message type enum (must match protocol.h)
MSG_TYPE = {
    'REGISTER_REQUEST': 1,         'REGISTER_RESPOND': 2,
    'LOGIN_REQUEST': 3,            'LOGIN_RESPOND': 4,
    'FIND_USER_REQUEST': 5,        'FIND_USER_RESPOND': 6,
    'ONLINE_USER_REQUEST': 7,      'ONLINE_USER_RESPOND': 8,
    'ADD_FRIEND_REQUEST': 9,       'ADD_FRIEND_RESPOND': 10,
    'ADD_FRIEND_ACCEPT_REQUEST': 11, 'ADD_FRIEND_ACCEPT_RESPOND': 12,
    'REFRESH_FRIEND_REQUEST': 13,  'REFRESH_FRIEND_RESPOND': 14,
    'DELETE_FRIEND_REQUEST': 15,   'DELETE_FRIEND_RESPOND': 16,
    'CHAT_REQUEST': 17,            'CHAT_RESPOND': 18,
    'CHAT_HISTORY_REQUEST': 19,    'CHAT_HISTORY_RESPOND': 20,
    'CREATE_FILE_REQUEST': 21,     'CREATE_FILE_RESPOND': 22,
    'REFRESH_FILE_REQUEST': 23,    'REFRESH_FILE_RESPOND': 24,
    'DELETE_FILE_REQUEST': 25,     'DELETE_FILE_RESPOND': 26,
    'RENAME_FILE_REQUEST': 27,     'RENAME_FILE_RESPOND': 28,
    'MOVE_FILE_REQUEST': 29,       'MOVE_FILE_RESPOND': 30,
    'UPLOAD_FILE_INIT_REQUEST': 31,'UPLOAD_FILE_INIT_RESPOND': 32,
    'UPLOAD_FILE_DEAL_REQUEST': 33,'UPLOAD_FILE_DEAL_RESPOND': 34,
    'SHARE_FILE_REQUEST': 35,      'SHARE_FILE_RESPOND': 36,
    'SHARE_FILE_ACCEPT_REQUEST': 37, 'SHARE_FILE_ACCEPT_RESPOND': 38,
    'HEARTBEAT_REQUEST': 39,       'HEARTBEAT_RESPOND': 40,
}

ERR_NONE = 0
MAGIC_MASK = 0x80000000
FRAME_VERSION = 2
FRAME_HEADER_SIZE = 15


class WonChatClient:
    """Minimal WonChat protocol client for integration testing."""

    def __init__(self, host='127.0.0.1', port=5000):
        self.host = host
        self.port = port
        self.sock = None
        self.username = None

    def connect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(10)
        self.sock.connect((self.host, self.port))
        self.sock.settimeout(3)

    def disconnect(self):
        if self.sock:
            self.sock.close()
            self.sock = None

    def _build_frame(self, msg_type, caData=b'', caMsg=b''):
        """Build a Frame: meta=caData, payload=caMsg."""
        meta = caData.ljust(64, b'\x00')[:64]
        payload = caMsg
        total_len = FRAME_HEADER_SIZE + len(meta) + len(payload)

        data = struct.pack('<I', total_len | MAGIC_MASK)
        data += struct.pack('B', FRAME_VERSION)
        data += struct.pack('<H', msg_type)
        data += struct.pack('<I', len(meta))
        data += struct.pack('<I', len(payload))
        data += meta
        data += payload
        return data

    def _recv_exact(self, n):
        data = b''
        while len(data) < n:
            chunk = self.sock.recv(n - len(data))
            if not chunk:
                return None
            data += chunk
        return data

    def _recv_frame(self, timeout=2.0):
        """Read one frame. Returns (msg_type, meta, payload) or None on timeout/error."""
        self.sock.settimeout(timeout)
        try:
            header = self._recv_exact(FRAME_HEADER_SIZE)
            if not header:
                return None
            total_len, version, msg_type, meta_len, payload_len = struct.unpack(
                '<IBHII', header)
            total_len &= ~MAGIC_MASK
            remaining = total_len - FRAME_HEADER_SIZE
            body = self._recv_exact(remaining)
            if not body:
                return None
            return (msg_type, body[:meta_len], body[meta_len:meta_len + payload_len])
        except socket.timeout:
            return None

    def _send_frame(self, msg_type, caData=b'', caMsg=b''):
        self.sock.sendall(self._build_frame(msg_type, caData, caMsg))

    def _request(self, msg_type, caData=b'', caMsg=b'', expect_type=None):
        """Send request and wait for response of expected type.
        Returns (meta, payload) or (None, None) on failure.
        Skips messages that don't match expect_type."""
        self._send_frame(msg_type, caData, caMsg)
        if expect_type is None:
            return None, None

        deadline = time.time() + 3.0
        while time.time() < deadline:
            remaining = deadline - time.time()
            frame = self._recv_frame(timeout=max(0.1, remaining))
            if not frame:
                return None, None
            msg_type, meta, payload = frame
            if msg_type == expect_type:
                return meta, payload
            # Skip unexpected messages (e.g., forwarded friend requests)
        return None, None

    def _get_error(self, meta):
        return struct.unpack('<H', meta[:2])[0] if meta and len(meta) >= 2 else -1

    # ---- High-level operations ----

    def register(self, username, password):
        caData = username.encode().ljust(32, b'\x00')[:32]
        caData += password.encode().ljust(32, b'\x00')[:32]
        meta, _ = self._request(MSG_TYPE['REGISTER_REQUEST'], caData=caData,
                                expect_type=MSG_TYPE['REGISTER_RESPOND'])
        code = self._get_error(meta)
        return code == ERR_NONE, code

    def login(self, username, password):
        caData = username.encode().ljust(32, b'\x00')[:32]
        caData += password.encode().ljust(32, b'\x00')[:32]
        meta, payload = self._request(MSG_TYPE['LOGIN_REQUEST'], caData=caData,
                                      expect_type=MSG_TYPE['LOGIN_RESPOND'])
        code = self._get_error(meta)
        if code == ERR_NONE:
            self.username = username
        # Extract offline message count (caData[4..7])
        offline_count = struct.unpack('<i', meta[4:8])[0] if len(meta) >= 8 else 0
        return code == ERR_NONE, code, offline_count

    def login_simple(self, username, password):
        """Login without offline count. Returns (success, code)."""
        ok, code, _ = self.login(username, password)
        return ok, code

    def find_user(self, username):
        caData = username.encode().ljust(32, b'\x00')[:32]
        meta, _ = self._request(MSG_TYPE['FIND_USER_REQUEST'], caData=caData,
                                expect_type=MSG_TYPE['FIND_USER_RESPOND'])
        return self._get_error(meta) == ERR_NONE, self._get_error(meta)

    def online_users(self):
        meta, payload = self._request(MSG_TYPE['ONLINE_USER_REQUEST'],
                                      expect_type=MSG_TYPE['ONLINE_USER_RESPOND'])
        if not payload:
            return []
        users = []
        for i in range(0, len(payload), 32):
            raw = payload[i:i+32]
            null_pos = raw.find(b'\x00')
            if null_pos >= 0:
                raw = raw[:null_pos]
            name = raw.decode(errors='replace')
            if name:
                users.append(name)
        return users

    def add_friend(self, target_user):
        """Returns (success, code). Success is True when server accepted.
        Note: add_friend returns no response when target is online (forwarded instead)."""
        caData = self.username.encode().ljust(32, b'\x00')[:32]
        caData += target_user.encode().ljust(32, b'\x00')[:32]
        meta, _ = self._request(MSG_TYPE['ADD_FRIEND_REQUEST'], caData=caData,
                                expect_type=MSG_TYPE['ADD_FRIEND_RESPOND'])
        code = self._get_error(meta) if meta else ERR_NONE
        return code == ERR_NONE, code

    def accept_friend(self, target_user):
        """Accept a friend request. Skips any forwarded ADD_FRIEND_REQUEST first."""
        caData = self.username.encode().ljust(32, b'\x00')[:32]
        caData += target_user.encode().ljust(32, b'\x00')[:32]
        meta, _ = self._request(MSG_TYPE['ADD_FRIEND_ACCEPT_REQUEST'], caData=caData,
                                expect_type=MSG_TYPE['ADD_FRIEND_ACCEPT_RESPOND'])
        return self._get_error(meta) == ERR_NONE, self._get_error(meta)

    def refresh_friends(self):
        caData = self.username.encode().ljust(32, b'\x00')[:32]
        meta, payload = self._request(MSG_TYPE['REFRESH_FRIEND_REQUEST'], caData=caData,
                                      expect_type=MSG_TYPE['REFRESH_FRIEND_RESPOND'])
        if not payload:
            return []
        friends = []
        for i in range(0, len(payload), 32):
            raw = payload[i:i+32]
            null_pos = raw.find(b'\x00')
            if null_pos >= 0:
                raw = raw[:null_pos]
            name = raw.decode(errors='replace')
            if name:
                friends.append(name)
        return friends

    def send_chat(self, receiver, message):
        """Send chat. Returns True if message was accepted (even without direct response)."""
        caData = self.username.encode().ljust(32, b'\x00')[:32]
        caData += receiver.encode().ljust(32, b'\x00')[:32]
        # Chat returns nullptr (message forwarded to receiver), no direct response
        self._send_frame(MSG_TYPE['CHAT_REQUEST'], caData=caData, caMsg=message.encode())
        return True, ERR_NONE

    def chat_history(self, other_user):
        caData = self.username.encode().ljust(32, b'\x00')[:32]
        caData += other_user.encode().ljust(32, b'\x00')[:32]
        meta, payload = self._request(MSG_TYPE['CHAT_HISTORY_REQUEST'], caData=caData,
                                      expect_type=MSG_TYPE['CHAT_HISTORY_RESPOND'])
        if not payload:
            return ''
        return payload.decode(errors='replace')

    def create_dir(self, cur_path, folder_name):
        """Create a directory inside cur_path. Format matches Client/file.cpp."""
        path_bytes = cur_path.encode()
        name_bytes = folder_name.encode()
        path_size = len(path_bytes)
        name_size = len(name_bytes)
        caData = struct.pack('<i', path_size) + struct.pack('<i', name_size)
        caMsg = path_bytes + name_bytes
        meta, _ = self._request(MSG_TYPE['CREATE_FILE_REQUEST'], caData=caData, caMsg=caMsg,
                                expect_type=MSG_TYPE['CREATE_FILE_RESPOND'])
        return self._get_error(meta) == ERR_NONE, self._get_error(meta)

    def refresh_files(self, cur_path):
        """Get file list. cur_path is the full path (e.g. './fliesys/username')."""
        meta, payload = self._request(MSG_TYPE['REFRESH_FILE_REQUEST'],
                                      caMsg=cur_path.encode(),
                                      expect_type=MSG_TYPE['REFRESH_FILE_RESPOND'])
        if not payload:
            return []
        files = []
        # FILE_INFO: 32 chars name + 4 bytes uint type = 36 bytes each
        for i in range(0, len(payload), 36):
            if i + 36 <= len(payload):
                raw = payload[i:i+32]
                # Truncate at first null (memcpy may copy garbage after \0)
                null_pos = raw.find(b'\x00')
                if null_pos >= 0:
                    raw = raw[:null_pos]
                name = raw.decode(errors='replace')
                if name:
                    ftype = struct.unpack('<I', payload[i+32:i+36])[0]
                    files.append((name, ftype))
        return files

    def rename_file(self, cur_path, old_name, new_name):
        """Rename. caData[0..31]=oldName, caData[32..63]=newName, caMsg=path."""
        caData = old_name.encode().ljust(32, b'\x00')[:32]
        caData += new_name.encode().ljust(32, b'\x00')[:32]
        meta, _ = self._request(MSG_TYPE['RENAME_FILE_REQUEST'], caData=caData,
                                caMsg=cur_path.encode(),
                                expect_type=MSG_TYPE['RENAME_FILE_RESPOND'])
        return self._get_error(meta) == ERR_NONE, self._get_error(meta)

    def delete_file(self, cur_path, filename):
        """Delete. caData=uint32 type(0=dir), caMsg=fullPath."""
        full_path = cur_path + '/' + filename
        caData = struct.pack('<I', 0)  # 0 = directory
        meta, _ = self._request(MSG_TYPE['DELETE_FILE_REQUEST'], caData=caData,
                                caMsg=full_path.encode(),
                                expect_type=MSG_TYPE['DELETE_FILE_RESPOND'])
        return self._get_error(meta) == ERR_NONE, self._get_error(meta)

    def drain(self, timeout=0.3):
        """Read and discard any pending messages."""
        while True:
            frame = self._recv_frame(timeout=timeout)
            if not frame:
                break


def random_suffix(length=6):
    return ''.join(random.choices(string.ascii_lowercase + string.digits, k=length))


class TestRunner:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.passed = 0
        self.failed = 0

    def assert_true(self, condition, msg):
        if condition:
            self.passed += 1
            print(f"  PASS: {msg}")
        else:
            self.failed += 1
            print(f"  FAIL: {msg}")

    def test_register_and_login(self):
        print("\n[Test] Register & Login")
        c = WonChatClient(self.host, self.port)
        try:
            c.connect()
        except Exception as e:
            self.assert_true(False, f"Connect failed: {e}")
            return

        suffix = random_suffix()
        username = f"ittest_{suffix}"
        password = "testpass123"

        ok, code = c.register(username, password)
        self.assert_true(ok, f"Register '{username}' (code={code})")

        ok2, code2 = c.register(username, password)
        self.assert_true(not ok2, f"Re-register rejected (code={code2})")

        ok3, code3 = c.login_simple(username, password)
        self.assert_true(ok3, f"Login '{username}' (code={code3})")

        ok4, code4 = c.login_simple(username, "wrongpass")
        self.assert_true(not ok4, f"Wrong password rejected (code={code4})")

        c.disconnect()

    def test_friend_operations(self):
        print("\n[Test] Friend Operations")
        suffix = random_suffix()
        alice_name = f"it_alice_{suffix}"
        bob_name = f"it_bob_{suffix}"
        password = "pass123"

        alice = WonChatClient(self.host, self.port)
        bob = WonChatClient(self.host, self.port)

        try:
            alice.connect()
            bob.connect()
        except Exception as e:
            self.assert_true(False, f"Connect failed: {e}")
            return

        ok, _ = alice.register(alice_name, password)
        self.assert_true(ok, f"Register Alice")
        ok, _ = bob.register(bob_name, password)
        self.assert_true(ok, f"Register Bob")

        ok, _ = alice.login_simple(alice_name, password)
        self.assert_true(ok, "Alice login")
        ok, _ = bob.login_simple(bob_name, password)
        self.assert_true(ok, "Bob login")

        # Bob adds Alice (sent to Alice if online, no response)
        ok, _ = bob.add_friend(alice_name)
        self.assert_true(ok, "Bob sends friend request to Alice")

        # Alice accepts Bob (skip forwarded ADD_FRIEND_REQUEST from Bob)
        ok, _ = alice.accept_friend(bob_name)
        self.assert_true(ok, "Alice accepts Bob")

        alice_friends = alice.refresh_friends()
        self.assert_true(len(alice_friends) >= 1, f"Alice has {len(alice_friends)} friends")

        bob_friends = bob.refresh_friends()
        self.assert_true(len(bob_friends) >= 1, f"Bob has {len(bob_friends)} friends")

        alice.disconnect()
        bob.disconnect()

    def test_chat(self):
        print("\n[Test] Chat Messaging")
        suffix = random_suffix()
        alice_name = f"it_chat_a_{suffix}"
        bob_name = f"it_chat_b_{suffix}"
        password = "pass123"

        alice = WonChatClient(self.host, self.port)
        bob = WonChatClient(self.host, self.port)

        try:
            alice.connect()
            bob.connect()
        except Exception as e:
            self.assert_true(False, f"Connect failed: {e}")
            return

        alice.register(alice_name, password)
        bob.register(bob_name, password)
        alice.login_simple(alice_name, password)
        bob.login_simple(bob_name, password)

        # Friend setup: Bob adds Alice, Alice accepts
        bob.add_friend(alice_name)
        alice.accept_friend(bob_name)

        alice.send_chat(bob_name, f"Hello Bob from Alice {suffix}")
        bob.send_chat(alice_name, f"Hello Alice from Bob {suffix}")

        hist = alice.chat_history(bob_name)
        self.assert_true(hist is not None and len(hist) > 0, f"Chat history non-empty ({len(hist)} chars)")

        alice.disconnect()
        bob.disconnect()

    def test_file_operations(self):
        print("\n[Test] File Operations")
        suffix = random_suffix()
        username = f"it_file_{suffix}"
        password = "pass123"
        dir_name = f"testdir_{suffix}"

        c = WonChatClient(self.host, self.port)
        try:
            c.connect()
        except Exception as e:
            self.assert_true(False, f"Connect failed: {e}")
            return

        c.register(username, password)
        c.login_simple(username, password)

        # Client uses "./fliesys/<username>" as the base path
        user_path = f"./fliesys/{username}"

        ok, code = c.create_dir(user_path, dir_name)
        self.assert_true(ok, f"Create directory '{dir_name}' (code={code})")

        ok2, code2 = c.create_dir(user_path, dir_name)
        self.assert_true(not ok2, f"Duplicate directory rejected (code={code2})")

        files = c.refresh_files(user_path)
        found = dir_name in [f[0] for f in files]
        self.assert_true(found, f"Created directory appears in file list ({len(files)} entries)")

        # Rename test
        new_name = f"renamed_{suffix}"
        ok, _ = c.rename_file(user_path, dir_name, new_name)
        self.assert_true(ok, f"Rename '{dir_name}' -> '{new_name}'")

        # Clean up
        c.delete_file(user_path, new_name)

        c.disconnect()

    def test_offline_messages(self):
        print("\n[Test] Offline Messages")
        suffix = random_suffix()
        alice_name = f"it_off_a_{suffix}"
        bob_name = f"it_off_b_{suffix}"
        password = "pass123"

        alice = WonChatClient(self.host, self.port)
        bob = WonChatClient(self.host, self.port)

        try:
            alice.connect()
            bob.connect()
        except Exception as e:
            self.assert_true(False, f"Connect failed: {e}")
            return

        # Register both
        alice.register(alice_name, password)
        bob.register(bob_name, password)

        # Both login, become friends
        ok, code = alice.login_simple(alice_name, password)
        self.assert_true(ok, "Alice login")
        ok, code = bob.login_simple(bob_name, password)
        self.assert_true(ok, "Bob login")
        bob.add_friend(alice_name)
        alice.accept_friend(bob_name)

        # Alice goes offline
        alice.disconnect()
        time.sleep(0.5)

        # Bob sends message to offline Alice
        bob.send_chat(alice_name, f"Offline message from Bob {suffix}")
        bob.disconnect()

        # Alice logs in again — should receive offline message
        try:
            alice.connect()
        except Exception as e:
            self.assert_true(False, f"Alice reconnect failed: {e}")
            return
        ok, code, offline_count = alice.login(alice_name, password)
        self.assert_true(ok, "Alice re-login")
        self.assert_true(offline_count >= 1, f"Offline messages delivered: {offline_count}")
        alice.disconnect()

    def test_concurrent_users(self):
        print("\n[Test] Concurrent Users (5 clients)")
        suffix = random_suffix()
        num = 5
        clients = []
        usernames = []

        for i in range(num):
            c = WonChatClient(self.host, self.port)
            try:
                c.connect()
            except Exception as e:
                self.assert_true(False, f"Client {i} connect failed: {e}")
                for cc in clients:
                    cc.disconnect()
                return
            clients.append(c)

        for i, c in enumerate(clients):
            uname = f"it_conc_{i}_{suffix}"
            usernames.append(uname)
            ok, _ = c.register(uname, "pass")
            self.assert_true(ok, f"User {i} registered")

        for i, c in enumerate(clients):
            ok, _ = c.login_simple(usernames[i], "pass")
            self.assert_true(ok, f"User {i} logged in")

        online = clients[0].online_users()
        self.assert_true(len(online) >= num, f"Online users: {len(online)} (expected >= {num})")

        for c in clients:
            c.disconnect()

    def run_all(self):
        print(f"WonChat Integration Tests")
        print(f"Server: {self.host}:{self.port}")
        print("=" * 50)

        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(3)
            sock.connect((self.host, self.port))
            sock.close()
        except Exception as e:
            print(f"\nERROR: Cannot connect to {self.host}:{self.port}")
            print(f"  Start the WonChat Server first. Error: {e}")
            sys.exit(1)

        self.test_register_and_login()
        self.test_friend_operations()
        self.test_chat()
        self.test_file_operations()
        self.test_offline_messages()
        self.test_concurrent_users()

        print("\n" + "=" * 50)
        total = self.passed + self.failed
        print(f"Results: {self.passed}/{total} passed, {self.failed} failed")
        return self.failed == 0


def main():
    parser = argparse.ArgumentParser(description='WonChat Integration Tests')
    parser.add_argument('--host', default='127.0.0.1', help='Server host')
    parser.add_argument('--port', type=int, default=5000, help='Server port')
    args = parser.parse_args()

    runner = TestRunner(args.host, args.port)
    ok = runner.run_all()
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
