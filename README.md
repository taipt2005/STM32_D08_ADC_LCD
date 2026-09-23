# STM32_D08_ADC_LCD

Đồ án Đề tài D08: Thu thập dữ liệu ADC đa kênh hiển thị LCD (STM32F103C8T6)

## 📌 Tính năng chính
- Khởi tạo ADC đa kênh kết hợp DMA quét liên tục các kênh cảm biến.
- Sử dụng Timer (TIM2) để tạo chu kỳ lấy mẫu / quét định kỳ.
- Giao tiếp I2C hiển thị thông số thu thập được lên màn hình LCD1602.

## 🛠 Phần cứng & Công cụ
- Vi điều khiển: STM32F103C8T6 (Blue Pill)
- Mạch nạp: ST-Link V2
- Trình biên dịch / IDE: Keil MDK v5, STM32CubeMX
- Mạch hiển thị: LCD 1602 + Mô-đun I2C PCF8574
- ADC1 : Biến trở 10K
- ADC2 : LDR
- ADC3 : LM35
