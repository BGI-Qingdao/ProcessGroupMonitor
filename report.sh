#!/bin/bash

mem=`tail pglog_* |grep MEM_max | awk 'BEGIN{x=0;}{if($2>x)x=$2;}END{print x;}'`
cpu=`tail pglog_* |grep CPU_max | awk 'BEGIN{x=0;}{if($2>x)x=$2;}END{print x;}'`
t=`tail pglog_* |grep TIME_total | awk 'BEGIN{x=0;}{if($2>x)x=$2;}END{print x;}'`
ct=`tail pglog_* |grep -E "CPU_max|TIME_total " | awk 'BEGIN{CT=0;c=0;}{if($1=="CPU_max"){c=0+$2;}else{t=0+$2; ct=c*t; CT=CT+ct;  c=0; }} END { printf("%d\n", CT ); }'`

echo "Total report begin =========="
echo "MEM_max               $mem   KB."
echo "CPU_max               $cpu%  threads."
echo "TIME                  $t     seconds."
echo "sum(CPU_max*TIME)*100 $ct    "
echo "Total report end =========="
