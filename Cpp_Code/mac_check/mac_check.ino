#include <WiFi.h>
// #include "esp_wifi.h"
// #include "WiFi.h"

void setup() {
  Serial.begin(115200);
  Serial.println();
#ifdef ESP8266
  Serial.print("ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
#elif defined ESP32
  WiFi.mode(WIFI_MODE_STA);
  Serial.print("ESP32 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
#endif
}

void loop() {

}

/* 项目使用 705125 字节（53%）的程序存储空间。最大值为 1310720 字节。
 个全局变量使用 42256 个字节（12%）的动态内存，剩下 285424 个字节用于局部变量。最大值为 327680 字节。
esptool.py v4.5.1
Serial port COM3

A fatal error occurred: Could not open COM3, the port doesn't exist
上传失败: 上传错误：exit status 2*/

// #include "WiFi.h"
 
// void setup(){
//   Serial.begin(115200);
//   WiFi.mode(WIFI_MODE_STA);
//   Serial.println(WiFi.macAddress());
// }
 
// void loop(){
//   Serial.print("Min:");
// }
