/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>

// Red LED on PA17
#define CONF_BOARD_RED_LED_PIN PIN_PA17

// Green LED on PA06
#define CONF_BOARD_GREEN_LED_PIN PIN_PA06

int main (void)
{
	system_init();

	ioport_init();
	ioport_set_pin_dir(CONF_BOARD_RED_LED_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(CONF_BOARD_GREEN_LED_PIN, IOPORT_DIR_OUTPUT);

	ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, true);
	ioport_set_pin_level(CONF_BOARD_GREEN_LED_PIN, true);

	while (true)
	{
		ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, false);
		ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, true);
	}
}
