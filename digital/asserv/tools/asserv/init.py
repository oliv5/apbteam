"""Default parameters for asserv."""
host_marcel = dict (
        scale = 0.0395840674352314, footing = 0xdd1,
        t_kp = 1, t_kd = 16,
        t_acc = 0.75, t_speed_max = 0x20, t_speed_slow = 0x10,
        a_kp = 2, a_kd = 16,
        a_acc = 0.25, a_speed_max = 0x20, a_speed_slow = 0x10,
        a0_kp = 4,
        a0_acc = 4, a0_speed_max = 0x60, a0_speed_slow = 0x10,
        a1_kp = 2,
        a1_acc = 16, a1_speed_max = 0x46, a1_speed_slow = 0x10,
        e_sat = 0x3ff, d_sat = 0x1ff,
        angle_limit = 0x1000,
        )
host_robospierre = dict (
        scale = 0.0395840674352314, footing = 0xdd1,
        t_kp = 1, t_kd = 16,
        t_acc = 0.75, t_speed_max = 0x60, t_speed_slow = 0x10,
        a_kp = 2, a_kd = 16,
        a_acc = 0.25, a_speed_max = 0x60, a_speed_slow = 0x10,
        e_sat = 0x3ff, d_sat = 0x1ff,
        angle_limit = 0x1000,
        )
host_guybrush = dict (
        aux_nb = 0,
        scale = 0.0395840674352314, footing = 0xdd1,
        t_kp = 1, t_kd = 16,
        t_acc = 0.75, t_speed_max = 0x60, t_speed_slow = 0x10,
        a_kp = 2, a_kd = 16,
        a_acc = 0.25, a_speed_max = 0x60, a_speed_slow = 0x10,
        e_sat = 0x3ff, d_sat = 0x1ff,
        angle_limit = 0x1000,
        )
host = {
        'giboulee': host_marcel,
        'marcel': host_marcel,
        'robospierre': host_robospierre,
        'guybrush': host_guybrush,
        'apbirthday': host_guybrush,
        }
target_marcel = dict (
        scale = 0.0415178942124, footing = 0xcef,
        encoder_right_correction = float (0x00ffbabf) / (1 << 24),
        t_kp = 1, t_kd = 16,
        t_acc = 0.75, t_speed_max = 0x20, t_speed_slow = 0x10,
        a_kp = 2, a_kd = 16,
        a_acc = 0.25, a_speed_max = 0x20, a_speed_slow = 0x10,
        a0_kp = 4, a0_kd = 16,
        a0_acc = 4, a0_speed_max = 0x60, a0_speed_slow = 0x10,
        a0_bd_error_limit = 256, a0_bd_speed_limit = 0x04, a0_bd_counter_limit = 10,
        a1_kp = 2,
        a1_acc = 16, a1_speed_max = 0x46, a1_speed_slow = 0x10,
        a1_bd_error_limit = 256, a1_bd_speed_limit = 0x0c, a1_bd_counter_limit = 10,
        e_sat = 0x3ff, d_sat = 0x1ff,
        angle_limit = 0x1000,
        )
target_robospierre = dict (
        scale = 0.0317975134344, footing = 0x134e,
        encoder_right_correction = float (0xffa897) / (1 << 24),
        t_kp = 1, t_kd = 16,
        t_acc = 0.75, t_speed_max = 0x60, t_speed_slow = 0x10,
        t_bd_error_limit = 256, t_bd_speed_limit = 0x08, t_bd_counter_limit = 40,
        a_kp = 2, a_kd = 16,
        a_acc = 0.5, a_speed_max = 0x60, a_speed_slow = 0x10,
        a_bd_error_limit = 128, a_bd_speed_limit = 0x08, a_bd_counter_limit = 40,
        e_sat = 0x3ff, d_sat = 0x1ff,
        angle_limit = 0x1000,
        )
target_guybrush = dict (
        aux_nb = 0,
        scale = 0.0317975134344, footing = 0x134e,
        encoder_right_correction = 0.99861627695742,
        t_kp = 1, t_kd = 16,
        t_acc = 0.75, t_speed_max = 0x60, t_speed_slow = 0x10,
        t_bd_error_limit = 256, t_bd_speed_limit = 0x08, t_bd_counter_limit = 40,
        a_kp = 2, a_kd = 16,
        a_acc = 0.5, a_speed_max = 0x60, a_speed_slow = 0x10,
        a_bd_error_limit = 128, a_bd_speed_limit = 0x08, a_bd_counter_limit = 40,
        e_sat = 0x3ff, d_sat = 0x1ff,
        r_reverse = True,
        angle_limit = 0x1000,
        )
target_apbirthday = dict (
        aux_nb = 0,
        scale = 0.0317975134344, footing = 0x1036,
        encoder_right_correction = 1.00078805775467,
        t_kp = 1, t_kd = 16,
        t_acc = 0.75, t_speed_max = 0x60, t_speed_slow = 0x10,
        t_bd_error_limit = 256, t_bd_speed_limit = 0x08, t_bd_counter_limit = 40,
        a_kp = 2, a_kd = 16,
        a_acc = 0.5, a_speed_max = 0x60, a_speed_slow = 0x10,
        a_bd_error_limit = 128, a_bd_speed_limit = 0x08, a_bd_counter_limit = 40,
        e_sat = 0x3ff, d_sat = 0x1ff,
        l_reverse = True,
        angle_limit = 0x1000,
        )
target = {
        'giboulee': target_marcel,
        'marcel': target_marcel,
        'robospierre': target_robospierre,
        'guybrush': target_guybrush,
        'apbirthday':target_apbirthday
        }
