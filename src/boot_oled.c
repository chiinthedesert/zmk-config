#include <zephyr/kernel.h>
#include <zephyr/init.h>

#if IS_ENABLED(CONFIG_BOOT_OLED)

#include <zmk/display.h>

static struct k_work_delayable oled_off_work;

static void oled_off_handler(struct k_work *work)
{
    zmk_display_sleep();
}

static int boot_oled_timeout_init(void)
{
    k_work_init_delayable(&oled_off_work, oled_off_handler);
    k_work_schedule(&oled_off_work, K_SECONDS(5));
    return 0;
}

SYS_INIT(boot_oled_timeout_init, APPLICATION, 99);

#endif
