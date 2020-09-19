#include "lv_thread.h"

#include "lcd.h"
#include "touch.h"

#include "the_game_of_life.h"

/**
 * @brief LV 线程入口
 *
 * @param parameter 参数指针
 */
void lv_thread_entry(void *parameter) {
    /* LVGL 缓冲区 */
    static lv_disp_buf_t disp_buf;
    static lv_color_t    buf_1[LV_HOR_RES_MAX * 10];

    /* LVGL 驱动 */
    lv_disp_drv_t  disp_drv;
    lv_indev_drv_t indev_drv;
    /* LVGL 初始化 */
    lv_init();
    /* 初始化 LCD */
    LCD_Init();
    /* 初始化缓冲区和驱动 */
    lv_disp_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * 10);
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer   = &disp_buf;
    disp_drv.flush_cb = LCD_Flush;
    /* 注册显示驱动 */
    lv_disp_drv_register(&disp_drv);
    /* 注册触摸屏驱动 */
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = Input_Read;
    lv_indev_drv_register(&indev_drv);

    the_game_of_life_widget();

    while (1) {
        /* 更新画面 */
        lv_task_handler();
        rt_thread_mdelay(10);
    }
}
