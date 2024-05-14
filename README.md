# Remote-controlled-boats

遥控小船的代码、文档.etc

## 一.代码端

### 1.发送端

(1)控制数据读取
在发送端,使用一个回复拉杆和一个非回复拉杆分别控制船的转向和行船速度。

```c
// 设置数据结构体
typedef struct struct_message {
  int velocity; 
  int pos;
} struct_message;
```

在setup()中设置舵机初始状态

```c
  myData.pos=135;
  myData.velocity=0;
```

在loop()中读取数据并处理数据

```c
  //读取并处理摇杆数据
  myData.pos=(double)analogRead(ana)*8/439+105.38;
  myData.velocity=analogRead(car_v);
  myData.velocity=map(myData.velocity,0,4095,0,180);
```

(2)数据发送
使用WIFI模块实现将数据从发送端发送到接受端。

```c
#include <WiFi.h>//WIFI模块
#include <esp_now.h>//通信协议

// 接收设备的 MAC 地址
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xE7, 0x44, 0x28};
```

在setup()中通信初始化

```c
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
```

在loop()中发送处理后的数据

```c
  //发送数据
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  delay(10);
```

### 2.接收端

(1)数据接收
同样使用WiFi模块接受数据

```c
#include <WiFi.h>
#include <esp_now.h>
```

使用多核操作,使得能够同时异步控制舵机和船马达。

```c
//多核操作
TaskHandle_t Task1;
TaskHandle_t Task0;

// 数据接收回调函数
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
}
```

在setup()函数内设置接收数据回调函数

```c
  // 设置接收数据回调函数
  esp_now_register_recv_cb(OnDataRecv);
```

(2)对舵机和马达的控制
使用esp专有的舵机库实现对舵机以及电调的控制

```c
#include <ESP32Servo.h>
//创建舵机控制对象及电调控制对象
Servo myServo; 
const int servo_pin=32;
Servo ESC;
const int ESC_pin=33;
```

在setup()函数内初始化两个控制实例

```c
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
                    1);          /* pin task to core 0 */
```

双核分别控制舵机和电调

```c
//Core_0操作
void Task1code( void * pvParameters ){
  for(;;){
    //打印当前速度,同时防止看门狗
    Serial.println(myData.velocity);

    //控制电调
    ESC.write(myData.velocity);
  }
}

//Core_1操作
void Task0code( void * pvParameters ){
  for(;;){
    //控制舵机
    myServo.write(myData.pos); 
  } 
}

void loop() {}
```