#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#if IS_ENABLED(CONFIG_BOOT_OLED)

static struct k_work_delayable oled_off_work;

static void oled_off_handler(struct k_work *work)
{
    const struct device *display =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display)) {
        return;
    }

    display_blanking_on(display);
}

/*
 * We hook into the system using a delayed thread,
 * not SYS_INIT.
 */
static void boot_oled_thread(void)
{
    /* Wait for LVGL + display to fully start */
    k_sleep(K_SECONDS(2));

    k_work_init_delayable(&oled_off_work, oled_off_handler);

    /* Turn off after x seconds */
    k_work_schedule(&oled_off_work, K_SECONDS(4));
}

/*
 * Run after kernel is fully started.
 */
K_THREAD_DEFINE(
    boot_oled_tid,
    512,
    boot_oled_thread,
    NULL, NULL, NULL,
    K_LOWEST_APPLICATION_THREAD_PRIO,
    0,
    0
);

#endif
