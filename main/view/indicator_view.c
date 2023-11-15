#include "indicator_view.h"
#include "indicator_wifi.h"

#include "ui.h"
#include "ui_helpers.h"
#include "indicator_util.h"

#include "esp_wifi.h"
#include <time.h>

#include "view_wifi.h"
#include "view_chart.h"
#include "view_time.h"

static const char *TAG = "view";

static void __view_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    lv_port_sem_take();
    switch (id)
    {
    case VIEW_EVENT_SCREEN_START:
    {
        uint8_t screen = *(uint8_t *)event_data;
        if (screen == SCREEN_WIFI_CONFIG)
        {
            lv_disp_load_scr(ui_ScreenWIFI);
        }
    }
    case VIEW_EVENT_TIME:
    {
        ESP_LOGI(TAG, "event: VIEW_EVENT_TIME");
        bool time_format_24 = true;
        if (event_data)
        {
            time_format_24 = *(bool *)event_data;
        }

        time_t now = 0;
        struct tm timeinfo = {0};
        char *p_wday_str;

        time(&now);
        localtime_r(&now, &timeinfo);
        char buf_h[3];
        char buf_m[3];
        char buf[6];
        int hour = timeinfo.tm_hour;

        if (!time_format_24)
        {
            if (hour >= 13 && hour <= 23)
            {
                hour = hour - 12;
            }
        }
        lv_snprintf(buf_h, sizeof(buf_h), "%02d", hour);
        lv_label_set_text(ui_hour_dis, buf_h);
        lv_snprintf(buf_m, sizeof(buf_m), "%02d", timeinfo.tm_min);
        lv_label_set_text(ui_min_dis, buf_m);

        lv_snprintf(buf, sizeof(buf), "%02d:%02d", hour, timeinfo.tm_min);
        lv_label_set_text(ui_time2, buf);
        lv_label_set_text(ui_time3, buf);

        switch (timeinfo.tm_wday)
        {
        case 0:
            p_wday_str = "Sunday";
            break;
        case 1:
            p_wday_str = "Monday";
            break;
        case 2:
            p_wday_str = "Tuesday";
            break;
        case 3:
            p_wday_str = "Wednesday";
            break;
        case 4:
            p_wday_str = "Thursday";
            break;
        case 5:
            p_wday_str = "Friday";
            break;
        case 6:
            p_wday_str = "Sunday";
            break;
        default:
            p_wday_str = "";
            break;
        }
        char buf1[32];
        lv_snprintf(buf1, sizeof(buf1), "%s, %02d / %02d / %04d", p_wday_str, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
        lv_label_set_text(ui_date, buf1);
        break;
    }

    case VIEW_EVENT_TIME_CFG_UPDATE:
    {
        ESP_LOGI(TAG, "event: VIEW_EVENT_TIME_CFG_UPDATE");
        struct view_data_time_cfg *p_cfg = (struct view_data_time_cfg *)event_data;

        if (p_cfg->time_format_24)
        {
            lv_dropdown_set_selected(ui_time_format_cfg, 0);
        }
        else
        {
            lv_dropdown_set_selected(ui_time_format_cfg, 1);
        }

        if (p_cfg->auto_update)
        {
            lv_obj_add_state(ui_auto_update_cfg, LV_STATE_CHECKED);
            lv_obj_add_flag(ui_date_time, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_clear_state(ui_auto_update_cfg, LV_STATE_CHECKED);
            lv_obj_clear_flag(ui_date_time, LV_OBJ_FLAG_HIDDEN);
        }

        struct tm *p_tm = localtime(&p_cfg->time);
        char buf[32];
        if (p_tm->tm_year + 1900 > 1970)
        {
            lv_snprintf(buf, sizeof(buf), "%02d/%02d/%d", p_tm->tm_mday, p_tm->tm_mon + 1, p_tm->tm_year + 1900);
            lv_textarea_set_text(ui_date_cfg, buf);
        }

        lv_roller_set_selected(ui_hour_cfg, p_tm->tm_hour, LV_ANIM_OFF);
        lv_roller_set_selected(ui_min_cfg, p_tm->tm_min, LV_ANIM_OFF);
        lv_roller_set_selected(ui_sec_cfg, p_tm->tm_sec, LV_ANIM_OFF);

        if (p_cfg->auto_update_zone)
        {
            lv_obj_add_state(ui_zone_auto_update_cfg, LV_STATE_CHECKED);
            lv_obj_add_flag(ui_time_zone, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_clear_state(ui_zone_auto_update_cfg, LV_STATE_CHECKED);
            lv_obj_clear_flag(ui_time_zone, LV_OBJ_FLAG_HIDDEN);
        }

        if (p_cfg->zone >= 0)
        {
            lv_dropdown_set_selected(ui_time_zone_sign_cfg_, 0);
            lv_dropdown_set_selected(ui_time_zone_num_cfg, p_cfg->zone);
        }
        else
        {
            lv_dropdown_set_selected(ui_time_zone_sign_cfg_, 1);
            lv_dropdown_set_selected(ui_time_zone_num_cfg, 0 - p_cfg->zone);
        }

        if (p_cfg->daylight)
        {
            lv_obj_add_state(ui_daylight_cfg, LV_STATE_CHECKED);
        }
        else
        {
            lv_obj_clear_state(ui_daylight_cfg, LV_STATE_CHECKED);
        }
        break;
    }

    case VIEW_EVENT_CITY:
    {
        ESP_LOGI(TAG, "event: VIEW_EVENT_CITY");
        char *p_data = (char *)event_data;
        lv_label_set_text(ui_city, p_data);
        break;
    }

    case VIEW_EVENT_DISPLAY_CFG:
    {
        ESP_LOGI(TAG, "event: VIEW_EVENT_DISPLAY_CFG");
        struct view_data_display *p_cfg = (struct view_data_display *)event_data;

        lv_slider_set_value(ui_brighness_cfg, p_cfg->brightness, LV_ANIM_OFF);
        if (p_cfg->sleep_mode_en)
        {
            lv_obj_clear_state(ui_screen_always_on_cfg, LV_STATE_CHECKED);
            lv_obj_clear_flag(ui_turn_off_after_time, LV_OBJ_FLAG_HIDDEN);
            char buf[32];
            lv_snprintf(buf, sizeof(buf), "%d", p_cfg->sleep_mode_time_min);
            lv_textarea_set_text(ui_turn_off_after_time_cfg, buf);
            // lv_label_set_text_fmt(ui_turn_off_after_time_cfg, "%"LV_PRIu32, p_cfg->sleep_mode_time_min);
        }
        else
        {
            lv_obj_add_state(ui_screen_always_on_cfg, LV_STATE_CHECKED);
            lv_obj_add_flag(ui_turn_off_after_time, LV_OBJ_FLAG_HIDDEN);
        }

        break;
    }

    case VIEW_EVENT_WIFI_ST:
        view_event_wifi_st(event_data);
        break;
    case VIEW_EVENT_WIFI_LIST: // Model sent a list of wifi networks
        view_event_wifi_list(event_data);
        break;

    case VIEW_EVENT_WIFI_CONNECT_RET:

        view_event_wifi_connet_ret(event_data);
        break;

    case VIEW_EVENT_SENSOR_DATA:
        view_event_sensor_data(event_data);
        break;
    case VIEW_EVENT_SENSOR_DATA_HISTORY:
        view_event_sensor_data_history(event_data);
        break;
        
    case VIEW_EVENT_SCREEN_CTRL:
    {
        bool *p_st = (bool *)event_data;

        if (*p_st == 1)
        {
            lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);
        }
        else
        {
            lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(lv_layer_top(), lv_color_black(), 0);
        }
        break;
    }
    case VIEW_EVENT_FACTORY_RESET:
    {
        ESP_LOGI(TAG, "event: VIEW_EVENT_FACTORY_RESET");
        lv_disp_load_scr(ui_screen_factory);
        break;
    }

    default:
        break;
    }
    lv_port_sem_give();
}

#include "view_chart.h"

int indicator_view_init(void)
{
    ui_init();

    wifi_list_event_init();
    sensor_chart_event_init();

    int i = 0;
    for (i = 0; i < VIEW_EVENT_ALL; i++)
    {
        ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                                 VIEW_EVENT_BASE, i,
                                                                 __view_event_handler, NULL, NULL));
    }
}