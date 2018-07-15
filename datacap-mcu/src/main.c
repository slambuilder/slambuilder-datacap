#include <asf.h>
#include "AppData.h"

void vApplicationMallocFailedHook(void);
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(void);

int main (void)
{
	system_init();

	ioport_init();
	ioport_set_pin_dir(CONF_BOARD_RED_LED_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(CONF_BOARD_GREEN_LED_PIN, IOPORT_DIR_OUTPUT);

	ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, true);
	ioport_set_pin_level(CONF_BOARD_GREEN_LED_PIN, true);

	//while (true)
	//{
		//ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, false);
		//ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, true);
	//}

	// Start the scheduler. Normally, this function never returns.
	vTaskStartScheduler();
}

void vApplicationMallocFailedHook(void)
{
    // TODO
}

void vApplicationTickHook(void)
{
    // TODO
}

void vApplicationStackOverflowHook(void)
{
    // TODO
}
