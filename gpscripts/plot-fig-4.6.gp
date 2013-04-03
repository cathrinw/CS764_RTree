set term postscript color dashed
set output "figure-4-6.eps"
set title "Space Efficiency for 1k records"
unset logscale y
set yrange [20:80]
set xtics (128, 256, 512, 1024,2048, 4096)
set xlabel "Bytes per page"
set ylabel "KB required"
plot "lin.out" using 1:3 title "L, m=M/2" with lines, "lin.out.2" using 1:3 title "L, m=2" with lines, \
 "quad.out" using 1:3 title "Q, m=M/2" with lines, "quad.out.2" using 1:3 title "Q, m=2" with lines, \
 "ex.out" using 1:3 title "E, m=M/2" with lines, "ex.out.2" using 1:3 title "E, m=2" with lines 
set output
set term pop