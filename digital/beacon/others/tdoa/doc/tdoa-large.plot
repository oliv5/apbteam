set xrange [0:3000]
set yrange [0:2100]
tdoa (x, y, x0, y0, x1, y1) = sqrt ((x - x0)**2 + (y - y0)**2) - sqrt ((x - x1)**2 + (y - y1)**2)
tdoa_b12 (x, y) = tdoa (x, y, 3000, 0, 3000, 2100)
tdoa_b23 (x, y) = tdoa (x, y, 3000, 0, 0, 1050)
tdoa_b31 (x, y) = tdoa (x, y, 3000, 2100, 0, 1050)
set contour
set cntrparam levels 50
unset surface
set view map
unset clabel
set term png
set output 'tdoa-large.png'
splot tdoa_b12 (x, y), tdoa_b23 (x, y), tdoa_b31 (x, y)
