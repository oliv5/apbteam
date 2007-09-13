global robot;
global trace;

% Robot characteristics:
robot.res = 2 * pi * 20 / 2000;	% Resolution (mm/step).
robot.radius = 250 / 2;	% Half distance between wheels (mm).

% Reset all simulation results.
function reset ()
    global robot;
    global trace;
    % Robot physical parameters:
    robot.t = 0;	% Current time (s).
    robot.tx = 0;	% Theta position (mm).
    robot.ts = 0;	% Theta speed (mm/s).
    robot.ta = 0;	% Theta acceleration (mm/s^2).
    robot.ax = 0;	% Alpha "position" (rad).
    robot.as = 0;	% Alpha speed (rad/s).
    robot.aa = 0;	% Alpha acceleration (rad/s^2).

    % Applied to encoders:
    robot.lc = 0;	% Absolute left coder position (mm).
    robot.rc = 0;	% Absolute right coder position (mm).

    % Integrated position:
    robot.x = 0;	% X position (mm).
    robot.y = 0;	% Y position (mm).
    robot.a = 0;	% Angle between the X axis and the robot front (rad).

    % Integer encoders:
    robot.lc_i = 0;	% Integer absolute left coder position (step).
    robot.rc_i = 0;	% Integer absolute right coder position (step).

    % Calculated position from integer encoder output:
    % TODO: is 2f248 sufficient? Look at the a = pi/4 example.
    robot.x_2f248 = 0;	% X position (2 * 2^8 => 1 step).
    robot.y_2f248 = 0;	% Y position (2 * 2^8 => 1 step).
    robot.a_f824 = 0;	% Angle (2^24 => 2 * pi).

    % Trace:
    trace.t = [ 0 ];
    trace.xya = [ 0, 0, 0 ];
    trace.xya_f = [ 0, 0, 0 ];
    trace.c = [ 0, 0 ];
    trace.c_i = [ 0, 0 ];
end

% Make a step of dt (s).
function step (delta)
    global robot;
    %% Floating point part:
    % First of all, update physical parameters.
    % a = constant
    % s = s_0 + a t
    % x = x_0 + s_0 t + 1/2 a_0 t^2
    dt = robot.ts * delta + robot.ta * delta^2 / 2;
    da = robot.as * delta + robot.aa * delta^2 / 2;
    robot.tx += dt;
    robot.ts += robot.ta * delta;
    robot.ax += da;
    robot.as += robot.aa * delta;
    % Compute encoders.
    robot.lc = robot.tx - robot.ax * robot.radius;
    robot.rc = robot.tx + robot.ax * robot.radius;
    % Integrate position.
    if (da == 0)
	% No angle, going right ahead.
	robot.x += cos (robot.a) * dt;
	robot.y += sin (robot.a) * dt;
    else
	% With an angle, draw an arc.
	arc_radius = dt / da;
	robot.x += arc_radius * (sin (robot.a + da) - sin (robot.a));
	robot.y += arc_radius * (cos (robot.a) - cos (robot.a + da));
    end
    robot.a += da;
    %% Fixed part:
    % Integer encoder value.
    % TODO: sampling error.
    lc_i = floor (robot.lc / robot.res);	% We want a regular step,
    rc_i = floor (robot.rc / robot.res);	% even around 0.
    dlc_i = lc_i - robot.lc_i;
    drc_i = rc_i - robot.rc_i;
    robot.lc_i = lc_i;
    robot.rc_i = rc_i;
    % Integrate position.
    dt_2f248 = (dlc_i + drc_i) * 2^8;
    if (dlc_i == drc_i)
	robot.x_2f248 += mul_f24 (dt_2f248, cos_f824 (robot.a_f824));
	robot.y_2f248 += mul_f24 (dt_2f248, sin_f824 (robot.a_f824));
    else
	c1_2pi_f032 = round (1/(2*pi) * 2^32);
	radius_2i = round (2 * robot.radius / robot.res);
	radius_factor_f032 = round (c1_2pi_f032 / radius_2i);
	dd_2f1616 = (drc_i - dlc_i) * 2^16;
	da_f824 = mul_f24 (dd_2f1616, radius_factor_f032);
	na_f824 = robot.a_f824 + da_f824;
	da_rad_f824 = mul_f24 (da_f824, round (2*pi * 2^24));
	dsc_f824 = sin_f824 (na_f824) - sin_f824 (robot.a_f824);
	dsc_f824 = div_f24 (dsc_f824, da_rad_f824);
	robot.x_2f248 += mul_f24 (dt_2f248, dsc_f824);
	dsc_f824 = cos_f824 (robot.a_f824) - cos_f824 (na_f824);
	dsc_f824 = div_f24 (dsc_f824, da_rad_f824);
	robot.y_2f248 += mul_f24 (dt_2f248, dsc_f824);
	robot.a_f824 = na_f824;
    end
    % Advance time and trace.
    robot.t += delta;
    global trace;
    trace.t = [ trace.t; robot.t ];
    trace.xya = [ trace.xya; robot.x, robot.y, robot.a ];
    trace.xya_f = [ trace.xya_f; robot.x_2f248, robot.y_2f248, robot.a_f824 ];
    trace.c = [ trace.c; robot.lc, robot.rc ];
    trace.c_i = [ trace.c_i; robot.lc_i, robot.rc_i ];
end

% Fixed point multiply, with 24 bit fractional part.
function r = mul_f24 (a, b)
    r = round (a * b / 2^24);
end

% Fixed point divide, with 24 bit fractional part.
function r = div_f24 (a, b)
    r = round (a * 2^24 / b);
end

% Fixed point sinus, with 24 bit fractional part.
function r_f824 = sin_f824 (a_f824)
    a_rad = a_f824 / 2^24 * 2*pi;
    r = sin (a_rad);
    r_f824 = round (r * 2^24);
end

% Fixed point cosinus, with 24 bit fractional part.
function r_f824 = cos_f824 (a_f824)
    a_rad = a_f824 / 2^24 * 2*pi;
    r = cos (a_rad);
    r_f824 = round (r * 2^24);
end

% Make a trajectory with the given acceleration, during the given time.
function traj (ta, aa, t)
    global robot;
    robot.ta = ta;
    robot.aa = aa / robot.radius;
    stop = robot.t + t;
    while (robot.t < stop)
	step (0.010);
    end
end

% Make sure the robot is stopped.
function stop ()
    global robot;
    robot.ts = 0;
    robot.as = 0;
end

% Example path.
function path ()
    global robot;
    traj (0, 500, 0.3);
    traj (0, 0, 0.5);
    traj (0, -500, 0.3);
    stop ();
    traj (1000, 0, 0.2);
    traj (0, 0, 2);
    traj (-1000, 0, 0.2);
    stop ();
    traj (1000, 500, 0.3);
    traj (0, 0, 2);
    traj (-1000, -500, 0.3);
    stop ();
end

motor;
global to_file = 0;

reset ();
path ();

conv = diag ([ robot.res / 2 / 2^8, robot.res / 2 / 2^8, 2*pi / 2^24 ]);

plotterm ("trace");
multiplot (2, 2);
mplot (trace.xya (:,1), trace.xya (:,2), "1;trace xy;");
mplot (trace.t, trace.xya (:,3), "2;trace a;");
xya_f_conv = trace.xya_f * conv;
mplot (xya_f_conv (:,1), xya_f_conv (:,2), "1;trace xy f;");
mplot (trace.t, xya_f_conv (:,3), "2;trace a f;");
multiplot (0, 0);

final_pos = [ robot.x, robot.y, robot.a ]
final_pos_f = [ robot.x_2f248, robot.y_2f248, robot.a_f824 ]
final_pos_f_conv = final_pos_f * conv
