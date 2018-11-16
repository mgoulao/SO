#!/bin/bash

maxThreads=$1
inputFile=$2

if [ -z "$maxThreads" ]; then
	echo "Falta Numero de threads"
	exit
fi

if [ $maxThreads -lt 1 ]; then
	echo "Numero tem de ser maior que 0"
	exit
fi

if [ -z "$inputFile" ]; then
	echo "Falta inputFile"
	exit
fi

if [ ! -f "$inputFile" ]; then
	echo "Ficheiro invalido"
	exit
fi

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
