#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>

#include <zmk/battery.h>

/* Battery thresholds */
#define HIGH_THRESHOLD 90
#define LOW_THRESHOLD  10

/* Blink timing */
#define FAST_DELAY_MS 400
#define SLOW_DELAY_MS 500

#define FAST_COUNT 3
#define SLOW_COUNT 5

/* Use board LED (led0) via alias */
#define LED_NODE DT_ALIAS(indicator_led)

#if !DT_NODE_HAS_STATUS(LED_NODE, okay)
#error "indicator-led alias not defined in overlay"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

/* Blink helper */
static void blink(int count, int delay_ms)
{
    for (int i = 0; i < count; i++) {
        gpio_pin_set_dt(&led, 1);
        k_msleep(delay_ms);
        gpio_pin_set_dt(&led, 0);
        k_msleep(delay_ms);
    }
}

/* Runs at boot */
static int battery_boot_blink(const struct device *dev)
{
    ARG_UNUSED(dev);

    if (!device_is_ready(led.port)) {
        return 0;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);

    int level = zmk_battery_state_of_charge();
    int retry = 0;

    while (level == 0 && retry++ < 10) {
        k_msleep(100);
        level = zmk_battery_state_of_charge();
    }


    if (level >= HIGH_THRESHOLD) {
        blink(FAST_COUNT, FAST_DELAY_MS);
    } else if (level <= LOW_THRESHOLD) {
        blink(SLOW_COUNT, SLOW_DELAY_MS);
    }

    return 0;
}

/* Register boot hook */
SYS_INIT(battery_boot_blink, APPLICATION, 99);
