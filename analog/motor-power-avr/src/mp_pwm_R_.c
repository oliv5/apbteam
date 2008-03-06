/* "mp_pwm_R_.c"
 * this file contains routines for managing the _R_ channel of mp board
 * the command sed -e 's/_R_/_R_/g' can be used for generating the _R_ file
 */

#include "mp_pwm_LR_.h"
#include "mp_pwm_R_.h"
#include "io.h"

// static variables
static uint8_t state_R_;
static uint8_t state_R_cmd;
static uint8_t pwm_R_;

// Le PC, afin de faire le saut calculé
//#define PC PC_REG

// init
void init_pwm_R_ (void) {
    state_R_cmd = CMD_STATE_HIGH_Z;
    pwm_R_ = 0x00;

    // Set outputs to 0 (ie HIGH_Z)
	_R_AL_0;
	_R_AH_0;
	_R_BL_0;
	_R_BH_0;

    // status LEDs
    _R_LED0_0;
    _R_LED1_0;

    // Set IOs as outputs
    _R_ACTIVATE_OUTPUTS;
}

// PWM rising edge on timer overflow IT
ISR(OVF_R_vect) {
    // programs the state which is ordered by the core code
    state_R_ = state_R_cmd;

    // the falling of the other side may have delayed a few our IT
    OCR_R_ = pwm_R_;

    //PC = PC + state_R_;	// j'aurais bien aimé faire un calculated jump

    switch (state_R_) 
      {
      case CMD_STATE_DIR_0:
	// dir 0
	//rise_R_label0:
	_R_BH_0;
	_R_BL_1;
	_R_AL_0;
	_R_AH_1;
	sei();	// set back interrupts

    // Display CMD_STATE on LEDs
    _R_LED0_1;
    _R_LED1_0;

	break;

      case CMD_STATE_DIR_1:
	// dir 1
	//org rise_R_label0 + 0x10
	_R_AH_0;
	_R_AL_1;
	_R_BL_0;
	_R_BH_1;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _R_LED0_0;
    _R_LED1_1;

	break;

      case CMD_STATE_BRAKE:
	// switch to forced low steady state
	//org rise_R_label0 + 0x20
	_R_AH_0;
	_R_AL_1;
	_R_BH_0;
	_R_BL_1;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _R_LED0_0;
    _R_LED1_0;

	break;

      case CMD_STATE_HIGH_Z:
      default:
	// switch to high impedance steady state
	//org rise_R_label0 + 0x30
	_R_AL_0;
	_R_AH_0;
	_R_BL_0;
	_R_BH_0;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _R_LED0_1;
    _R_LED1_1;

	break;

      }
}

// PWM falling edge on timer compare IT
ISR(COMP_R_vect) {
    //	PC = PC + state_R_; TODO :saut calculé ?

    switch (state_R_) 
      {
      case CMD_STATE_DIR_0:
	// in the case we are in 0x00 direction
	//fall_R_label0:
	_R_AH_0;
	_R_AL_1;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _R_LED0_0;
    _R_LED1_0;

	break;

      case CMD_STATE_DIR_1:
	// in the case we are in 0x10 direction
	//org fall_R_label0 + 0x10
	_R_BH_0;
	_R_BL_1;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _R_LED0_0;
    _R_LED1_0;

	break;

      case CMD_STATE_BRAKE:
	// forced low
	//org fall_R_label0 + 0x20
	sei(); 	// set back interrupts
	break;

      case CMD_STATE_HIGH_Z:
      default:
	// left high Z
	//org fall_R_label0 + 0x30
	sei(); 	// set back interrupts
	break;
      }
}

// overcurrent detected by comparators
ISR(ILIM_R_vect) {
    _R_AL_0;
    _R_AH_0;
    _R_BL_0;
    _R_BH_0;
    sei(); 	// set back interrupts
    // following line orders to keep high Z state when faling edge will arrive
    state_R_ = CMD_STATE_HIGH_Z;
    return;
}

// starts the motor
void start_motor_R_ (uint8_t pwmspeed, uint8_t direction) {
    // checking direction
    if (direction) 
    {
        state_R_cmd = CMD_STATE_DIR_1;
    }
    else
    {
        state_R_cmd = CMD_STATE_DIR_0;
    }

    // setting pwm value
    if (pwmspeed == 0)
    {// brake
        state_R_cmd = CMD_STATE_BRAKE;
        pwm_R_ = 0;
    }
    else
    {
        // adding offset
        pwmspeed = pwmspeed + PWM_OFFSET_LR_;

        // clamp the value
        UTILS_BOUND(pwmspeed, PWM_MIN_LR_, PWM_MAX_LR_);

        // Apply the value
        pwm_R_ = pwmspeed;
    }
}

// puts motor in high Z
void stop_motor_R_ (void) {
    state_R_ = CMD_STATE_HIGH_Z;
}
