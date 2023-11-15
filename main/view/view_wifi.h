/**
 * @file view_wifi.h
 * @date  15 November 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */

#ifndef VIEW_WIFI_H
#define VIEW_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl.h"

void wifi_list_event_init(void);

/*For View Event*/
void view_event_wifi_st(void *event_data);
void view_event_wifi_list(void *event_data);
void view_event_wifi_connet_ret(void *event_data);
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*VIEW_WIFI_H*/