// Generate multiple timer Interrupt

#include "pico_pic.h"
#include "pico/time.h"

#include <stdio.h>

#define MAX_PIC_TIMERS 3

// A fixed pool of only 3 events for now. gus-x only has 3 different timer events - two timers and one DMA
PIC_TimerEvent timerEvents[MAX_PIC_TIMERS];

alarm_pool_t* alarm_pool;

int64_t clear_irq(alarm_id_t id, void *user_data) {
    gpio_put(IRQ_PIN, 0); 
    return 0;
}

int64_t __isr __time_critical_func(PIC_HandleEvent)(alarm_id_t id, void *user_data) {

    PIC_TimerEvent* event = (PIC_TimerEvent *)user_data;
#ifdef USE_ALARM
    if (id != event->alarm_id) {
        return 0;
    }
#endif
    int32_t ret = (event->handler)(event->value);
    // printf("called event handler: %x %x, ret %d\n", event->handler, event->value, ret);
#ifdef USE_ALARM
    if (!ret) {
        event->alarm_id = 0;
        return ret;
    }
    // A negative return value re-sets the alarm from the time when it initially triggered
    return -(int64_t)ret;
    // return (int64_t)ret;
#else
    if (ret) {
        event->deadline = time_us_32() + ret;
    } else {
        event->active = false;
    }
    return 0;
#endif
}

void PIC_RemoveEvents(PIC_EventHandler handler) {
    // puts("removeevents");
    for (int i = 0; i < MAX_PIC_TIMERS; ++i) {
        if (timerEvents[i].handler == handler) {
#ifdef USE_ALARM
            if (timerEvents[i].alarm_id) {
                // cancel_alarm(timerEvents[i].alarm_id);
                alarm_pool_cancel_alarm(alarm_pool, timerEvents[i].alarm_id);
                timerEvents[i].alarm_id = 0;
            }
#else
            timerEvents[i].active = false;
#endif
        }
    }
}

void PIC_Init() {
#ifdef USE_ALARM
    alarm_pool = alarm_pool_create(2, PICO_TIME_DEFAULT_ALARM_POOL_MAX_TIMERS);
#if defined(RASPBERRYPI_PICO2) || defined(PIMORONI_PICO_PLUS2_RP2350)
    irq_set_priority(TIMER0_IRQ_2, 0x80 );   // PicoMEM : Must not be highest (for the DMA IRQ)
#else
    irq_set_priority(TIMER_IRQ_2, 0x80 );   // PicoMEM : Must not be highest (for the DMA IRQ)
#endif

    for (int i = 0; i < MAX_PIC_TIMERS; ++i) {
        timerEvents[i].alarm_id = 0;
        timerEvents[i].handler = 0;
    }
#else
    for (int i = 0; i < MAX_PIC_TIMERS; ++i) {
        timerEvents[i].active = false;
        timerEvents[i].deadline = UINT32_MAX;
        timerEvents[i].handler = 0;
    }
#endif
}
