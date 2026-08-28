#!/usr/bin/env python3
"""
Host-side temperature reader for qbMX keyboard, Using osx-cpu-temp to read Mac CPU/GPU temps and sends to Pico over USB serial.
Falls back to RP2040 internal temp if osx-cpu-temp unavailable.

Usage:
    python3 host_temp.py                    # Auto-detect serial port
    python3 host_temp.py /dev/tty.usbmodem* # Specify port
    python3 host_temp.py --test             # Test mode (no Pico needed)
"""

import sys
import time
import subprocess
import re
import argparse

def read_osx_cpu_temp():
    """Read CPU/GPU temps using osx-cpu-temp. Returns (cpu, gpu) in Celsius."""
    cpu = None
    gpu = None

    try:
        result = subprocess.run(
            ['osx-cpu-temp', '-c'],
            capture_output=True, text=True, timeout=5
        )
        # Parse "CPU: 65.0°C" or just "65.0°C"
        match = re.search(r'([\d.]+)\s*°?C', result.stdout)
        if match:
            val = float(match.group(1))
            if val > 0:
                cpu = val
    except (FileNotFoundError, subprocess.TimeoutExpired):
        pass

    try:
        result = subprocess.run(
            ['osx-cpu-temp', '-g'],
            capture_output=True, text=True, timeout=5
        )
        match = re.search(r'([\d.]+)\s*°?C', result.stdout)
        if match:
            val = float(match.group(1))
            if val > 0:
                gpu = val
    except (FileNotFoundError, subprocess.TimeoutExpired):
        pass

    return cpu, gpu


def find_pico_port():
    """Auto-detect the Pico's USB serial port."""
    import glob
    # Look for Pico CDC serial ports
    patterns = [
        '/dev/tty.usbmodem*',
        '/dev/tty.usbserial*',
    ]
    for pattern in patterns:
        ports = glob.glob(pattern)
        if ports:
            return ports[0]
    return None


def main():
    parser = argparse.ArgumentParser(description='Send Mac temps to qbMX keyboard')
    parser.add_argument('port', nargs='?', help='Serial port (auto-detect if omitted)')
    parser.add_argument('--test', action='store_true', help='Test mode, no Pico needed')
    parser.add_argument('--interval', type=float, default=2.0, help='Update interval in seconds')
    args = parser.parse_args()

    # Find port
    port_path = args.port or find_pico_port()
    if not port_path and not args.test:
        print("No Pico found. Use --test to preview output.")
        sys.exit(1)

    print(f"Port: {port_path or 'TEST MODE'}")
    print(f"Interval: {args.interval}s")
    print("Reading Mac CPU/GPU temperatures...")
    print("Press Ctrl+C to stop.\n")

    ser = None
    if not args.test:
        try:
            import serial
            ser = serial.Serial(port_path, 115200, timeout=1)
        except ImportError:
            print("pyserial not installed. Run: pip3 install pyserial")
            sys.exit(1)
        except Exception as e:
            print(f"Could not open {port_path}: {e}")
            print("Make sure the Pico is connected and not in BOOTSEL mode.")
            sys.exit(1)

    try:
        while True:
            cpu, gpu = read_osx_cpu_temp()

            # Calculate average
            temps = [t for t in [cpu, gpu] if t is not None]
            avg = sum(temps) / len(temps) if temps else None

            # Format message: "T:<cpu>:<gpu>:<avg>\n"
            # Use -1.0 for unavailable values
            cpu_val = cpu if cpu is not None else -1.0
            gpu_val = gpu if gpu is not None else -1.0
            avg_val = avg if avg is not None else -1.0

            msg = f"T:{cpu_val:.1f}:{gpu_val:.1f}:{avg_val:.1f}\n"

            if args.test:
                print(f"[TEST] {msg.strip()}")
            else:
                ser.write(msg.encode())
                ser.flush()
                cpu_str = f"{cpu:.1f}" if cpu else "N/A"
                gpu_str = f"{gpu:.1f}" if gpu else "N/A"
                avg_str = f"{avg:.1f}" if avg else "N/A"
                print(f"Sent: CPU={cpu_str} GPU={gpu_str} AVG={avg_str}")

            time.sleep(args.interval)

    except KeyboardInterrupt:
        print("\nStopped.")
    finally:
        if ser:
            ser.close()


if __name__ == '__main__':
    main()
