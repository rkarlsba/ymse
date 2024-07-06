#!/bin/bash

outfile="summary.txt"
for f in *TLD*txt 
do
	sn=$( basename $f .txt ) 
       	echo =================================== $sn =================================== 
       	cat $f 
       	echo 
done > $outfile
