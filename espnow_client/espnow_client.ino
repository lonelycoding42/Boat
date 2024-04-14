#include <WiFi.h>
#include <esp_now.h>

//摇杆引脚
const int ana=32; //行船方向 回复拉杆
const int car_v=33; //行船速度 非回复拉杆

// //RXTX数据接收
// byte getData[2];

// 设置数据结构体
typedef struct struct_message {
  int velocity;
  int pos;
} struct_message;

struct_message myData;

// 接收设备的 MAC 地址
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xE7, 0x44, 0x28};//08:D1:F9:E7:44:28

// //数据发送回调函数
// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   char macStr[18];
//   Serial.print("Packet to: ");
//   snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
//            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
//   Serial.println(macStr);
//   Serial.print("Send status: ");
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
//   Serial.println();
// }

// // 中断处理函数(for RXTX)
// void serial2ISR() {
//   // 检查是否有可用数据从 ESP32 的串口2 接收
//   if (Serial2.available()) {
//     // 读取 2 个字节到 receivedData 数组中
//     Serial2.readBytes(getData, 2);
//   }
//   myData.pos=getData[0];
//   myData.velocity=getData[1];
//   Serial.println(myData.pos);
//   Serial.println(myData.velocity);
// }

void setup() {
  //启用端口
  pinMode(ana,INPUT);
  pinMode(car_v,INPUT);

  //初始化串口
  Serial.begin(115200);
  Serial2.begin(921600);

  // 初始化 ESP-NOW(if->while)
  WiFi.mode(WIFI_STA);
  while (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    delay(100);
  }

  // //设置发送数据回调函数
  // esp_now_register_send_cb(OnDataSent);

  //绑定数据接收端
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  //使能wifi
  peerInfo.ifidx = WIFI_IF_STA;

  //检查设备是否配对成功(if->while)
  while (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    delay(100);
  }

  //初始化舵机模块
  myData.pos=135;
  myData.velocity=0;

  // //串口通信
  // pinMode(16, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(16), serial2ISR, FALLING);
}

void loop() {
  //读取并处理摇杆数据
  myData.pos=(double)analogRead(ana)*8/439+105.38;
  myData.velocity=analogRead(car_v);
  myData.velocity=map(myData.velocity,0,4095,0,180);

  // //串口使用
  Serial.println(myData.pos);
  Serial.println(myData.velocity);

  //发送数据
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  delay(10);

  // // 检查数据是否发送成功
  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // }
  // else {
  //   Serial.println("Error sending the data");
  // }
}