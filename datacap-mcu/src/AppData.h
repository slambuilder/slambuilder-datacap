#ifndef APPDATA_H_
#define APPDATA_H_

typedef struct TAppData
{
    TaskHandle_t hTaskRedLedControl;
	QueueHandle_t hTickQueue;
	TimerHandle_t hTimer;
} AppData;

#endif