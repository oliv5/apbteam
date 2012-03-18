#ifndef _SERVO_H
#define _SERVO_H

void init_timer_servo(void);
uint8_t servo_get_high_time (uint8_t servo);
void servo_set_high_time (uint8_t servo, uint8_t high_time);
void servo_apbteam_init (void);

void increase_angle_servo_1(void);
void decrease_angle_servo_1(void);
void increase_angle_servo_2(void);
void decrease_angle_servo_2(void);

#endif