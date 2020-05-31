#!/bin/bash

function run1(){
awk 'BEGIN{tb=-1;tp=-1;tn=-1;mm=m;mc=0;r=0;tct=0}{if($1 == "cmd" && r>1) print $0 ; if($1=="###"){ r+=1;if(r==1){ tb=0+$4; tp=tb;} }; if(r<2){if($1=="---"){ tn = 0+$4 ;} else if ($1 == "MEM") {if( $2 > mm ) mm=$2 ; } else if ( $1 == "CPU") {t=tn-tp; tct+=t*(0+$2);tp=tn; if($2>mc)mc=$2;} } } END{printf("MEM_max %s KB\nCPU_max %s\nreal_time %s seconds\ncpu_time %s seconds\n",mm,mc,tn-tb,tct/100);}' $1
}

for x in $*
do
        echo "----------------------------"
        echo "log_file: $x"
        run1 $x
        echo "----------------------------"
        echo ""
done

