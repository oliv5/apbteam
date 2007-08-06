% Compare different motors.
motor;

%global to_file = 1;

function u = control (t)
    global sys;
    if (t < 1)
	u = sys.motor.u;
    else
	u = 0;
    end
end

global sys;
sys.motor = re40g;
sys.gear = gear4x;
sys.load = robot10w80;
sys.load.J /= 2; % Two motors.
sys.control.func = "control";

plotterm ("re40g");
sys_plot (2);

global sys;
sys.motor = re25g;
sys.gear = gear20x;
sys.load = robot10w80;
sys.load.J /= 2; % Two motors.
sys.control.func = "control";

plotterm ("re25g");
sys_plot (2);

global sys;
sys.motor = re25cll;
sys.gear = gear10x;
sys.load = robot10w80;
sys.load.J /= 2; % Two motors.
sys.control.func = "control";

plotterm ("re25cll");
sys_plot (2);
