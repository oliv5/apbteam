% Limit current.
motor;

%global to_file = 1;

function u = onoff (t)
    global sys;
    if (t < 1)
	u = sys.control.u;
    else
	u = 0;
    end
end

function u = limit_current (t, x)
    global sys;
    u = onoff (t);
    % Limit current by knowing the current speed and the motor terminal
    % resistance.
    umax = sys.control.current * sys.motor.R + x(2) / sys.motor.Kw;
    umin = - sys.control.current * sys.motor.R + x(2) / sys.motor.Kw;
    % Uncomment this if the maximum current can be overshooted when braking:
    %umax = max ([umax, 0]);
    %umin = min ([umin, 0]);
    u = min ([u, umax]);
    u = max ([u, umin]);
end

global sys;
sys.motor = re40g;
sys.gear = gear4x;
sys.load = robot10w80;
sys.load.J /= 2; % Two motors.
sys.control.func = "limit_current";
sys.control.current = 20;
sys.control.u = sys.motor.u;

plotterm ("limit");
sys_plot (2);

sys.control.func = "onoff";

plotterm ("nolimit");
sys_plot (2);
