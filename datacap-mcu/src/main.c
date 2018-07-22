#include <asf.h>
#include <string.h>
#include "AppData.h"

void vApplicationMallocFailedHook(void);
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(void);

void timerCallback(TimerHandle_t hTimer);
void taskRedLedControl(void *pvParameters);


// ************************************************************************************************************************************************************

void configure_dma_example(void);

void configure_dma_resource(struct dma_resource *resource);
void setup_transfer_descriptor(DmacDescriptor *descriptor);

void configure_adc(void);
void configure_dac(void);
void configure_dac_channel(void);

struct dac_module dac_instance;
struct adc_module adc_instance;
struct dma_resource example_resource;

COMPILER_ALIGNED(16)
DmacDescriptor example_descriptor SECTION_DMAC_DESCRIPTOR;

void configure_adc(void)
{
	struct adc_config config_adc;
	adc_get_config_defaults(&config_adc);

	config_adc.gain_factor     = ADC_GAIN_FACTOR_2X;
	config_adc.resolution      = ADC_RESOLUTION_8BIT;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
	config_adc.reference       = ADC_REFERENCE_INTVCC1;
	config_adc.positive_input  = CONF_BOARD_ANALOG_CH1;
	config_adc.freerunning     = true;
	config_adc.left_adjust     = false;

	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
}

void configure_dac(void)
{
	struct dac_config config_dac;
	dac_get_config_defaults(&config_dac);

	config_dac.reference = DAC_REFERENCE_AVCC;
	dac_init(&dac_instance, DAC, &config_dac);
}

void configure_dac_channel(void)
{
	struct dac_chan_config config_dac_chan;

	dac_chan_get_config_defaults(&config_dac_chan);
	dac_chan_set_config(&dac_instance, DAC_CHANNEL_0, &config_dac_chan);
	dac_chan_enable(&dac_instance, DAC_CHANNEL_0);
}

void configure_dma_resource(struct dma_resource *resource)
{
	struct dma_resource_config config;

	dma_get_config_defaults(&config);

	config.peripheral_trigger = ADC_DMAC_ID_RESRDY;
	config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
	dma_allocate(resource, &config);
}

void setup_transfer_descriptor(DmacDescriptor *descriptor)
{
	struct dma_descriptor_config descriptor_config;

	dma_descriptor_get_config_defaults(&descriptor_config);

	descriptor_config.beat_size = DMA_BEAT_SIZE_HWORD;
	descriptor_config.dst_increment_enable = false;
	descriptor_config.src_increment_enable = false;
	descriptor_config.block_transfer_count = 1000;
	descriptor_config.source_address = (uint32_t)(&adc_instance.hw->RESULT.reg);
	descriptor_config.destination_address = (uint32_t)(&dac_instance.hw->DATA.reg);
	descriptor_config.next_descriptor_address = (uint32_t)descriptor;

	dma_descriptor_create(descriptor, &descriptor_config);
}

void configure_dma_example(void)
{
	system_init();

	configure_adc();
	configure_dac();
	configure_dac_channel();
	dac_enable(&dac_instance);
	configure_dma_resource(&example_resource);
	setup_transfer_descriptor(&example_descriptor);
	dma_add_descriptor(&example_resource, &example_descriptor);
	adc_start_conversion(&adc_instance);
	dma_start_transfer_job(&example_resource);
}

// ************************************************************************************************************************************************************


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

    // start DMA transfer
    configure_dma_example();

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
