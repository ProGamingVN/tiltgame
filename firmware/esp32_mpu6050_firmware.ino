/*
  ESP32 SuperMini + MPU6050 -> Serial (USB)
  ------------------------------------------
  Đọc góc roll/pitch từ MPU6050 và gửi liên tục qua Serial (USB)
  cho máy tính, dùng để điều khiển game (tilt_game.py).

  Định dạng gửi đi mỗi dòng: "roll,pitch\n"   (đơn vị: độ)

  THƯ VIỆN CẦN CÀI (Arduino IDE > Tools > Manage Libraries):
    - Adafruit MPU6050
    - Adafruit Unified Sensor
    - Adafruit BusIO

  BOARD (Tools > Board):
    - Nếu ESP32-C3 SuperMini  -> chọn "ESP32C3 Dev Module"
    - Nếu ESP32-S3 SuperMini  -> chọn "ESP32S3 Dev Module"

  ĐẤU DÂY (mặc định code này dùng SDA=GPIO4, SCL=GPIO5 để tránh
  đụng LED/nút Boot thường nằm ở GPIO8/9 trên nhiều board SuperMini):
    MPU6050 VCC -> 3V3
    MPU6050 GND -> GND
    MPU6050 SDA -> GPIO4
    MPU6050 SCL -> GPIO5

  Nếu board bạn khác, chỉ cần đổi 2 định nghĩa SDA_PIN / SCL_PIN bên dưới.
*/

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ---- Cấu hình chân I2C (đổi nếu board bạn khác) ----
#define SDA_PIN 4
#define SCL_PIN 5

Adafruit_MPU6050 mpu;

// Bộ lọc trung bình động (EMA) để làm mượt tín hiệu, đỡ rung/nhiễu
float rollFiltered  = 0;
float pitchFiltered = 0;
const float ALPHA = 0.2;   // 0..1, càng nhỏ càng mượt nhưng trễ hơn

// Giá trị "hiệu chỉnh gốc" (offset) khi giữ module ở vị trí trung tính
float rollOffset  = 0;
float pitchOffset = 0;

void calibrate() {
  // Lấy trung bình 100 mẫu để xác định vị trí "nằm ngang" của module
  float sumRoll = 0, sumPitch = 0;
  const int N = 100;
  for (int i = 0; i < N; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float roll  = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
    float pitch = atan2(-a.acceleration.x,
                    sqrt(a.acceleration.y * a.acceleration.y +
                         a.acceleration.z * a.acceleration.z)) * 180.0 / PI;
    sumRoll  += roll;
    sumPitch += pitch;
    delay(5);
  }
  rollOffset  = sumRoll / N;
  pitchOffset = sumPitch / N;
}

void setup() {
  Serial.begin(115200);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 3000) delay(10); // chờ tối đa 3s cho Serial sẵn sàng

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!mpu.begin()) {
    Serial.println("LOI: Khong tim thay MPU6050. Kiem tra day noi!");
    while (1) delay(1000);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(200);

  // Giữ module nằm yên, ngang trong lúc hiệu chỉnh (calibrate)
  Serial.println("Dang hieu chinh... giu module nam yen 1 giay.");
  calibrate();
  Serial.println("READY");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float roll  = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  float pitch = atan2(-a.acceleration.x,
                  sqrt(a.acceleration.y * a.acceleration.y +
                       a.acceleration.z * a.acceleration.z)) * 180.0 / PI;

  roll  -= rollOffset;
  pitch -= pitchOffset;

  rollFiltered  = ALPHA * roll  + (1 - ALPHA) * rollFiltered;
  pitchFiltered = ALPHA * pitch + (1 - ALPHA) * pitchFiltered;

  Serial.print(rollFiltered, 2);
  Serial.print(",");
  Serial.println(pitchFiltered, 2);

  delay(20); // ~50Hz, đủ mượt cho game
}
