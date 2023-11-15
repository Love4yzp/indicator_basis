#include "view_chart.h"

static const char *TAG = "view_chart";

static char date_hour[24][6] = {};
static char date_day[7][6] = {};
static uint16_t sensor_data_resolution = 0;
static uint16_t sensor_data_multiple = 1;

static void event_chart_week_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_PRESSED || code == LV_EVENT_RELEASED)
    {
        lv_obj_invalidate(obj); /*To make the value boxes visible*/
    }
    else if (code == LV_EVENT_DRAW_PART_BEGIN)
    {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_param(e);
        /*Set the markers' text*/
        if (dsc->part == LV_PART_TICKS && dsc->id == LV_CHART_AXIS_PRIMARY_X)
        {
            lv_snprintf(dsc->text, dsc->text_length, "%s", (char *)&date_day[dsc->value][0]);
        }
        else if (dsc->part == LV_PART_ITEMS)
        {

            const lv_chart_series_t *ser = dsc->sub_part_ptr;

            if (lv_chart_get_type(obj) == LV_CHART_TYPE_LINE)
            {
                dsc->rect_dsc->outline_color = lv_color_white();
                dsc->rect_dsc->outline_width = 2;
            }
            else
            {
                dsc->rect_dsc->shadow_color = ser->color;
                dsc->rect_dsc->shadow_width = 15;
                dsc->rect_dsc->shadow_spread = 0;
            }

            char buf[8];
            snprintf(buf, sizeof(buf), "%.*f", sensor_data_resolution, (float)dsc->value / sensor_data_multiple);

            lv_point_t text_size;
            lv_txt_get_size(&text_size, buf, &ui_font_font1, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            lv_area_t txt_area;

            txt_area.x1 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2 - text_size.x / 2;
            txt_area.x2 = txt_area.x1 + text_size.x;
            txt_area.y2 = dsc->draw_area->y1 - LV_DPX(15);
            if (ser == ui_sensor_chart_week_series_low)
            {
                txt_area.y2 += LV_DPX(70);
            }
            txt_area.y1 = txt_area.y2 - text_size.y;

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = lv_color_white();
            label_dsc.font = &lv_font_montserrat_16;

            if (lv_chart_get_pressed_point(obj) == dsc->id)
            {
                label_dsc.font = &lv_font_montserrat_20;
            }
            else
            {
                label_dsc.font = &lv_font_montserrat_16;
            }
            lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
        }
    }
}

static void event_chart_day_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_PRESSED || code == LV_EVENT_RELEASED)
    {
        lv_obj_invalidate(obj); /*To make the value boxes visible*/
    }
    else if (code == LV_EVENT_DRAW_PART_BEGIN)
    {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_param(e);
        /*Set the markers' text*/
        if (dsc->part == LV_PART_TICKS && dsc->id == LV_CHART_AXIS_PRIMARY_X)
        {
            lv_snprintf(dsc->text, dsc->text_length, "%s", (char *)&date_hour[dsc->value][0]);
        }
        else if (dsc->part == LV_PART_ITEMS)
        {

            /*Add  a line mask that keeps the area below the line*/
            if (dsc->p1 && dsc->p2)
            {
                lv_draw_mask_line_param_t line_mask_param;
                lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y,
                                              LV_DRAW_MASK_LINE_SIDE_BOTTOM);
                int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

                /*Add a fade effect: transparent bottom covering top*/
                lv_coord_t h = lv_obj_get_height(obj);
                lv_draw_mask_fade_param_t fade_mask_param;
                lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,
                                       obj->coords.y2);
                int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

                /*Draw a rectangle that will be affected by the mask*/
                lv_draw_rect_dsc_t draw_rect_dsc;
                lv_draw_rect_dsc_init(&draw_rect_dsc);
                draw_rect_dsc.bg_opa = LV_OPA_50;
                draw_rect_dsc.bg_color = dsc->line_dsc->color;

                lv_area_t obj_clip_area;
                _lv_area_intersect(&obj_clip_area, dsc->draw_ctx->clip_area, &obj->coords);
                const lv_area_t *clip_area_ori = dsc->draw_ctx->clip_area;
                dsc->draw_ctx->clip_area = &obj_clip_area;
                lv_area_t a;
                a.x1 = dsc->p1->x;
                a.x2 = dsc->p2->x - 1;
                a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
                a.y2 = obj->coords.y2;
                lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);
                dsc->draw_ctx->clip_area = clip_area_ori;
                /*Remove the masks*/
                lv_draw_mask_remove_id(line_mask_id);
                lv_draw_mask_remove_id(fade_mask_id);
            }

            const lv_chart_series_t *ser = dsc->sub_part_ptr;

            if (lv_chart_get_type(obj) == LV_CHART_TYPE_LINE)
            {
                dsc->rect_dsc->outline_color = lv_color_white();
                dsc->rect_dsc->outline_width = 2;
            }
            else
            {
                dsc->rect_dsc->shadow_color = ser->color;
                dsc->rect_dsc->shadow_width = 15;
                dsc->rect_dsc->shadow_spread = 0;
            }

            char buf[8];
            snprintf(buf, sizeof(buf), "%.*f", sensor_data_resolution, (float)dsc->value / sensor_data_multiple);

            lv_point_t text_size;
            lv_txt_get_size(&text_size, buf, &ui_font_font1, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            lv_area_t txt_area;

            txt_area.x1 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2 - text_size.x / 2;
            txt_area.x2 = txt_area.x1 + text_size.x;
            txt_area.y2 = dsc->draw_area->y1 - LV_DPX(15);
            txt_area.y1 = txt_area.y2 - text_size.y;

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = lv_color_white();
            label_dsc.font = &lv_font_montserrat_16;

            if (lv_chart_get_pressed_point(obj) == dsc->id)
            {
                label_dsc.font = &lv_font_montserrat_20;
            }
            else
            {
                label_dsc.font = &lv_font_montserrat_16;
            }
            lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
        }
    }
}

void sensor_chart_update(sensor_chart_display_t *p_display)
{
    int i = 0;
    struct view_data_sensor_history_data *p_info = p_display->p_info;

    sensor_data_resolution = p_info->resolution;
    sensor_data_multiple = pow(10, p_info->resolution);

    float diff = 0;
    float chart_day_min = 0;
    float chart_day_max = 0;
    float chart_week_min = 0;
    float chart_week_max = 0;

    diff = p_info->day_max - p_info->day_min;
    if (diff <= 2)
    {
        diff = 4;
    }
    chart_day_min = (p_info->day_min - diff / 2.0); // sub quad
    chart_day_max = (p_info->day_max + diff / 2.0); // add quad

    ESP_LOGI(TAG, "data max:%.1f, min:%.1f, char max:%.1f, min:%.1f ", p_info->day_max, p_info->day_min, chart_day_max, chart_day_min);

    diff = p_info->week_max - p_info->week_min;
    if (diff <= 2)
    {
        diff = 4;
    }
    chart_week_min = (p_info->week_min - diff / 2.0); // sub quad
    chart_week_max = (p_info->week_max + diff / 2.0); // add quad

    lv_label_set_text(ui_sensor_data_title, p_display->name);

    lv_chart_set_series_color(ui_sensor_chart_day, ui_sensor_chart_day_series, p_display->color);
    lv_chart_set_range(ui_sensor_chart_day, LV_CHART_AXIS_PRIMARY_Y, (lv_coord_t)chart_day_min * sensor_data_multiple, (lv_coord_t)chart_day_max * sensor_data_multiple);

    lv_chart_set_range(ui_sensor_chart_week, LV_CHART_AXIS_PRIMARY_Y, (lv_coord_t)chart_week_min * sensor_data_multiple, (lv_coord_t)chart_week_max * sensor_data_multiple);
    lv_chart_set_series_color(ui_sensor_chart_week, ui_sensor_chart_week_series_low, p_display->color);
    lv_chart_set_series_color(ui_sensor_chart_week, ui_sensor_chart_week_series_hight, p_display->color);

    for (i = 0; i < 24; i++)
    {
        if (p_info->data_day[i].valid)
        {
            lv_chart_set_value_by_id(ui_sensor_chart_day, ui_sensor_chart_day_series, i, sensor_data_multiple * p_info->data_day[i].data);
        }
        else
        {
            lv_chart_set_value_by_id(ui_sensor_chart_day, ui_sensor_chart_day_series, i, LV_CHART_POINT_NONE);
        }
        struct tm timeinfo = {0};
        localtime_r(&p_info->data_day[i].timestamp, &timeinfo);
        lv_snprintf((char *)&date_hour[i][0], 6, "%02d:00", timeinfo.tm_hour);
    }

    for (i = 0; i < 7; i++)
    {

        if (p_info->data_week[i].valid)
        {
            lv_chart_set_value_by_id(ui_sensor_chart_week, ui_sensor_chart_week_series_hight, i, sensor_data_multiple * p_info->data_week[i].max);
            lv_chart_set_value_by_id(ui_sensor_chart_week, ui_sensor_chart_week_series_low, i, sensor_data_multiple * p_info->data_week[i].min);
        }
        else
        {
            lv_chart_set_value_by_id(ui_sensor_chart_week, ui_sensor_chart_week_series_hight, i, LV_CHART_POINT_NONE);
            lv_chart_set_value_by_id(ui_sensor_chart_week, ui_sensor_chart_week_series_low, i, LV_CHART_POINT_NONE);
        }

        struct tm timeinfo = {0};
        localtime_r(&p_info->data_week[i].timestamp, &timeinfo);

        lv_snprintf((char *)&date_day[i][0], 6, "%02d/%02d", timeinfo.tm_mon + 1, timeinfo.tm_mday);
    }

    // change type color
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, p_display->color, lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    lv_chart_refresh(ui_sensor_chart_day);
    lv_chart_refresh(ui_sensor_chart_week);
}

void sensor_chart_event_init(void)
{
    int i = 0;
    struct view_data_sensor_history_data default_sensor_info;

    default_sensor_info.resolution = 1;

    time_t now = 0;
    time(&now);
    time_t time1 = (time_t)(now / 3600) * 3600;
    time_t time2 = (time_t)(now / 3600 / 24) * 3600 * 24;

    float min = 90;
    float max = 10;

    for (i = 0; i < 24; i++)
    {
        default_sensor_info.data_day[i].data = (float)lv_rand(10, 90);
        default_sensor_info.data_day[i].timestamp = time1 + i * 3600;
        default_sensor_info.data_day[i].valid = true;

        if (min > default_sensor_info.data_day[i].data)
        {
            min = default_sensor_info.data_day[i].data;
        }
        if (max < default_sensor_info.data_day[i].data)
        {
            max = default_sensor_info.data_day[i].data;
        }
    }
    default_sensor_info.day_max = max;
    default_sensor_info.day_min = min;

    min = 90;
    max = 10;

    for (i = 0; i < 7; i++)
    {
        default_sensor_info.data_week[i].max = (float)lv_rand(60, 90);
        default_sensor_info.data_week[i].min = (float)lv_rand(10, 40);
        default_sensor_info.data_week[i].timestamp = time2 + i * 3600 * 24;
        ;
        default_sensor_info.data_week[i].valid = true;

        if (min > default_sensor_info.data_week[i].min)
        {
            min = default_sensor_info.data_week[i].min;
        }
        if (max < default_sensor_info.data_week[i].max)
        {
            max = default_sensor_info.data_week[i].max;
        }
    }
    default_sensor_info.week_max = max;
    default_sensor_info.week_min = min;

    sensor_chart_display_t default_chart = {
        .color = lv_palette_main(LV_PALETTE_GREEN),
        .p_info = &default_sensor_info,
    };
    strcpy(default_chart.name, "TEST");
    strcpy(default_chart.units, "%%");

    sensor_chart_update(&default_chart);

    lv_obj_add_event_cb(ui_sensor_chart_day, event_chart_day_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_sensor_chart_week, event_chart_week_cb, LV_EVENT_ALL, NULL);
}



void view_event_sensor_data(void *event_data)
{
    ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_DATA");

    struct view_data_sensor_data *p_data = (struct view_data_sensor_data *)event_data;
    char data_buf[32];

    memset(data_buf, 0, sizeof(data_buf));
    switch (p_data->sensor_type)
    {
    case SENSOR_DATA_CO2:
    {
        snprintf(data_buf, sizeof(data_buf), "%d", (int)p_data->vaule);
        ESP_LOGI(TAG, "update co2:%s", data_buf);
        lv_label_set_text(ui_co2_data, data_buf);
        break;
    }
    case SENSOR_DATA_TVOC:
    {
        snprintf(data_buf, sizeof(data_buf), "%d", (int)p_data->vaule);
        ESP_LOGI(TAG, "update tvoc:%s", data_buf);
        lv_label_set_text(ui_tvoc_data, data_buf);
        break;
    }
    case SENSOR_DATA_TEMP:
    {
        snprintf(data_buf, sizeof(data_buf), "%.1f", p_data->vaule);
        ESP_LOGI(TAG, "update temp:%s", data_buf);
        lv_label_set_text(ui_temp_data_2, data_buf);
        break;
    }
    case SENSOR_DATA_HUMIDITY:
    {
        snprintf(data_buf, sizeof(data_buf), "%d", (int)p_data->vaule);
        ESP_LOGI(TAG, "update humidity:%s", data_buf);
        lv_label_set_text(ui_humidity_data_2, data_buf);
        break;
    }
    default:
        break;
    }
    break;
}

void view_event_sensor_data_history(void *event_data)
{
    ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_DATA_HISTORY");
    struct view_data_sensor_history_data *p_data = (struct view_data_sensor_history_data *)event_data;
    sensor_chart_display_t sensor_chart;

    switch (p_data->sensor_type)
    {
    case SENSOR_DATA_CO2:
    {
        sensor_chart.color = lv_color_hex(0x529D53);
        sensor_chart.p_info = p_data;
        strcpy(sensor_chart.name, "CO2");
        break;
    }
    case SENSOR_DATA_TVOC:
    {
        sensor_chart.color = lv_color_hex(0xE06D3D);
        sensor_chart.p_info = p_data;
        strcpy(sensor_chart.name, "tVOC");
        break;
    }
    case SENSOR_DATA_TEMP:
    {
        sensor_chart.color = lv_color_hex(0xEEBF41);
        sensor_chart.p_info = p_data;
        strcpy(sensor_chart.name, "Temp");
        break;
    }
    case SENSOR_DATA_HUMIDITY:
    {
        sensor_chart.color = lv_color_hex(0x4EACE4);
        sensor_chart.p_info = p_data;
        strcpy(sensor_chart.name, "Humidity");
        break;
    }
    default:
        break;
    }
    sensor_chart_update(&sensor_chart);
}
