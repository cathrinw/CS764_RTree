#!/bin/bash

gnuplot --version &> /dev/null
if [ $? -ne 0 ];
then
	echo "Gnuplot not found. You need to have gnuplot in your PATH variable"
	exit
fi

python --version &> /dev/null
if [ $? -ne 0 ];
then
        echo "Python not found. You need to have python in your PATH variable"
        exit
fi

epstopdf --version &> /dev/null
if [ $? -ne 0 ];
then
        echo "epstopdf not found. You need to have epstopdf in your PATH variable"
        exit
fi

make --version &> /dev/null
if [ $? -ne 0 ];
then
        echo "make not found. You need to have make in your PATH variable"
        exit
fi

bash --version &> /dev/null
if [ $? -ne 0 ];
then
        echo "bash not found. You need to have bash in your PATH variable. How did you run this script in the first place? -_-"
        exit
fi

########

echo "*** You are all set. Will now build software and run experiments. Please stand by..."
make
reqFiles=("randomData.bin" "randomDataDelete.bin" "searches.bin" "randomDataDyn.bin" "dynSearches.bin" "dynSearches3k.bin")
for file in $reqFiles 
do
	if [ ! -f $file ];
	then
		echo "File '$file' must exist in your directory to proceed. You may want to consider to untar the sampleData.tgz archive if you have no other data handy."
		exit
	fi
done
	
sh test.sh quad 1
sh test.sh lin 0
if [ $# -gt 0 ];
then
	echo "*** Running long version of test suite (10 iterations for exhaustive algorithm). Be prepared that this will take about 100 minutes with the sample data."
	sh test.sh ex 2
else 
	echo "*** Running short version of test suite (1 iteration for exhaustive algorithm). This will approximately take 10 minutes with the example data. Depending on your processor speed and data it may just take forever =)"
	sh test.sh ex 2 1
fi

echo "*** Averaging and plotting... "
bash doAverage.sh
bash doPlot.sh

echo "*** All done. Look at the pretty PDFs, be happy, and go have a drink! ;-)"




