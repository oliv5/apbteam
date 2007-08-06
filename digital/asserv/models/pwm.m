% Show influence of PWM control over voltage control.
motor;

%global to_file = 1;

global sys;
sys.motor = re40g;
sys.gear = gear4x;
sys.load = robot10w80;
sys.load.J /= 2; % Two motors.
sys.control.func = "pwm";
sys.control.period = 1/20000;

% PWM: 50%
sys.control.pwm = 0.5;
plotterm ("pwm");
sys_plot (10/20000);

% Voltage: 50%
sys.control.pwm = 1;
sys.motor.u *= 0.5;
plotterm ("voltage");
sys_plot (10/20000);
