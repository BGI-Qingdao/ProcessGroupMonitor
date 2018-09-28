# ProcessGroupMonitor

A sample tool to monitor a process group and log it CPU and MEM used.

##INSTALL

    > make          #to make PGM
    > make clean    # to remove PGM

##USAGE 

### TO monitor exsit progess group 

    use ./PGM pgid pgid_num_to_monitor
        example  , if pgid is 100   :
        > ./PGM pgid 100

### TO start a new command and monitor it 

    use ./PGM file_to_run  args_list

        example , if you want to run 
        > ./test.sh 10 
        and monitor it . you can simplely run :
        > ./PGM ./test.sh 10


