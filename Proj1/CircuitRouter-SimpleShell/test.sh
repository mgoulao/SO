#!/bin/bash
IFS=$'\n'       # make newlines the only separator

echo Test impleShell
echo --- Make ---
echo ""
echo $(make)
echo ""
echo --- Run ---
echo ""
for l in $(./CircuitRouter-SimpleShell 3 < simple-inputs.txt) 
do
	echo $l
done