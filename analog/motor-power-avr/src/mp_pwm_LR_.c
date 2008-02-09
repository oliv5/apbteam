#include "common.h"
#include "mp_pwm_LR_.h"
#include "mp_pwm_L_.h"

// Variables for current limitation
static uint8_t curLim_temp;
static uint8_t curLim_bat;
static uint8_t curLim_soft;

// This file contains general pwm routines for mp (motor-power) board
//
// state_L_ : x - Inhib - changeDir - Dir - x - x - x - x
// Timer_L_ : timer dedicated to Left side

void init_timer_LR_(void) {
    init_pwm_L_();
    //init_pwm_R_();

    TCNT_L_ = 0x00;
    TCNT_R_ = 0x80;	// 180° phase shifted to TCNT_L

    OCR_L_ = PWM_MIN_LR_;
    OCR_R_ = PWM_MIN_LR_;

    // set interrupts
    TIMSK |= TIMSK_LR_CFG;

    // launch timers
    TCCR_L_ = TCCR_LR_CFG;
    TCCR_R_ = TCCR_LR_CFG;
}

void init_curLim (void) {
    // TODO : set interrupts
    curLim_soft = 0x80;
    curLim_bat = 0x00;
    curLim_temp = 0x00;
}

uint8_t get_curLim_temp (uint8_t temperature) {
    return (temperature - 40) >> 2;	// TODO : ajuster la fonction de transfert
}

uint8_t get_curLim_bat (uint8_t battery) {
    return (battery - 40) >> 2;		// TODO : ajuster la fonction de transfert
}

// this function shall be called after each adjustment of any current limit
void update_curLim(void) {
    uint8_t curLim_tmp;

    // search for MIN(curLim_soft, curLim_temp, curLim_bat)
    curLim_tmp = curLim_soft;

    if (curLim_tmp > curLim_temp)
      {
        curLim_tmp = curLim_temp;
      }

    if (curLim_tmp > curLim_bat) 
      {
        curLim_tmp = curLim_bat;
      }

    if (curLim_tmp > CURLIM_MAX) 
      {
        curLim_tmp = CURLIM_MAX;
      }

    // set curlim for _L_ and _R_ channel
    OCR_CurLim_L_ = curLim_tmp;
    OCR_CurLim_R_ = curLim_tmp;
}

// this function shall be called every 2ms or more often
void launch_envTest(void) {
    // TODO : acquerir les donnees de batterie
    // TODO : acquerir les donnees de temperature
    curLim_temp = 50;
    curLim_bat = 60;

    update_curLim();
}

// set the software-programmed current limit
void setCurLim_soft(uint8_t curLim) {
    curLim_soft = curLim;
    update_curLim();
}

