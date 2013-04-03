set term postscript color dashed
set output "figure-4-10.eps"
set title "Space required for R-Tree vs. amount of data"
unset logscale y
set yrange [0:250]
set xtics 1000,1000
set xlabel "Number of records"
set ylabel "KB required"
plot "linNum.out" using 1:2 title "L, m=2" with lines, \
 "quadNum.out" using 1:2 title "Q, m=M/3" with lines
set output
set term pop