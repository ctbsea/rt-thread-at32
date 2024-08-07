/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-19     78677       the first version
 */
#include "main.h"
#include <rtdevice.h>
#include <drv_gpio.h>
#include <../user/inc/lv_port_disp.h>
#include <../user/inc/dht11.h>
#include <../user/inc/beep.h>

#define max_ser 5

lv_obj_t * chart;
lv_chart_series_t * ser1 ;
lv_chart_series_t * ser2 ;

uint16_t freq_tab[12]  = {262, 277, 294, 311, 330, 349, 369, 392, 415, 440, 466, 494};
uint8_t beep_volume = 5;

void beep()
{
    for (int i = 0; i < 12; i++)
    {
       beep_set(freq_tab[i], beep_volume);
       beep_on();
       rt_thread_mdelay(500);
       beep_off();
       rt_thread_mdelay(500);
    }
}

void update_temp(lv_timer_t *timer) {
    struct temp_result tr = read_tmp();
    lv_chart_set_next_value(chart, ser1, tr.temp);
    lv_chart_set_next_value(chart, ser2, tr.humi);
    lv_chart_refresh(chart);
    if (tr.temp >= 32) {
        beep();
    }
}


void lv_init_temo_chart(void)
{
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "current temp and lumi ");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 5, 5);
    /*Create a chart*/
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 150, 100);
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 10);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, max_ser);
    /*Add two data series*/
    ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);

    lv_timer_create(update_temp, 5000, NULL); // 刷新
}


int main(void)
{
    LcdInit();
    lv_init();
    lv_port_disp_init();
    beep_init();
    lv_init_temo_chart();
    return 0;
}
