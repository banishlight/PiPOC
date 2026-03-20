#!/usr/bin/env python3
"""
simulate.py — Virtual ELM327 OBD-II simulator for rx8-headunit.

Creates a pair of pseudo-terminals (pty). The slave end is symlinked to
/tmp/obd-sim so you can point your OBDService at it during development:

    Change port in OBDService.cpp:
        const std::string port = "/tmp/obd-sim";

Then run the simulator in one terminal:
    python3 simulate.py

And your binary in another:
    ./pipoc

The simulator responds to ELM327 AT commands and MODE 01 OBD PIDs with
realistic RX-8 data that cycles over time so you can see the gauges move.

Supported PIDs:
    010C  RPM              (idle ~900, rev to ~8000)
    0105  Coolant temp     (warms from 20 to 90 C)
    015C  Oil temp         (warms from 20 to 110 C)
    0111  Throttle pos     (follows RPM curve)
    010D  Vehicle speed    (0–120 km/h)
    010F  Intake air temp  (ambient ~25 C)
    0110  MAF              (follows RPM)
"""

import os
import pty
import sys
import math
import time
import threading
import select

SYM_PATH = "/tmp/obd-sim"

# ---------------------------------------------------------------------------
# Simulated sensor values — updated by a background thread
# ---------------------------------------------------------------------------
state = {
    "rpm":      900.0,
    "coolant":  20.0,
    "oil":      20.0,
    "throttle": 5.0,
    "speed":    0.0,
    "intake":   25.0,
    "maf":      2.0,
    "t":        0.0,   # simulation time (seconds)
}
state_lock = threading.Lock()


def update_state():
    """Runs in background thread, advances simulated sensor values."""
    while True:
        with state_lock:
            t = state["t"]

            # RPM: idle at 900, pulses up every ~10s to simulate driving
            rpm_wave = 900 + 3500 * max(0, math.sin(t / 10.0)) \
                           + 2000 * max(0, math.sin(t / 3.7))
            state["rpm"]      = min(max(rpm_wave, 800), 8500)
            state["throttle"] = min(100, (state["rpm"] - 800) / 80.0)
            state["speed"]    = min(120, state["rpm"] / 70.0)
            state["maf"]      = 2.0 + state["rpm"] / 500.0

            # Temps warm up gradually (cap at operating range)
            if state["coolant"] < 90:
                state["coolant"] += 0.05
            if state["oil"] < 105:
                state["oil"] += 0.03

            state["t"] += 0.1

        time.sleep(0.1)


# ---------------------------------------------------------------------------
# ELM327 response encoding
# ---------------------------------------------------------------------------
def encode_pid(pid_hex: str) -> str:
    """Return a compact (ATS0) mode-01 response string for a given PID."""
    with state_lock:
        pid = int(pid_hex, 16)

        if pid == 0x0C:  # RPM: ((A*256)+B)/4
            val = int(state["rpm"] * 4)
            A, B = (val >> 8) & 0xFF, val & 0xFF
            return f"410C{A:02X}{B:02X}"

        if pid == 0x05:  # Coolant: A-40
            A = int(state["coolant"]) + 40
            return f"4105{A:02X}"

        if pid == 0x5C:  # Oil temp: A-40
            A = int(state["oil"]) + 40
            return f"415C{A:02X}"

        if pid == 0x11:  # Throttle: (A/255)*100
            A = int(state["throttle"] * 255 / 100)
            return f"4111{A:02X}"

        if pid == 0x0D:  # Speed: A km/h
            A = int(state["speed"])
            return f"410D{A:02X}"

        if pid == 0x0F:  # Intake: A-40
            A = int(state["intake"]) + 40
            return f"410F{A:02X}"

        if pid == 0x10:  # MAF: ((A*256)+B)/100
            val = int(state["maf"] * 100)
            A, B = (val >> 8) & 0xFF, val & 0xFF
            return f"4110{A:02X}{B:02X}"

        if pid == 0x00:  # Supported PIDs 01-20 — claim all supported
            return "41000007E8EF"

    return "NO DATA"


def handle_command(cmd: str) -> str:
    """Map an ELM327 AT command or OBD command to a response string."""
    cmd = cmd.strip().upper()

    # AT commands
    if cmd == "ATZ":   return "ELM327 v1.5"
    if cmd == "ATE0":  return "OK"
    if cmd == "ATL0":  return "OK"
    if cmd == "ATS0":  return "OK"
    if cmd == "ATSP0": return "OK"
    if cmd.startswith("AT"): return "OK"

    # OBD mode 01 — "01XX"
    if len(cmd) == 4 and cmd.startswith("01"):
        return encode_pid(cmd[2:])

    return "?"


# ---------------------------------------------------------------------------
# Main pty loop
# ---------------------------------------------------------------------------
def main():
    master_fd, slave_fd = pty.openpty()
    slave_path = os.ttyname(slave_fd)

    # Symlink slave end so OBDService can find it at a fixed path
    if os.path.exists(SYM_PATH) or os.path.islink(SYM_PATH):
        os.remove(SYM_PATH)
    os.symlink(slave_path, SYM_PATH)

    print(f"[sim] Virtual OBD port: {slave_path}")
    print(f"[sim] Symlinked to:     {SYM_PATH}")
    print(f"[sim] Point OBDService at /tmp/obd-sim  (or {slave_path})")
    print(f"[sim] Ctrl-C to stop.\n")

    # Start sensor update thread
    t = threading.Thread(target=update_state, daemon=True)
    t.start()

    buf = ""
    try:
        while True:
            r, _, _ = select.select([master_fd], [], [], 1.0)
            if not r:
                continue

            data = os.read(master_fd, 256).decode("ascii", errors="ignore")
            buf += data

            while "\r" in buf:
                line, buf = buf.split("\r", 1)
                line = line.strip()
                if not line:
                    continue

                response = handle_command(line)
                print(f"[sim]  cmd={line!r:12s}  resp={response!r}")

                # ELM327 always terminates response with \r> 
                out = (response + "\r>").encode("ascii")
                os.write(master_fd, out)

    except KeyboardInterrupt:
        print("\n[sim] Stopped.")
    finally:
        if os.path.islink(SYM_PATH):
            os.remove(SYM_PATH)
        os.close(master_fd)
        os.close(slave_fd)


if __name__ == "__main__":
    main()
