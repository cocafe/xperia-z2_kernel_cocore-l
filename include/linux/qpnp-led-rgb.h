enum led_color
{
	LED_RED = 0,
	LED_GREEN,
	LED_BLUE,
};

int qpnp_led_rgb_get(enum led_color color);
void qpnp_led_rgb_set(enum led_color color, int brightness);
