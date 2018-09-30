#!/bin/bash

echo Analizar ficheiros de input:
echo ""

for f in $(ls inputs/*.txt | grep random)
do
	echo Input Name: $f
	echo Number of lines: $(wc --lines < $f)
done