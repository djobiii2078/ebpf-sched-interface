#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>
#include <thread>
#include <fstream>
#include <sys/syscall.h>
#include <string.h>
#include <sched.h>

#define __NR_set_bpf 440

#define NUM_OF_CORES 8
#define MAX_PRIME 100000

#define EBPF_TEST 0

#define PROCESS_ITEM 14

typedef struct {
	unsigned int pid;
	unsigned long utime;  //user time
	unsigned long stime;  //kernel time
	unsigned long cutime; //all user time
        unsigned long cstime; //all dead time
}Proc_Cpu_Occupy_t;

const char* get_items(const char*buffer ,unsigned int item){
	
	const char *p =buffer;
 
	int len = strlen(buffer);
	int count = 0;
	
	for (int i=0; i<len;i++){
		if (' ' == *p){
			count ++;
			if(count == item -1){
				p++;
				break;
			}
		}
		p++;
	}
 
	return p;
}


unsigned long get_cpu_proc_occupy(unsigned int pid){

	char file_name[64]={0};
	Proc_Cpu_Occupy_t t;
	FILE *fd;
	char line_buff[1024]={0};
	sprintf(file_name,"/proc/%d/stat",pid);

	fd = fopen(file_name,"r");
	if(nullptr == fd){
		return 0;
	}

	fgets(line_buff,sizeof(line_buff),fd);

	sscanf(line_buff,"%u",&t.pid);
	const char *q =get_items(line_buff,PROCESS_ITEM);
	sscanf(q,"%ld %ld %ld %ld",&t.utime,&t.stime,&t.cutime,&t.cstime);
	fclose(fd);

	return (t.utime + t.stime + t.cutime + t.cstime);
}


void do_primes()
{
    unsigned long i, num, primes = 0;
    for (num = 1; num <= MAX_PRIME; ++num) {
        for (i = 2; (i <= num) && (num % i != 0); ++i);
        if (i == num)
            ++primes;
    }
    printf("Calculated %d primes.\n", primes);
}

long sys_bpf(int val) {
	return syscall(__NR_set_bpf, val);
}

int main(int argc, char ** argv)
{
    struct timespec start={0,0}, end={0,0};
    //time_t run_time;
    unsigned long i;

    int num_cores = ( argc >= 2 ) ? atoi(argv[1]) : NUM_OF_CORES;

    printf("NB_CORES=%d\n",num_cores);
    pid_t pids[num_cores];
	
    struct sched_param param;
    param.sched_priority = 99;
    if(EBPF_TEST)
    {
        printf("EBPF mode\n");
    	/*long sys_ret = sys_bpf(1);
		if (sys_ret < 0) {
			printf("sys_bpf, ret: %ld\n", sys_ret);
			exit(1);
		}*/
    }
    

    /* start of test */
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < num_cores; ++i) {
        if (!(pids[i] = fork())) {
	
//	    printf("pids[%d]=%d--\n",i,pids[i]);
	    sched_setscheduler(getpid(),SCHED_FIFO,&param);
            do_primes();
//	    printf("pids[%d]=%d",i,pids[i]);
	    printf("%d CPU time used = %d\n",getpid(),get_cpu_proc_occupy(getpid()));
            exit(0);
        }
        if (pids[i] < 0) {
            perror("Fork");
            exit(1);
        }
    }
    for (i = 0; i < num_cores; ++i) {
        printf("pids[%d] = %d\n",i,pids[i]);
        waitpid(pids[i], NULL, 0);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
//    run_time = (end - start);
    printf("This machine calculated all prime numbers under %d %d times "
           "in %.5f seconds\n", MAX_PRIME, num_cores, ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec));

//    sleep(5);

    if(EBPF_TEST)
    {
    	long sys_ret = sys_bpf(0);
		if (sys_ret < 0) {
			printf("sys_bpf error, ret: %ld\n", sys_ret);
			exit(1);
		}
    }
    return 0;
}
