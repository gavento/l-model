unset title
unset key
unset xtics
unset ytics
set border 0
set terminal png
set output 'out.png'
set arrow from 10,0 to 10.5,0 nohead
set arrow from 10,0 to 10,13.5 nohead
set label "a" at 9.5,0
set arrow from 9,1 to 10.5,1 nohead
set arrow from 9,1 to 9,9.5 nohead
set label "b" at 8.5,1
set arrow from 8,5 to 10.5,5 nohead
set arrow from 8,5 to 8,11.5 nohead
set label "c" at 7.5,5
set arrow from 4,2 to 10.5,2 nohead
set arrow from 4,2 to 4,13.5 nohead
set label "d" at 3.5,2
set arrow from 1,11 to 10.5,11 nohead
set arrow from 1,11 to 1,13.5 nohead
set label "e" at 0.5,11
set arrow from 2,9 to 9.5,9 nohead
set arrow from 2,9 to 2,12.5 nohead
set label "f" at 1.5,9
set arrow from 7,8 to 10.5,8 nohead
set arrow from 7,8 to 7,8.5 nohead
set label "g" at 6.5,8
set arrow from 0,13 to 10.5,13 nohead
set arrow from 0,13 to 0,13.5 nohead
set label "h" at -0.5,13
set arrow from 0,3 to 10.5,3 nohead
set arrow from 0,3 to 0,3.5 nohead
set label "i" at -0.5,3
set arrow from 7,10 to 10.5,10 nohead
set arrow from 7,10 to 7,11.5 nohead
set label "j" at 6.5,10
set arrow from 3,4 to 9.5,4 nohead
set arrow from 3,4 to 3,9.5 nohead
set label "k" at 2.5,4
set arrow from 0,12 to 4.5,12 nohead
set arrow from 0,12 to 0,12.5 nohead
set label "l" at -0.5,12
set arrow from 6,7 to 8.5,7 nohead
set arrow from 6,7 to 6,11.5 nohead
set label "m" at 5.5,7
set arrow from 5,6 to 9.5,6 nohead
set arrow from 5,6 to 5,9.5 nohead
set label "n" at 4.5,6
set xrange [-1:11]
set yrange [-1:15]
plot -1.0
