#include <rtthread.h>
#include <rtdevice.h>
#include "sensor.h"
#include "sensor_dallas_dht11.h"
#include "drv_gpio.h"


#define DHT11_DEV_NAME  "dht11"
#define DHT11_DATA_PIN    GET_PIN(B, 12)
