set term postscript color dashed
set output "figure-4-4.eps"
set title "Search performance pages touched"
unset logscale y
set yrange [0:3]
set xtics (128, 256, 512, 1024,2048, 4096)
set xlabel "Bytes per page"
set ylabel "Pages touched per qualifying record"
plot "lin.out" using 1:6 title "L, m=M/2" with lines, "lin.out.2" using 1:6 title "L, m=2" with lines, \
 "quad.out" using 1:6 title "Q, m=M/2" with lines, "quad.out.2" using 1:6 title "Q, m=2" with lines, \
 "ex.out" using 1:6 title "E, m=M/2" with lines, "ex.out.2" using 1:6 title "E, m=2" with lines
set output
set term pop