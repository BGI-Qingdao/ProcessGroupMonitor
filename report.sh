#!/bin/bash

mem=`tail pglog_* |grep MEM_max | awk 'BEGIN{x=0;}{if($2>x)x=$2;}END{print x;}'`
cpu=`tail pglog_* |grep CPU_max | awk 'BEGIN{x=0;}{if($2>x)x=$2;}END{print x;}'`
t=`tail pglog_* |grep TIME | awk 'BEGIN{x=0;}{if($2>x)x=$2;}END{print x;}'`

echo "Total report begin =========="
echo "MEM_max   $mem    KB."
echo "CPU_max   $cpu    threads."
echo "TIME      $t     seconds."
echo "Total report end =========="
