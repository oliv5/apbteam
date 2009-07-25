set angles degrees
set xrange [0:3000]
set yrange [0:2100]
dist (x0, y0, x1, y1) = sqrt ((x0 - x1) ** 2 + (y0 - y1) ** 2)
dot (x0, y0, x1, y1) = x0 * x1 + y0 * y1
anglea (x, y, x0, y0, x1, y1) = acos (dot (x0 - x, y0 - y, x1 - x, y1 - y) / (dist (x, y, x0, y0) * dist (x, y, x1, y1)))
angle (x, y, x0, y0, x1, y1) = ((x0 - x) * (y1 - y) - (x1 - x) * (y0 - y) > 0 ? anglea (x, y, x0, y0, x1, y1) : 360 - anglea (x, y, x0, y0, x1, y1))
angle_b12 (x, y) = angle (x, y, 3000, 0, 3000, 2100)
angle_b23 (x, y) = angle (x, y, 3000, 0, 0, 1050)
angle_b31 (x, y) = angle (x, y, 3000, 2100, 0, 1050)

t1 = angle (3000, 0, 3000, 2100, 0, 1050)
t2 = angle (3000, 2100, 0, 1050, 3000, 0)
t3 = angle (0, 1050, 3000, 0, 3000, 2100)
b1 = dist (3000, 0, 3000, 2100)
b2 = dist (3000, 2100, 0, 1050)
b3 = dist (0, 1050, 3000, 0)

solve_angle (a1, a3) = atan ((-b1 + b3 * sin (a3 + t2) / sin (a3)) / (b1 * cos (a1) / sin (a1) + b3 * cos (a3 + t1) / sin (a3)))

set isosamples 50
set contour
set cntrparam levels incremental 0, 10, 360

set term png
set output 'triangle_surf12.png'
splot angle_b12 (x, y)
set output 'triangle_surf23.png'
splot angle_b23 (x, y)
set output 'triangle_surf31.png'
splot angle_b31 (x, y)

set contour
set cntrparam levels incremental 0, 1, 360
unset surface
set view map
unset clabel

set output 'triangle_contour12.png'
splot angle_b12 (x, y)
set output 'triangle_contour23.png'
splot angle_b23 (x, y)
set output 'triangle_contour31.png'
splot angle_b31 (x, y)
set output 'triangle_contour123.png'
splot angle_b12 (x, y), angle_b23 (x, y), angle_b31 (x, y)
set cntrparam levels incremental 0, 3, 360
set output 'triangle_contour123-3deg.png'
splot angle_b12 (x, y), angle_b23 (x, y), angle_b31 (x, y)
set cntrparam levels incremental 0, 1, 360

set size square
set xrange [0:100]
set yrange [0:100]
set output 'triangle_zoom+0+0.png'
splot angle_b12 (x, y), angle_b23 (x, y), angle_b31 (x, y)
set xrange [0:100]
set yrange [1000:1100]
set output 'triangle_zoom+0+1050.png'
splot angle_b12 (x, y), angle_b23 (x, y), angle_b31 (x, y)
set xrange [2900:3000]
set yrange [2000:2100]
set output 'triangle_zoom+3000+2100.png'
splot angle_b12 (x, y), angle_b23 (x, y), angle_b31 (x, y)
set xrange [1450:1550]
set yrange [1000:1100]
set output 'triangle_zoom+1500+1050.png'
splot angle_b12 (x, y), angle_b23 (x, y), angle_b31 (x, y)

reset
set xrange [0:3000]
set yrange [0:2100]
set isosamples 50
set angles degrees
set contour
set cntrparam levels incremental 0, 5, 360

set term png
set output 'triangle_solve_angle.png'
splot solve_angle (angle_b12 (x, y), angle_b31 (x, y))
