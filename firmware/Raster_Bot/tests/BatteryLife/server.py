#!/usr/bin/env python3
"""Battery voltage logging server for the BatteryTime sketch.

Listens for HTTP POSTs from the Raster_Bot and appends each battery
voltage reading to a CSV file along with the time it was received.

Usage:
    python3 server.py [--host 0.0.0.0] [--port 8080] [--logfile battery_log.csv]

The server prints the local IP addresses it can be reached at on startup;
use one of those (with the port) for SERVER_URL in BatteryTime.ino, e.g.
    http://192.168.1.100:8080/log
"""

import argparse
import csv
import json
import os
import socket
from datetime import datetime
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

LOG_FILE = "battery_log.csv"


def ensure_log_header(path):
    """Create the CSV file with a header row if it does not exist yet."""
    if not os.path.exists(path) or os.path.getsize(path) == 0:
        with open(path, "w", newline="") as f:
            csv.writer(f).writerow(["timestamp_iso", "epoch_seconds", "voltage"])


def append_reading(path, voltage):
    """Append a single voltage reading with the current timestamp."""
    now = datetime.now()
    with open(path, "a", newline="") as f:
        csv.writer(f).writerow([now.isoformat(timespec="seconds"), now.timestamp(), voltage])
    return now


class LogHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        length = int(self.headers.get("Content-Length", 0))
        raw = self.rfile.read(length) if length else b""

        voltage = self._parse_voltage(raw)
        if voltage is None:
            self.send_response(400)
            self.end_headers()
            self.wfile.write(b"could not parse voltage\n")
            return

        when = append_reading(self.server.log_file, voltage)
        print(f"[{when.isoformat(timespec='seconds')}] voltage = {voltage:.3f} V")

        self.send_response(200)
        self.send_header("Content-Type", "text/plain")
        self.end_headers()
        self.wfile.write(b"ok\n")

    @staticmethod
    def _parse_voltage(raw):
        """Extract a voltage value from a JSON body or a raw number."""
        text = raw.decode("utf-8", errors="ignore").strip()
        if not text:
            return None
        try:
            data = json.loads(text)
            if isinstance(data, dict) and "voltage" in data:
                return float(data["voltage"])
            return float(data)
        except (ValueError, TypeError):
            try:
                return float(text)
            except ValueError:
                return None

    def log_message(self, *args):
        # Silence the default per-request logging; we print our own line.
        pass


def local_ip_addresses():
    """Best-effort list of LAN IP addresses for this machine."""
    addrs = set()
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        addrs.add(s.getsockname()[0])
        s.close()
    except OSError:
        pass
    try:
        for info in socket.getaddrinfo(socket.gethostname(), None, socket.AF_INET):
            addrs.add(info[4][0])
    except socket.gaierror:
        pass
    return sorted(a for a in addrs if not a.startswith("127."))


def main():
    parser = argparse.ArgumentParser(description="Battery voltage logging server")
    parser.add_argument("--host", default="0.0.0.0", help="Bind address (default: all interfaces)")
    parser.add_argument("--port", type=int, default=8080, help="Port to listen on (default: 8080)")
    parser.add_argument("--logfile", default=LOG_FILE, help=f"CSV log file (default: {LOG_FILE})")
    args = parser.parse_args()

    ensure_log_header(args.logfile)

    server = ThreadingHTTPServer((args.host, args.port), LogHandler)
    server.log_file = args.logfile

    print(f"Logging battery readings to {os.path.abspath(args.logfile)}")
    print(f"Listening on {args.host}:{args.port}  (path: /log)")
    for ip in local_ip_addresses():
        print(f"  Set SERVER_URL = \"http://{ip}:{args.port}/log\" in BatteryTime.ino")
    print("Press Ctrl+C to stop.\n")

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down.")
        server.server_close()


if __name__ == "__main__":
    main()
