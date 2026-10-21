import csv
import queue
import threading
import time
from collections import deque
from datetime import datetime
from pathlib import Path

import tkinter as tk
from tkinter import ttk, filedialog, messagebox

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    raise SystemExit('Thieu pyserial. Cai: pip install pyserial matplotlib')
try:
    from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
    from matplotlib.figure import Figure
except ImportError:
    raise SystemExit('Thieu matplotlib. Cai: pip install matplotlib')

FIELDS = [
    'time_ms', 'frames', 'skipped', 'pot_v', 'light_pct', 'temp_c',
    'battery_v', 'vdda_v', 'raw0', 'raw1', 'raw2', 'raw3', 'raw4'
]
SERIES = [
    ('pot_v', 'Biến trở', 'V', '#60a5fa'),
    ('light_pct', 'Ánh sáng', '%', '#fbbf24'),
    ('temp_c', 'Nhiệt độ chip', '°C', '#fb7185'),
    ('battery_v', 'Điện áp pin', 'V', '#34d399'),
    ('vdda_v', 'Nguồn VDDA', 'V', '#a78bfa'),
]
BG, PANEL, LINE = '#0b1220', '#142137', '#293a54'
TEXT, MUTED, ACCENT = '#ecf3ff', '#9bb1ca', '#38bdf8'


def parse_line(line):
    parts = line.strip().split(',')
    if len(parts) != 13:
        return None
    try:
        x = [int(value.strip()) for value in parts]
    except ValueError:
        return None
    return dict(zip(FIELDS, [x[0], x[1], x[2], x[3] / 1000,
                             x[4] / 10, x[5] / 100, x[6] / 1000,
                             x[7] / 1000, *x[8:13]]))


class D08Dashboard(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title('D08 | STM32 ADC Monitoring Dashboard')
        self.geometry('1260x850')
        self.minsize(1000, 700)
        self.configure(bg=BG)
        self.rx_queue = queue.Queue()
        self.stop_event = threading.Event()
        self.thread = None
        self.connection = None
        self.connected = False
        self.recording = False
        self.data = []
        self.buffer = deque(maxlen=500)
        self.bad_lines = 0
        self.samples = 0
        self.start_time = None
        self.last_plot = 0
        self.last_stats = 0
        self.card_values = {}
        self.port = tk.StringVar(value='COM8')
        self.baud = tk.StringVar(value='115200')
        self.window_s = tk.StringVar(value='30')
        self.status = tk.StringVar(value='● Chưa kết nối')
        self.stats = tk.StringVar(value='0 mẫu  •  0 dòng lỗi')
        self._style()
        self._ui()
        self.refresh_ports()
        self.after(70, self.poll_queue)
        self.protocol('WM_DELETE_WINDOW', self.on_close)

    def _style(self):
        style = ttk.Style(self)
        style.theme_use('clam')
        style.configure('TCombobox', padding=6, fieldbackground='#f4f8fd', foreground='#13233a')
        style.configure('TButton', font=('Segoe UI', 10), padding=(12, 8))
        style.configure('Accent.TButton', font=('Segoe UI', 10, 'bold'), padding=(14, 9))

    def label(self, parent, text, size=10, color=TEXT, bold=False, **kwargs):
        return tk.Label(parent, text=text, bg=kwargs.pop('bg', BG), fg=color,
                        font=('Segoe UI', size, 'bold' if bold else 'normal'), **kwargs)

    def _ui(self):
        top = tk.Frame(self, bg=BG, padx=24, pady=17)
        top.pack(fill='x')
        self.label(top, 'D08  /  ADC MULTI-CHANNEL', 20, TEXT, True).pack(anchor='w')
        self.label(top, 'STM32F103 • Giám sát dữ liệu cảm biến qua UART • 13 trường CSV',
                   10, MUTED).pack(anchor='w', pady=(4, 0))

        tools = tk.Frame(self, bg=PANEL, padx=15, pady=14)
        tools.pack(fill='x', padx=24)
        self.label(tools, 'Cổng COM', color=MUTED, bg=PANEL).pack(side='left')
        self.port_box = ttk.Combobox(tools, textvariable=self.port, width=12)
        self.port_box.pack(side='left', padx=(8, 10))
        ttk.Button(tools, text='↻ Cập nhật', command=self.refresh_ports).pack(side='left', padx=(0, 18))
        self.label(tools, 'Baud', color=MUTED, bg=PANEL).pack(side='left')
        ttk.Combobox(tools, textvariable=self.baud, values=['9600', '57600', '115200', '230400'],
                     width=10, state='readonly').pack(side='left', padx=(8, 18))
        self.connect_btn = ttk.Button(tools, text='Kết nối', style='Accent.TButton', command=self.toggle_connection)
        self.connect_btn.pack(side='left')
        self.record_btn = ttk.Button(tools, text='● Bắt đầu ghi', command=self.toggle_record, state='disabled')
        self.record_btn.pack(side='left', padx=8)
        ttk.Button(tools, text='Xuất CSV', command=self.export_csv).pack(side='left', padx=5)
        ttk.Button(tools, text='Lưu biểu đồ', command=self.export_plot).pack(side='left', padx=5)

        statrow = tk.Frame(self, bg=BG, padx=25, pady=12)
        statrow.pack(fill='x')
        self.label(statrow, '', bg=BG, color=ACCENT, textvariable=self.status).pack(side='left')
        self.label(statrow, '', bg=BG, color=MUTED, textvariable=self.stats).pack(side='right')

        cards = tk.Frame(self, bg=BG, padx=19)
        cards.pack(fill='x')
        for idx, (key, title, unit, color) in enumerate(SERIES):
            box = tk.Frame(cards, bg=PANEL, padx=16, pady=16, highlightbackground=LINE, highlightthickness=1)
            box.grid(row=0, column=idx, sticky='nsew', padx=5)
            cards.grid_columnconfigure(idx, weight=1)
            self.label(box, title, 10, MUTED, bg=PANEL).pack(anchor='w')
            v = self.label(box, '—', 23, color, True, bg=PANEL)
            v.pack(anchor='w', pady=(8, 1))
            self.label(box, unit, 9, MUTED, bg=PANEL).pack(anchor='w')
            self.card_values[key] = v

        controls = tk.Frame(self, bg=BG, padx=24, pady=14)
        controls.pack(fill='x')
        self.label(controls, 'BIỂU ĐỒ THỜI GIAN THỰC', 12, TEXT, True).pack(side='left')
        self.label(controls, 'Cửa sổ (giây):', 10, MUTED).pack(side='left', padx=(24, 7))
        ttk.Combobox(controls, textvariable=self.window_s, values=['10','30','60','120','300'],
                     width=6, state='readonly').pack(side='left')
        ttk.Button(controls, text='Xóa đồ thị', command=self.clear_view).pack(side='right')

        chart = tk.Frame(self, bg=PANEL, padx=10, pady=10, highlightbackground=LINE, highlightthickness=1)
        chart.pack(fill='both', expand=True, padx=24, pady=(0, 16))
        self.fig = Figure(figsize=(11, 5.2), dpi=100, facecolor=PANEL)
        self.axes = []
        self.lines = []
        for i, (_, title, unit, color) in enumerate(SERIES):
            ax = self.fig.add_subplot(5, 1, i+1, sharex=self.axes[0] if self.axes else None)
            ax.set_facecolor(PANEL)
            ax.tick_params(colors=MUTED, labelsize=8)
            for side in ax.spines.values():
                side.set_color(LINE)
            ax.grid(alpha=.15, color='#dbeafe')
            ax.set_ylabel(unit, color=MUTED, fontsize=9, rotation=0, labelpad=22)
            ax.text(.01, .85, title, transform=ax.transAxes, color=color, fontsize=9, fontweight='bold')
            if i < 4:
                ax.tick_params(labelbottom=False)
            ln, = ax.plot([], [], color=color, lw=1.8)
            self.axes.append(ax)
            self.lines.append(ln)
        self.axes[-1].set_xlabel('Thời gian từ lúc nhận mẫu đầu (giây)', color=MUTED, fontsize=9)
        self.fig.subplots_adjust(left=.09, right=.98, top=.98, bottom=.1, hspace=.25)
        self.canvas = FigureCanvasTkAgg(self.fig, master=chart)
        self.canvas.get_tk_widget().pack(fill='both', expand=True)
        self.canvas.draw()
        bottom = tk.Frame(self, bg=BG, padx=24, pady=8)
        bottom.pack(fill='x')
        self.label(bottom, 'Ghi chú: raw0=POT • raw1=LDR • raw2=VBAT • raw3=TEMP • raw4=VREFINT',
                   9, MUTED).pack(side='left')
        self.label(bottom, 'Dữ liệu hiển thị là số đo STM32 gửi, không phải giá trị tham chiếu',
                   9, MUTED).pack(side='right')

    def refresh_ports(self):
        ports = [p.device for p in list_ports.comports()]
        self.port_box['values'] = ports
        if ports and self.port.get() not in ports:
            self.port.set(ports[0])

    def toggle_connection(self):
        if self.connected:
            self.disconnect()
            return
        try:
            port = self.port.get().strip()
            baud = int(self.baud.get())
            if not port:
                raise ValueError('Hãy chọn cổng COM.')
            self.connection = serial.Serial(port, baud, timeout=.3)
        except (OSError, ValueError, serial.SerialException) as exc:
            messagebox.showerror('Không kết nối được', f'{exc}\n\nKiểm tra cổng COM và đóng Serial Monitor/Plotter.')
            return
        self.connected = True
        self.stop_event.clear()
        self.thread = threading.Thread(target=self.reader, daemon=True)
        self.thread.start()
        self.status.set(f'● Đang kết nối {port} / {baud} baud')
        self.connect_btn.config(text='Ngắt kết nối')
        self.record_btn.config(state='normal')

    def disconnect(self):
        self.stop_event.set()
        self.connected = False
        self.recording = False
        self.record_btn.config(text='● Bắt đầu ghi', state='disabled')
        self.connect_btn.config(text='Kết nối')
        self.status.set('● Chưa kết nối')
        if self.connection:
            try:
                self.connection.close()
            except (OSError, serial.SerialException):
                pass
        self.connection = None

    def reader(self):
        ser = self.connection
        while not self.stop_event.is_set():
            try:
                raw = ser.readline()
                if not raw:
                    continue
                decoded = raw.decode('ascii', errors='ignore').strip()
                row = parse_line(decoded)
                self.rx_queue.put(('row', row) if row is not None else ('bad', None))
            except (OSError, serial.SerialException) as exc:
                if not self.stop_event.is_set():
                    self.rx_queue.put(('error', str(exc)))
                break

    def toggle_record(self):
        if not self.connected:
            return
        self.recording = not self.recording
        self.record_btn.config(text='■ Dừng ghi' if self.recording else '● Bắt đầu ghi')
        if self.recording:
            self.status.set('● Đang ghi dữ liệu')
        else:
            self.status.set('● Đang xem trực tiếp (tạm dừng ghi)')

    def poll_queue(self):
        updated = False
        count = 0
        while count < 1000:
            try:
                kind, value = self.rx_queue.get_nowait()
            except queue.Empty:
                break
            count += 1
            if kind == 'error':
                self.disconnect()
                messagebox.showerror('Lỗi UART', value)
                break
            if kind == 'bad':
                self.bad_lines += 1
                continue
            if self.start_time is None:
                self.start_time = value['time_ms']
            sec = (value['time_ms'] - self.start_time) / 1000.0
            # Xử lý cơ bản khi timestamp của board reset hoặc tràn.
            if self.buffer and sec < self.buffer[-1][0]:
                self.start_time = value['time_ms']
                self.buffer.clear()
                sec = 0.0
            self.buffer.append((sec, value))
            self.samples += 1
            if self.recording:
                self.data.append(dict(value))
            for key, _, unit, _ in SERIES:
                v = value[key]
                self.card_values[key].config(text=f'{v:.1f}' if key in ('light_pct','temp_c') else f'{v:.3f}')
            updated = True
        now = time.monotonic()
        if now - self.last_stats > .3:
            self.stats.set(f'{self.samples:,} mẫu  •  {len(self.data):,} mẫu đã ghi  •  {self.bad_lines:,} dòng lỗi')
            self.last_stats = now
        if updated and now - self.last_plot >= .25:
            self.redraw()
            self.last_plot = now
        self.after(70, self.poll_queue)

    def redraw(self):
        if not self.buffer:
            return
        window = float(self.window_s.get())
        max_t = self.buffer[-1][0]
        recent = [(t, v) for t, v in self.buffer if t >= max_t - window]
        if not recent:
            return
        tvals = [t for t, _ in recent]
        for ax, line, (key, _, _, _) in zip(self.axes, self.lines, SERIES):
            y = [v[key] for _, v in recent]
            line.set_data(tvals, y)
            lo, hi = min(y), max(y)
            margin = max((hi - lo) * .17, .02 if key != 'light_pct' else .5)
            ax.set_ylim(lo - margin, hi + margin)
            ax.set_xlim(max(0, max_t - window), max(max_t, window) if max_t < window else max_t+.05)
        self.canvas.draw_idle()

    def clear_view(self):
        self.buffer.clear()
        self.start_time = None
        self.samples = 0
        self.bad_lines = 0
        for v in self.card_values.values():
            v.config(text='—')
        for line in self.lines:
            line.set_data([], [])
        self.canvas.draw_idle()

    def export_csv(self):
        if not self.data:
            messagebox.showinfo('Chưa có dữ liệu', 'Bấm “Bắt đầu ghi” và chờ STM32 gửi dữ liệu trước khi xuất CSV.')
            return
        default = f'd08_data_{datetime.now():%Y%m%d_%H%M%S}.csv'
        path = filedialog.asksaveasfilename(defaultextension='.csv', initialfile=default,
                                           filetypes=[('CSV files','*.csv')])
        if not path:
            return
        try:
            with open(path, 'w', newline='', encoding='utf-8-sig') as handle:
                writer = csv.DictWriter(handle, fieldnames=FIELDS)
                writer.writeheader()
                writer.writerows(self.data)
        except OSError as exc:
            messagebox.showerror('Không lưu được', str(exc))
            return
        messagebox.showinfo('Đã xuất dữ liệu', f'Đã lưu {len(self.data)} mẫu:\n{path}')

    def export_plot(self):
        path = filedialog.asksaveasfilename(defaultextension='.png',
                initialfile=f'd08_plot_{datetime.now():%Y%m%d_%H%M%S}.png',
                filetypes=[('PNG image', '*.png')])
        if not path:
            return
        try:
            self.fig.savefig(path, dpi=180, facecolor=self.fig.get_facecolor())
        except OSError as exc:
            messagebox.showerror('Không lưu được', str(exc))
            return
        messagebox.showinfo('Đã lưu', path)

    def on_close(self):
        self.disconnect()
        self.destroy()


if __name__ == '__main__':
    D08Dashboard().mainloop()
