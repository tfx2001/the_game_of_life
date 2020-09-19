#include <lvgl/lvgl.h>

#include "the_game_of_life.h"

/**
 * Resource Declare
 */
LV_FONT_DECLARE(sourcehan_sans_cn_22);
LV_FONT_DECLARE(sourcehan_sans_cn_16);
LV_IMG_DECLARE(qr_code);

/**
 *  Private Functions Prototype
 */
LV_EVENT_CB_DECLARE(canvas_event_cb);
LV_EVENT_CB_DECLARE(btn_run_event_cb);
LV_EVENT_CB_DECLARE(btn_rules_event_cb);
LV_EVENT_CB_DECLARE(pg_btn_event_cb);
void task_main_cb(lv_task_t *task);

void    grid_repaint(void);
uint8_t grid_get_around_alive(uint8_t x, uint8_t y);

/**
 *  Private Const Variables
 */
const lv_color_t ALPHA_BLACK = {
    .full = 0x00FFFF,
};
const lv_color_t ALPHA_WHITE = {
    .full = 0x000000,
};
const lv_color_t ALPHA_GRAY = {
    .full = 0x0000FF,
};

/**
 *  Private Variables
 */
lv_obj_t *canvas, *mbox, *pg;

lv_draw_rect_dsc_t rect_dsc;
lv_draw_line_dsc_t line_dsc;

lv_task_t *task_main;

uint8_t  grid_data1[GRID_COUNT_X * GRID_COUNT_Y];
uint8_t  grid_data2[GRID_COUNT_X * GRID_COUNT_Y];
uint8_t *grid_data      = grid_data1;
uint8_t *grid_data_next = grid_data2;

/**
 * @brief lv_demo_widgets
 */
void the_game_of_life_widget(void) {
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_ALPHA_4BIT(CANVAS_WIDTH, CANVAS_HEIGHT)];
    lv_point_t        points[2];

    /* Create canvas */
    canvas = lv_canvas_create(lv_scr_act(), NULL);

    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_ALPHA_4BIT);
    lv_obj_set_click(canvas, true); /** Set canvas to clickable */
    lv_obj_set_size(canvas, CANVAS_WIDTH, CANVAS_HEIGHT);
    lv_obj_set_event_cb(canvas, canvas_event_cb);
    lv_obj_align(canvas, NULL, LV_ALIGN_CENTER, 0, -8);

    lv_canvas_fill_bg(canvas, ALPHA_WHITE, 0x3F);

    /* Create title label */
    lv_obj_t *label_title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label_title, "康威生命游戏");
    lv_obj_set_style_local_text_font(label_title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sourcehan_sans_cn_22);
    lv_obj_align_x(label_title, NULL, LV_ALIGN_CENTER, 0);
    lv_obj_set_y(label_title, 24);

    /* Create button */
    lv_obj_t *btn_run        = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *btn_run_text   = lv_label_create(btn_run, NULL);
    lv_obj_t *btn_rules      = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t *btn_rules_text = lv_label_create(btn_rules, NULL);
    lv_obj_set_style_local_text_font(btn_run_text, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sourcehan_sans_cn_16);
    lv_obj_set_style_local_text_font(btn_rules_text, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sourcehan_sans_cn_16);
    lv_label_set_text(btn_run_text, "开始");
    lv_label_set_text(btn_rules_text, "帮助");
    lv_obj_set_width(btn_run, 96);
    lv_obj_set_width(btn_rules, 96);
    lv_obj_align_x(btn_run, NULL, LV_ALIGN_CENTER, -56);
    lv_obj_align_x(btn_rules, NULL, LV_ALIGN_CENTER, 56);
    lv_obj_set_y(btn_run, 256);
    lv_obj_set_y(btn_rules, 256);
    lv_obj_set_event_cb(btn_run, btn_run_event_cb);
    lv_obj_set_event_cb(btn_rules, btn_rules_event_cb);

    /* Init draw dsc */
    lv_draw_rect_dsc_init(&rect_dsc);
    lv_draw_line_dsc_init(&line_dsc);

    rect_dsc.bg_color = ALPHA_BLACK;

    line_dsc.color = ALPHA_WHITE;
    line_dsc.width = 1;

    /* Draw grid vertical line */
    points[0].y = 0;
    points[1].y = CANVAS_HEIGHT;
    for (uint32_t i = 0; i <= GRID_COUNT_X; i++) {
        points[0].x = GRID_SIZE * i + i;
        points[1].x = GRID_SIZE * i + i;
        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
    }
    /* Draw grid horizontal line */
    points[0].x = 0;
    points[1].x = CANVAS_WIDTH;
    for (uint32_t i = 0; i <= GRID_COUNT_Y; i++) {
        points[0].y = GRID_SIZE * i + i;
        points[1].y = GRID_SIZE * i + i;
        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
    }

    /* Grid repaint */
    grid_repaint();

    /* Test */
    // printf("%d\n", (-1 + 10) % 10);
}

LV_EVENT_CB_DECLARE(canvas_event_cb) {
    if (e == LV_EVENT_CLICKED) {
        lv_point_t point;
        lv_indev_get_point(lv_indev_get_act(), &point);
        /* Transform to grid pos */
        uint8_t temp_x = (point.x - lv_obj_get_x(obj)) / (GRID_SIZE + 1);
        uint8_t temp_y = (point.y - lv_obj_get_y(obj)) / (GRID_SIZE + 1);

        grid_data[temp_y * GRID_COUNT_X + temp_x] = grid_data[temp_y * GRID_COUNT_X + temp_x] ? 0 : 1;
        grid_repaint();
    }
}

LV_EVENT_CB_DECLARE(btn_run_event_cb) {
    /* Unused */
    LV_UNUSED(obj);

    if (e == LV_EVENT_CLICKED) {
        task_main = lv_task_create(task_main_cb, 200, LV_TASK_PRIO_MID, NULL);
    }
}

LV_EVENT_CB_DECLARE(btn_rules_event_cb) {
    /* Unused */
    LV_UNUSED(obj);

    if (e == LV_EVENT_CLICKED) {
        pg                    = lv_page_create(lv_scr_act(), NULL);
        lv_obj_t *pg_label    = lv_label_create(pg, NULL);
        lv_obj_t *img_qr_code = lv_img_create(pg, NULL);
        lv_obj_t *pg_btn      = lv_btn_create(pg, NULL);
        lv_obj_t *pg_btn_text = lv_label_create(pg_btn, NULL);

        /* Config page */
        lv_obj_set_size(pg, 223, 300);
        lv_obj_align(pg, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_page_set_scrollable_fit(pg, LV_FIT_NONE);
        /* Config label */
        lv_label_set_text(pg_label, "QQ、微信扫码均可");
        lv_obj_set_style_local_text_font(pg_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sourcehan_sans_cn_16);
        lv_obj_align_x(pg_label, NULL, LV_ALIGN_CENTER, 0);
        /* Config image */
        lv_img_set_src(img_qr_code, &qr_code);
        lv_obj_align(img_qr_code, NULL, LV_ALIGN_CENTER, 0, -16);
        /* Config button */
        lv_obj_set_width(pg_btn, 128);
        lv_obj_set_style_local_text_font(pg_btn_text, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sourcehan_sans_cn_16);
        lv_label_set_text(pg_btn_text, "OK");
        lv_obj_set_event_cb(pg_btn, pg_btn_event_cb);
        lv_obj_align_x(pg_btn, NULL, LV_ALIGN_CENTER, 0);
        lv_obj_set_y(pg_btn, 228);
    }
}

LV_EVENT_CB_DECLARE(pg_btn_event_cb) {
    /* Unused */
    LV_UNUSED(obj);

    if (e == LV_EVENT_CLICKED) {
        lv_obj_del(pg);
        pg = NULL;
    }
}

void task_main_cb(lv_task_t *task) {
    uint8_t temp_alive;

    /* Unused */
    LV_UNUSED(task);

    for (uint8_t i = 0; i < GRID_COUNT_Y; i++) {
        for (uint8_t j = 0; j < GRID_COUNT_X; j++) {
            temp_alive = grid_get_around_alive(j, i);
            /* Alive */
            if (grid_data[i * GRID_COUNT_X + j]) {
                switch (temp_alive) {
                    /* too few */
                    case 0:
                    case 1:
                        grid_data_next[i * GRID_COUNT_X + j] = 0;
                        break;
                    /* suitable */
                    case 2:
                    case 3:
                        grid_data_next[i * GRID_COUNT_X + j] = 1;
                        break;
                    /* too many */
                    default:
                        grid_data_next[i * GRID_COUNT_X + j] = 0;
                }
            }
            /* Dead */
            else {
                if (temp_alive == 3) {
                    grid_data_next[i * GRID_COUNT_X + j] = 1;
                } else {
                    grid_data_next[i * GRID_COUNT_X + j] = 0;
                }
            }
        }
    }

    /* Swap pointer */
    uint8_t *temp_pointer = grid_data;
    grid_data             = grid_data_next;
    grid_data_next        = temp_pointer;

    /* Repaint */
    grid_repaint();
}

void grid_repaint(void) {
    for (uint8_t i = 0; i < GRID_COUNT_Y; i++) {
        for (uint8_t j = 0; j < GRID_COUNT_X; j++) {
            if (grid_data[i * GRID_COUNT_X + j]) {
                rect_dsc.bg_color = ALPHA_BLACK;
            } else {
                rect_dsc.bg_color = ALPHA_GRAY;
            }
            lv_canvas_draw_rect(canvas, GRID_POS_X(j), GRID_POS_Y(i), GRID_SIZE, GRID_SIZE, &rect_dsc);
        }
    }
}

uint8_t grid_get_around_alive(uint8_t x, uint8_t y) {
    uint8_t cnt = 0, calc_x, calc_y;
    for (int8_t i = y - 1; i <= y + 1; i++) {
        for (int8_t j = x - 1; j <= x + 1; j++) {
            /* Cross-border processing */
            calc_y = i < 0 ? i + GRID_COUNT_Y : i % GRID_COUNT_Y;
            calc_x = j < 0 ? j + GRID_COUNT_X : j % GRID_COUNT_X;
            /* Count surviving cells */
            if (calc_x == x && calc_y == y)
                continue;
            cnt = grid_data[calc_y * GRID_COUNT_X + calc_x] ? cnt + 1 : cnt;
        }
    }
    return cnt;
}
