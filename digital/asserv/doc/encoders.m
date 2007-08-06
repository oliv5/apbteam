disp ('max encoder freq (Hz), from datasheet');
f_max = 100000
disp ('pulse per revolution');
n = 500
disp ('max angular speed (tr/s)');
f_max/n
disp ('max angular speed (tr/min)');
f_max/n * 60

disp ('one state length (deg)');
phy_deg = [90-15, 90+15]
disp ('one state length at max freq (s)');
phy = 1/f_max * (phy_deg / 360)
disp ('minimum state length (s)');
min_phy = min (phy)

disp ('');

disp ('AVR freq (Hz)');
avr_f = 16000000
disp ('AVR instructions per cycle');
avr_instr = [32, 22]
disp ('AVR cycle length (s)');
avr_cycle = avr_instr / avr_f

disp ('');

disp ('CPLD cycles needed for correct detection with an input filter');
cpld_cy = 5
disp ('CPLD needed frequency (Hz)');
cpld_f = cpld_cy / min_phy
