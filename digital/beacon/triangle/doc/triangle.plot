set xrange [0:3000]
set yrange [0:2100]
dist (x0, y0, x1, y1) = sqrt ((x0 - x1) ** 2 + (y0 - y1) ** 2)
dot (x0, y0, x1, y1) = x0 * x1 + y0 * y1
angle (x, y, x0, y0, x1, y1) = acos (dot (x0 - x, y0 - y, x1 - x, y1 - y) / (dist (x, y, x0, y0) * dist (x, y, x1, y1)))
angle_b12 (x, y) = angle (x, y, 3000.0, 0.0, 3000.0, 2100.0)
angle_b23 (x, y) = angle (x, y, 3000, 0, 0, 1050)
angle_b31 (x, y) = angle (x, y, 3000, 2100, 0, 1050)

set isosamples 50
set angles degrees
set contour
set cntrparam levels incremental 0, 10, 180

set term png
set output 'triangle_surf12.png'
splot angle_b12 (x, y)
set output 'triangle_surf23.png'
splot angle_b23 (x, y)
set output 'triangle_surf31.png'
splot angle_b31 (x, y)

set contour
set cntrparam levels incremental 0, 1, 180
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
set cntrparam levels incremental 0, 3, 180
set output 'triangle_contour123-3deg.png'
splot angle_b12 (x, y), angle_b23 (x, y), angle_b31 (x, y)
set cntrparam levels incremental 0, 1, 180

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
