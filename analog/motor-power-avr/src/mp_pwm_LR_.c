

// This file contains general pwm routines for mp (motor-power) board
//
// state_L_ : x - Inhib - changeDir - Dir - x - x - x - x
// Timer_L_ : timer dedicated to Left side

void init_timer_LR_(void) {
    Timer_L_ = 0x00;
    state_L_ = 0x40;
    Timer_R_ = 0x80;
    state_R_ = 0x40;
    // TODO : set interrupts
    // TODO : set OCR to 0
    START_TIMER_L_;
    START_TIMER_R_;
}

void init_curLim (void) {
    // TODO : set interrupts
}

uint8_t get_curLim_temp (uint8_t temperature) {
    return (temperature - 40) >> 2;	// TODO : ajuster la fonction de transfert
}

uint8_t get_curLim_bat (uint8_t battery) {
    return (battery - 40) >> 2;		// TODO : ajuster la fonction de transfert
}

// this function shall be called after each adjustment of any current limit
void update_curLim(void) {
    uint8 curLim_tmp;
    uint8 ret;

    if (curLim_soft > curLim_temp) then
        curLim_tmp = curLim_temp
    else
        curLim_tmp = curLim_soft;

    if (curLim_tmp > curLim_bat) then
        curLim_tmp = curLim_bat;

    OCR_CurLim_L_ = curLim_tmp;
    OCR_CurLim_R_ = curLim_tmp;
}

// this function shall be called every 2ms or more often
void launch_envTest(void) {
    // TODO : acquerir les donnees de batterie
    // TODO : acquerir les donnees de temperature
    curLim_temp = 1;
    curLim_bat = 2;

    update_current_limit();
}

// set the software-programmed current limit
void setCurLim_soft(uint8_t curLim) {
    curLim_soft = curLim;
    update_current_limit();
}

