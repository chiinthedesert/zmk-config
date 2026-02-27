#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/pm/device.h>

#if IS_ENABLED(CONFIG_BOOT_OLED)

/*
 * Hard boot-only OLED.
 * Visible for 5 seconds after boot,
 * then display device is suspended permanently
 * until next reboot.
 */

static void boot_oled_thread(void)
{
    const struct device *display =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display)) {
        return;
    }

    /*
     * Let ZMK + LVGL fully initialize
     * and render default screen.
     */
    k_sleep(K_SECONDS(5));

    /* Blank framebuffer */
    display_blanking_on(display);

    /* Small delay to avoid race with LVGL refresh */
    k_sleep(K_MSEC(20));

    /* Suspend the display driver itself */
    pm_device_action_run(display, PM_DEVICE_ACTION_SUSPEND);
}

/*
 * Dedicated low-priority thread.
 * No SYS_INIT. No linker section conflicts.
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
