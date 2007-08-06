% Show influence of inductance when not doing PWM.
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

% With inductance.
plotterm ("w_induct");
sys_plot (2);

% With inductance zoom.
plotterm ("w_inductz");
sys_plot (2 / 100);

% Without inductance.
sys.motor.L = 0;
plotterm ("wo_induct");
sys_plot (2);

% Without inductance zoom.
plotterm ("wo_inductz");
sys_plot (2 / 100);
