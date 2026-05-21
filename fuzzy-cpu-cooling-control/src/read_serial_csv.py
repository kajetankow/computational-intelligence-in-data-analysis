import csv
import time
from datetime import datetime
from pathlib import Path

import serial


PORT = "COM3"
BAUDRATE = 115200
OUTPUT_FILE = "fuzzy/temperatury_target.csv"

Path(OUTPUT_FILE).parent.mkdir(parents=True, exist_ok=True)

COLUMNS = [
    "timestamp",
    "time_s",
    "T_CPU",
    "T_CASE",
    "T_AMBIENT",
    "noise",
    "pwm",
    "rpm",
    "mode",
    "raw_line",
]


def to_float(value):
    try:
        return float(value)
    except ValueError:
        return None


with serial.Serial(PORT, BAUDRATE, timeout=2) as ser, \
        open(OUTPUT_FILE, "w", newline="", encoding="utf-8") as file:

    writer = csv.writer(file)
    writer.writerow(COLUMNS)

    start = time.time()

    while True:
        raw = ser.readline()
        line = raw.decode("utf-8", errors="ignore").strip()

        if not line:
            continue

        if line.startswith("T_CPU") or line.startswith("ERROR"):
            print(line)
            continue

        parts = [p.strip() for p in line.split(",")]

        if len(parts) == 6:
            t_cpu = to_float(parts[0])
            t_case = to_float(parts[1])
            t_ambient = to_float(parts[2])
            noise = to_float(parts[3])
            pwm = to_float(parts[4])
            rpm = None
            mode = parts[5]

        elif len(parts) == 7:
            t_cpu = to_float(parts[0])
            t_case = to_float(parts[1])
            t_ambient = to_float(parts[2])
            noise = to_float(parts[3])
            pwm = to_float(parts[4])
            rpm = to_float(parts[5])
            mode = parts[6]

        else:
            print(f"Pominięto linię o złej liczbie pól ({len(parts)}): {line}")
            continue

        if None in [t_cpu, t_case, t_ambient, noise, pwm]:
            print(f"Pominięto linię z błędnymi danymi: {line}")
            continue

        elapsed = round(time.time() - start, 2)

        writer.writerow([
            datetime.now().isoformat(timespec="seconds"),
            elapsed,
            t_cpu,
            t_case,
            t_ambient,
            noise,
            pwm,
            rpm,
            mode,
            line,
        ])

        file.flush()

        rpm_text = "NA" if rpm is None else f"{rpm:.0f}"

        print(
            f"t={elapsed:7.2f}s | "
            f"T_CPU={t_cpu:5.2f}°C | "
            f"T_CASE={t_case:5.2f}°C | "
            f"T_AMB={t_ambient:5.2f}°C | "
            f"NOISE={noise:5.1f} | "
            f"PWM={pwm:5.1f}% | "
            f"RPM={rpm_text:>5} | "
            f"{mode}"
        )