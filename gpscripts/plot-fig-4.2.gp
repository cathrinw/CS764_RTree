set term postscript color dashed
set output "figure-4-2.eps"
set title "CPU cost of inserting records"
set logscale y
set yrange [.05:150]
set xtics (128, 256, 512, 1024,2048, 4096)
set xlabel "Bytes per page"
set ylabel "CPU microsec per insert"
plot "lin.out" using 1:4 title "L, m=M/2" with lines, "lin.out.2" using 1:4 title "L, m=2" with lines, \
 "quad.out" using 1:4 title "Q, m=M/2" with lines, "quad.out.2" using 1:4 title "Q, m=2" with lines, \
 "ex.out" using 1:4 title "E, m=M/2" with lines, "ex.out.2" using 1:4 title "E, m=2" with lines
set output
set term pop