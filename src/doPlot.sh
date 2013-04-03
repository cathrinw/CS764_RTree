
for  X in `ls ../gpscripts/*.gp` 
do
	gnuplot $X
done

for X in `ls *.eps`
do
	epstopdf --autorotate=All $X
	rm -f $X
done

