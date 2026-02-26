#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#if IS_ENABLED(CONFIG_BOOT_OLED)

/*
 * Hard blackout of OLED after 5 seconds at boot.
 * Uses Zephyr display driver directly (no ZMK wrapper).
 */

static struct k_work_delayable oled_off_work;

/* Turn display off (panel blanking) */
static void oled_off_handler(struct k_work *work)
{
    const struct device *display_dev =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev)) {
        return;
    }

    display_blanking_on(display_dev);
}

/* Schedule blackout */
static int boot_oled_init(void)
{
    /*
     * Allow display + LVGL to finish initializing
     * before starting countdown.
     */
    k_msleep(500);

    k_work_init_delayable(&oled_off_work, oled_off_handler);

    /* 5 second visible boot window */
    k_work_schedule(&oled_off_work, K_SECONDS(5));

    return 0;
}

/* Run late to avoid racing display init */
SYS_INIT(boot_oled_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY + 1);

#endif
