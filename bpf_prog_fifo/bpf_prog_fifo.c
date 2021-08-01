#include<linux/bpf.h>
#include<asm-generic/types.h>
#include<bpf/bpf_helpers.h>
#include<linux/threads.h>
#include<linux/sched.h>

char _license[] SEC("license") = "GPL";

#define __inline inline __attribute __((always_inline))
#define __noinline __attribute__((no_inline))
#define __nooptimize __attribute__((optnone))

#define memcpy(dest,src,n) __builtin_memcpy((dest),(src),n)
#define memset(dest, value, n) __builtin_memset((dest),(vale),(n))

char fmt[] = "[ebpf-sched] call to __pick_next_entity, chosed_pid=%d, banned=%d";

struct key_process_t {
	u64 pid_process;
};

struct bpf_map_def sec("maps") processes = {
	.type = BPF_MAP_TYPE_HASH,
	.key_size = sizeof(struct key_process_t),
	.value_size = sizeof(u64),
	.max_entries = PID_MAX_LIMIT,
};

struct bpf_map_def sec("maps") start = {
	.type = BPF_MAP_TYPE_HASH,
	.key_size = sizeof(u32),
	.value_size = sizeof(u64),
	.max_entries = 10000,
};

struct bpf_map_def sec("maps") end = {
	.type = BPF_MAP_TYPE_HASH,
	.key_size = sizeof(u32),
	.value_size = sizeof(u64),
	.max_entries = 1000,
};

/*
* Check if the process chosed for scheduling
* is allowed or not 
* @pid : process chosed
* @array_to_check : list of pids to check --- should be populated later via interface
* @len : len of array 
*/
bool is_restricted(u64 pid, u64 array_to_check, u64 len){
	int cnt = 0;
	for (cnt = 0; cnt < len; cnt++){
		if (pid == array_to_check[cnt]) return false;
	}
	return true;
}

/*
* Hook __pick_next_entity
* THe final goal is to explore the rb structure to find
* a task that does not corresponds to a task in our blacklist
*/

u64 blacklist_array[] = [] //Empty for the moment

SEC("prog")
__u32 int main_func(struct rq *ctx)
{
	struct rt_rq *rt_rq = &ctx->rt;
	struct rt_prio_array *array = &rt_rq->active;
	struct sched_rt_entity *next = NULL;
	struct list_head *queue;
	int idx;

	return sched_find_first_bit(array->bitmap);
	
}




