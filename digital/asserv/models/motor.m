% Motor models.

% Maxon RE40G 150W 24V 148877
re40g.u = 24;			% Nominal voltage (V).
re40g.Kw = 317 * 2*pi / 60;	% Speed constant ((rad/s)/V).
re40g.Km = 30.2 / 1000;		% Torque constant (N.m/A).
re40g.Rf = 0;			% Bearing friction (N.m/(rad/s)).
re40g.R = 0.316;		% Terminal resistance (Ohm).
re40g.L = 0.08 / 1000;		% Terminal inductance (H).
re40g.J = 134 / 1000 / 10000;	% Rotor moment of inertia (kg.m^2).

% Maxon RE25G 20W 24V 118752
re25g.u = 24;			% Nominal voltage (V).
re25g.Kw = 407 * 2*pi / 60;	% Speed constant ((rad/s)/V).
re25g.Km = 23.4 / 1000;		% Torque constant (N.m/A).
re25g.Rf = 0;			% Bearing friction (N.m/(rad/s)).
re25g.R = 2.32;			% Terminal resistance (Ohm).
re25g.L = 0.24 / 1000;		% Terminal inductance (H).
re25g.J = 10.3 / 1000 / 10000;	% Rotor moment of inertia (kg.m^2).

% Maxon RE25CLL 10W 12V 118743
re25cll.u = 12;			% Nominal voltage (V).
re25cll.Kw = 407 * 2*pi / 60;	% Speed constant ((rad/s)/V).
re25cll.Km = 23.4 / 1000;	% Torque constant (N.m/A).
re25cll.Rf = 0;			% Bearing friction (N.m/(rad/s)).
re25cll.R = 2.18;		% Terminal resistance (Ohm).
re25cll.L = 0.24 / 1000;	% Terminal inductance (H).
re25cll.J = 10.3 / 1000 / 10000;% Rotor moment of inertia (kg.m^2).

% x4 orthogonal gear box.
gear4x.i = 4;			% Ratio.
gear4x.ro = 0.75;		% Efficiency.

% x20.25 epicycloid gear box.
gear20x.i = 20.25;		% Ratio.
gear20x.ro = 0.75;		% Efficiency.

% x10 orthogonal gear box.
gear10x.i = 10;			% Ratio.
gear10x.ro = 0.75;		% Efficiency.

% 10 kg robot with 80 mm wheels.
robot10w80.wr = 0.04;		% Wheel radius (m).
robot10w80.m = 10;		% Mass (kg).
robot10w80.J = robot10w80.m * robot10w80.wr ^ 2;
	% Moment of inertia at wheel radius (kg.m^2).
%robot10w80.Rf = ;		% Friction ?, TODO.

% u(t) = R i(t) + L di(t)/dt + w(t)/Kw
% Jr dw(t)/dt = Km i(t) - Rf w(t)
% dth(t)/dt = w(t)
%
% t: Simulation time (s).
% u(t): Voltage at motor terminals (V).
% i(t): Current going trough the motor (A).
% w(t): Rotation speed (w for omega) (rad/s).
% th(t): Rotor position (th for theta) (rad).
% Kw: Speed constant ((rad/s)/V).
% Km: Torque constant (N.m/A).
% R: Motor terminal resistance (Ohm).
% L: Motor terminal inductance (H).
% Jr: Moment of inertia at rotor (with load) (kg.m^2).
% 	Jr = motor.J + load.J / gear.i ^ 2 / gear.ro
% Rf: Bearing friction (N.m/(rad/s)).

% if L == 0: i(t) = 1/R (u(t) - w(t)/Kw)
% else: di(t)/dt = 1/L (u(t) - R i(t) - w(t)/Kw)
% dw(t)/dt = 1/Jr (Km i(t) - Rf w(t))
% dth(t)/dt = w(t)

% x = [ i(t); w(t); th(t) ]
% xdot = [ di(t)/dt; dw(t)/dt; dth(t)/dt ]
function xdot = sys_der (x, t)
    global sys;
    mo = sys.motor;
    ge = sys.gear;
    lo = sys.load;

    persistent t_progress = -1
    if (t_progress > t + sys.progress * 50)
	t_progress = 0;
    end
    if (sys.progress && t > t_progress)
	printf ("time: %f %d%%\r", t, t / sys.progress);
	t_progress = t + sys.progress;
    end

    u = feval (sys.control.func, t, x);

    i = x(1);
    w = x(2);
    th = x(3);

    Jr = mo.J + lo.J / ge.i ** 2 / ge.ro;

    if (mo.L == 0)
	i = 1 / mo.R * (u - w / mo.Kw);
	xdot(1) = i; % will ignore this one.
    else
	xdot(1) = 1 / mo.L * (u - mo.R * i - w / mo.Kw);
    end
    xdot(2) = 1 / Jr * (mo.Km * i - mo.Rf * w);
    xdot(3) = w;
end

% When no feedback needed, resolve equation in one go.
function [t, x] = __sys_ode_simple__ (dur, steps)
    % first, last, number of values
    t = linspace (0, dur, steps + 1)';
    x0 = [ 0; 0; 0 ];
    x = lsode ("sys_der", x0, t);
end

% When feedback function needed, resolve equation step by step.
function [t, x] = __sys_ode_feedback__ (dur, steps)
    global sys;
    t = linspace (0, dur, steps + 1)';
    x = zeros (steps + 1, 3);
    for i = 1:steps
	feval (sys.feedback.func, x(i, :), t(i));
	%tx = lsode ("sys_der", x(i, :), t(i : i + 1));
	%x(i + 1, :) = tx(2, :);
	xdot = sys_der (x(i, :), t(i));
	x(i + 1, :) = x(i, :) + (t(i + 1) - t(i)) * xdot;
    end
end

% Plot the current system.
function sys_plot (dur, steps)
    global sys;
    % Default number of steps.
    if (nargin < 2)
	steps = 1024;
    end
    % Prepare progress meter.
    sys.progress = dur / 100;
    % Resolve differential equation.
    if (isfield (sys, "feedback"))
	[t, x] = __sys_ode_feedback__ (dur, steps);
    else
	[t, x] = __sys_ode_simple__ (dur, steps);
    end
    % Plot.
    multiplot (2, 2);
    if (sys.motor.L == 0)
	i = zeros (length(t), 1);
	for j = 1:length(t)
	    x(j, 1) = sys_der (x(j, :), t(j))(1);
	end
    end
    mplot (t, x(:, 1), "1;current (A);");
    if (0) % plot dw
	dw = zeros (length(t), 1);
	for i = 1:length(t)
	    xdot = sys_der (x(i, :), t(i));
	    dw(i) = xdot(2);
	end
	mplot (t, dw, "2;dw;");
    else
	mplot (t, x(:, 2) * 60/2/pi, "2;rpm;");
    end
    mplot (t, x(:, 2) / sys.gear.i * sys.load.wr, "3;speed (m/s);");
    mplot (t, x(:, 3) / sys.gear.i * sys.load.wr, "4;distance (m);");
    multiplot (0, 0);
    % Finish progress meter.
    printf ("\n");
end

% Setup plot for x11 or png depending of to_file global.
function plotterm (title)
    persistent win = -1;
    global to_file;
    if (to_file)
	__gnuplot_set__ term png;
	eval (sprintf ('__gnuplot_set__ output "%s.png"', title));
    else
	win += 1;
	eval (sprintf ('__gnuplot_set__ term x11 %d title "%s" persist',
	win, title));
    end
end

%% Control functions.

% Constant voltage.
function u = constant (t)
    global sys;
    u = sys.control.u;
end

% PWM voltage.
function u = pwm (t)
    global sys;
    if (mod (t, sys.control.period) < sys.control.pwm * sys.control.period)
	u = sys.motor.u;
    else
	u = 0;
    end
end

