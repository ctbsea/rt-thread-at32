#AT32F423

温度监测+显示+蜂鸣

## DHT11温度模块

|  名称 | 注释  |
| ------------ | ------------ |
| VDD  | 供电 3－5.5VDC  |
| DATA  | 串行数据，单总线  |
| NC  |  空脚 |
| GND  |  接地，电源负极 |

dht11在线包
#### 文档

https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/sensor/sensor

#### 引包

> 	外围设备和驱动->sensors drivers-> dht11: Digital temperature and humdity....
	可以开启sample

#### pin脚
> `#define DHT11_DATA_PIN    GET_PIN(B, 12)`
	此为data连线的pin口，对应开发版的，可以自由设定连接


## ST7735高清SPI 1.44寸
LVGL-v8.3.11
AT32 work bench 查看 SPI1 针脚
SCL  PA5
SDA  PA7
RST  PB0
DC   PB1
Cs   PA4
BLK  PB2
基于LVGL开发



## 蜂鸣器
PWM1 CH1 PA8
当温度大于？度，响
