#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(conversion_determ_init, unsigned long, address, unsigned long,
		token_addr)
{
	struct vm_area_struct *vma;

	vma = find_vma(current->mm, address);

	printk(KERN_EMERG
	       "in determ init sys call, is snapshot? %d %p address %p\n",
	       mmap_snapshot_instance.is_snapshot(vma, NULL, NULL),
	       mmap_snapshot_instance.conversion_determ_init, address);

	if (vma && mmap_snapshot_instance.is_snapshot &&
	    mmap_snapshot_instance.is_snapshot(vma, NULL, NULL) &&
	    mmap_snapshot_instance.conversion_determ_init) {
		printk(KERN_EMERG "made it in sys call\n");
		mmap_snapshot_instance.conversion_determ_init(
			vma,
			token_addr); //TODO: this function name in the struct should change$
	}
}

SYSCALL_DEFINE3(conversion_sync, unsigned long, address, int, flags, size_t,
		editing_distance)
{
	struct vm_area_struct *vma;

	vma = find_vma(current->mm, address);
	if (vma && mmap_snapshot_instance.is_snapshot &&
	    mmap_snapshot_instance.is_snapshot(vma, NULL, NULL) &&
	    mmap_snapshot_instance
		    .snapshot_msync) { //TODO: for commit, need to relax these constraints
		mmap_snapshot_instance.snapshot_msync(
			vma, flags,
			editing_distance); //TODO: this function name in the struct should change
	}
}