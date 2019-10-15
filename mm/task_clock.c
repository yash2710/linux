#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/syscalls.h>
#include <linux/task_clock.h>

struct task_clock_func task_clock_func;
EXPORT_SYMBOL(task_clock_func);

SYSCALL_DEFINE3(task_clock_open, int, fd, unsigned long, user_status, int, tid)
{
	current->task_clock.tid = tid;
	current->task_clock.fd = fd;
	current->task_clock.user_status =
		(struct task_clock_user_status *)user_status;
}

SYSCALL_DEFINE2(task_clock_close, int, fd, int, tid)
{
}

SYSCALL_DEFINE2(task_clock_do, uint32_t, operation, uint64_t, value)
{
	switch (operation) {
	case TASK_CLOCK_OP_STOP:
		if (current->task_clock.user_status &&
		    task_clock_func.task_clock_entry_stop) {
			task_clock_func.task_clock_entry_stop(
				current->task_clock.group_info);
		}
		break;
	case TASK_CLOCK_OP_START:
		if (current->task_clock.user_status &&
		    task_clock_func.task_clock_entry_start) {
			task_clock_func.task_clock_entry_start(
				current->task_clock.group_info);
		}
		break;
	case TASK_CLOCK_OP_START_COARSENED:
		if (current->task_clock.user_status &&
		    task_clock_func.task_clock_entry_start_no_notify) {
			task_clock_func.task_clock_entry_start_no_notify(
				current->task_clock.group_info);
		}
		break;
	}
}