#ifndef ex2_robot_h
#define ex2_robot_h
/* Example 2 type definition. */

#include "ex2.h"

/* Robot structure. */
struct robot_t
{
    /* The generated FSM. */
    ex2_state_t fsm;
    /* True for command with ice. */
    int ice;
    /* Number of glasses in the bottle. */
    int bottle;
};

#endif /* ex2_robot_h */
