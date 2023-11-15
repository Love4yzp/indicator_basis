/**
 * @file view_chart.h
 * @date  15 November 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */

#ifndef VIEW_CHART_H
#define VIEW_CHART_H
#include "lvgl.h"

typedef struct sensor_chart_display
{
    lv_color_t color;
    
    char name[32];
    char units[32];
    struct view_data_sensor_history_data *p_info;
} sensor_chart_display_t;

#ifdef __cplusplus
extern "C" {
#endif

void sensor_chart_event_init(void);

/*For View Event*/
void view_event_sensor_data(void *event_data);
void view_event_sensor_data_history(void *event_data);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*VIEW_CHART_H*/