#ifndef TASK_CLOCK_FUNC_H
#define TASK_CLOCK_FUNC_H

#include <linux/irq_work.h>

#define TASK_CLOCK_MAX_THREADS 1024

//gets defined later
struct listarray;

//used by userspace to know what is going on
struct task_clock_user_status {
	uint64_t lowest_clock; //set when you inactivate the clock
	uint64_t ticks;
	u_int64_t notifying_clock;
	u_int64_t notifying_id;
	u_int64_t notifying_sample;
	u_int64_t notifying_diff;
	uint8_t single_active_thread; //set to 1 when there is only one thread active
	uint8_t activated_lowest;
	uint8_t scaling_whole, scaling_fraction;
	uint32_t hwc_idx; //index of the hw perf counter
	uint64_t period_sets;
	uint8_t hit_bounded_fence;
	uint64_t ticks_to_add;
};
__attribute__((aligned(8), packed));

struct task_clock_info {
	uint32_t tid;
	struct task_clock_user_status *user_status;
	int fd;
	struct task_clock_group_info *group_info;
};

struct task_clock_entry_info {
	uint8_t initialized;
	uint8_t sleeping;
	uint8_t waiting;
	uint8_t inactive;
	uint8_t count_ticks;
	//when the counter is stopped, the count gets reset to 0...but we need
	//that to persist. So, "ticks" is the current counter value and base_ticks
	//stores the values in previous counter sessions
	uint64_t ticks;
	uint64_t base_ticks;
	//length of the current chunk
	uint64_t chunk_ticks;
	struct perf_event *event;
	//sync clocks
	uint64_t local_sync_barrier_clock;
	uint8_t userspace_reading;
	int64_t overflow_budget;
};

struct task_clock_group_info {
	spinlock_t nmi_lock;
	spinlock_t lock;
	int32_t lowest_tid;
	uint64_t lowest_ticks;
	struct task_clock_entry_info clocks[TASK_CLOCK_MAX_THREADS];
	struct irq_work pending_work;
	uint8_t notification_needed;
	uint8_t nmi_new_low;
	struct task_clock_user_status *user_status_arr;
	struct listarray *active_threads;
	uint64_t global_sync_barrier_clock;
	uint64_t wakeup_target; //used to make sure only one thread targets a waiter
};

struct task_clock_func {
	void (*task_clock_overflow_handler)(struct task_clock_group_info *,
					    struct pt_regs *regs);
	struct task_clock_group_info *(*task_clock_group_init)(void);
	void (*task_clock_entry_init)(struct task_clock_group_info *,
				      struct perf_event *);
	void (*task_clock_on_disable)(struct task_clock_group_info *);
	void (*task_clock_on_enable)(struct task_clock_group_info *);
	void (*task_clock_entry_activate)(struct task_clock_group_info *);
	void (*task_clock_entry_halt)(struct task_clock_group_info *);
	void (*task_clock_entry_activate_other)(struct task_clock_group_info *,
						int32_t id);
	void (*task_clock_entry_activate_and_set)(
		struct task_clock_group_info *, uint32_t clock_val);
	void (*task_clock_entry_wait)(struct task_clock_group_info *);
	void (*task_clock_entry_sleep)(struct task_clock_group_info *);
	void (*task_clock_entry_woke_up)(struct task_clock_group_info *);
	void (*task_clock_overflow_update_period)(
		struct task_clock_group_info *);
	void (*task_clock_add_ticks)(struct task_clock_group_info *,
				     int32_t ticks);
	void (*task_clock_debug_add_event)(struct task_clock_group_info *,
					   int32_t event);
	void (*task_clock_entry_stop)(struct task_clock_group_info *);
	void (*task_clock_entry_start)(struct task_clock_group_info *);
	void (*task_clock_entry_stop_no_notify)(struct task_clock_group_info *);
	void (*task_clock_entry_start_no_notify)(struct task_clock_group_info *);
	void (*task_clock_entry_reset)(struct task_clock_group_info *);
	int (*task_clock_entry_is_singlestep)(struct task_clock_group_info *,
					      struct pt_regs *regs);
	void (*task_clock_entry_read_clock)(struct task_clock_group_info *);
};

#define TASK_CLOCK_MAX_THREADS 1024
#define TASK_CLOCK_OP_STOP 1
#define TASK_CLOCK_OP_START 2
#define TASK_CLOCK_OP_START_COARSENED 3

extern struct task_clock_func task_clock_func;

#endif