# D08 – Thu thập dữ liệu ADC đa kênh hiển thị LCD

Dự án sử dụng STM32F103C8T6 thu thập dữ liệu biến trở, ánh sáng, nhiệt độ nội và điện áp pin bằng ADC kết hợp Timer và DMA. Dữ liệu được lọc trung bình, hiệu chỉnh bằng VREFINT, hiển thị trên LCD1602 I2C và gửi qua UART dạng CSV để vẽ đồ thị bằng Python. Hỗ trợ nút chuyển trang và cảnh báo ngưỡng từng kênh, trong đó nhiệt độ từ 30°C sẽ cảnh báo.

## Các module
- ADC: Quét 5 kênh gồm 4 đại lượng đo và VREFINT.
- Xử lý: Lọc trung bình, quy đổi V/°C/% và kiểm tra ngưỡng.
- LCD: Hiển thị từng kênh, màn hình tổng hợp và cảnh báo.
- UART: Gửi CSV, lưu dữ liệu và vẽ đồ thị bằng Python.

## Công cụ
STM32CubeMX, Keil C, GitHub và Python.
