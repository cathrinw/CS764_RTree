set term postscript color dashed
set output "figure-4-7.eps"
set title "CPU cost of inserts and deletes vs. amount of data"
unset logscale y
set yrange [0:1]
set xtics 1000,1000
set xlabel "Number of records"
set ylabel "CPU microsec per insert/delete"
plot "linNum.out" using 1:3 title "L, m=2, insert" with lines, "linNum.out" using 1:7 title "L, m=2, delete" with lines, \
 "quadNum.out" using 1:3 title "Q, m=M/3, insert" with lines, "quadNum.out" using 1:7 title "Q, m=M/3, delete" with lines
set output
set term pop