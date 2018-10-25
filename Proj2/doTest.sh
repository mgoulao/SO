#!/bin/bash

maxThreads=$1
inputFile=$2

outputFile="$inputFile.res"
csvFile="$inputFile.speedups.csv"


echo "#threads,exec_time,speedup" > $csvFile


./CircuitRouter-SeqSolver/CircuitRouter-SeqSolver $inputFile
tSequencial=$(cat $outputFile | grep "Elapsed time" | grep -Eo '[0-9]+([.][0-9]+)?')
echo 1S,$tSequencial,1 >> $csvFile

tParalela=0

for i in $(seq 1 $maxThreads)
do
	./CircuitRouter-ParSolver/CircuitRouter-ParSolver -t $i $inputFile
	tParalela=$(cat $outputFile | grep "Elapsed time" | grep -Eo '[0-9]+([.][0-9]+)?')
	speedup=$(echo "scale=6; ${tSequencial}/${tParalela}" | bc)
	echo $i,$tParalela,$speedup >> $csvFile
done

#seqTime=10
#$parTime=5
#speedup=$(echo "scale=6; ${seqTime}/${parTime}" | bc)
#echo Speedup = ${speedup}