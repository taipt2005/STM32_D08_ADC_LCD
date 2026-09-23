

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
- 
👥 Phân công nhiệm vụ chi tiết
-SV1 - Module ADC & Cảm biến: : Trần Tuấn Đạt

Cấu hình ADC đa kênh kết hợp DMA quét tự động.

Viết hàm khởi tạo, đọc giá trị thô và chuyển đổi sang điện áp/thông số cảm biến.

Đẩy code lên nhánh: feature/adc-sensor

-SV2 - Module LCD I2C: Trần Đình Khôi

Viết thư viện giao tiếp LCD 1602 qua module I2C (PCF8574).

Viết các hàm hiển thị chuỗi, số nguyên, số thực và định dạng giao diện hiển thị.

Đẩy code lên nhánh: feature/lcd-i2c

-SV3 - Module Timer & Định thời: Nguyễn Anh Dũng

Cấu hình Timer (TIM2) tạo ngắt định kỳ lấy mẫu dữ liệu.

Viết các hàm tạo độ trễ (delay) chính xác và tạo nhịp quét hiển thị.

Đẩy code lên nhánh: feature/timer-delay

-SV4 (Nguyễn Xuân Tài - Leader) - Trưởng nhóm & Tích hợp:

Khởi tạo cấu trúc dự án mẫu từ STM32CubeMX, thiết lập Repository và phân nhánh Git.

Review code và duyệt Pull Request (PR) từ các thành viên.

Tích hợp các module vào main.c, kiểm thử trên phần cứng thực tế và hoàn thiện báo cáo.
