/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2019-08-01     LuoGong         the first version.
 * 2019-08-15     MurphyZhao      add lock and modify code style
 */

#include <dht11.h>

#define DBG_LVL  DBG_INFO
#include <rtdbg.h>


void open_device()
{

    rt_uint8_t get_data_freq = 1; /* 1Hz */

    dev = rt_device_find(DHT11_DEV_NAME);
    if (dev == RT_NULL)
    {
        return;
    }

    rt_err_t te =  rt_device_open(dev, RT_DEVICE_FLAG_RDONLY);
    if (te != RT_EOK)
    {
        rt_kprintf("open device failed!  %d\n", te);
        return;
    }

    rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)(&get_data_freq));
}

INIT_APP_EXPORT(open_device);

struct temp_result read_tmp()
{
    struct rt_sensor_data sensor_data;
    rt_size_t res;
    struct temp_result tr;
    res = rt_device_read(dev, 0, &sensor_data, 1);

           if (res != 1)
           {
               rt_kprintf("read data failed! result is %d\n", res);
               rt_device_close(dev);
           }
           else
           {
               if (sensor_data.data.temp >= 0)
               {
                   tr.temp = (sensor_data.data.temp & 0xffff) >> 0;
                   tr.humi = (sensor_data.data.temp & 0xffff0000) >> 16;
                   rt_kprintf("1. temp:%d, humi:%d\n" ,tr.temp, tr.humi);
               }
           }
           return tr;
}


static int rt_hw_dht11_port(void)
{
    struct rt_sensor_config cfg;
    
    cfg.intf.user_data = (void *)DHT11_DATA_PIN;
    rt_hw_dht11_init(DHT11_REG_DEV_NAME, &cfg);

    return RT_EOK;
}

INIT_COMPONENT_EXPORT(rt_hw_dht11_port);
