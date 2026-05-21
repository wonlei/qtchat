#!/usr/bin/env python3
"""
WonChat Server Stress Test
Spawns concurrent clients, measures QPS and latency.

Usage: python tools/stress_test.py [--host 127.0.0.1] [--port 5000]
                                   [--clients 50] [--rounds 10]

Requires the WonChat server to be running.
"""

import socket
import struct
import argparse
import time
import sys
import threading
import random
import string
from collections import defaultdict

MAGIC_MASK = 0x80000000
FRAME_VERSION = 2
FRAME_HEADER_SIZE = 15

MSG_TYPE_REGISTER_REQUEST = 1
MSG_TYPE_REGISTER_RESPOND = 2
MSG_TYPE_LOGIN_REQUEST = 3
MSG_TYPE_LOGIN_RESPOND = 4
MSG_TYPE_CHAT_REQUEST = 17
MSG_TYPE_ONLINE_USER_REQUEST = 7
MSG_TYPE_ONLINE_USER_RESPOND = 8
MSG_TYPE_HEARTBEAT_REQUEST = 39
MSG_TYPE_HEARTBEAT_RESPOND = 40


def build_frame(msg_type, caData=b'', caMsg=b''):
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


def recv_exact(sock, n):
    data = b''
    while len(data) < n:
        chunk = sock.recv(n - len(data))
        if not chunk:
            return None
        data += chunk
    return data


def recv_frame(sock, timeout=2.0):
    sock.settimeout(timeout)
    try:
        header = recv_exact(sock, FRAME_HEADER_SIZE)
        if not header:
            return None
        total_len, version, msg_type, meta_len, payload_len = struct.unpack(
            '<IBHII', header)
        total_len &= ~MAGIC_MASK
        remaining = total_len - FRAME_HEADER_SIZE
        body = recv_exact(sock, remaining)
        if not body:
            return None
        return (msg_type, body[:meta_len], body[meta_len:meta_len + payload_len])
    except socket.timeout:
        return None


def make_creds(username, password):
    caData = username.encode().ljust(32, b'\x00')[:32]
    caData += password.encode().ljust(32, b'\x00')[:32]
    return caData


def do_register(sock, username, password):
    caData = make_creds(username, password)
    sock.sendall(build_frame(MSG_TYPE_REGISTER_REQUEST, caData=caData))
    resp = recv_frame(sock, timeout=3.0)
    if resp:
        code = struct.unpack('<H', resp[1][:2])[0]
        return code == 0
    return False


def do_login(sock, username, password):
    caData = make_creds(username, password)
    sock.sendall(build_frame(MSG_TYPE_LOGIN_REQUEST, caData=caData))
    resp = recv_frame(sock, timeout=3.0)
    if resp:
        code = struct.unpack('<H', resp[1][:2])[0]
        return code == 0
    return False


def do_chat(sock, sender, receiver, message):
    caData = sender.encode().ljust(32, b'\x00')[:32]
    caData += receiver.encode().ljust(32, b'\x00')[:32]
    sock.sendall(build_frame(MSG_TYPE_CHAT_REQUEST, caData=caData, caMsg=message.encode()))
    return True  # chat returns no response (forwarded)


def do_heartbeat(sock):
    sock.sendall(build_frame(MSG_TYPE_HEARTBEAT_REQUEST))
    resp = recv_frame(sock, timeout=2.0)
    return resp is not None


class LatencyStats:
    def __init__(self):
        self.times = []
        self.lock = threading.Lock()

    def record(self, ms):
        with self.lock:
            self.times.append(ms)

    def summary(self):
        with self.lock:
            if not self.times:
                return {}
            sorted_times = sorted(self.times)
            n = len(sorted_times)
            return {
                'count': n,
                'avg_ms': sum(sorted_times) / n,
                'p50_ms': sorted_times[n // 2],
                'p95_ms': sorted_times[int(n * 0.95)],
                'p99_ms': sorted_times[int(n * 0.99)],
                'min_ms': sorted_times[0],
                'max_ms': sorted_times[-1],
            }


def client_worker(host, port, username, password, rounds, stats, errors, barrier):
    """Single stress client: connect, auth, send N rounds of requests."""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    try:
        sock.connect((host, port))
    except Exception as e:
        with errors['lock']:
            errors['data'].append(f"connect: {e}")
        return

    try:
        # Register & login
        t0 = time.time()
        if not do_register(sock, username, password):
            # Might already exist, try login directly
            pass
        t1 = time.time()
        if not do_login(sock, username, password):
            with errors['lock']:
                errors['data'].append(f"login failed: {username}")
            sock.close()
            return
        t2 = time.time()
        stats.record((t2 - t1) * 1000)

        barrier.wait()  # synchronize all clients

        for r in range(rounds):
            t0 = time.time()
            do_chat(sock, username, f"stress_target_{random.randint(0,9)}",
                    f"msg_{r}")
            t1 = time.time()
            do_heartbeat(sock)
            t2 = time.time()
            stats.record((t2 - t0) * 1000)

    except Exception as e:
        with errors['lock']:
            errors['data'].append(str(e))
    finally:
        sock.close()


def run_stress_test(host, port, num_clients, rounds):
    print(f"WonChat Stress Test")
    print(f"Server: {host}:{port}")
    print(f"Clients: {num_clients}, Rounds per client: {rounds}")
    print("=" * 50)

    # Check connectivity
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(3)
        sock.connect((host, port))
        sock.close()
    except Exception as e:
        print(f"ERROR: Cannot connect to server: {e}")
        sys.exit(1)

    stats = LatencyStats()
    errors = {'data': [], 'lock': threading.Lock()}
    barrier = threading.Barrier(num_clients, timeout=30)
    threads = []
    suffix = ''.join(random.choices(string.ascii_lowercase, k=4))

    print(f"\nStarting {num_clients} concurrent clients...")
    t_start = time.time()

    for i in range(num_clients):
        username = f"stress_{i}_{suffix}"
        t = threading.Thread(target=client_worker,
                             args=(host, port, username, "pass",
                                   rounds, stats, errors, barrier))
        t.daemon = True
        threads.append(t)
        t.start()

    for t in threads:
        t.join(timeout=30)

    t_end = time.time()
    elapsed = t_end - t_start

    # Report
    print(f"\n--- Results ---")
    total_ops = num_clients * rounds * 2  # chat + heartbeat per round
    print(f"Total operations: {total_ops}")
    print(f"Wall time: {elapsed:.2f}s")
    print(f"Throughput: {total_ops / elapsed:.1f} ops/s")
    print(f"Errors: {len(errors['data'])}")
    for e in errors['data'][:5]:
        print(f"  - {e}")

    s = stats.summary()
    if s:
        print(f"\nLatency (per operation pair):")
        print(f"  Count:  {s['count']}")
        print(f"  Avg:    {s['avg_ms']:.1f} ms")
        print(f"  P50:    {s['p50_ms']:.1f} ms")
        print(f"  P95:    {s['p95_ms']:.1f} ms")
        print(f"  P99:    {s['p99_ms']:.1f} ms")
        print(f"  Min:    {s['min_ms']:.1f} ms")
        print(f"  Max:    {s['max_ms']:.1f} ms")


def main():
    parser = argparse.ArgumentParser(description='WonChat Stress Test')
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=5000)
    parser.add_argument('--clients', type=int, default=20, help='Concurrent clients')
    parser.add_argument('--rounds', type=int, default=5, help='Request rounds per client')
    args = parser.parse_args()
    run_stress_test(args.host, args.port, args.clients, args.rounds)


if __name__ == '__main__':
    main()
