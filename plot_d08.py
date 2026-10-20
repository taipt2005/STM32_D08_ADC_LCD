import csv
import time
import serial
import matplotlib.pyplot as plt

PORT = "COM8"       # Sua theo Device Manager
BAUD = 115200
SECONDS = 30        # Thoi gian ghi du lieu

headers = [
    "time_ms", "frames", "skipped",
    "pot_v", "light_pct", "temp_c", "battery_v", "vdda_v",
    "raw0", "raw1", "raw2", "raw3", "raw4"
]

data = []

with serial.Serial(PORT, BAUD, timeout=1) as ser:
    print(f"Dang doc {PORT} trong {SECONDS} giay...")
    start = time.monotonic()

    while time.monotonic() - start < SECONDS:
        line = ser.readline().decode("ascii", errors="ignore").strip()
        parts = line.split(",")

        if len(parts) != 13:
            continue

        try:
            x = [int(value) for value in parts]
        except ValueError:
            continue

        row = [
            x[0], x[1], x[2],
            x[3] / 1000,  # Bien tro: V
            x[4] / 10,    # Anh sang: %
            x[5] / 100,   # Nhiet do: do C
            x[6] / 1000,  # Pin thuc: V
            x[7] / 1000,  # VDDA: V
            *x[8:13]
        ]
        data.append(row)

        if len(data) % 10 == 0:
            print(f"{len(data)} mau | Bien tro {row[3]:.3f} V"
                  f" | Pin {row[6]:.3f} V")

if not data:
    raise SystemExit("Khong nhan duoc dong CSV 13 cot. Kiem tra COM, baud va PA9-RXD.")

with open("d08_data.csv", "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f)
    writer.writerow(headers)
    writer.writerows(data)

t = [(row[0] - data[0][0]) / 1000 for row in data]

fig, axes = plt.subplots(4, 1, figsize=(10, 9), sharex=True)
series = [
    (3, "Bien tro", "V"),
    (4, "Anh sang", "%"),
    (5, "Nhiet do chip", "°C"),
    (6, "Dien ap pin thuc", "V"),
]

for ax, (col, title, unit) in zip(axes, series):
    ax.plot(t, [row[col] for row in data])
    ax.set_ylabel(unit)
    ax.set_title(title)
    ax.grid(True)

axes[-1].set_xlabel("Thoi gian (s)")
fig.tight_layout()
fig.savefig("d08_plot.png", dpi=160)
print(f"Da luu {len(data)} mau vao d08_data.csv va d08_plot.png")
plt.show()