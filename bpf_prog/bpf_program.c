#include<linux/bpf.h>
#include<asm-generic/types.h>
#include<bpf/bpf_helpers.h>
//#include<linux/threads.h>
#include<linux/sched.h>

char _license[] SEC("license") = "GPL";

#define __inline inline __attribute __((always_inline))
#define __noinline __attribute__((no_inline))
#define __nooptimize __attribute__((optnone))

#define memcpy(dest,src,n) __builtin_memcpy((dest),(src),n)
#define memset(dest, value, n) __builtin_memset((dest),(vale),(n))

char fmt[] = "[ebpf-sched] call to __pick_next_entity, chosed_pid=%d, banned=%d";

/*
* Check if the process chosed for scheduling
* is allowed or not 
* @pid : process chosed
* @array_to_check : list of pids to check --- should be populated later via interface
* @len : len of array 
*/
int is_restricted(int pid, int *array_to_check, int len){
	int cnt = 0;
	for (cnt = 0; cnt < len; cnt++){
		if (pid == array_to_check[cnt]) return 0;
	}
	return 1;
}

/*
* Hook __pick_next_entity
* THe final goal is to explore the rb structure to find
* a task that does not corresponds to a task in our blacklist
*/

SEC("prog")
__u32 int main_func(struct bpf_sched *ctx)
{
	long number = 0;
    int i = 0;
    #pragma unroll
    &ctx->length = 1; 
    for (i = 0; i < (1024 / sizeof(int)); ++i) {
        memcpy(&number, &ctx->data[i * sizeof(long)], sizeof(long));
    }
    return 0;
}


