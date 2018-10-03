#!/bin/bash
IFS=$'\n'       # make newlines the only separator

echo Test 1
echo --- Make ---
echo ""
echo $(make)
echo ""
echo --- Run ---
echo ""
for l in $(./CircuitRouter-SeqSolver -x 2 inputs/random-x32-y32-z3-n64.txt -b 2) 
do
	echo $l
done