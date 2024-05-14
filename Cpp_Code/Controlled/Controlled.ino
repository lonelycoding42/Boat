#include "PID.h"
#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

//多核操作
TaskHandle_t Task1;
TaskHandle_t Task0;

//创建舵机控制对象及电调控制对象
Servo myServo; 
const int servo_pin=33;
int angle_now;
Servo ESC;
const int ESC_pin=32;

// 设置数据结构体
typedef struct struct_message {
  int velocity;
  int pos;
} struct_message;
struct_message myData;

// 数据接收回调函数
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
}

void setup() {
  //启用串口
  Serial.begin(115200);

  // 初始化 ESP-NOW
  WiFi.mode(WIFI_STA);
  while (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    delay(100);
  }

  // 设置接收数据回调函数
  esp_now_register_recv_cb(OnDataRecv);

  // 舵机初始化
  myServo.attach(servo_pin);
  myData.pos=75;
  angle_now=myData.pos;

  //PWM,启动!
  ESC.attach(ESC_pin,1000,2000);
  myData.velocity=0;

  //Core_0调用,控制电调
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */

  //Core_1调用,控制舵机
  xTaskCreatePinnedToCore(
                    Task0code,   /* Task function. */
                    "Task0",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task0,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
}

//Core_0操作
void Task1code( void * pvParameters ){
  for(;;){
    //打印当前速度,同时防止看门狗
    Serial.println(myData.velocity);

    //控制电调
    ESC.write(myData.velocity/2);
  }
}

//Core_1操作
void Task0code( void * pvParameters ){
  for(;;){
    Serial.println(myData.pos);
    Serial.println(angle_now);
    //控制舵机
    if(myData.pos>angle_now){
      angle_now+=10;
      angle_now=angle_now<myData.pos?angle_now:myData.pos;
    }
    else if(myData.pos<angle_now){
      angle_now-=10;
      angle_now=angle_now>myData.pos?angle_now:myData.pos;
    }
    myServo.write(angle_now);
  }
}

void loop() {

}