set xrange [-1000:1000]
set yrange [-1000:1000]
tdoa (x, y, x0, y0, x1, y1) = sqrt ((x - x0)**2 + (y - y0)**2) - sqrt ((x - x1)**2 + (y - y1)**2)
set isosamples 50
set pm3d
unset surface
set term png
set output 'tdoa-small_tm3d.png'
splot tdoa (x, y, 4, -4, -4, 4)

set contour
set cntrparam levels 15
unset pm3d
set view map
unset clabel
set term png
set output 'tdoa-small_contour.png'
#splot tdoa (x, y, -4, -4, 4, 4), tdoa (x, y, -4, 4, 4, -4)
#splot tdoa (x, y, -15, 0, -10, 0), tdoa (x, y, 15, 0, 10, 0)
splot tdoa (x, y, -15, 0, -11, -4), tdoa (x, y, 15, 0, 15, -4)
#set cntrparam levels discrete 8.42638136229502
#set cntrparam levels discrete 2.99544039802068
#set cntrparam levels discrete 5.43094096427434
#splot tdoa (x, y, -4, -4, 0, 4), tdoa (x, y, 4, -4, 0, 4), tdoa (x, y, -4, -4, 4, -4)

