/* "mp_pwm_L_.c"
 * this file contains routines for managing the _L_ channel of mp board
 * the command sed -e 's/_L_/_R_/g' can be used for generating the _R_ file
 */

#include "mp_pwm_LR_.h"
#include "mp_pwm_L_.h"
#include "io.h"

// static variables
static uint8_t state_L_;
static uint8_t state_L_cmd;

// Le PC, afin de faire le saut calculé
//#define PC PC_REG

// init
void init_pwm_L_ (void) {
    state_L_cmd = CMD_STATE_HIGH_Z;
    OCR_L_ = 0x00;

    // Set outputs to 0 (ie HIGH_Z)
	_L_AL_0;
	_L_AH_0;
	_L_BL_0;
	_L_BH_0;

    // status LEDs
    _L_LED0_0;
    _L_LED1_0;

    // Set IOs as outputs
    _L_ACTIVATE_OUTPUTS;
}

// PWM rising edge on timer overflow IT
ISR(OVF_L_vect) {
    // programs the state which is ordered by the core code
    state_L_ = state_L_cmd;

    switch (state_L_) 
      {
      case CMD_STATE_DIR_0:
	// dir 0
	//rise_L_label0:
	_L_BH_0;
	_L_BL_1;
	_L_AL_0;
	_L_AH_1;
	sei();	// set back interrupts

    // Display CMD_STATE on LEDs
    _L_LED0_1;
    _L_LED1_0;

	break;

      case CMD_STATE_DIR_1:
	// dir 1
	//org rise_L_label0 + 0x10
	_L_AH_0;
	_L_AL_1;
	_L_BL_0;
	_L_BH_1;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _L_LED0_0;
    _L_LED1_1;

	break;

      case CMD_STATE_BRAKE:
	// switch to forced low steady state
	//org rise_L_label0 + 0x20
	_L_AH_0;
	_L_AL_1;
	_L_BH_0;
	_L_BL_1;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _L_LED0_0;
    _L_LED1_0;

	break;

      case CMD_STATE_HIGH_Z:
      default:
	// switch to high impedance steady state
	//org rise_L_label0 + 0x30
	_L_AL_0;
	_L_AH_0;
	_L_BL_0;
	_L_BH_0;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _L_LED0_1;
    _L_LED1_1;

	break;

      }
}

// PWM falling edge on timer compare IT
ISR(COMP_L_vect) {

    switch (state_L_) 
      {
      case CMD_STATE_DIR_0:
	// in the case we are in 0x00 direction
	//fall_L_label0:
	_L_AH_0;
	_L_AL_1;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _L_LED0_0;
    _L_LED1_0;

	break;

      case CMD_STATE_DIR_1:
	// in the case we are in 0x10 direction
	//org fall_L_label0 + 0x10
	_L_BH_0;
	_L_BL_1;
	sei(); 	// set back interrupts

    // Display CMD_STATE on LEDs
    _L_LED0_0;
    _L_LED1_0;

	break;

      case CMD_STATE_BRAKE:
	// forced low
	//org fall_L_label0 + 0x20
	sei(); 	// set back interrupts
	break;

      case CMD_STATE_HIGH_Z:
      default:
	// left high Z
	//org fall_L_label0 + 0x30
	sei(); 	// set back interrupts
	break;
      }
}

// overcurrent detected by comparators
ISR(ILIM_L_vect) {
    // Set outputs in High-Z
    _L_AL_0;
    _L_AH_0;
    _L_BL_0;
    _L_BH_0;

    // following line orders to keep high Z state when faling edge will arrive
    state_L_ = CMD_STATE_HIGH_Z;

    sei(); 	// set back interrupts

    // Update LEDs
    _L_LED0_0;
    _L_LED1_0;

    return;
}

// starts the motor
void start_motor_L_ (uint8_t pwmspeed, uint8_t direction) {
    // checking direction
    if (direction) 
    {
        state_L_cmd = CMD_STATE_DIR_1;
    }
    else
    {
        state_L_cmd = CMD_STATE_DIR_0;
    }

    // setting pwm value
    if (pwmspeed == 0)
    {// brake
        state_L_cmd = CMD_STATE_BRAKE;
        OCR_L_ = 0;
    }
    else
    {
        // adding offset
        pwmspeed = pwmspeed + PWM_OFFSET_LR_;

        // clamp the value
        UTILS_BOUND(pwmspeed, PWM_MIN_LR_, PWM_MAX_LR_);

        // Apply the value
        OCR_L_ = pwmspeed;
    }
}

// puts motor in high Z
void stop_motor_L_ (void) {
    state_L_cmd = CMD_STATE_HIGH_Z;
}
