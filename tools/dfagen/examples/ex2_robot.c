/* Example 2 extra code. */
#include "ex2_robot.h"

#include <stdio.h>

int
main (void)
{
    robot_t robot;
    ex2_init (&robot);
    robot.bottle = 3;
    puts ("A glass:");
    robot.ice = 1;
    ex2_handle_event (&robot, EX2_EVENT_command);
    ex2_handle_event (&robot, EX2_EVENT_ice_dropped);
    ex2_handle_event (&robot, EX2_EVENT_glass_filled);
    puts ("Another glass:");
    robot.ice = 0;
    ex2_handle_event (&robot, EX2_EVENT_command);
    ex2_handle_event (&robot, EX2_EVENT_glass_filled);
    puts ("Yet another glass:");
    robot.ice = 0;
    ex2_handle_event (&robot, EX2_EVENT_command);
    ex2_handle_event (&robot, EX2_EVENT_glass_filled);
    puts ("There is no more liquid:");
    robot.ice = 0;
    ex2_handle_event (&robot, EX2_EVENT_command);
    puts ("Replace bootle:");
    ex2_handle_event (&robot, EX2_EVENT_replace_bottle);
    puts ("Another glass:");
    robot.ice = 1;
    ex2_handle_event (&robot, EX2_EVENT_command);
    puts ("Commands are ignore while the robot is fonctionning:");
    ex2_handle_event (&robot, EX2_EVENT_command);
    ex2_handle_event (&robot, EX2_EVENT_ice_dropped);
    ex2_handle_event (&robot, EX2_EVENT_command);
    ex2_handle_event (&robot, EX2_EVENT_glass_filled);
    return 0;
}
