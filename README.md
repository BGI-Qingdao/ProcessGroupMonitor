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
        > ./test.sh 10  # this test.sh need 1 parameter 
        and monitor it . you can simplely run :
        > ./PGM ./test.sh 10

### Output

####PGM will generate a log file for each process . 
        * the name of log file is : cmd + pid + start_time .
        * any charactor in cmd that both not a digit and 
            not a alphabet will become a '_' :
            like __share_app_bwa_0_7_12_bwa_index_chr19_standard_contig__206046_1538103905

####To get the overview of a finsihed process , use 

            > tail -n 6 your_log_file_name 

            and you will get a report like 

            > ###     Final report    1538103946      ###
            > cmd  /share/app/bwa-0.7.12/bwa index chr19_standard.contig
            > pid     206046
            > CPU_max 98.5
            > MEM_max 126148
            > TIME    41 seconds


            if you want to more than just a report , you can  read the whole log file and get information for each snapshot .

### Snapshot frequrency
            
            default is 5 seconds a snapshot .
            if you need another frequrency , modify the code of sleep function.
