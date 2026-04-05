#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

typedef struct {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
} SensorData;

SensorData dataFoot;   
SensorData dataHand;   

const char* ssid = "ESP_TAY";
const char* password = "12345678";

WiFiServer server(8888);
WiFiClient client;

#define BUZZER D5   

void onReceiveFoot(uint8_t *mac, uint8_t *data, uint8_t len) {
  memcpy(&dataFoot, data, sizeof(dataFoot));
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  Wire.begin();
  mpu.initialize();

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  Serial.println("AP: ESP_TAY - IP: " + WiFi.softAPIP().toString());

  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onReceiveFoot);

  server.begin();
  server.setNoDelay(true);
  Serial.println("Sẵn sàng - Chờ Python kết nối...");
}

void loop() {
  
  if (!client.connected()) {
    client = server.available();
    if (client) Serial.println("Python đã kết nối!");
  }

  // Nhận lệnh FOG 
  if (client.connected() && client.available()) {
    String cmd = client.readStringUntil('\n');
    cmd.trim();
    if (cmd == "FOG") {
      tone(BUZZER, 3500, 3000);   // 3500Hz trong 3s 
      Serial.println("FOG → LOA KÊU 3S (3500Hz - SIÊU TO)");
    }
  }

  //Gửi dữ liệu cảm biến về Python
  if (client.connected()) {
    mpu.getMotion6(&dataHand.ax, &dataHand.ay, &dataHand.az,
                   &dataHand.gx, &dataHand.gy, &dataHand.gz);

    String msg = String(dataFoot.ax) + "," +
                 String(dataFoot.ay) + "," +
                 String(dataFoot.az) + "," +
                 String(dataHand.ax) + "," +
                 String(dataHand.ay) + "," +
                 String(dataHand.az) + "\n";

    client.print(msg);
  }

}