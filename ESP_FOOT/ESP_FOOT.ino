#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Cấu trúc dữ liệu gửi qua ESP-NOW
struct SensorData {
  int16_t ax;
  int16_t ay;
  int16_t az;
};

SensorData dataFoot;

// MAC của ESP tay
uint8_t macTay[] = {0xC8, 0x2B, 0x96, 0x14, 0xAF, 0xB0};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(macTay, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("Warning: Không kết nối được MPU6050!");
  } else {
    Serial.println("ESP CHÂN khởi động xong.");
  }
}

void loop() {
  // Lấy giá trị thô từ MPU6050
  int16_t raw_ax, raw_ay, raw_az;
  mpu.getAcceleration(&raw_ax, &raw_ay, &raw_az);

  // Gửi dữ liệu thô (không lọc)
  dataFoot.ax = raw_ax;
  dataFoot.ay = raw_ay;
  dataFoot.az = raw_az;

  // Gửi qua ESP-NOW
  esp_now_send(macTay, (uint8_t*)&dataFoot, sizeof(dataFoot));

  // In dữ liệu thô cho Serial Plotter
  Serial.print(dataFoot.ax);
  Serial.print(",");
  Serial.print(dataFoot.ay);
  Serial.print(",");
  Serial.println(dataFoot.az);

}