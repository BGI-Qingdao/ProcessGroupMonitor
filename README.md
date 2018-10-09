# ProcessGroupMonitor

A simple tool to monitor a process group and record its CPU, MEM and time usage.

## INSTALL

    > make          # to make PGM
    > make clean    # to remove PGM

## USAGE 

### Monitor a running process group 

./PGM pgid [pgid_num_to_monitor]

For example, if pgid is 100:

> ./PGM pgid 100

### Start a new command and monitor it 

./PGM file_to_run  args_list

For example, if you want to run 

> ./test.sh 10  # this test.sh needs 1 parameter 

and monitor its CPU, MEM and time usage, then you can simply run:

> ./PGM ./test.sh 10

### Output

#### PGM will generate a log file for each process. 

* the name of log file : cmd + pid + start_time .

* any character in cmd that is neither a digit nor
  an alphabet will become a '_' like:

> __share_app_bwa_0_7_12_bwa_index_chr19_standard_contig__206046_1538103905

#### To get the overview of a complete process, use 

> tail -n 6 your_log_file_name 

You will get a report like 

             ###     Final report    1538103946      ###
             cmd  /share/app/bwa-0.7.12/bwa index chr19_standard.contig
             pid     206046
             CPU_max 98.5
             MEM_max 126148
             TIME    41 seconds


If you want more information other than just a simple report, then you can read the whole log file and get information for each snapshot.

### Snapshot frequency
            
The default value is 5 seconds per snapshot.

If a different frequency is desired, then you can try to modify the codes of sleep function.
