#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <cassert>

#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

struct AProc
{
    bool touched ;
    private:

    int pid ;

    long start_time ;

    float CPU_max ;

    float CPU_curr ;

    long Mem_max ;

    long Mem_curr ;

    long curr_time ;

    std::string cmd ;

    std::string cmd_full ;

    std::ofstream * ofs ;

    std::string Photo()
    {
        std::ostringstream ost ;
        ost<<"---\t"<<curr_time<<"\t---\n";
        ost<<"pid\t"<<pid<<'\n';
        ost<<"CPU\t"<<CPU_curr<<'\n';
        ost<<"MEM\t"<<Mem_curr;
        return ost.str();
    }

    std::string Final()
    {
        std::ostringstream ost ;
        ost<<"###\tFinal report\t"<<curr_time<<"\t###\n";
        ost<<"pid\t"<<pid<<'\n';
        ost<<"CPU_max\t"<<CPU_max<<'\n';
        ost<<"MEM_max\t"<<Mem_max<<'\n';
        ost<<"TIME\t"<<curr_time-start_time<<" seconds";
        return ost.str();
    }
    public:
    void End()
    {
        std::cerr<<" End of "<<pid<<" cmd "<<cmd<<std::endl;
        (*ofs)<<Final();
        (*ofs)<<std::endl;
        delete ofs ;
        ofs = NULL ;
    }
    void Init(int p , const std::string & c, const std::string & c1 ,long time_steamp )
    {
        pid = p ;
        start_time = time_steamp ;
        curr_time = time_steamp ;
        cmd = c ;
        cmd_full = c1 ;
        CPU_max = 0 ;
        Mem_max = 0 ;
        touched = false ;
        std::string path ;
        for(char c : cmd)
        {
            if( std::isalpha(c) )
                path += c;
            else
                path += '_';
        }
        std::string file_name = path +"_"+ std::to_string(pid)+"_"+std::to_string(curr_time);
        std::cerr<<" start of "<<pid<<" cmd "<<cmd<<" into file "<<file_name<<std::endl;
        ofs = new std::ofstream(file_name);
    }

    void Touch(float CPU , long MEM ,long MEM_Max, long time_steamp )
    {
        assert( MEM <= MEM_Max );
        CPU_curr = CPU ;
        if ( CPU > CPU_max )
            CPU_max = CPU ;
        Mem_curr = MEM ;
        if ( Mem_max < MEM_Max )
            Mem_max = MEM ;
        curr_time = time_steamp ;
        touched  = true ;
        (*ofs)<<Photo()<<std::endl;
    }

    void UnTouch()
    {
        touched = false ;
    }
};

bool GetProcInfo(int pid, float & CPU , long & MEM , long & MEM_Max)
{
    std::string cmd = std::string("ps ux | grep ")+std::to_string(pid)+" | grep -v grep";
    //root      2112  5.2  1.9 2927056 169380 ?      Sl   Sep25 148:36 /usr/bin/kwin_x11
    FILE * pp ;
    if( (pp = popen(cmd.c_str(), "r")) == NULL )
    {
        printf("popen() error!/n");
        exit(1);
    }
    bool ret = false ;
    char buf[10240];
    while(fgets(buf, sizeof buf, pp))
    {
        char tmp[10240];
        char status[10];
        int pid1 ,tmp1;
        float  mem ;
        sscanf(buf,"%s %d %f %f %lu %d %s %s ",
                tmp,&pid1,&CPU,&mem ,&MEM,
                &tmp1,tmp,status);
        if( pid1 != pid  )
            continue ;
        if( status[0] == 'Z' || status[0] == 'z')
            continue;
        MEM_Max = MEM ;
        ret = true ;
        break;
    }
    pclose(pp);
    return ret;
};

struct PGGroup
{
    std::map< int , AProc >  datas ;

    long curr_time ;
    void TouchStart( long time )
    {
        curr_time = time ;
        for( auto & pair : datas )
        {
            pair.second.UnTouch();
        }
    };

    bool Touch( int pid , std::string cmd , std::string cmd_full )
    {
        if( datas.find( pid ) == datas.end () )
        {
            datas[pid].Init(pid,cmd,cmd_full,curr_time);
        }
        long MEM , MEM_Max ;
        float CPU ;
        if( GetProcInfo(pid , CPU , MEM , MEM_Max) )
        {
            datas[pid].Touch(CPU ,MEM,MEM_Max , curr_time );
            return true ;
        }
        return false;
    }

    void TouchEnd()
    {
        std::set<int> dels;
        for( auto pair : datas )
        {
            if( ! pair.second.touched )
            {
                pair.second.End();
                dels.insert(pair.first);
            }
        }
        for( int x :dels )
            datas.erase(x);
    }
} pgdata;

void monitor_pgid(int pgid , int pid_self)
{
    int tick = 0 ;
    std::ostringstream ost;
    ost<<"ps -A -o stat,pid,pgid,cmd | grep "<<pgid<<" | grep -v ps | grep -v grep ";
    std::string cmd = ost.str();
    do
    {
        char buf[10240];
        FILE *pp = NULL ;
        long time_steamp = time(0);
        pgdata.TouchStart(time_steamp);

        if( (pp = popen(cmd.c_str(), "r")) == NULL )
        {
            printf("popen() error!/n");
            exit(1);
        }
        while(fgets(buf, sizeof buf, pp))
        {
            char exec_file[10240];
            char status[10];
            int pid1 , pgid1 ;
            sscanf(buf,"%s %d %d %s",status,&pid1, &pgid1,exec_file);
            if( pid1 == pid_self )
                continue ;
            if ( pgid1 != pgid )
                continue ;
            if( status[0] == 'Z' || status[0] == 'z')
                continue;
            std::string pgid_str = std::to_string(pgid1);
            std::string cmd_buffer1 = std::string(buf);
            std::string cmd = std::string (exec_file);
            std::string cmd_full = cmd_buffer1.substr(cmd_buffer1.find(pgid_str)+pgid_str.size());
            if ( cmd_full[cmd_full.size()-1] == '\n' ) cmd_full[cmd_full.size()-1] = ' ';
            if( pgdata.Touch(pid1,cmd,cmd_full) )
            {
                std::cerr<<" touch pid "<<pid1<<" cmd [ "<<cmd_full<<" ] succ !!! !!! "<<std::endl;
                tick = 0 ;
            }
        }
        tick ++ ;
        pgdata.TouchEnd();
        pclose(pp);
        sleep(5);
    } while( tick < 3 );
}

int main( int argc , char **argv )
{
    if( argc < 2 )
    {
        std::cerr<<"Usage   :   "<< std::endl;
        std::cerr<<"    to monitor exsit pgid , use "<<argv[0]<<" pgid pgid_num_to_monitor "<< std::endl;
        std::cerr<<"        example     :   "<<argv[0]<<" pgid 100"<< std::endl;
        std::cerr<<"    to start a new command adn monitor it , use "<<argv[0]<<" file_to run  args_list"<< std::endl;
        std::cerr<<"        example     :   "<<argv[0]<<" /bin/ls ./"<< std::endl;
        return 1 ;
    }
    if( std::string(argv[1]) == "pgid" )
    {
        if( argc != 3 )
        {
            std::cerr<<"Usage for pgid  :   "<<argv[0]<<" pgid pgid_num_to_monitor "<< std::endl;
            std::cerr<<"        example     :   "<<argv[0]<<" pgid 100"<< std::endl;
            return 1 ;
        }
        int pgid_to_monitor ;
        try 
        {
            pgid_to_monitor = std::stoi(std::string(argv[2]));
        }
        catch ( ... )
        {
            std::cerr<<"ERROR   :   "<<argv[2]<<" is not a valid pgid "<<std::endl;
            std::cerr<<"Usage for pgid  :   "<<argv[0]<<" pgid pgid_num_to_monitor "<< std::endl;
            std::cerr<<"        example     :   "<<argv[0]<<" pgid 100"<< std::endl;
            return 1 ;
        }
        std::cerr<<"Info    :   pgid to be moniterd is -- "<<pgid_to_monitor<<std::endl;
        pid_t pid = getpid();
        monitor_pgid(pgid_to_monitor,pid);
        return 0 ;
    }

    std::cerr<<"Info    :   command to be moniterd is -- ";
    for( int i = 1 ; i < argc ; i++ )
        std::cerr<<" "<<argv[i];
    std::cerr<<std::endl;
    pid_t pid = getpid();
    pid_t pgid = getpgid(pid);
    if( pid != pgid )
    {
        setpgid(pid,pid);
        std::cerr<<"Info    :   old pgid is -- "<<pgid<<std::endl;
        std::cerr<<"Info    :   set pgid to -- "<<pid<<std::endl;
    }
    else
        std::cerr<<"Info    :   curr pgid is -- "<<pid<<std::endl;
    char ** argv_new = new char * [argc-1];
    for( int i = 0 ; i < argc -1 ; i ++ )
    {
        argv_new[i] =argv[i+1];
    }
    argv_new[argc-1] = NULL;
    pid_t child_pid = fork();

    if( child_pid < 0 )
    {
        std::cerr<<"Error   :   fork failed !!!"<<strerror(errno)<<std::endl;
        return 1 ;
    }
    if (child_pid > 0) {    // Monitor process 
        monitor_pgid(pid,pid);
        delete []argv_new ;
        wait(NULL);
        return 0 ;
    }
    else { // Job process
       execv(argv[1] , argv_new);
       std::cerr<<"Error   :   execv failed !!!"<<strerror(errno)<<std::endl;
       return 1 ;
    }
    return 0 ;
}

