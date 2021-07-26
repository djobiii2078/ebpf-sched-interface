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
    pid_t pids[num_cores];

    if(EBPF_TEST)
    {
    	long sys_ret = sys_bpf(1);
		if (sys_ret < 0) {
			printf("sys_bpf, ret: %ld\n", sys_ret);
			exit(1);
		}
    }
    

    /* start of test */
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < num_cores; ++i) {
        if (!(pids[i] = fork())) {
            do_primes();
            exit(0);
        }
        if (pids[i] < 0) {
            perror("Fork");
            exit(1);
        }
    }
    for (i = 0; i < NUM_OF_CORES; ++i) {
        waitpid(pids[i], NULL, 0);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
//    run_time = (end - start);
    printf("This machine calculated all prime numbers under %d %d times "
           "in %.5f seconds\n", MAX_PRIME, num_cores, ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec));

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
