#!/usr/bin/env python3
"""
Tiny UDP client to exercise the GPIO inverter service.
"""

import socket
import struct
import sys
from typing import Tuple


def parse_args() -> Tuple[str, int, int]:
    if len(sys.argv) < 3 or len(sys.argv) > 4:
        print(f"Usage: {sys.argv[0]} <board_ip> <word> [port]", file=sys.stderr)
        print("  <word> accepts 32-bit decimal or 0x-prefixed hex", file=sys.stderr)
        sys.exit(1)
    board_ip = sys.argv[1]
    value = int(sys.argv[2], 0) & 0xFFFFFFFF
    port = int(sys.argv[3]) if len(sys.argv) == 4 else 5005
    return board_ip, value, port


def main() -> None:
    board_ip, value, port = parse_args()
    payload = struct.pack("<I", value)

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.settimeout(3.0)
        sock.sendto(payload, (board_ip, port))
        data, addr = sock.recvfrom(1024)

    if len(data) == 8:
        write_word, read_word = struct.unpack("<II", data[:8])
        print(f"Sent 0x{write_word:08x}, read back 0x{read_word:08x} from {addr[0]}:{addr[1]}")
    else:
        print(f"Received {len(data)} byte reply: {data!r} from {addr[0]}:{addr[1]}")


if __name__ == "__main__":
    main()
