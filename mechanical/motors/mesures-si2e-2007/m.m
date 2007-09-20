% Plot or return matrix from a file.
function [rt, rx, rs, ra] = m (file, f)
    tres = 65536/14745600;
    cres = 2/75.78 / 1000;
    % Load file.
    eval (sprintf ("x = load %s;", file));
    % Make time vector.
    t = 0:tres:tres * (length (x) -1);
    % Split and scale.
    x *= cres;
    s = x(:, 3:4);
    a = x(:, 5:6);
    x(:, 3:6) = [];
    s *= 1/tres;
    a *= 1/(tres^2);
    % The optional argument override the filter factor.
    if (nargin == 1)
	% A little bit aggressive...
	f = 50;
    end
    if (f != 0)
	a(:, 1) = filter (ones (1, f), [f], a(:, 1));
	a(:, 2) = filter (ones (1, f), [f], a(:, 2));
    end
    % Plot if no output asked.
    if (nargout == 0)
	multiplot (3, 1);
	mplot (t, x(:, 1), ";xl;", t, x(:, 2), ";xr;");
	mplot (t, s(:, 1), ";sl;", t, s(:, 2), ";sr;");
	mplot (t, a(:, 1), ";al;", t, a(:, 2), ";ar;");
	oneplot ();
    else
	rt = t;
	rx = x;
	rs = s;
	ra = a;
    end
end
