#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#if IS_ENABLED(CONFIG_BOOT_OLED)

/*
 * We directly control the Zephyr display driver.
 * This avoids using any non-exported ZMK display symbols.
 */

static struct k_work_delayable oled_off_work;

/* Turn display off (hard blackout) */
static void oled_off_handler(struct k_work *work)
{
    const struct device *display_dev =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev)) {
        return;
    }

    display_blanking_on(display_dev);
}

/* Schedule blackout after boot */
static int boot_oled_timeout_init(void)
{
    /*
     * Give LVGL + display subsystem time to fully initialize
     * before starting the 5-second countdown.
     */
    k_msleep(500);

    k_work_init_delayable(&oled_off_work, oled_off_handler);

    /* Blackout after 5 seconds */
    k_work_schedule(&oled_off_work, K_SECONDS(5));

    return 0;
}

/* Run late in APPLICATION phase to avoid racing display init */
SYS_INIT(boot_oled_timeout_init, APPLICATION, 120);

#endif
