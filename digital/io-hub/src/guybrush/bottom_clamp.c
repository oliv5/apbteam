/* bottom_clamp.c */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Julien Balleyguier
 *
 * APBTeam:
 *        Web: http://apbteam.org/
 *      Email: team AT apbteam DOT org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * }}} */
#include "common.h"
#include "output.h"

#include "mimot.h"
#include "contact.h"
#include "bot.h"


#define FSM_NAME AI
#include "fsm.h"
#include "fsm_queue.h"

/*
 * There is two FSM in this file.
 *
 * The clamp FSM handles high level clamp behaviour, new elements, drop, and
 * gives orders to the clamp move FSM.
 *
 * The clamp move FSM only handle moving the clamp without load or moving an
 * element from a slot to another one.
 */

FSM_STATES (
    /* Initial state. */
    CLAMP_START,

    /* Initialisation sequence: */
    /* opening the 2 clamps */
    CLAMP_INIT_OPEN,
    /* Finding 0 position */
    CLAMP_INIT_FIND_0,
    /* Hide the clamp in order not to go over the robot. */
    CLAMP_INIT_HIDE_CLAMP,
    /* Clamp ready, waiting in rest position. */
    CLAMP_INIT_READY,
    /* Returning to idle position. */
    CLAMP_GOING_IDLE,
    /* Waiting external events, clamp at middle level. */
    CLAMP_IDLE,
    /* Taking a coin */
    CLAMP_TAKE_COIN,
    /*  turning half way */
    CLAMP_TURN_HALF_WAY,
    /* Droping the cd. */
    CLAMP_DROP_CD,
    /* Clamp blocked. */
    CLAMP_BLOCKED,
    BOTTOM_CLAMP_HIDE_POS,
    UNFOLD_UPPER_SET,
    BOTTOM_CLAMP_READY,
    READY_TO_EMPTY_TREE,
    CLOSE_ALL_CLAMPS,
    REARRANGE_CD,
    BOTTOM_CLAMP_HIDE_POS2,
    RELEASE_ASSERV,
    FOLD_UPPER_SET,
    OPEN_UPPER_CLAMPS,
    REARRANGE_CD_CLAMP,
    BOTOM_CLAMP_BACK
    )

FSM_EVENTS (
    /* coin detected in the clamp */
    coin_detected,
    /* Lower clamp rotation motor success. */
    lower_clamp_rotation_success,
    /* Lower clamp rotation motor failure. */
    lower_clamp_rotation_failure,
    /*upper set is totally open*/
    upper_set_up,
    /*upper set is totally close*/
    upper_set_down,
    /* Tree detected, get ready to empty it. */
    tree_detected,
    /* stop the tree-emptying cycle*/
    stop_tree_approach,
    /* signal sent when the robot is ready to empty the tree. the approach has been finished*/
    clamps_ready,
    /* the robot is ready to empty the tree, and the top fsm is asking him to do so*/
    empty_tree,
    robot_is_back
     )


FSM_START_WITH (CLAMP_START)

/*-------------------------------------
         TIMEOUT DEFINITION
---------------------------------------*/
#define TIMEOUT_OPEN_CLAMPS 40
#define TIMEOUT_CLOSE_CLAMPS 40
#define TIMEOUT_DROP_CD 100
#define TIMEOUT_FREE_ASSERV 100
#define TIMEOUT_RECENTRAGE 100

/*-------------------------------------
         ROTATION DEFINITION
---------------------------------------*/
#define HIDE_POS 3
#define BACK_TO_READY 16-HIDE_POS
#define HALF_TURN 8
#define HIDE_POS_TREE 3
#define HIDE_POS_TREE_2 1
#define BACK_TO_READY_TREE 16-HIDE_POS_TREE
#define SPEED_ROTATION 0x20
/*-------------------------------------
         Clamp context
---------------------------------------*/
struct clamp_t
{
    /** Current position. */
    int pos_current;
    /* Clamp_1 is low, which means, it is ready to take a coin*/
    uint8_t clamp_1_down;
    /** True if clamp 1 is open. */
    uint8_t clamp_1_open;
    /** True if clamp 2 is open. */
    uint8_t clamp_2_open;
};


static void
clamp_blocked (void)
{
}

/*Global context. */
struct clamp_t clamp_global;
#define ctx clamp_global

/*---------------------------------------------------------*/
/*  INIT part of the FSM                                   */
/*---------------------------------------------------------*/

/* Init Bottom clamp. */

FSM_TRANS (CLAMP_START, init_actuators, CLAMP_INIT_OPEN)
{
    /* Opening the 2 clamps. */
    IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
    IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
    /* recentrage the middle clamp. */
    IO_SET (OUTPUT_UPPER_CLAMP_OUT);
    IO_CLR (OUTPUT_UPPER_CLAMP_IN);

    return FSM_NEXT (CLAMP_START,init_actuators);

}

FSM_TRANS_TIMEOUT (CLAMP_INIT_OPEN, TIMEOUT_OPEN_CLAMPS, CLAMP_INIT_FIND_0)
{
    /*Findig the 0 position. */
    mimot_motor0_find_zero (0x05, 1, 0);
    ctx.pos_current = 0;
    return FSM_NEXT_TIMEOUT (CLAMP_INIT_OPEN);
}

FSM_TRANS (CLAMP_INIT_FIND_0, lower_clamp_rotation_success, CLAMP_INIT_HIDE_CLAMP)
{
    /* Hidding the clamp inside the robot. */
    int move_needed;
    move_needed = HIDE_POS * 250;
    ctx.pos_current += move_needed;
    mimot_move_motor0_absolute (ctx.pos_current, SPEED_ROTATION);
    return FSM_NEXT (CLAMP_INIT_FIND_0, lower_clamp_rotation_success);
}

FSM_TRANS (CLAMP_INIT_HIDE_CLAMP, lower_clamp_rotation_success, CLAMP_INIT_READY)
{
    /*Nothing to do,just waiting for the clamp to be hidden*/
    return FSM_NEXT (CLAMP_INIT_HIDE_CLAMP, lower_clamp_rotation_success);
}


FSM_TRANS (CLAMP_INIT_HIDE_CLAMP, lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    /*The clamp is blocked somehow*/
    clamp_blocked();
    return FSM_NEXT (CLAMP_INIT_HIDE_CLAMP, lower_clamp_rotation_failure);
}

FSM_TRANS (CLAMP_INIT_READY,init_start_round, CLAMP_GOING_IDLE)
{
    int move_needed;
    move_needed = BACK_TO_READY * 250;   
    ctx.pos_current += move_needed;
    mimot_move_motor0_absolute (ctx.pos_current, SPEED_ROTATION);
    ctx.clamp_1_down = 1;
    return FSM_NEXT (CLAMP_INIT_READY, init_start_round);
}

FSM_TRANS (CLAMP_GOING_IDLE, lower_clamp_rotation_success, CLAMP_IDLE)
{
    /*Going back to the idle position, ready for showtime.*/
    return FSM_NEXT (CLAMP_GOING_IDLE, lower_clamp_rotation_success);
}

FSM_TRANS (CLAMP_GOING_IDLE, lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    /* The clamp is blocked somehow. */
    clamp_blocked();
    return FSM_NEXT (CLAMP_GOING_IDLE, lower_clamp_rotation_failure);
}


/*---------------------------------------------------------*/
/*  parts of the FSM that Takes coin                       */
/*---------------------------------------------------------*/
FSM_TRANS (CLAMP_IDLE, coin_detected, CLAMP_TAKE_COIN)
{
    if (ctx.clamp_1_down)
    {
        /*Close it.*/
        IO_SET (OUTPUT_LOWER_CLAMP_1_CLOSE);
    }
    /*If the clamp 2 is ready*/
    else             
    {
        IO_SET(OUTPUT_LOWER_CLAMP_2_CLOSE);
    }
    return FSM_NEXT (CLAMP_IDLE, coin_detected);
}


FSM_TRANS_TIMEOUT (CLAMP_TAKE_COIN, TIMEOUT_CLOSE_CLAMPS, CLAMP_TURN_HALF_WAY)
{
    int move_needed;
    move_needed = HALF_TURN * 250;
    ctx.pos_current += move_needed;
    mimot_move_motor0_absolute (ctx.pos_current, SPEED_ROTATION);
    return FSM_NEXT_TIMEOUT (CLAMP_TAKE_COIN);
}

FSM_TRANS_TIMEOUT (CLAMP_TURN_HALF_WAY, TIMEOUT_DROP_CD, CLAMP_DROP_CD)
{
    /*If the clamp 1 has the CD.*/
    if (ctx.clamp_1_down)
    {     
        /*Open it.*/
        IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
        /*Clamp 1 is now up (clamp 2 is down).*/
        ctx.clamp_1_down = 0;
    }
    /*If the clamp 2 is closed. */
    else       
    {
        IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
        /*Clamp 1 is now down (clamp 2 is up). */
        ctx.clamp_1_down = 1;            
    }
    return FSM_NEXT_TIMEOUT (CLAMP_TURN_HALF_WAY);
}

FSM_TRANS (CLAMP_DROP_CD,lower_clamp_rotation_success,CLAMP_IDLE)
{
    /*Nothing to do, just waiting to be in IDLE state again*/
    return FSM_NEXT (CLAMP_DROP_CD,lower_clamp_rotation_success);
}

FSM_TRANS (CLAMP_DROP_CD, lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    /*The clamp is blocked somehow. */
    clamp_blocked();
    return FSM_NEXT (CLAMP_DROP_CD, lower_clamp_rotation_failure);
}
/*---------------------------------------------------------*/
/*  Parts of the FSM that is Approaching the tree (totem)  */
/*---------------------------------------------------------*/

FSM_TRANS (CLAMP_IDLE, tree_detected, BOTTOM_CLAMP_HIDE_POS)
{
    /*Contrepression*/
    IO_SET (OUTPUT_UPPER_CLAMP_UP);
    /*Hidding the clamp inside the robot*/
    int move_needed;
    if (ctx.clamp_1_down)
    {
        move_needed = HIDE_POS_TREE * 250;
    }
    else
    {
        move_needed = (HALF_TURN + HIDE_POS_TREE) * 250;
        ctx.clamp_1_down = 1;
    }
    ctx.pos_current += move_needed;
    mimot_move_motor0_absolute (ctx.pos_current, SPEED_ROTATION);
    return FSM_NEXT (CLAMP_IDLE, tree_detected);
}


FSM_TRANS (BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_success, UNFOLD_UPPER_SET)
{
    IO_CLR (OUTPUT_UPPER_CLAMP_UP);
    IO_SET (OUTPUT_UPPER_CLAMP_DOWN);
    return FSM_NEXT (BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_success);
}

FSM_TRANS (BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    /*The clamp is blocked somehow.*/
    clamp_blocked();
    return FSM_NEXT (BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_failure);
}


FSM_TRANS (UNFOLD_UPPER_SET, upper_set_down, BOTTOM_CLAMP_READY)
{
    /*Putting the bottom clamp back to ready.*/
    int move_needed;
    move_needed = BACK_TO_READY_TREE * 250;    
    ctx.pos_current += move_needed;
    mimot_move_motor0_absolute (ctx.pos_current, SPEED_ROTATION);

    /*Opening the top clamp.*/
     IO_SET (OUTPUT_UPPER_CLAMP_OPEN);

    return FSM_NEXT (UNFOLD_UPPER_SET, upper_set_down);

}

FSM_TRANS (BOTTOM_CLAMP_READY, lower_clamp_rotation_success, READY_TO_EMPTY_TREE)
{

    fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
    return FSM_NEXT (BOTTOM_CLAMP_READY, lower_clamp_rotation_success);

}

FSM_TRANS (BOTTOM_CLAMP_READY, lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    /*The clamp is blocked somehow.*/
    clamp_blocked();
    return FSM_NEXT (BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_failure);
}

FSM_TRANS (READY_TO_EMPTY_TREE, empty_tree, CLOSE_ALL_CLAMPS)
{
    /*Closgin bottom clamp. */
    IO_SET (OUTPUT_LOWER_CLAMP_1_CLOSE);
    /*We need to close both clamp to have an easier turn around. */
    IO_SET (OUTPUT_LOWER_CLAMP_2_CLOSE);


    /*Closing upper & middle clamps.*/
    IO_CLR (OUTPUT_UPPER_CLAMP_OPEN);

    fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
    return FSM_NEXT (READY_TO_EMPTY_TREE, empty_tree);

}

/*---------------------------------------------------------*/
/*  Parts of the FSM that is emptying the tree (totem)     */
/*---------------------------------------------------------*/

FSM_TRANS(CLOSE_ALL_CLAMPS, robot_is_back,REARRANGE_CD)
{
    IO_CLR (OUTPUT_UPPER_CLAMP_OUT);
    IO_SET (OUTPUT_UPPER_CLAMP_IN);
    return FSM_NEXT (CLOSE_ALL_CLAMPS, robot_is_back);

}

FSM_TRANS_TIMEOUT (REARRANGE_CD, TIMEOUT_RECENTRAGE, BOTTOM_CLAMP_HIDE_POS2)
{
    /*Hidding the clamp inside the robot.*/
    int move_needed;
    move_needed = HIDE_POS_TREE_2 * 250;
    ctx.pos_current += move_needed;
    mimot_move_motor0_absolute (ctx.pos_current, SPEED_ROTATION);
    return FSM_NEXT_TIMEOUT (REARRANGE_CD);
}
FSM_TRANS (BOTTOM_CLAMP_HIDE_POS2, lower_clamp_rotation_success, RELEASE_ASSERV)
{
    mimot_motor0_free();
    return FSM_NEXT (BOTTOM_CLAMP_HIDE_POS2, lower_clamp_rotation_success);

}

FSM_TRANS_TIMEOUT (RELEASE_ASSERV, TIMEOUT_FREE_ASSERV, FOLD_UPPER_SET)
{
    IO_CLR (OUTPUT_UPPER_CLAMP_DOWN);
    IO_SET (OUTPUT_UPPER_CLAMP_UP);
    return FSM_NEXT_TIMEOUT (RELEASE_ASSERV);

}
FSM_TRANS (FOLD_UPPER_SET, upper_set_up, OPEN_UPPER_CLAMPS)
{
    IO_SET (OUTPUT_UPPER_CLAMP_OPEN);
    IO_CLR (OUTPUT_UPPER_CLAMP_IN);
    IO_SET (OUTPUT_UPPER_CLAMP_OUT);
    return FSM_NEXT (FOLD_UPPER_SET, upper_set_up);

}

FSM_TRANS_TIMEOUT (OPEN_UPPER_CLAMPS, TIMEOUT_OPEN_CLAMPS, CLAMP_TURN_HALF_WAY)
{
    IO_SET (OUTPUT_UPPER_CLAMP_OPEN);
    /*We reopen clamp 2.*/
    IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
    ctx.clamp_2_open = 1;
    int move_needed;
    move_needed = (HALF_TURN-HIDE_POS_TREE_2) * 250;
    ctx.pos_current += move_needed;
    mimot_move_motor0_absolute (ctx.pos_current, SPEED_ROTATION);
    fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
    return FSM_NEXT_TIMEOUT (OPEN_UPPER_CLAMPS);
}

