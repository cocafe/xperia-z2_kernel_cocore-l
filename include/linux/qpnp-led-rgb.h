enum led_color
{
	LED_RED = 0,
	LED_GREEN,
	LED_BLUE,
};

void qpnp_led_rgb_set(enum led_color color, int brightness);
