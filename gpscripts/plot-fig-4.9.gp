set term postscript color dashed
set output "figure-4-9.eps"
set title "Search performance vs. amount of data \nCPU cost"
unset logscale y
set yrange [0:.1]
set xtics 1000,1000
set xlabel "Number of records"
set ylabel "CPU nanosec per qualifying record"
plot "linNum.out" using 1:6 title "L, m=2" with lines,\
 "quadNum.out" using 1:6 title "Q, m=M/3" with lines
set output
set term pop