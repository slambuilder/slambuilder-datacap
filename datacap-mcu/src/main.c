#include <asf.h>
#include <string.h>
#include "AppData.h"

void vApplicationMallocFailedHook(void);
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(void);

void timerCallback(TimerHandle_t hTimer);
void taskRedLedControl(void *pvParameters);

// global instances
AppData g_appData;

int main (void)
{
	system_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	board_init();

	ioport_init();
	ioport_set_pin_dir(CONF_BOARD_RED_LED_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(CONF_BOARD_GREEN_LED_PIN, IOPORT_DIR_OUTPUT);

	memset(&g_appData, 0, sizeof(g_appData));

	// ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, true);
	ioport_set_pin_level(CONF_BOARD_GREEN_LED_PIN, true);

    system_interrupt_enable_global();

	g_appData.hTickQueue = xQueueCreate(1, sizeof(TickType_t));

	// PID Control loop task has priority 3 above an idle
	xTaskCreate(taskRedLedControl, "RED_TGL", configMINIMAL_STACK_SIZE * 2, &g_appData, tskIDLE_PRIORITY + 3, &g_appData.hTaskRedLedControl);

	g_appData.hTimer = xTimerCreate("TempSample", 5, pdTRUE, NULL, timerCallback);
	xTimerStart(g_appData.hTimer, 0);

	// Start the scheduler. Normally, this function never returns.
	vTaskStartScheduler();
}

void vApplicationMallocFailedHook(void)
{
    // TODO
	ioport_set_pin_level(CONF_BOARD_GREEN_LED_PIN, false);
}

void vApplicationTickHook(void)
{
    // TODO
	ioport_set_pin_level(CONF_BOARD_GREEN_LED_PIN, false);
}

void vApplicationStackOverflowHook(void)
{
    // TODO
	ioport_set_pin_level(CONF_BOARD_GREEN_LED_PIN, false);
}

void vApplicationAssertHook(void)
{
    // TODO
    ioport_set_pin_level(CONF_BOARD_GREEN_LED_PIN, false);
}

// This is a timer callback function.
// It should never block.
// Current implementation only sends a message in a queue using non-blocking call.
void timerCallback(TimerHandle_t hTimer)
{
    static bool level = true;

	ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, level);

    TickType_t currentTicks = xTaskGetTickCount();
    xQueueSendToBack(g_appData.hTickQueue, &currentTicks, 0);
}

// control reading task waits for a "Tick" queue
void taskRedLedControl(void *pvParameters)
{
    TickType_t ticks;
    BaseType_t ret;
    static bool level = true;

    ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, level);

    while (true)
    {
        ret = xQueueReceive(g_appData.hTickQueue, &ticks, portMAX_DELAY);
        if (ret != pdTRUE)
        {
            continue;
        }

        // toggle LED
	    level = !level;
	    ioport_set_pin_level(CONF_BOARD_RED_LED_PIN, level);
    }
}
