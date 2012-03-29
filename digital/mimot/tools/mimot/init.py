"""Default parameters for asserv."""
target_marcel = dict (
        a0_kp = 4,
        a0_acc = 16, a0_speed_max = 0x60, a0_speed_slow = 0x10,
        a0_error_limit = 256, a0_bd_speed_limit = 0x18, a0_bd_counter_limit = 5,
        a1_kp = 4,
        a1_acc = 16, a1_speed_max = 0x60, a1_speed_slow = 0x10,
        a1_error_limit = 256, a1_bd_speed_limit = 0x18, a1_bd_counter_limit = 5,
        e_sat = 0x3ff, d_sat = 0x1ff,
        )
target_robospierre = dict (
        a0_kp = 8, a0_kd = 1,
        a0_acc = 2, a0_speed_max = 0x60, a0_speed_slow = 0x10,
        a0_error_limit = 32, a0_bd_speed_limit = 0x08, a0_bd_counter_limit = 125,
        a1_kp = 4,
        a1_acc = 0.5, a1_speed_max = 0x30, a1_speed_slow = 0x08,
        a1_error_limit = 64, a1_bd_speed_limit = 0x08, a1_bd_counter_limit = 5,
        e_sat = 0x3ff, d_sat = 0x1ff,
        )
target_guybrush = dict (
        a0_kp = 1, a0_kd = 16,
        a0_acc = 4, a0_speed_max = 0x20, a0_speed_slow = 0x02,
        a0_error_limit = 32, a0_bd_speed_limit = 0x08, a0_bd_counter_limit = 125,
        e_sat = 0x3ff, d_sat = 0x1ff,
        )
target = {
        'marcel': target_marcel,
        'robospierre': target_robospierre,
        'guybrush': target_guybrush,
        }

host = target
