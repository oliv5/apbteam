#include "mp_pwm_LR_.h"
#include "mp_pwm_L_.h"



// Le PC, afin de faire le saut calculé
#define PC PC_REG

// Les sorties de contrôdu pont en H
// A/B : branche du pont
// H/L : High ou Low
// _L_ : pont Left
#define _L_AL // TODO ! à conpléter !
#define _L_AH // TODO ! à conpléter !
#define _L_BL // TODO ! à conpléter !
#define _L_BH // TODO ! à conpléter !

// static variables
static uint8_t state_L_;
static uint8_t state_L_cmd = 0x03;
static uint8_t pwm_L_;


// this file contains routines for managing the _L_ channel of mp board
// the command sed -e 's/_L_/_R_/g' can be used for generating the _R_ file
//
// rising edge = timer overflow = TOV interrupt (TODO : à programmer)
void rise_L_ (void) {
    // programs the state which is ordered by the core code
    state_L_ = state_L_cmd;

    // the falling of the other side may have delayed a few our IT
    OCR_L_ = pwm_L_ + Timer_L_; // TODO: OCR_L_value shall be > than x%

    //PC = PC + state_L_;	// j'aurais bien aimé faire un calculated jump

    switch (state_L_) 
      {
      case 0x00:
	// dir 0
	//rise_L_label0:
	_L_BH = 0;
	_L_BL = 1;
	_L_AL = 0;
	_L_AH = 1;
	sei();	// set back interrupts
	return;
	break;

      case 0x01:
	// dir 1
	//org rise_L_label0 + 0x10
	_L_AH = 0;
	_L_AL = 1;
	_L_BL = 0;
	_L_BH = 1;
	sei(); 	// set back interrupts
	return;
	break;

      case 0x02:
	// switch to forced low steady state
	//org rise_L_label0 + 0x20
	_L_AH = 0;
	_L_AL = 1;
	_L_BH = 0;
	_L_BL = 1;
	sei(); 	// set back interrupts
	return;
	break;

      case 0x03:
	// switch to high impedance steady state
	//org rise_L_label0 + 0x30
	_L_AL = 0;
	_L_AH = 0;
	_L_BL = 0;
	_L_BH = 0;
	sei(); 	// set back interrupts
	return;
	break;

      }

}

// falling edge = timer crossing OCR : OCn interrupt (TODO : à programmer)
void fall_L_ (void) {
    //	PC = PC + state_L_; TODO :saut calculé ?

    switch (state_L_) 
      {
      case 0x00:
	// in the case we are in 0x00 direction
	_L_AH = 0;
	_L_AL = 1;
	sei(); 	// set back interrupts
	return;
	break;

      case 0x01:
	// in the case we are in 0x10 direction
	//org fall_L_label0 + 0x10
	_L_BH = 0;
	_L_BL = 1;
	sei(); 	// set back interrupts
	return;
	break;

      case 0x02:
	// forced low
	//org fall_L_label0 + 0x20
	sei(); 	// set back interrupts
	return;
	break;

      case 0x03:
	// left high Z
	//org fall_L_label0 + 0x30
	sei(); 	// set back interrupts
	return;
	break;
      }
}


// overcurrent detected by comparators
void ovc_L_ (void) {
    _L_AL = 0;
    _L_AH = 0;
    _L_BL = 0;
    _L_BH = 0;
    sei(); 	// set back interrupts
    // following line orders to keep high Z state when faling edge will arrive
    state_L_ = 0x30;
    return;
}

// starts the motor
void start_motor_L_ (uint8_t pwmspeed, uint8_t direction) {
    // checking direction
    if (direction) 
      {
	state_L_cmd = 0x10;
      }
    else
      {
	state_L_cmd = 0x00;
      }

    // setting pwm value
    if (pwmspeed == 0)
      {// brake
	state_L_cmd = 0x20;
	pwm_L_ = 0;
      }
    else if (pwmspeed > PWM_MAX_LR_) 
      {// over PWM_MAX_LR_
	pwm_L_ = PWM_MAX_LR_ + PWM_OFFSET_LR_;
      }
    else if (pwmspeed < PWM_MIN_LR_) 
      {// under PWM_MIN_LR_
	pwm_L_ = PWM_MIN_LR_ + PWM_OFFSET_LR_;
      }
    else 
      {// correct PWM value
	pwm_L_ = pwmspeed + PWM_OFFSET_LR_;
      }
}

// puts motor in high Z
void stop_motor_L_ (void) {
    state_L_ = 0x30;
}

