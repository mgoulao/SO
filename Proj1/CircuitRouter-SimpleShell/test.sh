#!/bin/bash
IFS=$'\n'       # make newlines the only separator

echo Test SHell
echo --- Make ---
echo ""
echo $(make)
echo ""
echo --- Run ---
echo ""
for l in $(./CircuitRouter-SimpleShell < simple-inputs.txt) 
do
	echo $l
done