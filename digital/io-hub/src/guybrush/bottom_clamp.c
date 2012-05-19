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
#include "main.h"
#include "bottom_clamp.h"
#include "pressure.h"
#include "strat.h"


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

    /* ---------------------Initialisation sequence---------------------- */
    /*Opening all the clamps. */
    CLAMP_INIT_OPEN_ALL_CLAMPS,
    /*Closing all the clamps (except bottom clamp 2). */
    CLAMP_INIT_CLOSE_ALL_CLAMPS,
    /*Finding 0 position. */
    CLAMP_INIT_FIND_0,
    /* Hide the clamp in order not to go over the robot. */
    CLAMP_INIT_HIDE_CLAMP,
    /* Clamp ready, waiting in rest position. */
    CLAMP_INIT_READY,
    /* Returning to idle position. */
    CLAMP_GOING_IDLE,

    /*--------------------------IDLE---------------------------------------*/
    /*Wait before going idle*/
    CLAMP_WAIT_BEFORE_IDLE,

    /* Waiting external events. */
    CLAMP_IDLE,
    /*---------------------------Cleaning area sequence---------------------*/
    CLAMP_READY_TO_CLEAN,
    CLAMP_CATCH_COIN,
    CLAMP_READY_TO_LOAD, 
    /*---------------------------Taking coin sequence-----------------------*/
    /*Taking a coin. */
    CLAMP_TAKE_COIN,
    /*Turning half way. */
    CLAMP_TURN_HALF_WAY,
    /* Droping the cd. */
    CLAMP_DROP_CD,

    /*--------------------------Approaching Tree sequence------------------*/
    /*Hide bottom clamp to unfold the upper set. */
    CLAMP_BOTTOM_CLAMP_HIDE_POS,
    /*unfold the upper set. */
    CLAMP_UNFOLD_UPPER_SET,
    /*Putting the bottom clamp back in position. */
    CLAMP_BOTTOM_CLAMP_READY,
    /*All the clamps are now open and the robot is ready to empty the tree. */
    CLAMP_READY_TO_EMPTY_TREE,
    /*Close all clamps to empty the tree. */
    CLAMP_CLOSE_ALL_CLAMPS,
    /*Recentrage of the CD clamps. */
    CLAMP_REARRANGE_CD,
    /*Hiding bottom clamp to fold back the upper set. */
    CLAMP_BOTTOM_CLAMP_HIDE_POS2,
    /*Release the asserve not to block the clamp when going back up. */
    CLAMP_RELEASE_ASSERV,
    /*fold back the upper set. */
    CLAMP_FOLD_UPPER_SET,
    /*Open upper clamps to release objects. */
    CLAMP_OPEN_UPPER_CLAMPS,
    /*Make the coin "fall" into the clamp (the clamp has to be 90° from the floor*/
    CLAMP_READY_TO_RECALE,
    CLAMP_READY_TO_RECALE_2,
    CLAMP_TEMPO_RECALE,
    CLAMP_END_RECALE,

    /*-------------------------Stop tree approach extra states--------------*/
    /*Letting the bottom clamp to finish it's move before starting the stop tree procedure. */
    CLAMP_FINISH_BOTTOM_CLAMP_READY,
    /*Putting the bottom clamp back in position after a stop tree approach signal. */
    CLAMP_BOTTOM_CLAMP_BACK,

    /*------------------------ Clamp blocked------------------------------. */
    CLAMP_BLOCKED_UPPER_SET_DOWN,
    CLAMP_BLOCKED,
    CLAMP_OPEN_BOTTOM_CLAMPS,
    CLAMP_TURN_BACKWARD,
    CLAMP_WAIT,
    CLAMP_TURN_FORWARD,
    CLAMP_SHITTY_STATE,
    /*-------------------------Upper set not going down-------------------- */
    CLAMP_UPPER_SET_BLOCKED_UP,
    CLAMP_UPPER_SET_FOLD_BACK,
    CLAMP_UPPER_SET_DEAD,
    CLAMP_TRY_ROTATION
    )

FSM_EVENTS (
    /* O detected*/
    0_found,
    /* coin detected in the clamp */
    coin_detected,
    /*signal sent to the top fsm when taking a coin*/
    taking_coin,
    /*time to drop the cd. Sent by main*/
    time_to_drop_coin,
    /* Lower clamp rotation motor success. */
    lower_clamp_rotation_success,
    /* Lower clamp rotation motor failure. */
    lower_clamp_rotation_failure,
    /*upper set is totally open*/
    upper_set_up,
    /*upper set is totally close*/
    upper_set_down,
    /*Clean the area*/
    clean_start,
    /*Clos the bottom clamp*/
    clean_catch,
    /* Load the coin*/
    clean_load,
    /* Tree detected, get ready to empty it. */
    tree_detected,
    /* stop the tree-emptying cycle*/
    stop_tree_approach,
    /* signal sent when the robot is ready to empty the tree. the approach has been finished*/
    clamps_ready,
    /* the robot is ready to empty the tree, and the top fsm is asking him to do so*/
    empty_tree,
    /* The robot has step back. Event given by the main.c programm(after an empty tree, or a clamp_block event. */
    robot_is_back,
    /* The clamp is blocked. We can try to unblock it*/
    clamp_blocked,
    /* Try to unblock clamp. */
    clamp_unblock,
    /* We tryed to unblock the clamp too many time. we can now say that the bottom clamp is out of order*/
    clamp_is_dead,
    /* The upper set is not moving anymore*/
    upper_set_is_dead
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
#define TIMEOUT_BLOCKED 100
#define TIMEOUT_IDLE 70
#define TIMEOUT_UPPER_SET_DOWN 1000

/*-------------------------------------
        ROTATON SPEED DEFINITION
---------------------------------------*/
#define FAST_ROTATION 0x30
#define MEDIUM_ROTATION 0x10
#define SLOW_ROTATION 0x01

/*-------------------------------------
        PRESSURE DEFINITION
---------------------------------------*/
#define LOW_PRESSURE 0x300
#define HIGH_PRESSURE 0x380

/*-------------------------------------
        POSITION DEFINITION
---------------------------------------*/
#define RECALE_COIN 1100
#define CLAMP_1_DOWN 3929
#define CLAMP_1_UP 1929
#define POS_TO_DROP 800
#define HIDE_POS_TREE 550
#define HIDE_POS_TREE_PLUS (HIDE_POS_TREE + 100)
#define DECALAGE_CD_BAS 319
#define LET_UPPER_SET_UP 500
#define DECALAGE_INIT 1662
/*-------------------------------------
         Clamp context
---------------------------------------*/
struct clamp_t
{
    /** Current position. */
    int32_t current_pos;
    /** Init position. */
    uint16_t init_pos;
    /* Clamp_1 is low, which means, it is ready to take a coin*/
    uint8_t clamp_1_down;
    /** True we are stopping the tree approach. */
    uint8_t stop_tree_approach;
    /** True if we are in "Calm mod" (clamps can't move)*/
    uint8_t calm_mode;   
    /** Compting the number of time the bottom clamp is being blocked in a row (reset when going back to  idle)*/
    uint8_t cpt_blocked;
    /** Request from top FSM to be processed once in IDLE state. */
    uint16_t idle_request;
    /** Compting how many times the upper set is blocked*/
    uint8_t upper_set_blocked_cpt;
    uint8_t unblocking_upper_set;
    uint8_t upper_set_is_dead;
};

/*Global context. */
struct clamp_t clamp_global;
#define ctx clamp_global


static void
clamp_blocked (void)
{
}

static int is_clamp_1_down(uint32_t pos)
{        
    if ((pos%4000 > 3000) || (pos%4000 < 1000))
        return 1;
    else
        return 0;
}



static void move_needed2(uint16_t angle,uint8_t speed,int8_t direction)
{
    int32_t  new_pos;
    if (angle >= ctx.current_pos%4000)
    {
        if (direction==1)
        {
             new_pos=ctx.current_pos-ctx.current_pos%4000+angle;
        }
        else
        {
             new_pos=ctx.current_pos-ctx.current_pos%4000+4000-angle;
        }
       
    }
    else
    {
        if (direction==1)
        {
            new_pos=ctx.current_pos-ctx.current_pos%4000+angle+4000;
        }
        else
        {  
            new_pos=ctx.current_pos-ctx.current_pos%4000+angle;
        }
    }
    mimot_move_motor0_absolute (new_pos+ctx.init_pos, speed);    
    ctx.current_pos = new_pos;
}


void clamp_calm_mode(int mode)
{
    ctx.calm_mode=mode;
}

uint8_t clamp_calm_mode_read(void)
{
    return ctx.calm_mode;
}

void
clamp_request (uint16_t event)
{
    /* If event can not be handled right now, keep it until going in IDLE
     * state. */
    if (!FSM_HANDLE_VAR (AI, event))
    {
        assert (!ctx.idle_request);
        ctx.idle_request = event;
    }
}

uint8_t clamp_read_blocked_cpt(void)
{
    return ctx.cpt_blocked;
}


/*---------------------------------------------------------*/
/*  INIT part of the FSM                                   */
/*---------------------------------------------------------*/

/* Init Bottom clamp. */
FSM_TRANS (CLAMP_START, init_actuators, CLAMP_INIT_OPEN_ALL_CLAMPS)
{
    /* Starting the pump */
    pressure_set(LOW_PRESSURE);
    /*Opening bottom Clamps*/
    IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
    IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
    /*Opening upper clamps*/
    IO_SET (OUTPUT_UPPER_CLAMP_OPEN);
    
    return FSM_NEXT (CLAMP_START,init_actuators);
    
}

FSM_TRANS_TIMEOUT (CLAMP_INIT_OPEN_ALL_CLAMPS,TIMEOUT_OPEN_CLAMPS, 
           upper_set_in,CLAMP_INIT_CLOSE_ALL_CLAMPS,
           upper_set_out,CLAMP_SHITTY_STATE)
{
    /*if upper set is inside the robot*/
    if(IO_GET(CONTACT_UPPER_CLAMP_DOWN))
    {
        /* closing all the clamps except the clamp 1 to proceed the find_O function. */
        IO_SET (OUTPUT_LOWER_CLAMP_2_CLOSE);
        IO_CLR (OUTPUT_UPPER_CLAMP_OPEN);
        return FSM_NEXT_TIMEOUT (CLAMP_INIT_OPEN_ALL_CLAMPS,upper_set_in);
    }
    /*if it is outside, go to shitty state and stop everything*/
    else
    {
        return FSM_NEXT_TIMEOUT (CLAMP_INIT_OPEN_ALL_CLAMPS,upper_set_out);
    }
    

}

FSM_TRANS_TIMEOUT (CLAMP_INIT_CLOSE_ALL_CLAMPS, 5*TIMEOUT_OPEN_CLAMPS, CLAMP_INIT_FIND_0)
{
    /*Findig the 0 position. */
    ctx.current_pos = 0;
    /* Init of the upper set blocked cpt */
    ctx.upper_set_blocked_cpt = 0;
    ctx.unblocking_upper_set = 0;
    ctx.upper_set_is_dead = 0;
    move_needed2(8000,SLOW_ROTATION,1);
    return FSM_NEXT_TIMEOUT (CLAMP_INIT_CLOSE_ALL_CLAMPS);
}

FSM_TRANS (CLAMP_INIT_FIND_0, 0_found, CLAMP_INIT_HIDE_CLAMP)
{
    /*init of the position*/
    ctx.init_pos = mimot_get_motor0_position() + DECALAGE_INIT;
    ctx.current_pos = 0;
    /* Robot is ready with clamp 1 at the bottom. */
    move_needed2(CLAMP_1_DOWN,FAST_ROTATION,1);
    ctx.clamp_1_down = 1;
    return FSM_NEXT (CLAMP_INIT_FIND_0, 0_found);
}

FSM_TRANS (CLAMP_INIT_HIDE_CLAMP, lower_clamp_rotation_success, 
           normal_start,CLAMP_INIT_READY,
           demo_start,CLAMP_GOING_IDLE)
{
    /*Opening the clamp 2. We do it know to save the batteries*/
    IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
    /*Looking if we are in demo mode or normal mode*/
    if (!main_demo)
        return FSM_NEXT (CLAMP_INIT_HIDE_CLAMP,lower_clamp_rotation_success,normal_start);
    else
        return FSM_NEXT (CLAMP_INIT_HIDE_CLAMP,lower_clamp_rotation_success,demo_start);
}

FSM_TRANS (CLAMP_INIT_READY,init_start_round, CLAMP_GOING_IDLE)
{
    return FSM_NEXT (CLAMP_INIT_READY, init_start_round);
}

FSM_TRANS_TIMEOUT (CLAMP_GOING_IDLE, 3*TIMEOUT_OPEN_CLAMPS, CLAMP_WAIT_BEFORE_IDLE)
{
    /*Going back to the idle position, ready for showtime.*/
    ctx.cpt_blocked = 0;
    /* Open upper clamp. */
    IO_SET (OUTPUT_UPPER_CLAMP_OPEN);
    /* recentrage the middle clamp. */
    IO_SET (OUTPUT_UPPER_CLAMP_OUT);
    IO_CLR (OUTPUT_UPPER_CLAMP_IN);
    /*Contrepression*/
    IO_SET (OUTPUT_UPPER_CLAMP_UP);
    return FSM_NEXT_TIMEOUT (CLAMP_GOING_IDLE);
}

/*---------------------------------------------------------*/
/*  IDLE!!                                                 */
/*---------------------------------------------------------*/
FSM_TRANS_TIMEOUT (CLAMP_WAIT_BEFORE_IDLE, TIMEOUT_IDLE,
                   request_clean_start, CLAMP_READY_TO_CLEAN,
		   request_tree_detected, CLAMP_BOTTOM_CLAMP_HIDE_POS,
		   idle, CLAMP_IDLE)
{
    if (ctx.idle_request)
    {
        uint16_t idle_request = ctx.idle_request;
        ctx.idle_request = 0;
        if (idle_request == FSM_EVENT (AI, clean_start))
        {
            fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
            return FSM_NEXT_TIMEOUT (CLAMP_WAIT_BEFORE_IDLE,
                                     request_clean_start);
        }
        else if (idle_request == FSM_EVENT (AI, tree_detected))
        {
            /* Hidding the clamp inside the robot. */
            if (is_clamp_1_down(ctx.current_pos))
            {
                move_needed2 (HIDE_POS_TREE, FAST_ROTATION,1);
            }
            else
            {
                move_needed2 (HIDE_POS_TREE, FAST_ROTATION,1);
                ctx.clamp_1_down = 1;
            }
            return FSM_NEXT_TIMEOUT (CLAMP_WAIT_BEFORE_IDLE,
                                     request_tree_detected);
        }
        else
        {
            assert (0);
            return FSM_NEXT_TIMEOUT (CLAMP_WAIT_BEFORE_IDLE, idle);
        }
    }
    else
    {
        /* Going back to the idle position, ready for showtime. */
        fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
        return FSM_NEXT_TIMEOUT (CLAMP_WAIT_BEFORE_IDLE, idle);
    }
}
/*---------------------------------------------------------*/
/*  parts of the FSM that Clean areas                      */
/*---------------------------------------------------------*/
FSM_TRANS (CLAMP_IDLE, clean_start,CLAMP_READY_TO_CLEAN) 
{
    fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
    return FSM_NEXT(CLAMP_IDLE,clean_start);
}

FSM_TRANS (CLAMP_READY_TO_CLEAN, clean_catch,CLAMP_CATCH_COIN) 
{
     /*If the clamp 1 has the CD.*/
    if (is_clamp_1_down(ctx.current_pos))
    {     
        /*Close it.*/
        IO_SET (OUTPUT_LOWER_CLAMP_1_CLOSE);
    }
    /*If the clamp 2 is closed. */
    else       
    {
        IO_SET (OUTPUT_LOWER_CLAMP_2_CLOSE);
    }
    return FSM_NEXT(CLAMP_READY_TO_CLEAN,clean_catch);
}
FSM_TRANS_TIMEOUT (CLAMP_CATCH_COIN, TIMEOUT_CLOSE_CLAMPS*2,CLAMP_READY_TO_LOAD) 
{
    fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
    return FSM_NEXT_TIMEOUT(CLAMP_CATCH_COIN);
}

FSM_TRANS (CLAMP_READY_TO_LOAD, clean_load,CLAMP_TAKE_COIN) 
{
    return FSM_NEXT(CLAMP_READY_TO_LOAD,clean_load);
}

/*---------------------------------------------------------*/
/*  parts of the FSM that Takes coin                       */
/*---------------------------------------------------------*/
FSM_TRANS (CLAMP_IDLE, coin_detected,CLAMP_TAKE_COIN)
{
    strat_coin_taken ();
    if (is_clamp_1_down(ctx.current_pos))
    {
        /*Close it.*/
        IO_SET (OUTPUT_LOWER_CLAMP_1_CLOSE);
    }
    /*If the clamp 2 is ready*/
    else             
    {
        IO_SET(OUTPUT_LOWER_CLAMP_2_CLOSE);
    }
    fsm_queue_post_event (FSM_EVENT (AI, taking_coin));
    return FSM_NEXT (CLAMP_IDLE, coin_detected);
}

FSM_TRANS_TIMEOUT (CLAMP_TAKE_COIN, TIMEOUT_CLOSE_CLAMPS, CLAMP_TURN_HALF_WAY)
{
    main_set_drop_coin_pos(ctx.current_pos + POS_TO_DROP + ctx.init_pos);
    if (is_clamp_1_down(ctx.current_pos))
    {
        move_needed2(CLAMP_1_UP,FAST_ROTATION,1);
    }
    else
    {
        move_needed2(CLAMP_1_DOWN,FAST_ROTATION,1);
    }

    return FSM_NEXT_TIMEOUT (CLAMP_TAKE_COIN);
}

FSM_TRANS (CLAMP_TURN_HALF_WAY, time_to_drop_coin, CLAMP_DROP_CD)
{
    /*If the clamp 1 up (ready to drop the cd).*/
    if (!is_clamp_1_down(ctx.current_pos))
    {     
        /*Open it.*/
        IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
    }
    /*If the clamp 2 is closed. */
    else       
    {
        IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);           
    }
    return FSM_NEXT (CLAMP_TURN_HALF_WAY,time_to_drop_coin);
}

FSM_TRANS (CLAMP_TURN_HALF_WAY, lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    return FSM_NEXT (CLAMP_TURN_HALF_WAY,lower_clamp_rotation_failure);
}

FSM_TRANS (CLAMP_DROP_CD,lower_clamp_rotation_success,CLAMP_WAIT_BEFORE_IDLE)
{
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

FSM_TRANS (CLAMP_IDLE, tree_detected,CLAMP_BOTTOM_CLAMP_HIDE_POS)
{
    uint16_t rotation;
    if (ctx.upper_set_blocked_cpt>0)
    {
        rotation = HIDE_POS_TREE_PLUS;
    }
    else
    {
        rotation = HIDE_POS_TREE;
    }
    /*Hidding the clamp inside the robot*/
    move_needed2(rotation,FAST_ROTATION,1);
    return FSM_NEXT (CLAMP_IDLE, tree_detected);
}


FSM_TRANS (CLAMP_BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_success, CLAMP_UNFOLD_UPPER_SET)
{
    IO_CLR (OUTPUT_UPPER_CLAMP_UP);
    IO_SET (OUTPUT_UPPER_CLAMP_DOWN);
    return FSM_NEXT (CLAMP_BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_success);
}

FSM_TRANS (CLAMP_BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    /*The clamp is blocked somehow.*/
    clamp_blocked();
    return FSM_NEXT (CLAMP_BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_failure);
}


FSM_TRANS (CLAMP_UNFOLD_UPPER_SET, upper_set_down, CLAMP_BOTTOM_CLAMP_READY)
{
    /*Putting the bottom clamp back to ready.*/
    move_needed2(DECALAGE_CD_BAS,FAST_ROTATION,-1);    
    
    /*Opening the top clamp.*/
    IO_SET (OUTPUT_UPPER_CLAMP_OPEN);

    return FSM_NEXT (CLAMP_UNFOLD_UPPER_SET, upper_set_down);

}

/** If the upper set doesn't go down*/
FSM_TRANS_TIMEOUT (CLAMP_UNFOLD_UPPER_SET, TIMEOUT_UPPER_SET_DOWN, CLAMP_UPPER_SET_BLOCKED_UP)
{
    return FSM_NEXT_TIMEOUT (CLAMP_UNFOLD_UPPER_SET);
}

FSM_TRANS (CLAMP_BOTTOM_CLAMP_READY, lower_clamp_rotation_success, CLAMP_READY_TO_EMPTY_TREE)
{

    fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
    return FSM_NEXT (CLAMP_BOTTOM_CLAMP_READY, lower_clamp_rotation_success);

}

FSM_TRANS (CLAMP_BOTTOM_CLAMP_READY, lower_clamp_rotation_failure, CLAMP_BLOCKED_UPPER_SET_DOWN)
{
    return FSM_NEXT (CLAMP_BOTTOM_CLAMP_HIDE_POS, lower_clamp_rotation_failure);
}

FSM_TRANS (CLAMP_READY_TO_EMPTY_TREE, empty_tree, CLAMP_CLOSE_ALL_CLAMPS)
{
    /*Closgin bottom clamp. */
    IO_SET (OUTPUT_LOWER_CLAMP_1_CLOSE);
    /*We need to close both clamp to have an easier turn around. */
    IO_SET (OUTPUT_LOWER_CLAMP_2_CLOSE);


    /*Closing upper & middle clamps.*/
    IO_CLR (OUTPUT_UPPER_CLAMP_OPEN);

    fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
    return FSM_NEXT (CLAMP_READY_TO_EMPTY_TREE, empty_tree);

}

/*---------------------------------------------------------*/
/*  Parts of the FSM that is emptying the tree (totem)     */
/*---------------------------------------------------------*/

FSM_TRANS(CLAMP_CLOSE_ALL_CLAMPS, robot_is_back,CLAMP_REARRANGE_CD)
{
    IO_CLR (OUTPUT_UPPER_CLAMP_OUT);
    IO_SET (OUTPUT_UPPER_CLAMP_IN);
    return FSM_NEXT (CLAMP_CLOSE_ALL_CLAMPS, robot_is_back);

}

FSM_TRANS (CLAMP_CLOSE_ALL_CLAMPS, stop_tree_approach, CLAMP_READY_TO_EMPTY_TREE)
{
    IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
    IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
    IO_SET (OUTPUT_UPPER_CLAMP_OPEN);
    fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
    return FSM_NEXT (CLAMP_CLOSE_ALL_CLAMPS, stop_tree_approach);
}

FSM_TRANS_TIMEOUT (CLAMP_REARRANGE_CD, TIMEOUT_RECENTRAGE, CLAMP_BOTTOM_CLAMP_HIDE_POS2)
{
    /*Hidding the clamp inside the robot.*/
    move_needed2(LET_UPPER_SET_UP,MEDIUM_ROTATION,1);
    return FSM_NEXT_TIMEOUT (CLAMP_REARRANGE_CD);
}
FSM_TRANS (CLAMP_BOTTOM_CLAMP_HIDE_POS2, lower_clamp_rotation_success, CLAMP_RELEASE_ASSERV)
{
    mimot_motor_free (0, 1);
    return FSM_NEXT (CLAMP_BOTTOM_CLAMP_HIDE_POS2, lower_clamp_rotation_success);

}

FSM_TRANS (CLAMP_BOTTOM_CLAMP_HIDE_POS2, lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    clamp_blocked();
    return FSM_NEXT (CLAMP_BOTTOM_CLAMP_HIDE_POS2, lower_clamp_rotation_failure);

}

FSM_TRANS_TIMEOUT (CLAMP_RELEASE_ASSERV, TIMEOUT_FREE_ASSERV, CLAMP_FOLD_UPPER_SET)
{
    IO_CLR (OUTPUT_UPPER_CLAMP_DOWN);
    IO_SET (OUTPUT_UPPER_CLAMP_UP);
    return FSM_NEXT_TIMEOUT (CLAMP_RELEASE_ASSERV);

}
FSM_TRANS (CLAMP_FOLD_UPPER_SET, upper_set_up, CLAMP_OPEN_UPPER_CLAMPS)
{
    IO_SET (OUTPUT_UPPER_CLAMP_OPEN);
    IO_CLR (OUTPUT_UPPER_CLAMP_IN);
    IO_SET (OUTPUT_UPPER_CLAMP_OUT);
    return FSM_NEXT (CLAMP_FOLD_UPPER_SET, upper_set_up);

}

FSM_TRANS_TIMEOUT (CLAMP_OPEN_UPPER_CLAMPS, TIMEOUT_OPEN_CLAMPS, 
                   stop_tree_branch,CLAMP_TURN_HALF_WAY,
                   continue_empty_tree_branch,CLAMP_READY_TO_RECALE)
{
    if (is_clamp_1_down(ctx.current_pos))
        IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
    else
        IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
    if (ctx.stop_tree_approach)
    {
        main_set_drop_coin_pos(ctx.current_pos + POS_TO_DROP + ctx.init_pos);
        move_needed2(CLAMP_1_UP,FAST_ROTATION,1);
        ctx.stop_tree_approach = 0;
        fsm_queue_post_event (FSM_EVENT (AI, clamps_ready));
        return FSM_NEXT_TIMEOUT (CLAMP_OPEN_UPPER_CLAMPS,stop_tree_branch);
    }
    else
    {
        move_needed2(RECALE_COIN,MEDIUM_ROTATION,1);
        return FSM_NEXT_TIMEOUT (CLAMP_OPEN_UPPER_CLAMPS,continue_empty_tree_branch);
    }
}

FSM_TRANS (CLAMP_READY_TO_RECALE, lower_clamp_rotation_success, CLAMP_READY_TO_RECALE_2)
{
    IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
    return FSM_NEXT (CLAMP_READY_TO_RECALE,lower_clamp_rotation_success);
}

FSM_TRANS (CLAMP_READY_TO_RECALE, lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    return FSM_NEXT (CLAMP_READY_TO_RECALE,lower_clamp_rotation_failure);
}

FSM_TRANS_TIMEOUT (CLAMP_READY_TO_RECALE_2, TIMEOUT_OPEN_CLAMPS, CLAMP_TEMPO_RECALE)
{
    IO_SET (OUTPUT_LOWER_CLAMP_1_CLOSE);
    return FSM_NEXT_TIMEOUT (CLAMP_READY_TO_RECALE_2);
}

FSM_TRANS_TIMEOUT (CLAMP_TEMPO_RECALE, TIMEOUT_OPEN_CLAMPS,CLAMP_END_RECALE)
{
    move_needed2(CLAMP_1_DOWN,FAST_ROTATION,-1);
    return FSM_NEXT_TIMEOUT (CLAMP_TEMPO_RECALE);
}

FSM_TRANS (CLAMP_END_RECALE, lower_clamp_rotation_failure,CLAMP_BLOCKED)
{
    return FSM_NEXT (CLAMP_END_RECALE,lower_clamp_rotation_failure);
}

FSM_TRANS (CLAMP_END_RECALE, lower_clamp_rotation_success,CLAMP_TURN_HALF_WAY)
{
    
    main_set_drop_coin_pos(ctx.current_pos + POS_TO_DROP + ctx.init_pos);
    move_needed2(CLAMP_1_UP,FAST_ROTATION,1);
    return FSM_NEXT (CLAMP_END_RECALE, lower_clamp_rotation_success);
}
/*---------------------------------------------------------------------------------*/
/*Parts of the FSM that goes back to idle after receiving the stop approach signal */
/*---------------------------------------------------------------------------------*/
FSM_TRANS (CLAMP_BOTTOM_CLAMP_HIDE_POS,stop_tree_approach,CLAMP_BOTTOM_CLAMP_BACK)
{
    /*Putting the clamp back to take coin*/
    if (is_clamp_1_down(ctx.current_pos))
    {
        move_needed2(CLAMP_1_UP,FAST_ROTATION,1);
    }
    else
    {
        move_needed2(CLAMP_1_DOWN,FAST_ROTATION,1);
    }
    return FSM_NEXT(CLAMP_BOTTOM_CLAMP_HIDE_POS,stop_tree_approach);
  
}

FSM_TRANS (CLAMP_BOTTOM_CLAMP_BACK,lower_clamp_rotation_success,CLAMP_WAIT_BEFORE_IDLE)
{
    return FSM_NEXT(CLAMP_BOTTOM_CLAMP_BACK,lower_clamp_rotation_success);
    
}

FSM_TRANS (CLAMP_BOTTOM_CLAMP_BACK,lower_clamp_rotation_failure,CLAMP_BLOCKED)
{
    clamp_blocked();
    return FSM_NEXT(CLAMP_BOTTOM_CLAMP_BACK,lower_clamp_rotation_failure);
    
}

FSM_TRANS (CLAMP_UNFOLD_UPPER_SET, stop_tree_approach,CLAMP_RELEASE_ASSERV)
{
    ctx.stop_tree_approach = 1;
    return FSM_NEXT (CLAMP_UNFOLD_UPPER_SET, stop_tree_approach);

}

FSM_TRANS (CLAMP_BOTTOM_CLAMP_READY, stop_tree_approach,CLAMP_FINISH_BOTTOM_CLAMP_READY)
{
    return FSM_NEXT (CLAMP_BOTTOM_CLAMP_READY, stop_tree_approach);

}
FSM_TRANS (CLAMP_FINISH_BOTTOM_CLAMP_READY, lower_clamp_rotation_success,CLAMP_REARRANGE_CD)
{
    return FSM_NEXT (CLAMP_FINISH_BOTTOM_CLAMP_READY, lower_clamp_rotation_success);

}
FSM_TRANS (CLAMP_FINISH_BOTTOM_CLAMP_READY, lower_clamp_rotation_failure,CLAMP_BLOCKED)
{
    clamp_blocked();
    return FSM_NEXT (CLAMP_FINISH_BOTTOM_CLAMP_READY, lower_clamp_rotation_failure);

}

FSM_TRANS (CLAMP_READY_TO_EMPTY_TREE, stop_tree_approach, CLAMP_REARRANGE_CD)
{
    /*Closgin bottom clamp. */
    IO_SET (OUTPUT_LOWER_CLAMP_1_CLOSE);
    /*We need to close both clamp to have an easier turn around. */
    IO_SET (OUTPUT_LOWER_CLAMP_2_CLOSE);

    /*Closing upper & middle clamps.*/
    IO_CLR (OUTPUT_UPPER_CLAMP_OPEN);
    return FSM_NEXT (CLAMP_READY_TO_EMPTY_TREE, stop_tree_approach);
}

/*---------------------------------------------------------------------------------*/
/*     Parts of the FSM that takes care of the bottom clamp when it's blocked      */
/*     with the uppser set is DOWN                                                 */
/*---------------------------------------------------------------------------------*/
FSM_TRANS (CLAMP_BLOCKED_UPPER_SET_DOWN, upper_set_up, CLAMP_BLOCKED)
{
    return FSM_NEXT (CLAMP_BLOCKED_UPPER_SET_DOWN, upper_set_up);
}

FSM_TRANS_TIMEOUT (CLAMP_BLOCKED_UPPER_SET_DOWN, TIMEOUT_UPPER_SET_DOWN, CLAMP_BLOCKED)
{
    /** folding upper set */
    IO_CLR (OUTPUT_UPPER_CLAMP_DOWN);
    IO_SET (OUTPUT_UPPER_CLAMP_UP);
    return FSM_NEXT_TIMEOUT (CLAMP_BLOCKED_UPPER_SET_DOWN);
}

/*---------------------------------------------------------------------------------*/
/*     Parts of the FSM that takes care of the bottom clamp when it's blocked      */
/*     with the uppser set up                                                      */
/*---------------------------------------------------------------------------------*/


FSM_TRANS_TIMEOUT (CLAMP_BLOCKED,TIMEOUT_OPEN_CLAMPS,CLAMP_OPEN_BOTTOM_CLAMPS)
{
    fsm_queue_post_event (FSM_EVENT (AI, clamp_blocked));
    /* Opening OR Closing the 2 clamps. */
    if (ctx.cpt_blocked%2==0)
    {
        IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
        IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
    }
    else
    {
        IO_SET (OUTPUT_LOWER_CLAMP_1_CLOSE);
        IO_SET (OUTPUT_LOWER_CLAMP_2_CLOSE);  
    }
    /*Setting upper clamp up*/
    IO_CLR (OUTPUT_UPPER_CLAMP_DOWN);
    IO_SET (OUTPUT_UPPER_CLAMP_UP);
    return FSM_NEXT_TIMEOUT (CLAMP_BLOCKED);
}

FSM_TRANS (CLAMP_OPEN_BOTTOM_CLAMPS, clamp_unblock, CLAMP_WAIT)
{
    uint32_t pos;
    pos = mimot_get_motor0_position();
    if (ctx.cpt_blocked%2==0)
    {
        mimot_move_motor0_absolute (pos-4000, MEDIUM_ROTATION);
    }
    else
    {
        mimot_move_motor0_absolute (4000-pos, MEDIUM_ROTATION);
    }
    return FSM_NEXT (CLAMP_OPEN_BOTTOM_CLAMPS, clamp_unblock);
}

FSM_TRANS (CLAMP_OPEN_BOTTOM_CLAMPS, tree_detected, CLAMP_OPEN_BOTTOM_CLAMPS)
{
    fsm_queue_post_event (FSM_EVENT (AI, clamp_blocked));
    return FSM_NEXT (CLAMP_OPEN_BOTTOM_CLAMPS, tree_detected);
}

FSM_TRANS (CLAMP_OPEN_BOTTOM_CLAMPS, robot_is_back, CLAMP_OPEN_BOTTOM_CLAMPS)
{
    fsm_queue_post_event (FSM_EVENT (AI, clamp_blocked));
    return FSM_NEXT (CLAMP_OPEN_BOTTOM_CLAMPS, robot_is_back);
}

FSM_TRANS (CLAMP_OPEN_BOTTOM_CLAMPS, clean_start, CLAMP_OPEN_BOTTOM_CLAMPS)
{
    fsm_queue_post_event (FSM_EVENT (AI,  clamp_blocked));
    return FSM_NEXT (CLAMP_OPEN_BOTTOM_CLAMPS, clean_start);
}

FSM_TRANS (CLAMP_OPEN_BOTTOM_CLAMPS,clean_catch, CLAMP_OPEN_BOTTOM_CLAMPS)
{
    fsm_queue_post_event (FSM_EVENT (AI,  clamp_blocked));
    return FSM_NEXT (CLAMP_OPEN_BOTTOM_CLAMPS, clean_catch);
}

FSM_TRANS (CLAMP_OPEN_BOTTOM_CLAMPS,clean_load, CLAMP_OPEN_BOTTOM_CLAMPS)
{
    fsm_queue_post_event (FSM_EVENT (AI,  clamp_blocked));
    return FSM_NEXT (CLAMP_OPEN_BOTTOM_CLAMPS, clean_load);
}

FSM_TRANS (CLAMP_OPEN_BOTTOM_CLAMPS,empty_tree, CLAMP_OPEN_BOTTOM_CLAMPS)
{
    fsm_queue_post_event (FSM_EVENT (AI,  clamp_blocked));
    return FSM_NEXT (CLAMP_OPEN_BOTTOM_CLAMPS, empty_tree);
}

FSM_TRANS (CLAMP_OPEN_BOTTOM_CLAMPS,stop_tree_approach, CLAMP_OPEN_BOTTOM_CLAMPS)
{
    fsm_queue_post_event (FSM_EVENT (AI,  clamp_blocked));
    return FSM_NEXT (CLAMP_OPEN_BOTTOM_CLAMPS, stop_tree_approach);
}


FSM_TRANS (CLAMP_OPEN_BOTTOM_CLAMPS,clamp_is_dead, CLAMP_SHITTY_STATE)
{
    return FSM_NEXT (CLAMP_OPEN_BOTTOM_CLAMPS, clamp_is_dead);
}

FSM_TRANS (CLAMP_WAIT,lower_clamp_rotation_success, CLAMP_TURN_BACKWARD)
{
    /* Opening OR Closing the 2 clamps. */
    if (ctx.cpt_blocked%2==1)
    {
        IO_CLR (OUTPUT_LOWER_CLAMP_1_CLOSE);
        IO_CLR (OUTPUT_LOWER_CLAMP_2_CLOSE);
    }
    else
    {
        IO_SET (OUTPUT_LOWER_CLAMP_1_CLOSE);
        IO_SET (OUTPUT_LOWER_CLAMP_2_CLOSE);  
    }
    return FSM_NEXT (CLAMP_WAIT, lower_clamp_rotation_success);
}

FSM_TRANS (CLAMP_WAIT,lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    ctx.cpt_blocked += 1;
    return FSM_NEXT (CLAMP_WAIT, lower_clamp_rotation_failure);
}

FSM_TRANS_TIMEOUT (CLAMP_TURN_BACKWARD,TIMEOUT_BLOCKED, CLAMP_TURN_FORWARD)
{
    int sens;
    if (ctx.cpt_blocked%2==0)
    {
        sens=1;
    }
    else
    {
        sens=-1;
    }
    move_needed2(CLAMP_1_DOWN,MEDIUM_ROTATION,sens);
    return FSM_NEXT_TIMEOUT (CLAMP_TURN_BACKWARD);
}


FSM_TRANS (CLAMP_TURN_FORWARD,lower_clamp_rotation_success, 
           back_to_idle,CLAMP_WAIT_BEFORE_IDLE,
           back_to_empty_tree, CLAMP_BOTTOM_CLAMP_HIDE_POS)
{
    if (ctx.unblocking_upper_set)
    {
        /*Hidding the clamp inside the robot*/
        if (is_clamp_1_down(ctx.current_pos))
        {
            move_needed2(HIDE_POS_TREE,FAST_ROTATION,1);
        }
        else
        {
            move_needed2(HIDE_POS_TREE,FAST_ROTATION,1);
        }    
        ctx.unblocking_upper_set = 0;
        return FSM_NEXT (CLAMP_TURN_FORWARD, lower_clamp_rotation_success, back_to_empty_tree);
    }
    else
    {
        ctx.cpt_blocked = 0;
        if (ctx.upper_set_is_dead)
        {
            fsm_queue_post_event (FSM_EVENT (AI, upper_set_is_dead));
        }
        return FSM_NEXT (CLAMP_TURN_FORWARD, lower_clamp_rotation_success,back_to_idle);
    }
}


FSM_TRANS (CLAMP_TURN_BACKWARD,lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    ctx.cpt_blocked += 1;
    return FSM_NEXT (CLAMP_TURN_BACKWARD, lower_clamp_rotation_failure);
}


FSM_TRANS (CLAMP_TURN_FORWARD,lower_clamp_rotation_failure, CLAMP_BLOCKED)
{
    ctx.cpt_blocked += 1;
    return FSM_NEXT (CLAMP_TURN_FORWARD, lower_clamp_rotation_failure);
}
/*---------------------------------------------------------------------------------*/
/*     Parts of the FSM that takes care of the upper set when it's not going down  */
/*---------------------------------------------------------------------------------*/
FSM_TRANS_TIMEOUT (CLAMP_UPPER_SET_BLOCKED_UP, TIMEOUT_OPEN_CLAMPS, 
           branch_upper_set_is_dead, CLAMP_UPPER_SET_DEAD,
           branch_try_to_deblock, CLAMP_UPPER_SET_FOLD_BACK)
{
    ctx.upper_set_blocked_cpt ++;
    IO_CLR (OUTPUT_UPPER_CLAMP_DOWN);
    IO_SET (OUTPUT_UPPER_CLAMP_UP);
    if (ctx.upper_set_blocked_cpt>=2)
        {
            return FSM_NEXT_TIMEOUT (CLAMP_UPPER_SET_BLOCKED_UP, branch_upper_set_is_dead);
        }
    else
        {
            return FSM_NEXT_TIMEOUT (CLAMP_UPPER_SET_BLOCKED_UP,branch_try_to_deblock);
        }
}

FSM_TRANS_TIMEOUT (CLAMP_UPPER_SET_DEAD,TIMEOUT_OPEN_CLAMPS, CLAMP_BLOCKED)
{
    ctx.upper_set_is_dead = 1;
    return FSM_NEXT_TIMEOUT (CLAMP_UPPER_SET_DEAD);
}

FSM_TRANS (CLAMP_UPPER_SET_FOLD_BACK,upper_set_up, CLAMP_TRY_ROTATION)
{
    ctx.upper_set_is_dead = 1;
    mimot_move_motor0_absolute (ctx.current_pos-4000, MEDIUM_ROTATION);
    return FSM_NEXT (CLAMP_UPPER_SET_FOLD_BACK,upper_set_up);
}

FSM_TRANS (CLAMP_TRY_ROTATION,lower_clamp_rotation_success,CLAMP_BOTTOM_CLAMP_HIDE_POS)
{
    /*Hidding the clamp inside the robot*/
    if (is_clamp_1_down(ctx.current_pos))
    {
       move_needed2(HIDE_POS_TREE,FAST_ROTATION,1);
    }
    else
    {
        move_needed2(HIDE_POS_TREE,FAST_ROTATION,1);
    }
    return FSM_NEXT (CLAMP_TRY_ROTATION,lower_clamp_rotation_success);
}

FSM_TRANS (CLAMP_TRY_ROTATION,lower_clamp_rotation_failure,CLAMP_BLOCKED)
{
    ctx.unblocking_upper_set = 1;
    return FSM_NEXT (CLAMP_TRY_ROTATION,lower_clamp_rotation_failure);
}



/* vim: set cino={0\:0t0(0 et: */
