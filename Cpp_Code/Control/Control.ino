#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>
int l = 0;

//摇杆引脚
const int ana=32;   //舵机控制
const int LED1 = 25; //舵机状态灯

const int car_v=33; //电调控制
const int LED2 = 26; //电调状态灯

// 设置数据结构体
typedef struct struct_message {
  int velocity;
  int pos;
} struct_message;

struct_message myData;

// 接收设备的 MAC 地址
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xE7, 0x44, 0x28};
                            //0x08, 0xD1, 0xF9, 0xE6, 0xCC, 0xEC

void setup() {
  Serial.begin(115200);

  //启用端口
  pinMode(ana,INPUT);
  pinMode(car_v,INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // 初始化 ESP-NOW(if->while)
  WiFi.mode(WIFI_STA);
  while (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    delay(100);
  }

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
  myData.pos=90;
  myData.velocity=0;
}

void loop() {
  //读取并处理摇杆数据
  myData.pos = analogRead(ana);
  analogWrite(LED1, abs((myData.pos-1877)/16));
  // Serial.println(myData.pos);

  if(myData.pos<1880){
    myData.pos= map(myData.pos,0,1880,160,75);
  }else{
    myData.pos= map(myData.pos,1880,4095,75,0);
  }
  //(double)analogRead(ana)*8/439+105.38;
  // Serial.println(myData.pos);


  myData.velocity=analogRead(car_v);
  // Serial.println(myData.velocity);
  analogWrite(LED2, abs(myData.velocity-2322)/16);
  myData.velocity=map(myData.velocity,4095,0,180,0);
  Serial.println(myData.velocity);

  // 调试时使用
  
  // Serial.println(myData.velocity);

  //发送数据
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  delay(10);
}