#pragma once

extern uint8_t dev_joystick_install();
extern void dev_joystick_remove();
extern void dev_joystick_update();
extern bool dev_joystick_ior(uint32_t CTRL_AL8,uint8_t *Data );
extern void dev_joystick_iow(uint32_t CTRL_AL8,uint32_t ISAIOW_Data);