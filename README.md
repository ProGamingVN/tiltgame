# esp32-mpu-tilt-games

Điều khiển game trên trình duyệt bằng cách nghiêng module **ESP32 SuperMini + MPU6050**,
truyền dữ liệu qua **USB Serial** và đọc trực tiếp bằng **Web Serial API** — không cần
server, không cần app trung gian.

## Nội dung

| File | Mô tả |
|---|---|
| `firmware/esp32_mpu6050_firmware.ino` | Firmware nạp cho ESP32, đọc góc nghiêng từ MPU6050 và gửi qua Serial (115200 baud), định dạng mỗi dòng `roll,pitch`. |
| `web/tilt_game.html` | Game "Né Vật Cản" — điều khiển quả bóng né các khối rơi xuống bằng cách nghiêng module. |
| `web/fruit_ninja_3d.html` | Game "Chém Hoa Quả 3D" dựng bằng Three.js — quét lưỡi dao bằng cách nghiêng module để chém trái cây, né bom. |

Cả 2 game đều dùng chung 1 firmware, và đều có chế độ chơi dự phòng bằng chuột/bàn phím khi chưa có board.

## Phần cứng

| MPU6050 | ESP32 SuperMini |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO4 |
| SCL | GPIO5 |

> Chân I2C trên board "SuperMini" tùy nhà sản xuất — nếu board bạn khác, sửa `SDA_PIN`/`SCL_PIN` đầu file `.ino`.

## Cài đặt & chạy

1. **Nạp firmware:** mở `firmware/esp32_mpu6050_firmware.ino` bằng Arduino IDE, cài board package `esp32 by Espressif Systems` và thư viện `Adafruit MPU6050`, `Adafruit Unified Sensor`, `Adafruit BusIO`, rồi nạp vào board. Kiểm tra qua Serial Monitor (115200 baud) thấy roll,pitch chạy là được — nhớ đóng Serial Monitor lại trước khi mở game.
2. **Chơi game:** mở file `web/tilt_game.html` hoặc `web/fruit_ninja_3d.html` bằng **Chrome hoặc Edge** (double-click là chạy, không cần server). Bấm "Kết nối ESP32" → chọn cổng COM/USB → "Bắt đầu chơi".

Chỉ hỗ trợ Chrome/Edge trên desktop (Web Serial API chưa có trên Firefox/Safari/điện thoại).

## Giấy phép

MIT — xem file [LICENSE](LICENSE).
