#include <rtthread.h>
#include <rtdevice.h>
#include "sensor_dallas_dht11.h"
#include "drv_gpio.h"


#define DHT11_DEV_NAME  "temp_dht"
#define DHT11_REG_DEV_NAME  "dht"
#define DHT11_DATA_PIN    GET_PIN(B, 12)

struct temp_result {
     uint8_t temp;
     uint8_t humi;
};

static  rt_device_t dev;

struct temp_result read_tmp(); // 此处必须struct

