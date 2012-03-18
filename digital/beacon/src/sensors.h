#ifndef _SENSORS_H
#define _SENSORS_H

void init_ILS();
void irq_laser(void);
void irq_ILS(void);
void init_sensors(void);
int calcul_temps();
void regul_vitesse(int temps);
void control_motor_ms(int value);

#endif